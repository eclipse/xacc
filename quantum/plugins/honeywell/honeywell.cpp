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
#include "honeywell.hpp"

#include <cpr/cpr.h>
#include <spdlog/fmt/fmt.h>

#include <cctype>
#include <ctime>
#include <fstream>
#include <regex>
#include <thread>

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "json.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {

std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

std::pair<bool, std::string> need_refresh(std::vector<int> old_key_time) {
  std::time_t t = std::time(0); // get time now
  std::tm *now2 = std::localtime(&t);

  double hours = 0.0;
  {
    std::tm t1;
    t1.tm_year = old_key_time[0];
    t1.tm_mon = old_key_time[1];
    t1.tm_mday = old_key_time[2];
    t1.tm_hour = old_key_time[3];
    t1.tm_min = old_key_time[4];
    t1.tm_sec = old_key_time[5];

    std::tm t2;
    t2.tm_year = (now2->tm_year + 1900);
    t2.tm_mon = (now2->tm_mon + 1);
    t2.tm_mday = now2->tm_mday;
    t2.tm_hour = now2->tm_hour;
    t2.tm_min = now2->tm_min;
    t2.tm_sec = now2->tm_sec;

    std::time_t time_t1 = std::mktime(&t1);
    std::time_t time_t2 = std::mktime(&t2);
    auto start = std::chrono::system_clock::from_time_t(time_t1);
    auto end = std::chrono::system_clock::from_time_t(time_t2);

    auto duration =
        std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
    hours = duration / 60.;
  }

  xacc::info("It has been " + std::to_string(hours) +
             " hours since last token generated.");

  // Refresh if it has been close to an hour since
  // the token was generated.
  return std::make_pair(hours > .95,
                        fmt::format("{}_{}_{}_{}_{}_{}", (now2->tm_year + 1900),
                                    (now2->tm_mon + 1), now2->tm_mday,
                                    now2->tm_hour, now2->tm_min, now2->tm_sec));
}

void HoneywellAccelerator::refresh_tokens(bool force_refresh) {
  if (!time_str.empty()) {
    std::vector<int> old_time_key;
    auto time_split = xacc::split(time_str, '_');
    for (auto i = 0; i < time_split.size(); i++)
      old_time_key.push_back(std::stoi(time_split[i]));

    auto [_need_refresh, now] = need_refresh(old_time_key);
    if (_need_refresh || force_refresh) {
      xacc::info("Refreshing id-token");
      auto response =
          post(url, "login",
               "{" + fmt::format("\"refresh-token\":\"{}\"", refresh_key) + "}",
               generateRequestHeader());
      auto response_json = nlohmann::json::parse(response);
      api_key = response_json["id-token"].get<std::string>();
      refresh_key = response_json["refresh-token"].get<std::string>();

      std::ofstream out(std::string(getenv("HOME")) + "/.honeywell_config");
      out << "key:" << api_key << "\n";
      out << "refresh:" << refresh_key << "\n";
      out << "time:" << now << "\n";
      out.close();
      time_str = now;
    }
  }
}

void HoneywellAccelerator::initialize(const HeterogeneousMap &params) {
  if (!initialized) {
    searchAPIKey(api_key);

    // If we have a time string, then we can check
    // for necessary id token refreshes (must occur every hour)
    auto force_refresh = params.get_or_default<bool>("force-refresh", false);
    refresh_tokens(force_refresh);

    if (params.keyExists<int>("shots")) {
      shots = params.get<int>("shots");
    }

    if (params.stringExists("job-name")) {
      job_name = params.getString("job-name");
    }

    if (params.stringExists("retrieve-job-id")) {
      retrieve_job_id = params.getString("job-id");
    }

    // Query avalable backends:
    const auto get_avail_backends_rsp =
        get(url, "machine/?config=true", generateRequestHeader());
    auto avail_backends_json = nlohmann::json::parse(get_avail_backends_rsp);
    available_backends.clear();
    for (auto it = avail_backends_json.begin(); it != avail_backends_json.end();
         ++it) {
      // std::cout << (*it)["name"].get<std::string>() << "\n";
      // std::cout << (*it)["n_qubits"].get<size_t>() << "\n";
      available_backends.emplace_back((*it)["name"].get<std::string>());
    }

    if (params.stringExists("backend")) {
      backend = params.getString("backend");
      if (!xacc::container::contains(available_backends, backend)) {
        std::stringstream ss;
        ss << backend
           << " is an invalid Honeywell backend or you do not have access to "
              "that backend.\nAvailable backends: \n";
        for (const auto &name : available_backends) {
          ss << name << "\n";
        }
        xacc::error(ss.str());
      }
    }

    // No backend was provided....
    if (backend.empty()) {
      std::stringstream ss;
      ss << "Please provide a backend name.\nAvailable backends: \n";
      for (const auto &name : available_backends) {
        ss << name << "\n";
      }
      xacc::error(ss.str());
    }

    initialized = true;
  }
}

void HoneywellAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> circuit) {
  if (backend.empty()) {
    xacc::error(
        "Please specify a honeywell backend in your getAccelerator() call.");
  }
  auto qasm = getNativeCode(circuit);
  while (true) {
    auto backend_status =
        get(url, "machine/" + backend, generateRequestHeader());
    auto status_J = nlohmann::json::parse(backend_status);
    // xacc::info("\nHoneywell status:\n" + backend_status);
    // If offline, terminate
    if (status_J["state"] == "offline") {
      xacc::error("Cannot run on " + backend + ", it is offline.");
    }
    // If online, continue to run
    if (status_J["state"] == "online") {
      break;
    }
    // If status = 'in maintenance', wait for a bit and try again
    xacc::info("Honeywell is '" + std::string(status_J["state"]) +
               "', waiting for a bit...");
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }
  xacc::info("\nHoneywell sending qasm:\n" + qasm);

  nlohmann::json j;

  j["machine"] = backend;
  j["language"] = "OPENQASM 2.0";
  j["program"] = qasm;
  j["priority"] = "normal";
  j["count"] = shots;
  j["options"] = nullptr;
  if (!job_name.empty()) {
    j["name"] = job_name;
  }

  // Check if we should refresh
  refresh_tokens();

  std::map<std::string, std::string> headers{
      {"Authorization", api_key},
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Accept", "*/*"}};

  std::string get_job_status = "";
  nlohmann::json get_job_status_json;

  // if retrieve_job_id was provided, check status and return
  if (!retrieve_job_id.empty()) {

    get_job_status = get(url, "job/" + retrieve_job_id, headers);
    get_job_status_json = nlohmann::json::parse(get_job_status);

  } else {

    auto response = post(url, "job", j.dump(), headers);
    auto response_json = nlohmann::json::parse(response);
    auto job_id = response_json["job"].get<std::string>();

    xacc::info("Honeywell job-id: " + job_id);

    // Job has started, so watch for status == COMPLETED
    int dots = 1;
    while (true) {
      get_job_status = get(url, "job/" + job_id, headers);
      get_job_status_json = nlohmann::json::parse(get_job_status);

      if (get_job_status_json["status"].get<std::string>().find("failed") !=
          std::string::npos) {
        xacc::error("Honeywell job failed: " + get_job_status_json.dump(4));
      }
      if (get_job_status_json["status"].get<std::string>() == "completed") {
        break;
      }

      if (dots > 4)
        dots = 1;
      std::stringstream ss;
      ss << "\033[0;32m"
         << "Honeywell Job "
         << "\033[0;36m" << job_id << "\033[0;32m"
         << " Status: " << get_job_status_json["status"].get<std::string>();
      for (int i = 0; i < dots; i++)
        ss << '.';
      dots++;
      std::cout << '\r' << ss.str() << std::setw(20) << std::setfill(' ')
                << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  std::cout << "\033[0m"
            << "\n";

  xacc::info("\nHoneywell job result json:\n" + get_job_status);
  auto results =
      get_job_status_json["results"].begin()->get<std::vector<std::string>>();
  for (auto &bitstring : results) {
    buffer->appendMeasurement(bitstring);
  }

  return;
}

void HoneywellAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  for (auto &f : compositeInstructions) {
    auto tmpBuffer =
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
    execute(tmpBuffer, f);
    buffer->appendChild(f->name(), tmpBuffer);
  }
}

void HoneywellAccelerator::searchAPIKey(std::string &key) {
  //   // Search for the API Key in $HOME/.ibm_config,
  //   // $HW_CONFIG, or in the command line argument --ibm-api-key
  std::string hwConfig(std::string(getenv("HOME")) + "/.honeywell_config");
  if (xacc::fileExists(hwConfig)) {
    findApiKeyInFile(key, hwConfig);
  } else {
    xacc::error("Cannot find Honeywell Config file with credentials "
                "(~/.honeywell_config).");
  }
}

void HoneywellAccelerator::findApiKeyInFile(std::string &apiKey,
                                            const std::string &path) {
  std::ifstream stream(path);
  std::string contents((std::istreambuf_iterator<char>(stream)),
                       std::istreambuf_iterator<char>());

  std::vector<std::string> lines;
  lines = xacc::split(contents, '\n');
  for (auto l : lines) {
    if (l.find("key") != std::string::npos) {
      std::vector<std::string> split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      api_key = key;
    } else if (l.find("refresh") != std::string::npos) {
      std::vector<std::string> split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      refresh_key = key;
    } else if (l.find("time") != std::string::npos) {
      std::vector<std::string> split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      time_str = key;
    }
  }
}

const std::string RestClient::post(const std::string &remoteUrl,
                                   const std::string &path,
                                   const std::string &postStr,
                                   std::map<std::string, std::string> headers) {
  if (headers.empty()) {
    headers.insert(std::make_pair("Content-type", "application/json"));
    // headers.insert(std::make_pair("Connection", "keep-alive"));
    // headers.insert(std::make_pair("Accept", "*/*"));
  }

  cpr::Header cprHeaders;
  for (auto &kv : headers) {
    cprHeaders.insert({kv.first, kv.second});
  }

  if (verbose)
    xacc::info("Posting to " + remoteUrl + path + ", with data " + postStr);

  auto r = cpr::Post(cpr::Url{remoteUrl + path}, cpr::Body(postStr), cprHeaders,
                     cpr::VerifySsl(false));
  if (r.status_code != 200) {
    std::cout << r.status_code << ", " << r.text << ", " << r.error.message
              << "\n";
    throw std::runtime_error("HTTP POST Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);
  }

  return r.text;
}

const std::string
RestClient::get(const std::string &remoteUrl, const std::string &path,
                std::map<std::string, std::string> headers,
                std::map<std::string, std::string> extraParams) {
  if (headers.empty()) {
    headers.insert(std::make_pair("Content-type", "application/json"));
    // headers.insert(std::make_pair("Connection", "keep-alive"));
    // headers.insert(std::make_pair("Accept", "*/*"));
  }

  cpr::Header cprHeaders;
  for (auto &kv : headers) {
    cprHeaders.insert({kv.first, kv.second});
  }

  cpr::Parameters cprParams;
  for (auto &kv : extraParams) {
    cprParams.AddParameter({kv.first, kv.second});
  }

  auto r = cpr::Get(cpr::Url{remoteUrl + path}, cprHeaders, cprParams,
                    cpr::VerifySsl(false));

  if (r.status_code != 200) {
    std::cout << r.status_code << ", " << r.text << ", " << r.error.message
              << "\n";

    throw std::runtime_error("HTTP GET Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);
  }

  return r.text;
}

std::string
HoneywellAccelerator::post(const std::string &_url, const std::string &path,
                           const std::string &postStr,
                           std::map<std::string, std::string> headers) {
  std::string postResponse;
  int retries = 10;
  std::exception ex;
  bool succeeded = false;

  // Execute HTTP Post
  do {
    try {
      postResponse = restClient->post(_url, path, postStr, headers);
      succeeded = true;
      break;
    } catch (std::exception &e) {
      ex = e;
      xacc::info("Remote Accelerator " + name() +
                 " caught exception while calling restClient->post() "
                 "- " +
                 std::string(e.what()));
      retries--;
      if (retries > 0) {
        xacc::info("Retrying HTTP Post.");
      }
    }
  } while (retries > 0);

  if (!succeeded) {
    cancel();
    xacc::error("Remote Accelerator " + name() +
                " failed HTTP Post for Job Response - " +
                std::string(ex.what()));
  }

  return postResponse;
}

std::string
HoneywellAccelerator::get(const std::string &_url, const std::string &path,
                          std::map<std::string, std::string> headers,
                          std::map<std::string, std::string> extraParams) {
  std::string getResponse;
  int retries = 10;
  std::exception ex;
  bool succeeded = false;
  // Execute HTTP Get
  do {
    try {
      getResponse = restClient->get(_url, path, headers, extraParams);
      succeeded = true;
      break;
    } catch (std::exception &e) {
      ex = e;
      xacc::info("Remote Accelerator " + name() +
                 " caught exception while calling restClient->get() "
                 "- " +
                 std::string(e.what()));
      // s1.find(s2) != std::string::npos) {
      if (std::string(e.what()).find("Caught CTRL-C") != std::string::npos) {
        cancel();
        xacc::error(std::string(e.what()));
      }
      retries--;
      if (retries > 0) {
        xacc::info("Retrying HTTP Get.");
      }
    }
  } while (retries > 0);

  if (!succeeded) {
    cancel();
    xacc::error("Remote Accelerator " + name() +
                " failed HTTP Get for Job Response - " +
                std::string(ex.what()));
  }

  return getResponse;
}

std::map<std::string, std::string>
HoneywellAccelerator::generateRequestHeader() const {
  std::map<std::string, std::string> headers{
      {"Authorization", api_key},
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Accept", "*/*"}};
  return headers;
}

std::string HoneywellAccelerator::getNativeCode(
    std::shared_ptr<CompositeInstruction> circuit,
    const HeterogeneousMap &config) {
  // Need to replace swaps with cnots
  auto provider = xacc::getIRProvider("quantum");
  auto replace_swaps = [&](std::shared_ptr<Instruction> swap_inst)
      -> std::vector<std::shared_ptr<Instruction>> {
    auto bits = swap_inst->bits();
    auto buffer_names = swap_inst->getBufferNames();
    auto cx_1 = provider->createInstruction("CNOT", bits);
    auto cx_2 = provider->createInstruction("CNOT", {bits[1], bits[0]});
    auto cx_3 = provider->createInstruction("CNOT", bits);
    cx_1->setBufferNames(buffer_names);
    cx_2->setBufferNames(buffer_names);
    cx_3->setBufferNames(buffer_names);
    return {cx_1, cx_2, cx_3};
  };

  //   std::cout << "before:\n" << circuit->toString() << "\n\n";
  for (int i = circuit->nInstructions() - 1; i >= 0; i--) {
    auto inst = circuit->getInstruction(i);
    if (inst->name() == "Swap") {
      auto comp =
          provider->createComposite("swap_replace_" + std::to_string(i));
      comp->addInstructions(replace_swaps(inst));
      circuit->replaceInstruction(i, comp);
    }
  }

  //   std::cout << "after:\n" << circuit->toString() << "\n";

  auto qasm_compiler = xacc::getCompiler("staq");
  auto qasm = qasm_compiler->translate(circuit);
  return qasm;
}
} // namespace quantum
} // namespace xacc

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL HoneywellActivator : public BundleActivator {
public:
  HoneywellActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto xt = std::make_shared<xacc::quantum::HoneywellAccelerator>();
    context.RegisterService<xacc::Accelerator>(xt);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(HoneywellActivator)
