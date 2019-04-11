/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
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

std::shared_ptr<AcceleratorBuffer>
DWAccelerator::createBuffer(const std::string &varId) {
  std::string solverName = "DW_2000Q_VFYC_2_1";
  if (xacc::optionExists("dwave-solver")) {
    solverName = xacc::getOption("dwave-solver");
  }
  if (!availableSolvers.count(solverName)) {
    xacc::error(solverName + " is not available for creating a buffer.");
  }
  auto solver = availableSolvers[solverName];
  auto buffer = std::make_shared<AcceleratorBuffer>(varId, solver.nQubits);
  storeBuffer(varId, buffer);
  return buffer;
}

std::shared_ptr<AcceleratorBuffer>
DWAccelerator::createBuffer(const std::string &varId, const int size) {
  if (!isValidBufferSize(size)) {
    xacc::error("Invalid buffer size.");
  }

  auto buffer = std::make_shared<AcceleratorBuffer>(varId, size);
  storeBuffer(varId, buffer);
  return buffer;
}

bool DWAccelerator::isValidBufferSize(const int NBits) { return NBits > 0; }

void DWAccelerator::initialize() {
  searchAPIKey(apiKey, url);

  if (xacc::optionExists("dwave-skip-initialization")) {
      return;
  }
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

const std::string
DWAccelerator::processInput(std::shared_ptr<AcceleratorBuffer> buffer,
                            std::vector<std::shared_ptr<Function>> functions) {

  if (functions.size() > 1)
    xacc::error("D-Wave Accelerator can only launch one job at a time.");

  auto dwKernel = std::dynamic_pointer_cast<DWFunction>(functions[0]);
  if (!dwKernel) {
    xacc::error("Invalid kernel.");
  }

  //   auto aqcBuffer = std::dynamic_pointer_cast<AQCAcceleratorBuffer>(buffer);
  //   if (!aqcBuffer) {
  //     xacc::error("Invalid AcceleratorBuffer passed to DW Accelerator. Must
  //     be "
  //                 "an AQCAcceleratorBuffer.");
  //   }

  auto tmpembedding = mpark::get<std::map<int, std::vector<int>>>(
      buffer->getInformation("embedding"));
  Embedding embedding;
  for (auto &kv : tmpembedding) {
    embedding.insert({kv.first, kv.second});
  }

  // Get the ParameterSetter
  std::shared_ptr<ParameterSetter> parameterSetter;
  if (xacc::optionExists("dwave-parameter-setter")) {
    parameterSetter = xacc::getService<ParameterSetter>(
        xacc::getOption("dwave-parameter-setter"));
  } else {
    parameterSetter = xacc::getService<ParameterSetter>("default");
  }

  // Get the maximum qubit index
  auto instructions = dwKernel->getInstructions();
  int maxBitIdx = 0;
  for (auto inst : instructions) {
    if (inst->name() == "dw-qmi") {
      auto qbit1 = inst->bits()[0];
      auto qbit2 = inst->bits()[1];
      if (qbit1 > maxBitIdx)
        maxBitIdx = qbit1;
      if (qbit2 > maxBitIdx)
        maxBitIdx = qbit2;
    }
  }

  // Reconstruct the Problem Graph
  std::shared_ptr<Anneal> annealingSchedule;
  auto hardwareconnections = getAcceleratorConnectivity();
        std::set<int> nUniqueBits;
        for (auto& edge : hardwareconnections) {
            nUniqueBits.insert(edge.first);
            nUniqueBits.insert(edge.second);
        }

        int nBits = *std::max_element(nUniqueBits.begin(), nUniqueBits.end()) + 1;

       auto hardwareGraph = xacc::getService<Graph>("boost-ugraph");
        for (int i = 0; i < nBits; i++) {
            std::map<std::string,InstructionParameter> m{{"bias",1.0}};
            hardwareGraph->addVertex(m);
        }
        for (auto& edge : hardwareconnections) {
            hardwareGraph->addEdge(edge.first, edge.second);
        }

//   auto problemGraph = std::make_shared<DWGraph>(maxBitIdx + 1);
  auto problemGraph = xacc::getService<Graph>("boost-ugraph");
        for (int i = 0; i < maxBitIdx+1;i++) {
            std::map<std::string,InstructionParameter> m{{"bias",1.0}};
            problemGraph->addVertex(m);
        }
  for (auto inst : instructions) {
    if (inst->name() == "dw-qmi") {
      auto qbit1 = inst->bits()[0];
      auto qbit2 = inst->bits()[1];
      double weightOrBias = mpark::get<double>(inst->getParameter(0));
      if (qbit1 == qbit2) {
          auto p = inst->getParameter(0);
        problemGraph->setVertexProperty(qbit1, "bias", p);
      } else {
        problemGraph->addEdge(qbit1, qbit2, weightOrBias);
      }
    } else if (inst->name() == "anneal") {
      // get annealing schedule
      annealingSchedule = std::make_shared<Anneal>(inst->getParameters());
    }
  }
  // Set the parameters with the problem graph, the hardware graph, and
  // embedding
  auto insts =
      parameterSetter->setParameters(problemGraph, hardwareGraph, embedding);
  auto newKernel = std::make_shared<DWFunction>(dwKernel->name());
  // Add the instructions to the Kernel
  for (auto i : insts) {
    newKernel->addInstruction(i);
  }

  std::vector<std::string> splitLines;
  splitLines = xacc::split( newKernel->toString(""),'\n');
  auto nQMILines = splitLines.size();
  std::string jsonStr = "", solverName = "DW_2000Q_VFYC_2_1", solveType = "ising",
              trials = "100", annealTime = "20";

  if (xacc::optionExists("dwave-solver")) {
    solverName = xacc::getOption("dwave-solver");
  }

  if (!availableSolvers.count(solverName)) {
    xacc::error(solverName + " is not available.");
  }

  if (xacc::optionExists("dwave-solve-type")) {
    solveType = xacc::getOption("dwave-solve-type");
  }

  auto solver = availableSolvers[solverName];

  if (xacc::optionExists("dwave-anneal-time")) {
    annealTime = xacc::getOption("dwave-anneal-time");
  }

  std::vector<std::pair<double, double>> as;
  std::string annealingStr = "";
  AnnealScheduleGenerator gen;
  double s = 1;
  if (annealingSchedule) {
    as = gen.generate(annealingSchedule);
  } else {
    as.push_back({0, 0});
    as.push_back({std::stod(annealTime), s});
  }

  annealingStr = gen.getAsString(as);
  xacc::info("Annealing Schedule: " + annealingStr);

  if (xacc::optionExists("dwave-num-reads")) {
    trials = xacc::getOption("dwave-num-reads");
  }

  auto program = newKernel->toString("");
  program = std::regex_replace(program, std::regex(";"), "");
  jsonStr += "[{ \"solver\" : \"" + solverName + "\", \"type\" : \"" +
             solveType + "\", \"data\" : \"" + std::to_string(solver.nQubits) +
             " " + std::to_string(nQMILines) + "\\n" + program +
             "\", \"params\": { \"num_reads\" : " + trials;
  if (solverName.find("c4-sw") == std::string::npos) {
    jsonStr += ", \"anneal_schedule\" : " + annealingStr;
  }
  jsonStr += ", \"auto_scale\" : true } }]";

  jsonStr = std::regex_replace(jsonStr, std::regex("\n"),"\\n");
  return jsonStr;
}

std::vector<std::shared_ptr<AcceleratorBuffer>>
DWAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                               const std::string &response) {

  //   auto aqcBuffer = std::dynamic_pointer_cast<AQCAcceleratorBuffer>(buffer);

  bool jobCompleted = false;
  Document doc;
  // Parse the json string
  doc.Parse(response);

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
    xacc::info(msg);
  }

  // We've completed, so let's get
  // teh results.
  doc.Parse(msg);
  if (doc["status"] == "COMPLETED") {
    std::vector<double> energies;
    std::vector<int> numOccurrences, active_vars;
    auto energyArray = doc["answer"]["energies"].GetArray();
    auto numOccArray = doc["answer"]["num_occurrences"].GetArray();
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

    buffer->addExtraInfo("ground_energy", ExtraInfo(minEnergy));
    auto solutionsStrEncoded =
        std::string(doc["answer"]["solutions"].GetString());
    auto decoded = base64_decode(solutionsStrEncoded);
    // xacc::info("DECODED: " + decoded);
    std::string bitStr = "";
    std::stringstream ss;
    for (std::size_t i = 0; i < decoded.size(); ++i) {
      ss << std::bitset<8>(decoded.c_str()[i]);
    }

    bitStr = ss.str();

    auto activeVarsSize = doc["answer"]["active_variables"].GetArray().Size();
    auto activeVars = doc["answer"]["active_variables"].GetArray();
    for (int i = 0; i < activeVarsSize; i++) {
      active_vars.push_back(activeVars[i].GetInt());
    }

    int start = 0;
    for (auto &count : numOccurrences) {
      auto solution = bitStr.substr(start, activeVarsSize);
      buffer->appendMeasurement(solution, count);
      if (start == 0) {
          buffer->addExtraInfo("ground_state", ExtraInfo(solution));
      }
      start += activeVarsSize;
    }

    // FIXME CHECK WE HAVE total_real_time
    auto& timing = doc["answer"]["timing"];
    if (timing.HasMember("total_real_time")) {
        buffer->addExtraInfo("execution-time", ExtraInfo(timing["total_real_time"].GetInt() * 1e-6));
    }

    buffer->addExtraInfo("energies", ExtraInfo(energies));
    buffer->addExtraInfo("num-occurrences", ExtraInfo(numOccurrences));
    buffer->addExtraInfo("active-vars", ExtraInfo(active_vars));

  } else {
    xacc::error("Error in executing D-Wave QPU.");
  }

  return std::vector<std::shared_ptr<AcceleratorBuffer>>{buffer};
}

void DWAccelerator::searchAPIKey(std::string &key, std::string &url) {

  // Search for the API Key in $HOME/.dwave_config,
  // $DWAVE_CONFIG, or in the command line argument --dwave-api-key
  std::string dwaveConfig(std::string(getenv("HOME")) +
                                      "/.dwave_config");

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
  lines = xacc::split(contents,'\n');
  for (auto l : lines) {
    if (l.find("key") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto key = split[1];
      xacc::trim(key);
      apiKey = key;
    } else if (l.find("url") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split( l, ':');
      auto key = split[1] + ":" + split[2];
      xacc::trim(key);
      url = key;
    }
  }
}

/**
 * Return the graph structure for this Accelerator.
 *
 * @return connectivityGraph The graph structure of this Accelerator
 */
std::vector<std::pair<int,int>> DWAccelerator::getAcceleratorConnectivity() {
  std::string solverName = "DW_2000Q_VFYC_2_1";

  if (xacc::optionExists("dwave-solver")) {
    solverName = xacc::getOption("dwave-solver");
  }

  if (!availableSolvers.count(solverName)) {
    xacc::error(solverName + " is not available.");
  }

  auto solver = availableSolvers[solverName];

  std::vector<std::pair<int,int>> graph;
  for (auto es : solver.edges) {
    graph.push_back({es.first,es.second});
  }

  return graph;
}
} // namespace quantum
} // namespace xacc
