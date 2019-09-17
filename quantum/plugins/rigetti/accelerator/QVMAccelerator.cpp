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
#include "QVMAccelerator.hpp"
#include <algorithm>
#include <typeinfo>
#include <regex>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
using namespace rapidjson;
namespace xacc {
namespace quantum {

const std::string QVMAccelerator::processInput(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  // Get the runtime options map, and initialize
  // some basic variables we are going to need
  std::string type = "multishot";
  std::string jsonStr = "", apiKey = "", userId = "";
  std::string trials = "100", backend = "QVM";
  std::string declareStr = "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]";

  // Search for the API key
//   searchAPIKey(apiKey, userId);

  // Create the Instruction Visitor that is going
  // to map our IR to Quil.
  auto visitor = std::make_shared<QuilVisitor>(true);

  headers.insert(
      std::make_pair("Content-type", "application/json; charset=utf-8"));
  headers.insert(std::make_pair("Connection", "keep-alive"));
  headers.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
  headers.insert(std::make_pair("Accept", "application/octet-stream"));

  remoteUrl = "http://127.0.0.1:5000";
  postPath = "/qvm";

  if (xacc::optionExists("rigetti-backend")) {
    backend = xacc::getOption("rigetti-backend");
  }

  if (xacc::optionExists("rigetti-shots")) {
    trials = xacc::getOption("rigetti-shots");
  }

  if (functions.size() > 1)
    xacc::error("Rigetti QVMAccelerator can only launch one job at a time.");

  InstructionIterator it(functions[0]);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
      if (nextInst->name() == "Measure") {
        currentMeasurementSupports.push_back(nextInst->bits()[0]);
      }
    }
  }

  std::string measuredQubitsString = "[";
  for (auto m : visitor->getMeasuredQubits()) {
    measuredQubitsString += std::to_string(m) + ",";
  }
  measuredQubitsString =
      measuredQubitsString.substr(0, measuredQubitsString.length() - 1) + "]";

  auto quilStr = visitor->getQuilString();
  quilStr = std::regex_replace(quilStr, std::regex("\n"),"\\n");

  // Create the Json String
  jsonStr += "{\"type\": \"" + type + "\", " + "\"addresses\": {\"ro\": " + measuredQubitsString + "}, \"trials\": " + trials + ", \"compiled-quil\": \"" + declareStr + "\\n" + quilStr + "\"}";

  return jsonStr;
}
void
QVMAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                                    const std::string &response) {
//   xacc::info(response);

  Document document;;
  document.Parse(response);
  const Value &results = document["ro"];

  std::map<std::string, int> counts;
  for (SizeType i = 0; i < results.Size(); i++){
    std::string bitString = "";
    for (int i = 0; i < buffer->size(); ++i)
        bitString += "0";
    for (SizeType j = 0; j < results[i].Size(); ++j){
        bitString.replace(buffer->size() - currentMeasurementSupports[j] - 1, 1, std::to_string(results[i][j].GetInt()));
    }
    if (counts.find(bitString) != counts.end()) {
        counts[bitString]++;
    } else {
        counts.insert(std::make_pair(bitString, 1));
      }
  }

  for (auto &kv : counts) {
    buffer->appendMeasurement(kv.first, kv.second);
  }

  currentMeasurementSupports.clear();

  return;
}

} // namespace quantum
} // namespace xacc
