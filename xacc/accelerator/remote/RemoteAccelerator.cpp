/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "RemoteAccelerator.hpp"
#include "xacc.hpp"

#include <cpr/cpr.h>

namespace xacc {

const std::string Client::post(const std::string &remoteUrl,
                               const std::string &path,
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

  auto r = cpr::Post(cpr::Url{remoteUrl + path}, cpr::Body(postStr), cprHeaders,
                     cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP POST Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
}

const std::string Client::get(const std::string &remoteUrl,
                              const std::string &path,
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

  auto r = cpr::Get(cpr::Url{remoteUrl + path}, cprHeaders, cprParams,
                    cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP GET Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
}

void RemoteAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> circuit) {

  auto jsonPostStr = processInput(
      buffer, std::vector<std::shared_ptr<CompositeInstruction>>{circuit});

  auto responseStr =
      handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

  processResponse(buffer, responseStr);

  return;
}

void RemoteAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> circuits) {
  auto jsonPostStr = processInput(buffer, circuits);

  auto responseStr =
      handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

  processResponse(buffer, responseStr);
}

std::string RemoteAccelerator::handleExceptionRestClientPost(
    const std::string &_url, const std::string &path,
    const std::string &postStr, std::map<std::string, std::string> headers) {
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

std::string RemoteAccelerator::handleExceptionRestClientGet(
    const std::string &_url, const std::string &path,
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

} // namespace xacc
