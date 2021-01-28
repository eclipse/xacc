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

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  using json = nlohmann::json;

  // Map IR to Native Quil string
  auto visitor = std::make_shared<QuilVisitor>(true);
  std::set<int> qbitIdxs;
  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
      if (nextInst->name() == "Measure") {
        qbitIdxs.insert(nextInst->bits()[0]);
      }
    }
  }
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;
  std::cout << "\nQuil Program:\n" << quilStr << "\n";
    
  json j;
  j["quilt"] = quilStr;
  j["num_shots"] = shots;
  j["_type"] = "QuiltBinaryExecutableRequest";
  std::string json_data = j.dump();
  std::map<std::string, std::string> headers{
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Accept", "application/octet-stream"},
      {"Content-Length", std::to_string(json_data.length())},
      {"Authorization", "Bearer " + auth_token}};

  auto resp =
      post(qpu_compiler_endpoint,
                 "/devices/"+backend+"/native_quilt_to_binary", json_data, headers);
  std::string prog = json::parse(resp)["program"].get<std::string>();
    
  auto locals = py::dict();
  locals["program"] = prog;
  locals["client_public_key"] = py::bytes(client_public);
  locals["client_secret_key"] = py::bytes(client_secret);
  locals["server_public_key"] = py::bytes(server_public);
  locals["endpoint"] = qpu_endpoint;
  locals["userId"] = user_id;
    
  // Setup the python src.
  auto py_src =
      R"#(from typing import Any, Dict
import uuid, numpy as np
from rpcq._client import Client, ClientAuthConfig
from rpcq.messages import QuiltBinaryExecutableResponse, QPURequest
from pyquil.api._qpu import decode_buffer

auth = ClientAuthConfig(client_public_key=locals()['client_public_key'],
                 client_secret_key=locals()['client_secret_key'],
                 server_public_key=locals()['server_public_key'])
client = Client(locals()['endpoint'], auth_config=auth)
request = QPURequest(program=locals()['program'], patch_values={}, id=str(uuid.uuid4()))
print('calling execute qpu request')
job_id = client.call('execute_qpu_request', request=request, user=locals()['userId'], priority=1)
buffers = client.call("get_buffers", job_id, wait=True)
results = {}
# original results = {k: decode_buffer(v) for k, v in buffers.items()}
for k, v in buffers.items():
    buf = np.frombuffer(v["data"], dtype=v["dtype"])
    results[k] = buf.reshape(v["shape"])
)#";

  try {
    py::exec(py_src, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC QCS Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }

    auto results = locals["results"];
    py::print(results);

  // Decode the results, update AcceleratorBuffer
  ResultsDecoder().decode(buffer, results, qbitIdxs, shots);
  return;
}

void ResultsDecoder::decode(std::shared_ptr<AcceleratorBuffer> buffer,
                            py::object result,
                            std::set<int> qbitIdxs, int shots) {

  auto as_dict = result.cast<py::dict>();
    
  Eigen::MatrixXi bits = Eigen::MatrixXi::Zero(shots, as_dict.size() / 2);
  int counter = 0;
  for (auto kv : as_dict) {
    py::print(kv.first);
    py::print(kv.second);
    
    for (int k = 0; k < shots; k++) {
      auto bit = kv.second.cast<py::list>()[k].cast<int>(); //.data[k]);
      std::cout << bit << "\n";
      bits(k, counter) = bit;
    }
    counter++;
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