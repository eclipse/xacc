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
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY DIRECT,
 *INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "IBMAccelerator.hpp"
#include "OpenPulseVisitor.hpp"
#include "OpenQasmVisitor.hpp"
#include <cctype>

#include "QObjectExperimentVisitor.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

#include "XACC.hpp"

#include "QObject.hpp"

#include <regex>
#include <thread>

namespace xacc {
namespace quantum {

// curl -X POST
// https://q-console-api.mybluemix.net/api/Network/ibm-q-ornl/Groups/ornl-internal/Projects/algorithms-team/jobs/5c88fc789f1161005df7b68c/cancel?access_token=ZoEqimQMun3F5iRILIoXRVx6DzAq60iXm4offm8UfApeG6PM4DH2kXibEovpkRPD

std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

const std::vector<double> IBMAccelerator::getOneBitErrorRates() {
  return chosenBackend.gateErrors;
}

const std::vector<std::pair<std::pair<int, int>, double>>
IBMAccelerator::getTwoBitErrorRates() {
  // Return list of ((q1,q2),ERROR_RATE)
  std::vector<std::pair<std::pair<int, int>, double>> twobiter;
  for (int i = 0; i < chosenBackend.multiQubitGates.size(); i++) {
    auto mqg = chosenBackend.multiQubitGates[i];
    // boost::replace_all(mqg, "CX", "");
    mqg = std::regex_replace(mqg, std::regex("CX"), "");
    std::vector<std::string> split;
    split = xacc::split(mqg, '_'); // boost::is_any_of("_"));
    twobiter.push_back({{std::stoi(split[0]), std::stoi(split[1])},
                        chosenBackend.multiQubitGateErrors[i]});
  }

  return twobiter;
}
std::shared_ptr<AcceleratorBuffer>
IBMAccelerator::createBuffer(const std::string &varId) {
  if (!isValidBufferSize(30)) {
    xacc::error("Invalid buffer size.");
  }

  std::shared_ptr<AcceleratorBuffer> buffer =
      std::make_shared<AcceleratorBuffer>(varId, 30);

  storeBuffer(varId, buffer);
  return buffer;
}

std::shared_ptr<AcceleratorBuffer>
IBMAccelerator::createBuffer(const std::string &varId, const int size) {
  if (!isValidBufferSize(size)) {
    xacc::error("Invalid buffer size.");
  }

  std::shared_ptr<AcceleratorBuffer> buffer =
      std::make_shared<AcceleratorBuffer>(varId, size);

  storeBuffer(varId, buffer);
  return buffer;
}

bool IBMAccelerator::isValidBufferSize(const int NBits) {
  return NBits > 0 && NBits < 31;
}

std::vector<std::shared_ptr<IRTransformation>>
IBMAccelerator::getIRTransformations() {

  std::vector<std::shared_ptr<IRTransformation>> transformations;
  return transformations;
}

void IBMAccelerator::initialize() {
  if (!initialized) {
    std::string jsonStr = "", apiKey = "";
    searchAPIKey(apiKey, url, hub, group, project);

    std::string getBackendPath = "/api/Backends?access_token=";
    std::string postJobPath = "/api/Jobs?access_token=";
    if (!hub.empty()) {
      getBackendPath = "/api/Network/" + hub + "/Groups/" + group +
                       "/Projects/" + project + "/devices?access_token=";
      postJobPath = "/api/Network/" + hub + "/Groups/" + group + "/Projects/" +
                    project + "/jobs?access_token=";
    }

    std::string tokenParam = "apiToken=" + apiKey;

    std::map<std::string, std::string> headers{
        {"Content-Type", "application/x-www-form-urlencoded"},
        {"Connection", "keep-alive"},
        {"Content-Length", std::to_string(tokenParam.length())}};

    auto response = handleExceptionRestClientPost(
        url, "/api/users/loginWithToken", tokenParam, headers);

    if (response.find("error") != std::string::npos) {
      xacc::error("Error received from IBM\n" + response);
    }

    Document d;
    d.Parse(response);
    currentApiToken = d["id"].GetString();

    response =
        handleExceptionRestClientGet(url, getBackendPath + currentApiToken);

    //   xacc::info("DEVICES: " + response);
    d.Parse(response);

    auto backendArray = d.GetArray();
    for (auto &b : backendArray) {
      IBMBackend backend;
      if (b.HasMember("nQubits")) {
        backend.nQubits = b["nQubits"].GetInt();
      }
      backend.name = b["name"].GetString();
      backend.description =
          b.HasMember("description") ? b["description"].GetString() : "";
      backend.status = !(std::string(b["status"].GetString()).find("off") !=
                         std::string::npos);

      backend.isSimulator = b["simulator"].GetBool();
      if (!backend.isSimulator && b.HasMember("couplingMap") &&
          b["couplingMap"].IsArray()) {
        auto couplers = b["couplingMap"].GetArray();
        for (int j = 0; j < couplers.Size(); j++) {
          backend.couplers.push_back(
              std::make_pair(couplers[j][0].GetInt(), couplers[j][1].GetInt()));
        }

        if (b.HasMember("gateSet")) {
          backend.gateSet = b["gateSet"].GetString();
          backend.basisGates = b["basisGates"].GetString();
        }
      }

      if (!backend.isSimulator && !hub.empty()) {
        std::string getBackendCalibrationPath, getBackendParametersPath;
        if (!hub.empty()) {
          getBackendCalibrationPath =
              "/api/Network/" + hub + "/devices/" + backend.name +
              "/calibration?access_token=" + currentApiToken;
          getBackendParametersPath =
              "/api/Network/" + hub + "/devices/" + backend.name +
              "/parameters?access_token=" + currentApiToken;
        } else {
          getBackendCalibrationPath =
              "/api/Backends/" + backend.name +
              "/calibration?access_token=" + currentApiToken;
          getBackendParametersPath =
              "/api/Backends/" + backend.name +
              "/parameters?access_token=" + currentApiToken;
        }

        auto response =
            handleExceptionRestClientGet(url, getBackendCalibrationPath);
        Document d;
        d.Parse(response);

        if (d.HasMember("qubits") && d.HasMember("multiQubitGates")) {
          auto qubits = d["qubits"].GetArray();
          auto mqGates = d["multiQubitGates"].GetArray();
          for (int i = 0; i < qubits.Size(); i++) {
            const auto &value = qubits[i]["readoutError"]["value"].GetDouble();
            const auto &error = qubits[i]["gateError"]["value"].GetDouble();
            backend.readoutErrors.push_back(value);
            backend.gateErrors.push_back(error);
          }

          for (int i = 0; i < mqGates.Size(); i++) {
            const auto &name = mqGates[i]["name"].GetString();
            const auto &error = mqGates[i]["gateError"]["value"].GetDouble();
            backend.multiQubitGates.push_back(name);
            backend.multiQubitGateErrors.push_back(error);
          }
        }

        response = handleExceptionRestClientGet(url, getBackendParametersPath);
        d.Parse(response);

        if (d.HasMember("qubits")) {
          auto qubits = d["qubits"].GetArray();
          for (int i = 0; i < qubits.Size(); i++) {
            const auto &t1 = qubits[i]["T1"]["value"].GetDouble();
            const auto &t2 = qubits[i]["T2"]["value"].GetDouble();
            const auto &freq = qubits[i]["frequency"]["value"].GetDouble();
            backend.T1s.push_back(t1);
            backend.T2s.push_back(t2);
            backend.frequencies.push_back(freq);
          }
        }
      }

      availableBackends.insert(std::make_pair(backend.name, backend));
    }

    // Set these for RemoteAccelerator.execute
    postPath = postJobPath + currentApiToken;
    remoteUrl = url;
    initialized = true;

    std::string backendName = "ibmq_qasm_simulator";
    if (xacc::optionExists("ibm-backend")) {
      auto newBackend = xacc::getOption("ibm-backend");
      if (availableBackends.find(newBackend) == availableBackends.end()) {
        xacc::error("Invalid IBM Backend string");
      }
      if (!availableBackends[newBackend].status) {
        xacc::error(newBackend + " is currently unavailable, status = off");
      }

      chosenBackend = availableBackends[newBackend];
    }
  }
}

bool IBMAccelerator::isPhysical() {
  std::string backendName = "ibmq_qasm_simulator";
  if (xacc::optionExists("ibm-backend")) {
    auto newBackend = xacc::getOption("ibm-backend");
    if (availableBackends.find(newBackend) == availableBackends.end()) {
      xacc::error("Invalid IBM Backend string");
    }
    backendName = newBackend;
    return !availableBackends[backendName].isSimulator;
  } else {
    return false;
  }
}

/**
 * take ir, generate json post string
 */
const std::string
IBMAccelerator::processInput(std::shared_ptr<AcceleratorBuffer> buffer,
                             std::vector<std::shared_ptr<Function>> functions) {

  // Local Declarations
  std::string backendName = "ibmq_qasm_simulator";
  int shots = 1024;

  // Get the specified backend.
  if (xacc::optionExists("ibm-backend")) {
    auto newBackend = xacc::getOption("ibm-backend");
    if (availableBackends.find(newBackend) == availableBackends.end()) {
      xacc::error("Invalid IBM Backend string");
    }
    if (!availableBackends[newBackend].status) {
      xacc::error(newBackend + " is currently unavailable, status = off");
    }
    backendName = newBackend;
    chosenBackend = availableBackends[newBackend];
  }
  auto connectivity = getAcceleratorConnectivity();

  if (xacc::optionExists("ibm-shots")) {
    shots = std::stoi(xacc::getOption("ibm-shots"));
  }

  // Create a QObj
  xacc::ibm::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("QASM");
  qobj.set_header(QObjectHeader());

  // Create the Experiments
  std::vector<xacc::ibm::Experiment> experiments;
  int maxMemSlots = 0;
  for (auto &kernel : functions) {

    auto uniqueBits = kernel->bits();

    auto visitor =
        std::make_shared<QObjectExperimentVisitor>(kernel->name(), uniqueBits);

    InstructionIterator it(kernel);
    int memSlots = 0;
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled()) {
        nextInst->accept(visitor);
      }
    }

    // After calling getExperiment, maxMemorySlots should be
    // maxClassicalBit + 1
    auto experiment = visitor->getExperiment();
    experiments.push_back(experiment);
    if (visitor->maxMemorySlots > maxMemSlots) {
      maxMemSlots = visitor->maxMemorySlots;
    }

    // Ensure that this Experiment maps onto the
    // hardware connectivity. Before now, we assume
    // any IRTransformations have been run.
    for (auto &inst : experiment.get_instructions()) {
      if (inst.get_name() == "cx") {
        std::pair<int, int> connection{inst.get_qubits()[0],
                                       inst.get_qubits()[1]};
        auto it =
            std::find_if(connectivity.begin(), connectivity.end(),
                         [&connection](const std::pair<int, int> &element) {
                           return element == connection;
                         });
        if (it == std::end(connectivity)) {
          std::stringstream ss;
          ss << "Invalid logical program connectivity, no connection between "
             << inst.get_qubits();
          xacc::error(ss.str());
        }
      }
    }
  }

  // Create the QObj Config
  xacc::ibm::QObjectConfig config;
  config.set_shots(shots);
  config.set_memory(false);
  config.set_meas_level(2);
  config.set_memory_slots(maxMemSlots);
  config.set_meas_return("avg");
  config.set_memory_slot_size(100);
  config.set_n_qubits(chosenBackend.nQubits);

  // Add the experiments and config
  qobj.set_experiments(experiments);
  qobj.set_config(config);

  // Set the Backend
  xacc::ibm::Backend bkend;
  bkend.set_name(backendName);

  // Create the Root node of the QObject
  xacc::ibm::QObjectRoot root;
  root.set_backend(bkend);
  root.set_q_object(qobj);

  // Create the JSON String to send
  nlohmann::json j;
  nlohmann::to_json(j, root);
  auto jsonStr = j.dump();

//   xacc::info("Qobj:\n" + jsonStr);
//   exit(0);

  return jsonStr;
}

/**
 * take response and create
 */
std::vector<std::shared_ptr<AcceleratorBuffer>>
IBMAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                                const std::string &response) {

  if (response.find("error") != std::string::npos) {
    xacc::error(response);
  }

  Document d;
  d.Parse(response);

  std::string jobId = std::string(d["id"].GetString());
  std::string getPath =
      "/api/Jobs/" + jobId + "?access_token=" + currentApiToken;
  std::string getResponse = handleExceptionRestClientGet(url, getPath);

  jobIsRunning = true;
  currentJobId = jobId;

  std::cout << "Current Access Token: " << currentApiToken << "\n";
  std::cout << "Job ID: " << jobId << "\n";
  if (!hub.empty()) {
      std::cout << "\nTo cancel job, open a new terminal and run:\n\n"
                << "curl -X POST " << url << "/api/Network/" << hub
                << "/Groups/" << group << "/Projects/" << project << "/jobs/"
                << jobId << "/cancel?access_token=" << currentApiToken << "\n\n";
  }

  // Loop until the job is complete,
  // get the JSON response
  std::string msg;
  bool jobCompleted = false;
  while (!jobCompleted) {

    getResponse = handleExceptionRestClientGet(url, getPath);
    if (getResponse.find("COMPLETED") != std::string::npos) {
      jobCompleted = true;
    }

    if (getResponse.find("ERROR_RUNNING_JOB") != std::string::npos) {
      xacc::info(getResponse);
      xacc::error("Error encountered running IBM job.");
    }

    Document d;
    d.Parse(getResponse);

    if (d.HasMember("infoQueue") &&
        std::string(d["infoQueue"]["status"].GetString()) != "FINISHED") {
      auto info = d["infoQueue"].GetObject();
      std::cout << "\r"
                << "Job Status: " << d["status"].GetString()
                << ", queue: " << d["infoQueue"]["status"].GetString()
                << std::flush;
      if (info.HasMember("position")) {
        std::cout << " position " << d["infoQueue"]["position"].GetInt()
                  << std::flush;
      }
    } else {
      std::cout << "\r"
                << "Job Status: " << d["status"].GetString() << std::flush;
    }

    if (d["status"] == "CANCELLED") {
        xacc::info("Job Cancelled.");
        xacc::Finalize();
        exit(0);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << std::endl;

  jobIsRunning = false;
  currentJobId = "";

  d.Parse(getResponse);
  xacc::info(getResponse);

  auto &qobjNode = d["qObject"];
  auto &qobjResultNode = d["qObjectResult"];

  StringBuffer sb, sb2;
  Writer<StringBuffer> jsWriter(sb), jsWriter2(sb2);
  qobjResultNode.Accept(jsWriter);
  qobjNode.Accept(jsWriter2);
  std::string qobjResultAsString = sb.GetString();
  std::string qobjAsString = sb2.GetString();

  xacc::ibm::QObjectResult qobjResult;
  xacc::ibm::QObject qobj;
  nlohmann::json j = nlohmann::json::parse(qobjResultAsString);
  nlohmann::from_json(j, qobjResult);
  nlohmann::json j2 = nlohmann::json::parse(qobjAsString);
  nlohmann::from_json(j2, qobj);

  auto resultsArray = qobjResult.get_results();
  auto experiments = qobj.get_experiments();

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;
  if (!chosenBackend.isSimulator) {
    buffer->addExtraInfo("qobject", sb2.GetString());
    buffer->addExtraInfo("gateSet", chosenBackend.gateSet);
    buffer->addExtraInfo("basisGates", chosenBackend.basisGates);
    buffer->addExtraInfo("readoutErrors", chosenBackend.readoutErrors);
    buffer->addExtraInfo("gateErrors", chosenBackend.gateErrors);
    buffer->addExtraInfo("multiQubitGates", chosenBackend.multiQubitGates);
    buffer->addExtraInfo("multiQubitGateErrors",
                         chosenBackend.multiQubitGateErrors);
    buffer->addExtraInfo("frequencies", chosenBackend.frequencies);
    buffer->addExtraInfo("T1", chosenBackend.T1s);
    buffer->addExtraInfo("T2", chosenBackend.T2s);
  }

  for (int i = 0; i < resultsArray.size(); i++) {

    auto currentExperiment = experiments[i];
    auto tmpBuffer =
        createBuffer(currentExperiment.get_header().get_name(), buffer->size());

    auto counts = resultsArray[i].get_data().get_counts();
    for (auto &kv : counts) {

      std::string hexStr = kv.first;
      int nOccurrences = kv.second;

      auto bitStr = hex_string_to_binary_string(hexStr);

    //   xacc::info("IBM Results: " + std::string(hexStr) + ":" +
    //              std::to_string(nOccurrences));

    //   xacc::info("Our Results: " + std::string(bitStr) + ":" +
    //              std::to_string(nOccurrences));

      if (resultsArray.size() == 1) {
        buffer->appendMeasurement(bitStr, nOccurrences);
      } else {
        tmpBuffer->appendMeasurement(bitStr, nOccurrences);
      }
    }

    buffers.push_back(tmpBuffer);
  }

  return buffers;
}

void IBMAccelerator::cancel() {
  if (!hub.empty() && jobIsRunning && !currentJobId.empty()) {
      xacc::info("Canceling IBM Job " + currentJobId);
    std::map<std::string, std::string> headers{
        {"Content-Type", "application/x-www-form-urlencoded"},
        {"Connection", "keep-alive"},
        {"Content-Length", ""}};
    auto path = "/api/Network/" + hub + "/Groups/" + group + "/Projects/" +
                project + "/jobs/" + currentJobId +
                "/cancel?accessToken=" + currentApiToken;
    auto response = handleExceptionRestClientPost(url, path, "", headers);
    xacc::info("CancelResponse: " + response);
    jobIsRunning = false;
    currentJobId = "";
  }
}

std::vector<std::pair<int, int>> IBMAccelerator::getAcceleratorConnectivity() {
  std::string backendName = "ibmq_qasm_simulator";

  if (xacc::optionExists("ibm-backend")) {
    backendName = xacc::getOption("ibm-backend");
  }

  if (!availableBackends.count(backendName)) {
    xacc::error(backendName + " is not available.");
  }

  auto backend = availableBackends[backendName];
  //   auto graph = std::make_shared<AcceleratorGraph>(backend.nQubits);
  std::vector<std::pair<int, int>> graph;
  if (!backend.couplers.empty()) {
    for (auto es : backend.couplers) {
      //   graph->addEdge(es.first, es.second);
      graph.push_back({es.first, es.second});
    }
  } else {
    for (int i = 0; i < backend.nQubits; i++) {
      for (int j = 0; j < backend.nQubits; j++) {
        if (i < j) {
          //   graph->addEdge(i, j);
          graph.push_back({i, j});
        }
      }
    }
  }

  return graph;
}

void IBMAccelerator::searchAPIKey(std::string &key, std::string &url,
                                  std::string &hub, std::string &group,
                                  std::string &project) {

  // Search for the API Key in $HOME/.ibm_config,
  // $IBM_CONFIG, or in the command line argument --ibm-api-key
  std::string ibmConfig(std::string(getenv("HOME")) + "/.ibm_config");
  if (xacc::fileExists(ibmConfig)) {
    findApiKeyInFile(key, url, hub, group, project, ibmConfig);
  } else if (const char *nonStandardPath = getenv("IBM_CONFIG")) {
    std::string nonStandardIbmConfig(nonStandardPath);
    findApiKeyInFile(key, url, hub, group, project, nonStandardIbmConfig);
  } else {

    // Ensure that the user has provided an api-key
    if (!xacc::optionExists("ibm-api-key")) {
      xacc::error("Cannot execute kernel on IBM chip without API Key.");
    }

    // Set the API Key
    key = xacc::getOption("ibm-api-key");

    if (xacc::optionExists("ibm-api-url")) {
      url = xacc::getOption("ibm-api-url");
    }

    if (xacc::optionExists("ibm-api-hub")) {
      hub = xacc::getOption("ibm-api-hub");
    }
    if (xacc::optionExists("ibm-api-group")) {
      group = xacc::getOption("ibm-api-group");
    }
    if (xacc::optionExists("ibm-api-project")) {
      project = xacc::getOption("ibm-api-project");
    }
  }

  // If its still empty, then we have a problem
  if (key.empty()) {
    xacc::error("Error. The API Key is empty. Please place it "
                "in your $HOME/.ibm_config file, $IBM_CONFIG env var, "
                "or provide --ibm-api-key argument.");
  }
}

void IBMAccelerator::findApiKeyInFile(std::string &apiKey, std::string &url,
                                      std::string &hub, std::string &group,
                                      std::string &project,
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
      apiKey = key;
    } else if (l.find("url") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto key = split[1] + ":" + split[2];
      xacc::trim(key);
      url = key;
    } else if (l.find("hub") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto _hub = split[1];
      xacc::trim(_hub);
      hub = _hub;
    } else if (l.find("group") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto _group = split[1];
      xacc::trim(_group);
      group = _group;
    } else if (l.find("project") != std::string::npos) {
      std::vector<std::string> split;
      split = xacc::split(l, ':');
      auto _project = split[1];
      xacc::trim(_project);
      project = _project;
    }
  }
}
} // namespace quantum
} // namespace xacc

//  // Get the runtime options map, and initialize
//   // some basic variables we are going to need
//   std::string backendName = "ibmq_qasm_simulator";
//   std::string shots = "1024";
//   std::map<std::string, std::string> headers;

//     if (xacc::optionExists("ibm-backend")) {
//       auto newBackend = xacc::getOption("ibm-backend");
//       if (availableBackends.find(newBackend) == availableBackends.end()) {
//         xacc::error("Invalid IBM Backend string");
//       }
//       if (!availableBackends[newBackend].status) {
//         xacc::error(newBackend + " is currently unavailable, status = off");
//       }

//       backendName = newBackend;
//       chosenBackend = availableBackends[newBackend];
//     }

//   if (xacc::optionExists("ibm-shots")) {
//     shots = xacc::getOption("ibm-shots");
//   }

//   int nAnalogs = 0;
//   bool isAnalog = false;
//   for (auto &kernel : functions) {
//     if (kernel->isAnalog()) {
//       xacc::info("We have an analog circuit.");
//       isAnalog = true;
//       nAnalogs++;
//     }
//   }

//   if (nAnalogs > 0 && nAnalogs != functions.size()) {
//     xacc::error("Cannot mix OpenQasm and OpenPulse executions. You have " +
//                 std::to_string(functions.size()) + " functions but " +
//                 std::to_string(nAnalogs) + " analog functions.");
//   }

//   std::string jsonStr = "", initStr = "";
//   if (isAnalog) {
//     auto measlofreq =
//         "7.02304866,6.988508651,7.021087557,6.914462388,7.085532103,6."
//         "949279909,7.108946695,6.909172065,6.994924756,6.901232112,7.046906055,"
//         "7.002697717,7.056662542, "
//         "6.944036858,7.102260098,6.942593417,7.087802113,6.89580952,7."
//         "003100376,6.89580523";
//     auto qubitlofreq =
//         "4.920222867865243,4.832046403202443,4.940398096698374,4."
//         "514466758212148,4.662970579288438,4.957220580605086,4.995704948314061,"
//         "4.811225865860375,5.0124078826037755,5.0564924363956045,4."
//         "72033587911043,4.899360137518408,4.772030460533516,5.110023809671274,"
//         "4.990894220665049,4.806558974441183,4.956319934060655,4."
//         "599367389589547,4.827896792802955,4.938182460530852";
//     initStr =
//         "{ \"qObject\": {\"qobj_id\": \"xacc-qobj\", \"schema_version\":
//         \"1.0.0\", \"type\": "
//         "\"PULSE\", \"header\": { \"description\": \"\", \"backend_name\":
//         \"" + backendName + "\" }, \"config\": { \"meas_lo_freq\": [" +
//         measlofreq +
//         "], \"qubit_lo_freq\": [" + qubitlofreq +
//         "], \"rep_time\": 500, \"meas_level\": 1, \"shots\":" + shots +
//         ", \"meas_return\": \"avg\", \"memory_slot_size\": 1, \"seed\": 1, "
//         "\"pulse_library\": [";
//     // add pulse library

//     jsonStr += "] }, \"experiments\": [";
//   } else {
//     jsonStr = "{\"qasms\": [";
//   }

//   int kernelCounter = 0;
//   for (auto &kernel : functions) {
//     // Create the Instruction Visitor that is going
//     // to map our IR to Quil.
//     std::shared_ptr<BaseInstructionVisitor> visitor;
//     if (!isAnalog) {
//       visitor = std::make_shared<OpenQasmVisitor>(buffer->size());
//     } else {
//       visitor = std::make_shared<OpenPulseVisitor>(kernel->name());
//     }

//     // Our QIR is really a tree structure
//     // so create a pre-order tree traversal
//     // InstructionIterator to walk it
//     InstructionIterator it(kernel);
//     measurementSupports.insert(
//         std::make_pair(kernelCounter, std::vector<int>{}));

//     while (it.hasNext()) {
//       // Get the next node in the tree
//       auto nextInst = it.next();
//       if (nextInst->isEnabled()) {
//         nextInst->accept(visitor);
//         if (nextInst->name() == "Measure") {
//           auto qbitIdx = nextInst->bits()[0];
//           measurementSupports[kernelCounter].push_back(qbitIdx);
//         }
//       }
//     }

//     if (isAnalog) {
//       // build up experiments {} json
//       auto expStr = std::dynamic_pointer_cast<OpenPulseVisitor>(visitor)
//                         ->getOpenPulseInstructionsJson();
//       jsonStr += expStr + ",";
//     } else {
//       auto qasmStr = visitor->getNativeAssembly();
//       qasmStr = std::regex_replace(qasmStr, std::regex("\n"), "\\n");
//       jsonStr += "{\"qasm\": \"" + qasmStr + "\"},";
//     }
//     kernelNames.push_back(kernel->name());
//     kernelCounter++;
//   }

//   if (isAnalog) {

//     // Fill out the rest of the jsonStr for OpenPulse
//     // Add everything from the pulseLibrary
//     // FIXME PULSE LIBRARY CACHE AS OPTION, DEFAULT to QCOR
//     auto cache = xacc::getCache("qcor_pulse_library.json");
//     for (auto &kv : cache) {
//       std::stringstream ss;
//       initStr += "{\"name\": \"" + kv.first + "\", \"samples\": ";
//       ss << kv.second.toString();
//       initStr += ss.str() + "},";
//     }
//     jsonStr = initStr.substr(0, initStr.length() - 1) +
//               jsonStr.substr(0, jsonStr.length() - 1) + "]}, \"backend\":
//               {\"name\": \""+backendName+"\"}}";

//   } else {
//     jsonStr = jsonStr.substr(0, jsonStr.size() - 1) + "]";
//     jsonStr += ", \"shots\": " + shots +
//                ", \"maxCredits\": 5, "
//                "\"backend\": {\"name\": \"" +
//                backendName + "\"}}";
//   }
//   // Check that the qpu is online
//   if (backendName != "ibmq_qasm_simulator") {
//     std::string checkBackend = "/api/Backends/" + backendName;
//     if (!hub.empty()) {
//       checkBackend = "/api/Network/ibm-q-ornl/devices/" + backendName +
//                      "?access_token=" + this->currentApiToken;
//     }
//     std::string deviceResponse =
//         handleExceptionRestClientGet(remoteUrl, checkBackend);
//     Document d;
//     d.Parse(deviceResponse);
//     std::string _status = d["status"].GetString();
//     while (_status != "on") {
//       xacc::info(backendName + " is offline. Please wait");
//       std::this_thread::sleep_for(std::chrono::milliseconds(400));
//       deviceResponse = handleExceptionRestClientGet(remoteUrl, checkBackend);
//       d.Parse(deviceResponse);
//       _status = d["status"].GetString();
//     }
//   }

//   return jsonStr;