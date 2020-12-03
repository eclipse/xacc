/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "quilc.hpp"
#include "cpr/api.h"
#include "cpr/unix_socket.h"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <cpr/cpr.h>
#include "json.hpp"
#include <regex>
#include <string>

using json = nlohmann::json;

namespace xacc {

namespace quilc {

Quilc::Quilc() : Compiler() {}

bool Quilc::canParse(const std::string &src) {
  // We define canParse as we have docker and rigetti/quilc
  // and this source string is Quil-like
  cpr::UnixSocket unix_socket{"/var/run/docker.sock"};
  auto get_images = cpr::Get(cpr::Url("http:/v1.35/images/json"), unix_socket);
  if (get_images.status_code != 200) {
    return false;
  }

  auto get_images_json = json::parse(get_images.text);
  bool found_quilc = false;
  for (auto &image : get_images_json) {
    auto repo_tag = image["RepoTags"][0].get<std::string>();
    if (repo_tag.find("rigetti/quilc") != std::string::npos) {
      found_quilc = true;
      break;
    }
  }

  if (!found_quilc) {
    return false;
  }

  if (src.find("__qpu__") != std::string::npos) {
    // valid xacc kernel
    return xacc::getCompiler("quil")->canParse(src);
  } else {
    // invalid xacc kernel, add function prototype to it
    return xacc::getCompiler("quil")->canParse(
        "__qpu__ void tmp_quil(qbit q) {\n" + src + "}\n");
  }
}

std::shared_ptr<IR> Quilc::compile(const std::string &src,
                                   std::shared_ptr<Accelerator> acc) {

  bool verbose = false;
  cpr::UnixSocket unix_socket{"/var/run/docker.sock"};
  cpr::Url createUrl{"http:/v1.35/containers/create"};

  auto get_images = cpr::Get(cpr::Url("http:/v1.35/images/json"), unix_socket);
  auto get_images_json = json::parse(get_images.text);
  bool found_quilc = false;
  for (auto &image : get_images_json) {
    if (!image.empty()) {
      auto repo_tag = image["RepoTags"][0].get<std::string>();
      if (repo_tag.find("rigetti/quilc") != std::string::npos) {
        found_quilc = true;
        break;
      }
    }
  }

  if (!found_quilc) {
    xacc::error(
        "\n\nThe quilc xacc::Compiler uses the rigetti/quilc docker image.\n"
        "Unable to find rigetti/quilc docker image.\nMake sure you have docker "
        "installed and pull the "
        "image before\nusing the quilc compiler.\n\n  docker pull "
        "rigetti/quilc\n");
  }

  // preprocess src string, can be xacc-like or just standard quil
  // if xacc-like, get all parameters
  std::string _src = src;
  if (src.find("__qpu__") != std::string::npos) {
    // this is xacc-like, get all double / std::vector<double> parameters
    auto prototype = src.substr(0, src.find_first_of("{"));
    auto args_str =
        prototype.substr(prototype.find_first_of("("), prototype.length());
    args_str.erase(
        std::remove_if(args_str.begin(), args_str.end(),
                       [](char ch) { return ch == '(' || ch == ')'; }),
        args_str.end());

    auto args = xacc::split(args_str, ',');

    std::vector<std::string> paramLines;
    for (auto &arg : args) {
      xacc::trim(arg);
      auto type_var = xacc::split(arg, ' ');
      if (type_var[0] == "double") {
        paramLines.push_back("DECLARE " + type_var[1] + " REAL[1]");
      } else if (type_var[0] == "std::vector<double>") {
      }
    }

    _src = src.substr(src.find_first_of("{") + 1, src.length());
    _src.erase(std::remove_if(_src.begin(), _src.end(),
                              [](char ch) { return ch == '}'; }),
               _src.end());

    std::string tmp = "";
    for (auto p : paramLines) {
      tmp += p + "\n";
    }
    _src = tmp + _src;
  }
  cpr::Header cprHeaders;
  cprHeaders.insert({"Content-Type", "application/json"});

  json j;
  j["Image"] = "rigetti/quilc";
  j["AttachStdin"] = false;
  j["OpenStdin"] = true;
  j["Tty"] = false;
  j["StdinOnce"] = false;
  j["AttachStdout"] = false;
  j["AttachStderr"] = false;
  j["NetworkDisabled"] = false;

  auto post_str = j.dump();
  auto r = cpr::Post(createUrl, unix_socket, cpr::Body(post_str), cprHeaders,
                     cpr::Verbose(verbose));
  //   std::cout << "r.status_code :" << r.status_code << std::endl;
  //   std::cout << "r.text :" << r.text << std::endl;
  auto jj = json::parse(r.text);
  std::string container_id = jj["Id"].get<std::string>();

  // START CONTAINER
  //  5516  curl --unix-socket /var/run/docker.sock
  //  http:/v1.35/containers/f7b908f2d8b83ac3344b57c0cb4579a37f4416705a68284b4cd32aeeec6d3227/start
  //  -X POST

  cpr::Url startUrl{"http:/v1.35/containers/" + container_id + "/start"};
  auto start_response = cpr::Post(startUrl, unix_socket, cpr::Verbose(verbose));
  //   std::cout << "start_response.status_code :" << start_response.status_code
  //             << std::endl;
  //   std::cout << "start_response.text :" << start_response.text << std::endl;

  // EXEC
  //  5519  curl --unix-socket /var/run/docker.sock
  //  http:/v1.35/containers/f7b908f2d8b83ac3344b57c0cb4579a37f4416705a68284b4cd32aeeec6d3227/exec
  //  -X POST -H "Content-Type: application/json" -d '{"Container":
  //  "f7b908f2d8b83ac3344b57c0cb4579a37f4416705a68284b4cd32aeeec6d3227",
  //  "User": "", "Privileged": false, "Tty": false,
  //   "AttachStdin": false, "AttachStdout": true, "AttachStderr": true, "Cmd":
  //   ["/bin/bash", "-c", "echo H 0 | quilc"]}'

  cpr::Url execUrl{"http:/v1.35/containers/" + container_id + "/exec"};
  json execJson;
  execJson["Container"] = container_id;
  execJson["User"] = "";
  execJson["Priviliged"] = false;
  execJson["AttachStdin"] = false;
  execJson["AttachStdout"] = true;
  execJson["AttachStderr"] = true;
  execJson["Tty"] = false;
  execJson["Cmd"] = std::vector<std::string>{"/bin/bash", "-c",
                                             "printf \"" + _src + "\" | quilc"};

  auto execR = cpr::Post(execUrl, unix_socket, cpr::Body(execJson.dump()),
                         cprHeaders, cpr::Verbose(verbose));
  //   std::cout << "\n\nexecR.status_code :" << execR.status_code << std::endl;
  //   std::cout << "execR.text :" << execR.text << std::endl;
  json jjj = json::parse(execR.text);

  // start the exec
  //  5520  curl --unix-socket /var/run/docker.sock
  //  http:/v1.35/exec/0c4b0ea67096feec14e9fda831774ef1126923e9d2aea904f1c065ee3c441653/start
  //  -X POST -H "Content-Type: application/json" -d '{"Tty": false, "Detach":
  //  false}'

  json startExec;
  startExec["Detach"] = false;
  startExec["Tty"] = false;
  cpr::Url startExecUrl{"http:/v1.35/exec/" + jjj["Id"].get<std::string>() +
                        "/start"};

  auto execStart =
      cpr::Post(startExecUrl, unix_socket, cpr::Body(startExec.dump()),
                cprHeaders, cpr::Verbose(verbose));
  //   std::cout << "\n\nexecStartR.status_code :" << execStart.status_code
  //             << std::endl;
  //   std::cout << "execStartR.text :\n" << execStart.text << std::endl;

  auto result_quil = execStart.text.substr(8, execStart.text.length());

  //   std::cout << "RESULTQUIL:\n" << result_quil << "\n";
  // stop the container, --rm will also delete it
  cpr::Url stopUrl{"http:/v1.35/containers/" + container_id + "/stop"};

  // Stop and remove the container, like we were never even here...
  auto stopResponse =
      cpr::Post(stopUrl, unix_socket, cprHeaders, cpr::Verbose(verbose));
  auto deleted = cpr::Delete(cpr::Url("http:/v1.35/containers/" + container_id),
                             unix_socket);

  // Process the returned flat quil
  auto lines = xacc::split(result_quil, '\n');
  std::string processed_quil = "";
  for (auto line : lines) {
    bool addChars = true;
    std::string processed_line = "";
    for (int i = 0; i < line.length(); i++) {
      if (line[i] == '#') {
        addChars = false;
      }
      if (line.find("HALT") != std::string::npos) {
        break;
      }

      if (addChars && line.find("DECLARE") == std::string::npos) {
        processed_line += line[i];
      }
    }
    xacc::trim(processed_line);
    processed_quil += processed_line + "\n";
  }

  auto simple_quil = xacc::getCompiler("quil");

  //   std::cout << "processed:\n" << processed_quil << "\n";
  if (src.find("__qpu__") != std::string::npos) {
    auto prototype = src.substr(0, src.find_first_of("{"));
    auto new_src = prototype + "{\n" + processed_quil + "}";
    // std::cout << "New:\n" << new_src << "\n";
    return simple_quil->compile(new_src, acc);
  } else {
    return simple_quil->compile(
        "__qpu__ void tmp_quil(qbit q) {\n" + processed_quil + "}\n", acc);
  }
}

std::shared_ptr<IR> Quilc::compile(const std::string &src) {
  return compile(src, nullptr);
}

} // namespace quilc

} // namespace xacc
