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
#include "QCSAccelerator.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "QuilVisitor.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include <Eigen/Dense>
#include <regex>
#include <chrono>
#include <cpr/cpr.h>

namespace xacc {
namespace quantum {


void QCSPlacement::apply(std::shared_ptr<CompositeInstruction> program,
                         const std::shared_ptr<Accelerator> acc,
                         const HeterogeneousMap &options) {

  // If we are here - this is a contributed service, not public - 
  // then we already have python initialized, and the backend has been given 
  // to us. 
  auto visitor = std::make_shared<QuilVisitor>(true, true);
  InstructionIterator it(program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    } 
  }
  auto quilStr = visitor->getQuilString();
  quilStr = "DECLARE ro BIT[" + std::to_string(program->nLogicalBits()) +
            "]\n" + quilStr;

  auto locals = py::dict();
  locals["quil_str"] = quilStr;
  locals["backend"] = backend;

//    std::cout << "Original:\n" << quilStr << "\n";

  auto py_src_get_native =
      R"#(import json
from pyquil import Program, get_qc

# very simple, just get the compiler
qc = get_qc(locals()['backend'])
compiler = qc.compiler

p = Program(locals()['quil_str'])
np = compiler.quil_to_native_quil(p)
#print('printing this,', np)
result = str(np)
)#";

  try {
    py::exec(py_src_get_native, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC QCS Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }

//   py::print("AfterPlacement:\n", locals["result"]);
  std::string tmp2 = "";
  auto tmp = locals["result"].cast<std::string>();
  for (auto line : xacc::split(tmp, '\n')) {
    if (line.find("DECLARE") == std::string::npos) {
      tmp2 += line + "\n";
    }
  }

  auto compiler = xacc::getCompiler("quil");
  auto new_program = compiler->compile("__qpu__ void foo(qreg q) {\n" + tmp2 + "}\n")
                         ->getComposites()[0];
    
  program->clear();
  program->addInstructions(new_program->getInstructions());
    
  return;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  for (auto f : functions) {
    auto tmpBuffer =
        std::make_shared<AcceleratorBuffer>(f->name(), buffer->size());
    execute(tmpBuffer, f);
    buffer->appendChild(tmpBuffer->name(), tmpBuffer);
  }
  return;
}

void QCSAccelerator::initialize(const HeterogeneousMap &params) {

  if (params.stringExists("forest_server_url")) {
    forest_server_url = params.getString("forest_server_url");
  }
  if (params.stringExists("engagement_url")) {
    engagement_url = params.getString("engagement_url");
  }
  if (params.stringExists("qpu_compiler_url")) {
    qpu_compiler_url = params.getString("qpu_compiler_url");
  }

  if (params.stringExists("auth_token")) {
    auth_token = params.getString("auth_token");
  }

  if (params.keyExists<bool>("use_rpcq_auth_config")) {
    use_rpcq_auth_config = params.get<bool>("use_rpcq_auth_config");
  }

  if (params.stringExists("endpoint")) {
    endpoint = params.getString("endpoint");
  }

  std::string isa_str = "";
  if (params.stringExists("backend")) {
    backend = params.getString("backend");

    auto response = this->get(forest_server_url, "/lattices/" + backend);

    auto lattice_json = json::parse(response);
    auto twoq = lattice_json["lattice"]["isa"]["2Q"].get<json::object_t>();

    // Save the ISA for TargetDevice quil_to_native_quil
    isa_str = lattice_json["lattice"]["isa"].dump();

    for (auto element : twoq) {
      if (element.second.find("dead") == element.second.end()) {
        auto bits = xacc::split(element.first, '-');
        latticeEdges.push_back(
            std::make_pair(std::stoi(bits[0]), std::stoi(bits[1])));
      }
    }
  }

  if (!guard && !Py_IsInitialized()) {
    guard = std::make_shared<py::scoped_interpreter>();
    //       libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY |
    //       RTLD_GLOBAL);
    initialized = true;
  }

  _internal_init();

  // This Accelerator requires a custom Placement strategy
  auto placement = std::make_shared<QCSPlacement>(backend, isa_str);
  xacc::contributeService("qcs-quilc", placement);

  updateConfiguration(params);
}

void QCSAccelerator::_internal_init() {

  if (auth_token.empty()) {
    if (!xacc::fileExists(std::getenv("HOME") +
                          std::string("/.qcs/user_auth_token"))) {
      xacc::warning("[QCS] $HOME/.qcs/user_auth_token file does not exist.");
    }
    std::ifstream stream(std::getenv("HOME") +
                         std::string("/.qcs/user_auth_token"));
    std::string contents((std::istreambuf_iterator<char>(stream)),
                         std::istreambuf_iterator<char>());
    auto auth_json = json::parse(contents);
    auth_token = auth_json["access_token"].get<std::string>();
  }

  if (endpoint.empty()) {
    endpoint = backend;
  }

  std::string json_data = "{\"endpointId\": \"" + endpoint + "\"}";
  std::map<std::string, std::string> headers{
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Content-Length", std::to_string(json_data.length())},
      {"Authorization", "Bearer " + auth_token}};
  auto resp = this->post(engagement_url, "/engagements", json_data, headers);

  auto resp_json = json::parse(resp);

  client_public = resp_json["credentials"]["clientPublic"].get<std::string>();
  client_secret = resp_json["credentials"]["clientSecret"].get<std::string>();
  server_public = resp_json["credentials"]["serverPublic"].get<std::string>();
  qpu_endpoint = resp_json["address"];
  qpu_compiler_endpoint = qpu_compiler_url;
  user_id = resp_json["userId"].get<std::string>();
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  using json = nlohmann::json;

  // Map IR to Native Quil string
  auto visitor = std::make_shared<QuilVisitor>(true);
  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (!nextInst->isComposite() && nextInst->isEnabled()) {
        nextInst->accept(visitor);
    }
  }
    
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;
  
//   std::cout << "\nBefore Quil Program:\n" << quilStr << "\n";

  quilStr = std::regex_replace(quilStr, std::regex("1.5708"), "pi/2");
  quilStr = std::regex_replace(quilStr, std::regex("3.14159"), "pi");

//   std::cout << "\nAfter Quil Program:\n" << quilStr << "\n";

    
  json j;
  j["quil"] = quilStr;
  j["num_shots"] = shots;
  j["_type"] = "QuilBinaryExecutableRequest";
  std::string json_data = j.dump();
  std::map<std::string, std::string> headers{
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Accept", "application/octet-stream"},
      {"Content-Length", std::to_string(json_data.length())},
      {"Authorization", "Bearer " + auth_token}};
  auto resp = post(qpu_compiler_endpoint,
                   "/devices/" + backend + "/native_quil_to_binary", json_data,
                   headers);
  std::string prog = json::parse(resp)["program"].get<std::string>();

  auto locals = py::dict();
  locals["program"] = prog;
  locals["client_public_key"] = py::bytes(client_public);
  locals["client_secret_key"] = py::bytes(client_secret);
  locals["server_public_key"] = py::bytes(server_public);
  locals["endpoint"] = qpu_endpoint;
  locals["userId"] = user_id;
  locals["use_rpcq_auth_config"] = use_rpcq_auth_config;

  // Setup the python src.
  auto py_src =
      R"#(from typing import Any, Dict
import uuid, numpy as np
from rpcq._client import Client, ClientAuthConfig
from rpcq.messages import QuiltBinaryExecutableResponse, QPURequest

client = None
if locals()['use_rpcq_auth_config']:
    auth = ClientAuthConfig(client_public_key=locals()['client_public_key'],
                     client_secret_key=locals()['client_secret_key'],
                     server_public_key=locals()['server_public_key'])
    client = Client(locals()['endpoint'], auth_config=auth)
else: 
    client = Client(locals()['endpoint'])

request = QPURequest(program=locals()['program'], patch_values={}, id=str(uuid.uuid4()))
job_id = client.call('execute_qpu_request', request=request, user=locals()['userId'], priority=1)
buffers = client.call("get_buffers", job_id, wait=True)
results = {}
for k, v in buffers.items():
    buf = np.frombuffer(v["data"], dtype=v["dtype"])
    results[k] = buf.reshape(v["shape"])
)#";

  // return;
  try {
    py::exec(py_src, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC QCS Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }

  auto results = locals["results"];
//   py::print("C++ Results:\n",results);

  // Decode the results, update AcceleratorBuffer
  if (endpoint == backend) ResultsDecoder().decode(buffer, results, shots);
  return;
}

void ResultsDecoder::decode(std::shared_ptr<AcceleratorBuffer> buffer,
                            py::object result, 
                            int shots) {

  auto as_dict = result.cast<py::dict>();

  Eigen::MatrixXi bits = Eigen::MatrixXi::Zero(shots, as_dict.size() / 2);
  int counter = 0;
  for (auto kv : as_dict) {
    if (kv.first.cast<std::string>().find("unclassified") ==
        std::string::npos) {
        // FIXME can I cast to std::vector<int> once? 
      auto single_cast = kv.second.cast<py::list>();
      for (int k = 0; k < shots; k++) {
        auto bit = single_cast[k].cast<int>(); 
        bits(k, counter) = bit;
      }
      counter++;
    }
  }
  for (int i = 0; i < bits.rows(); i++) {

    std::string bitstr = "";
    for (int j = 0; j < bits.cols(); j++) {
      bitstr += std::to_string(bits(i, j));
    }

    buffer->appendMeasurement(bitstr);
  }
  return;
}

std::string QCSAccelerator::post(const std::string &_url,
                                 const std::string &path,
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
QCSAccelerator::get(const std::string &_url, const std::string &path,
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

const std::string
QCSRestClient::post(const std::string &remoteUrl, const std::string &path,
                    const std::string &postStr,
                    std::map<std::string, std::string> headers) {

  if (headers.empty()) {
    headers.insert(std::make_pair("Content-type", "application/json"));
    headers.insert(std::make_pair("Connection", "keep-alive"));
    headers.insert(std::make_pair("Accept", "*/*"));
  }

  cpr::Header cprHeaders;
  for (auto &kv : headers) {
    cprHeaders.insert({kv.first, kv.second});
  }

  if (verbose)
    xacc::info("Posting to " + remoteUrl + path + ", with data " + postStr);

  auto r = cpr::Post(cpr::Url{remoteUrl + path}, cpr::Body(postStr), cprHeaders,
                     cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP POST Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
}

const std::string
QCSRestClient::get(const std::string &remoteUrl, const std::string &path,
                   std::map<std::string, std::string> headers,
                   std::map<std::string, std::string> extraParams) {
  if (headers.empty()) {
    headers.insert(std::make_pair("Content-type", "application/json"));
    headers.insert(std::make_pair("Connection", "keep-alive"));
    headers.insert(std::make_pair("Accept", "*/*"));
  }

  cpr::Header cprHeaders;
  for (auto &kv : headers) {
    cprHeaders.insert({kv.first, kv.second});
  }

  cpr::Parameters cprParams;
  for (auto &kv : extraParams) {
    cprParams.AddParameter({kv.first, kv.second});
  }

  if (verbose)
    xacc::info("GET at " + remoteUrl + path);

  auto r = cpr::Get(cpr::Url{remoteUrl + path}, cprHeaders, cprParams,
                    cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP GET Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
}
} // namespace quantum
} // namespace xacc

#include "xacc_plugin.hpp"
REGISTER_ACCELERATOR(xacc::quantum::QCSAccelerator)