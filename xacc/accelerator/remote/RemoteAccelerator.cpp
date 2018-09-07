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

  xacc::info("Posting to " + remoteUrl + path);
  xacc::info("Body = " + postStr);
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

  xacc::info("Getting " + remoteUrl + path);
  auto r =
      cpr::Get(cpr::Url{remoteUrl + path}, cprHeaders, cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP GET Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
}

void RemoteAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                                const std::shared_ptr<Function> function) {

  auto jsonPostStr =
      processInput(buffer, std::vector<std::shared_ptr<Function>>{function});

  auto responseStr =
      handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

  processResponse(buffer, responseStr);

  return;
}

std::vector<std::shared_ptr<AcceleratorBuffer>> RemoteAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<Function>> functions) {
  auto jsonPostStr = processInput(buffer, functions);

  auto responseStr =
      handleExceptionRestClientPost(remoteUrl, postPath, jsonPostStr, headers);

  return processResponse(buffer, responseStr);
}

} // namespace xacc
