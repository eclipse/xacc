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
#include "DWAccelerator.hpp"
#include "ParameterSetter.hpp"
#include <fstream>
#include <memory>
#include <thread>
#include "xacc_service.hpp"
#include <bitset>
#include <regex>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;
namespace xacc {

namespace quantum {
void DWAccelerator::initialize(const HeterogeneousMap &params) {
  searchAPIKey(apiKey, url);
  updateConfiguration(params);

  // Set up the extra HTTP headers we are going to need
  headers.insert({"X-Auth-Token", apiKey});
  headers.insert({"Content-type", "application/x-www-form-urlencoded"});
  headers.insert({"Accept", "*/*"});

  auto message =
      handleExceptionRestClientGet(url, "/sapi/solvers/remote", headers);

  Document document;
  document.Parse(message);

  if (document.IsArray()) {
    for (auto i = 0; i < document.Size(); i++) {
      DWSolver solver;
      solver.name = document[i]["id"].GetString();
      xacc::trim(solver.name);
      solver.description = document[i]["description"].GetString();
      if (document[i]["properties"].FindMember("j_range") !=
          document[i]["properties"].MemberEnd()) {
        solver.jRangeMin = document[i]["properties"]["j_range"][0].GetDouble();
        solver.jRangeMax = document[i]["properties"]["j_range"][1].GetDouble();
        solver.hRangeMin = document[i]["properties"]["h_range"][0].GetDouble();
        solver.hRangeMax = document[i]["properties"]["h_range"][1].GetDouble();
      }
      solver.nQubits = document[i]["properties"]["num_qubits"].GetInt();

      // Get the connectivity
      auto couplers = document[i]["properties"]["couplers"].GetArray();
      for (int j = 0; j < couplers.Size(); j++) {
        solver.edges.push_back(
            std::make_pair(couplers[j][0].GetInt(), couplers[j][1].GetInt()));
      }
      availableSolvers.insert(std::make_pair(solver.name, solver));
    }
  }

  remoteUrl = url;
  postPath = "/sapi/problems";
}

const std::string DWAccelerator::processInput(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  if (functions.size() > 1)
    xacc::error("D-Wave Accelerator can only launch one job at a time.");

  auto dwKernel = functions[0];

  auto instructions = dwKernel->getInstructions();
  auto newKernel = std::make_shared<AnnealingProgram>(dwKernel->name());
  // Add the instructions to the Kernel
  for (auto i : instructions) {
    newKernel->addInstruction(i);
  }

  std::vector<std::string> splitLines;
  splitLines = xacc::split(newKernel->toString(), '\n');
  auto nQMILines = splitLines.size();
  std::string jsonStr = "", solverName = backend,
              solveType = "ising", trials = std::to_string(shots),
              annealTime = "20";

  auto solver = availableSolvers[solverName];

  auto program = newKernel->toString();
  program = std::regex_replace(program, std::regex(";"), "");
  jsonStr += "[{ \"solver\" : \"" + solverName + "\", \"type\" : \"" +
             solveType + "\", \"data\" : \"" + std::to_string(solver.nQubits) +
             " " + std::to_string(nQMILines) + "\\n" + program +
             "\", \"params\": { \"num_reads\" : " + trials;
  jsonStr += ", \"auto_scale\" : true } }]";

  jsonStr = std::regex_replace(jsonStr, std::regex("\n"), "\\n");
    std::cout << "sending: " << jsonStr << "\n";
  return jsonStr;
}

void DWAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                                    const std::string &response) {

  bool jobCompleted = false;
  Document doc;
  // Parse the json string
  doc.Parse(response);

    std::cout << response << "\n";
  // Get the JobID
  std::string jobId = std::string(doc[0]["id"].GetString());

  // Loop until the job is complete,
  // get the JSON response
  std::string msg =
      handleExceptionRestClientGet(url, "/sapi/problems/" + jobId, headers);
  while (!jobCompleted) {

    // Execute HTTP Get
    msg = handleExceptionRestClientGet(url, "/sapi/problems/" + jobId, headers);

    // Search the result for the status : COMPLETED indicator
    if (msg.find("COMPLETED") != std::string::npos) {
      jobCompleted = true;
    }

    if (msg.find("FAILED") != std::string::npos) {
      Document d;
      d.Parse(msg);
      xacc::error("D-Wave Execution Failure: " +
                  std::string(d["error_message"].GetString()));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // xacc::info(msg);
  }

  std::cout << msg << "\n";
  // We've completed, so let's get
  // teh results.
  doc.Parse(msg);
  if (doc["status"] == "COMPLETED") {
    std::vector<double> energies;
    std::vector<int> numOccurrences, active_vars;
    auto energyArray = doc["answer"]["energies"].GetArray();
    auto numOccArray = doc["answer"]["num_occurrences"].GetArray();
    auto activeVarsSize = doc["answer"]["active_variables"].GetArray().Size();
    auto activeVars = doc["answer"]["active_variables"].GetArray();
    for (int i = 0; i < activeVarsSize; i++) {
      active_vars.push_back(activeVars[i].GetInt());
    }
    double minEnergy = std::numeric_limits<double>::max();
    int idx = 0;
    for (int i = 0; i < energyArray.Size(); i++) {
      energies.push_back(energyArray[i].GetDouble());
      numOccurrences.push_back(numOccArray[i].GetInt());
      if (energyArray[i].GetDouble() < minEnergy) {
        minEnergy = energyArray[i].GetDouble();
        idx = i;
      }
    }
    auto num_vars = doc["answer"]["num_variables"].GetInt();
    buffer->addExtraInfo("num_vars", num_vars);
    buffer->addExtraInfo("ground_energy", ExtraInfo(minEnergy));
    auto solutionsStrEncoded =
        std::string(doc["answer"]["solutions"].GetString());
    // auto decodedTest = decodeSolutions(solutionsStrEncoded, energyArray.Size(),
    //                                    num_vars, active_vars, -1);
    // auto decoded = base64_decode(solutionsStrEncoded);

    // std::cout << "DECODED SIZE: " << decodedTest.size() << "\n";
    // std::vector<std::vector<char>> bitStrings;
    // for (auto j = 0; j < energies.size(); j++) {
    // //   std::stringstream sss;

    //   auto first = decodedTest.begin() + j * num_vars;
    //   auto last = decodedTest.begin() + j * num_vars + num_vars;
    //   std::vector<char> sub(first, last);
    //   bitStrings.push_back(sub);
    // //   for (int i = 0; i < sub.size(); i++) {
    // //     sss << static_cast<int>(sub[i]);
    // //   }
    // //   auto bs = sss.str();
    // //   buffer->appendMeasurement(bs, numOccurrences[j]);
    // }

    buffer->addExtraInfo("solutions", solutionsStrEncoded);
    // FIXME CHECK WE HAVE total_real_time
    auto &timing = doc["answer"]["timing"];
    if (timing.HasMember("total_real_time")) {
      buffer->addExtraInfo(
          "execution-time",
          ExtraInfo(timing["total_real_time"].GetInt() * 1e-6));
    }

    buffer->addExtraInfo("energies", ExtraInfo(energies));
    buffer->addExtraInfo("num-occurrences", ExtraInfo(numOccurrences));
    buffer->addExtraInfo("active-vars", ExtraInfo(active_vars));

  } else {
    xacc::error("Error in executing D-Wave QPU.");
  }

  return;
}

void DWAccelerator::searchAPIKey(std::string &key, std::string &url) {

  // Search for the API Key in $HOME/.dwave_config,
  // $DWAVE_CONFIG, or in the command line argument --dwave-api-key
  std::string dwaveConfig(std::string(getenv("HOME")) + "/.dwave_config");

  if (xacc::fileExists(dwaveConfig)) {
    findApiKeyInFile(key, url, dwaveConfig);
  } else if (const char *nonStandardPath = getenv("DWAVE_CONFIG")) {
    std::string nonStandardDwaveConfig(nonStandardPath);
    findApiKeyInFile(key, url, nonStandardDwaveConfig);
  } else {

    // Ensure that the user has provided an api-key
    if (!xacc::optionExists("dwave-api-key")) {
      xacc::error("Cannot execute kernel on DW chip without API Key.");
    }

    // Set the API Key
    key = xacc::getOption("dwave-api-key");

    if (xacc::optionExists("dwave-api-url")) {
      url = xacc::getOption("dwave-api-url");
    }
  }

  // If its still empty, then we have a problem
  if (key.empty()) {
    xacc::error("Error. The API Key is empty. Please place it "
                "in your $HOME/.dwave_config file, $DWAVE_CONFIG env var, "
                "or provide --dwave-api-key argument.");
  }
}

void DWAccelerator::findApiKeyInFile(std::string &apiKey, std::string &url,
                                     const std::string &p) {
  std::ifstream stream(p);
  std::string contents((std::istreambuf_iterator<char>(stream)),
                       std::istreambuf_iterator<char>());

  std::vector<std::string> lines;
  lines = xacc::split(contents, '\n');
  for (auto l : lines) {
    if (l.find("key") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      apiKey = key;
    } else if (l.find("url") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto key = split[1] + ":" + split[2];
      xacc::trim(key);
      url = key;
    }
  }
}

std::vector<std::pair<int, int>> DWAccelerator::getConnectivity() {
  std::string solverName = backend;

  if (!availableSolvers.count(solverName)) {
    xacc::error(solverName + " is not available.");
  }

  auto solver = availableSolvers[solverName];

  std::vector<std::pair<int, int>> graph;
  for (auto es : solver.edges) {
    graph.push_back({es.first, es.second});
  }

  return graph;
}
} // namespace quantum
} // namespace xacc
