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
#include "IBMAccelerator.hpp"
#include <cctype>
#include <fstream>

#include "Properties.hpp"
#include "QObjectExperimentVisitor.hpp"
#include "OpenPulseVisitor.hpp"
#include "CountGatesOfTypeVisitor.hpp"

#include <cpr/cpr.h>

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "Pulse.hpp"
#include "Scheduler.hpp"

#include "QObject.hpp"
#include "PulseQObject.hpp"

#include <regex>
#include <thread>

namespace xacc {
namespace quantum {
const std::string IBMAccelerator::IBM_AUTH_URL =
    "https://auth.quantum-computing.ibm.com";
const std::string IBMAccelerator::IBM_API_URL =
    "https://api-qcon.quantum-computing.ibm.com";
const std::string IBMAccelerator::DEFAULT_IBM_BACKEND = "ibmq_qasm_simulator";
const std::string IBMAccelerator::IBM_LOGIN_PATH = "/api/users/loginWithToken";

std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

void IBMAccelerator::initialize(const HeterogeneousMap &params) {
  if (!initialized) {
    std::string apiKey = "";
    std::string getBackendPath = "/api/Backends?access_token=";
    std::string tokenParam = "{\"apiToken\": \"";
    std::string getBackendPropertiesPath;

    // Set backend, shots, etc.
    // and get the apikey, hub, group, and project
    updateConfiguration(params);
    searchAPIKey(apiKey, hub, group, project);

    // We should have backend set by now

    if (!hub.empty()) {
      IBM_CREDENTIALS_PATH =
          "/api/Network/" + hub + "/Groups/" + group + "/Projects/" + project;
      getBackendPath = IBM_CREDENTIALS_PATH + "/devices?access_token=";
      getBackendPropertiesPath =
          "/api/Network/" + hub + "/devices/" + backend + "/properties";
    } else {
      xacc::error(
          "We do not currently support running on the open IBM devices.");
    }

    // Post apiKey to get temp api key
    tokenParam += apiKey + "\"}";
    std::map<std::string, std::string> headers{
        {"Content-Type", "application/json"},
        {"Connection", "keep-alive"},
        {"Content-Length", std::to_string(tokenParam.length())}};
    auto response = post(IBM_AUTH_URL, IBM_LOGIN_PATH, tokenParam, headers);
    auto response_json = json::parse(response);

    // set the temp API token
    currentApiToken = response_json["id"].get<std::string>();

    // Get all backend information
    response = get(IBM_API_URL, getBackendPath + currentApiToken);
    backends_root = json::parse("{\"backends\":" + response + "}");
    getBackendPropsResponse = "{\"backends\":" + response + "}";

    // Get current backend properties
    if (backend != DEFAULT_IBM_BACKEND) {
      auto response = get(IBM_API_URL, getBackendPropertiesPath, {},
                          {std::make_pair("version", "1"),
                           std::make_pair("access_token", currentApiToken)});

      auto props = json::parse(response);
      backendProperties.insert({backend, props});
      for (auto &b : backends_root["backends"]) {
        if (b.count("backend_name") &&
            b["backend_name"].get<std::string>() == backend) {
          availableBackends.insert(std::make_pair(backend, b));
        }
      }

      defaults_response =
          get(IBM_API_URL,
              IBM_CREDENTIALS_PATH + "/devices/" + backend + "/defaults", {},
              {std::make_pair("version", "1"),
               std::make_pair("access_token", currentApiToken)});

    }
    initialized = true;
  }
}

void IBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> circuit) {
  execute(buffer, std::vector<std::shared_ptr<CompositeInstruction>>{circuit});
}

std::string QasmQObjGenerator::getQObjJsonStr(
    std::vector<std::shared_ptr<CompositeInstruction>> circuits,
    const int &shots, const nlohmann::json &backend,
    const std::string getBackendPropsResponse,
    std::vector<std::pair<int, int>> &connectivity) {

  // Create a QObj
  xacc::ibm::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("QASM");
  qobj.set_header(QObjectHeader());

  // Create the Experiments
  std::vector<xacc::ibm::Experiment> experiments;
  int maxMemSlots = 0;
  for (auto &kernel : circuits) {

    auto visitor = std::make_shared<QObjectExperimentVisitor>(
        kernel->name(), backend["n_qubits"].get<int>());

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
  config.set_n_qubits(backend["n_qubits"].get<int>());

  // Add the experiments and config
  qobj.set_experiments(experiments);
  qobj.set_config(config);

  // Set the Backend
  xacc::ibm::Backend bkend;
  bkend.set_name(backend["backend_name"].get<std::string>());

  xacc::ibm::QObjectHeader qobjHeader;
  qobjHeader.set_backend_version("1.0.0");
  qobjHeader.set_backend_name(backend["backend_name"].get<std::string>());
  qobj.set_header(qobjHeader);

  // Create the JSON String to send
  nlohmann::json j;
  nlohmann::to_json(j, qobj);

  return j.dump();
}

std::string PulseQObjGenerator::getQObjJsonStr(
    std::vector<std::shared_ptr<CompositeInstruction>> circuits,
    const int &shots, const nlohmann::json &backend,
    const std::string getBackendPropsResponse,
    std::vector<std::pair<int, int>> &connectivity) {

  xacc::ibm_pulse::PulseQObject root;
  xacc::ibm_pulse::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("PULSE");
  xacc::ibm_pulse::QObjectHeader h;
  h.set_backend_name(backend["backend_name"].get<std::string>());
  h.set_backend_version("1.2.1");
  qobj.set_header(h);

  auto scheduler = xacc::getService<Scheduler>("pulse");

  std::vector<xacc::ibm_pulse::Experiment> experiments;
  // std::vector<xacc::ibm_pulse::PulseLibrary> all_pulses;
  std::map<std::string, xacc::ibm_pulse::PulseLibrary> all_pulses;
  for (auto &kernel : circuits) {

    // Schedule the pulses
    scheduler->schedule(kernel);

    auto visitor = std::make_shared<OpenPulseVisitor>();

    InstructionIterator it(kernel);
    int memSlots = 0;
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled()) {
        nextInst->accept(visitor);
      }
    }

    xacc::ibm_pulse::ExperimentHeader hh;
    hh.set_name(kernel->name());
    hh.set_memory_slots(backend["n_qubits"].get<int>());

    xacc::ibm_pulse::Experiment experiment;
    experiment.set_instructions(visitor->instructions);
    experiment.set_header(hh);
    experiments.push_back(experiment);

    for (auto p : visitor->library) {
      if (!all_pulses.count(p.get_name())) {
        all_pulses.insert({p.get_name(), p});
      }
    }
  }

  qobj.set_experiments(experiments);

  xacc::ibm_pulse::Config config;

  std::vector<xacc::ibm_pulse::PulseLibrary> pulses;
  for (auto &kv : all_pulses) {
    pulses.push_back(kv.second);
  }
  config.set_pulse_library(pulses);
  config.set_meas_level(2);
  config.set_memory_slots(backend["n_qubits"].get<int>());
  config.set_meas_return("avg");
  config.set_rep_time(1000);
  config.set_memory_slot_size(100);
  config.set_memory(false);
  config.set_shots(shots);
  config.set_max_credits(10);

  // auto j = json::parse(getBackendPropsResponse);
  // set meas lo and qubit lo
  std::vector<double> meas_lo_freq, qubit_lo_freq;
  std::vector<std::vector<double>> meas_lo_range =
      backend["meas_lo_range"].get<std::vector<std::vector<double>>>();
  for (auto &pair : meas_lo_range) {
    meas_lo_freq.push_back((pair[0] + pair[1]) / 2.);
  }
  std::vector<std::vector<double>> qubit_lo_range =
      backend["qubit_lo_range"].get<std::vector<std::vector<double>>>();
  for (auto &pair : qubit_lo_range) {
    qubit_lo_freq.push_back((pair[0] + pair[1]) / 2.);
  }
  config.set_meas_lo_freq(meas_lo_freq);
  config.set_qubit_lo_freq(qubit_lo_freq);

  qobj.set_config(config);

  root.set_q_object(qobj);

  xacc::ibm_pulse::Backend b;
  b.set_name(backend["backend_name"].get<std::string>());

  root.set_backend(b);
  root.set_shots(shots);

  nlohmann::json jj;
  nlohmann::to_json(jj, root.get_q_object());
  return jj.dump();
}

void IBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> circuits) {

  // Local Declarations
  std::string backendName = backend;
  chosenBackend = availableBackends[backend];
  auto connectivity = getConnectivity();

  // Figure out if these circuits are pulse-level
  // We assume all circuits in the list are
  // either pulse or qasm
  std::string qobj_type = "qasm";
  for (auto &c : circuits) {
    if (c->isAnalog()) {
      qobj_type = "pulse";
      break;
    }
  }

  // Get the correct QObject Generator
  auto qobjGen = xacc::getService<QObjGenerator>(qobj_type);

  // Generate the QObject JSON
  auto jsonStr = qobjGen->getQObjJsonStr(circuits, shots, chosenBackend,
                                         getBackendPropsResponse, connectivity);

  xacc::info("qobj: " + jsonStr);

  // Now we have JSON QObj, lets start the object upload
  // First reserve the Job on IBM's end
  auto reserve_job =
      std::string("{\"backend\": {\"name\": \"" + backend +
                  "\"}, \"shots\": 1, \"allowObjectStorage\": true}");
  std::map<std::string, std::string> headers{
      {"Content-Type", "application/json"},
      {"Connection", "keep-alive"},
      {"Content-Length", std::to_string(reserve_job.length())}};
  auto reserve_response =
      post(IBM_API_URL,
           IBM_CREDENTIALS_PATH + "/Jobs?access_token=" + currentApiToken,
           reserve_job, headers);
  auto reserve_response_json = json::parse(reserve_response);

  // Job reserved, get the Job ID
  auto job_id = reserve_response_json["id"].get<std::string>();
  currentJobId = job_id;

  buffer->addExtraInfo("ibm-job-id", job_id);

  // Now we ask IBM for an upload URL for the QObj
  auto job_upload_url_response =
      get(IBM_API_URL, IBM_CREDENTIALS_PATH + "/Jobs/" + job_id +
                           "/jobUploadUrl?access_token=" + currentApiToken);
  auto job_upload_url_response_json = json::parse(job_upload_url_response);
  auto upload_url = job_upload_url_response_json["url"];

  // Got the URL, now issue an HTTP Put of the QObj json to that URL
  headers["Content-Length"] = std::to_string(jsonStr.length());
  put(upload_url, jsonStr, headers);

  // Now tell IBM that the data has been uploaded, this
  // kicks off the job (or the queue)
  auto uploaded_response =
      post(IBM_API_URL,
           IBM_CREDENTIALS_PATH + "/Jobs/" + job_id +
               "/jobDataUploaded?access_token=" + currentApiToken,
           "");
  auto uploaded_response_json = json::parse(uploaded_response);

  // Get the Job status for the first time, will likely have
  // just started
  auto get_job_status =
      get(IBM_API_URL, IBM_CREDENTIALS_PATH + "/Jobs/" + job_id +
                           "?access_token=" + currentApiToken);
  auto get_job_status_json = json::parse(get_job_status);
  jobIsRunning = true;

  // Job has started, so watch for status == COMPLETED
  int dots = 1;
  while (true) {
    get_job_status = get(IBM_API_URL, IBM_CREDENTIALS_PATH + "/Jobs/" + job_id +
                                          "?access_token=" + currentApiToken);
    get_job_status_json = json::parse(get_job_status);

    // Bool to indicate if we should print status
    // from infoQueue or just the job status itself
    bool printInfoQueue =
        get_job_status_json.find("infoQueue") != get_job_status_json.end() &&
        get_job_status_json["infoQueue"]["status"].get<std::string>() !=
            "RUNNING";

    // Give the user some feedback on the currently
    // running job
    if (xacc::verbose) {
      if (printInfoQueue) {
        xacc::info(
            "IBM Job " + job_id + " Status: " +
            get_job_status_json["infoQueue"]["status"].get<std::string>());
      } else {
        xacc::info("IBM Job " + job_id + " Status: " +
                   get_job_status_json["status"].get<std::string>());
      }
    } else {
      std::stringstream ss;
      if (printInfoQueue) {
        ss << "\033[0;32m"
           << "IBM Job "
           << "\033[0;36m" << job_id << "\033[0;32m"
           << " Status: "
           << get_job_status_json["infoQueue"]["status"].get<std::string>();
        std::cout << '\r' << std::setw(28) << std::setfill(' ') << ss.str()
                  << std::flush;
      } else {
        if (dots > 4)
          dots = 1;
        ss << "\033[0;32m"
           << "IBM Job "
           << "\033[0;36m" << job_id << "\033[0;32m"
           << " Status: " << get_job_status_json["status"].get<std::string>();
        for (int i = 0; i < dots; i++)
          ss << '.';
        dots++;
        std::cout << '\r' << ss.str() << std::setw(20) << std::setfill(' ')
                  << std::flush;
      }
    }

    if (get_job_status_json["status"].get<std::string>().find("ERROR") !=
        std::string::npos) {
      xacc::error("IBM Job Failed: " + get_job_status_json.dump(4));
    }
    if (get_job_status_json["status"].get<std::string>() == "COMPLETED") {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << "\033[0m"
            << "\n";
  jobIsRunning = false;

  // Job is done, now get the results download URL from IBM
  auto result_download_response = get(
      IBM_API_URL, IBM_CREDENTIALS_PATH + "/Jobs/" + job_id +
                       "/resultDownloadUrl?access_token=" + currentApiToken);
  auto result_download_response_json = json::parse(result_download_response);
  auto download_url = result_download_response_json["url"];

  // Download the results
  auto result_get = get(download_url, "");
  auto results_json = json::parse(result_get);

  xacc::info("Results Json:\n" + results_json.dump());

  // Persist the results to the Buffer(s)
  int counter = 0;
  auto results = results_json["results"];
  for (auto it = results.begin(); it != results.end(); ++it) {
    auto currentExperiment = circuits[counter]->name(); // experiments[counter];
    auto counts = (*it)["data"]["counts"].get<std::map<std::string, int>>();
    auto tmpBuffer =
        std::make_shared<AcceleratorBuffer>(currentExperiment, buffer->size());
    for (auto &kv : counts) {
      std::string hexStr = kv.first;
      int nOccurrences = kv.second;
      auto bitStr = hex_string_to_binary_string(hexStr);

      // Process bitStr to be an n-Measure string in msb
      std::string actual = "";
      CountGatesOfTypeVisitor<Measure> cc(circuits[counter]);
      int nMeasures = cc.countGates();
      for (int i = 0; i < nMeasures; i++)
        actual += "0";
      for (int i = 0; i < nMeasures; i++)
        actual[actual.length() - 1 - i] = bitStr[bitStr.length() - i - 1];

      if (results.size() == 1) {
        buffer->appendMeasurement(actual, nOccurrences);
      } else {
        tmpBuffer->appendMeasurement(actual, nOccurrences);
      }
    }

    if (results.size() > 1) {
      buffer->appendChild(currentExperiment, tmpBuffer);
    }

    counter++;
  }
}

void IBMAccelerator::cancel() {
  xacc::info("Attempting to cancel IBM job " + currentJobId);
  if (!hub.empty() && jobIsRunning && !currentJobId.empty()) {
    xacc::info("Canceling IBM Job " + currentJobId);
    std::map<std::string, std::string> headers{
        {"Content-Type", "application/x-www-form-urlencoded"},
        {"Connection", "keep-alive"},
        {"Content-Length", "0"}};
    auto path = IBM_CREDENTIALS_PATH + "/Jobs/" + currentJobId +
                "/cancel?access_token=" + currentApiToken;
    auto response = post(IBM_API_URL, path, "", headers);
    xacc::info("Cancel Response: " + response);
    jobIsRunning = false;
    currentJobId = "";
  }
}

std::vector<std::pair<int, int>> IBMAccelerator::getConnectivity() {

  if (!availableBackends.count(backend)) {
    xacc::error(backend + " is not available.");
  }

  std::vector<std::pair<int, int>> graph;

  auto backend__ = availableBackends[backend];
  if (backend__.count("coupling_map") && !backend__["coupling_map"].empty()) {
    auto couplers = backend__["coupling_map"];
    for (auto coupler : couplers) {
      auto first = coupler[0].get<int>();
      auto second = coupler[1].get<int>();
      graph.push_back({first, second});
    }
  } else {
    auto nq = backend__["n_qubits"].get<int>();
    for (int i = 0; i < nq; i++) {
      for (int j = 0; j < nq; j++) {
        if (i < j) {
          //   graph->addEdge(i, j);
          graph.push_back({i, j});
        }
      }
    }
  }

  return graph;
}

void IBMAccelerator::searchAPIKey(std::string &key, std::string &hub,
                                  std::string &group, std::string &project) {

  // Search for the API Key in $HOME/.ibm_config,
  // $IBM_CONFIG, or in the command line argument --ibm-api-key
  std::string ibmConfig(std::string(getenv("HOME")) + "/.ibm_config");
  if (xacc::fileExists(ibmConfig)) {
    findApiKeyInFile(key, hub, group, project, ibmConfig);
  } else {
    xacc::error(
        "Cannot find IBM Config file with credentials (~/.ibm_config).");
  }
}

void IBMAccelerator::findApiKeyInFile(std::string &apiKey, std::string &hub,
                                      std::string &group, std::string &project,
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

HeterogeneousMap IBMAccelerator::getProperties() {
  HeterogeneousMap m;

  if (backendProperties.count(backend)) {
    auto props = backendProperties[backend];

    m.insert("total-json", props.dump());
    auto qubit_props = props["qubits"];

    std::vector<double> p01s, p10s;

    for (auto &qp : qubit_props) {
      for (auto &q : qp) {
        if (q["name"].get<std::string>() == "prob_meas0_prep1") {
          p01s.push_back(q["value"].get<double>());
        } else if (q["name"].get<std::string>() == "prob_meas1_prep0") {
        }
        p10s.push_back(q["value"].get<double>());
      }
    }

    m.insert("p01s", p01s);
    m.insert("p10s", p10s);
  }

  return m;
} // namespace quantum

void IBMAccelerator::contributeInstructions(
    const std::string &custom_json_config) {
  auto provider = xacc::getIRProvider("quantum");
  json j;
  if (custom_json_config.empty()) {
    j = json::parse(defaults_response);
  } else {
    j = json::parse(custom_json_config);
  }

  auto pulse_library = j["pulse_library"];
  auto cmd_defs = j["cmd_def"];

  int counter = 0;
  for (auto pulse_iter = pulse_library.begin();
       pulse_iter != pulse_library.end(); ++pulse_iter) {
    auto pulse_name = (*pulse_iter)["name"].get<std::string>();
    // std::cout << counter << ", Pulse: " << pulse_name << "\n";
    auto samples =
        (*pulse_iter)["samples"].get<std::vector<std::vector<double>>>();

    auto pulse = std::make_shared<Pulse>(pulse_name);
    pulse->setSamples(samples);

    xacc::contributeService(pulse_name, pulse);
    counter++;
  }

  // also add frame chagne
  auto fc = std::make_shared<Pulse>("fc");
  xacc::contributeService("fc", fc);
  auto aq = std::make_shared<Pulse>("acquire");
  xacc::contributeService("acquire", aq);

  for (auto cmd_def_iter = cmd_defs.begin(); cmd_def_iter != cmd_defs.end();
       ++cmd_def_iter) {
    auto cmd_def_name = (*cmd_def_iter)["name"].get<std::string>();
    auto qbits = (*cmd_def_iter)["qubits"].get<std::vector<std::size_t>>();

    std::string tmpName;
    if (cmd_def_name == "measure") {
      tmpName = "pulse::" + cmd_def_name;
    } else {
      tmpName = "pulse::" + cmd_def_name + "_" + std::to_string(qbits[0]);
    }

    if (qbits.size() == 2) {
      tmpName += "_" + std::to_string(qbits[1]);
    }
    auto cmd_def = provider->createComposite(tmpName);

    if (cmd_def_name == "u3") {
      cmd_def->addVariables({"P0", "P1", "P2"});
    } else if (cmd_def_name == "u1") {
      cmd_def->addVariables({"P0"});
    } else if (cmd_def_name == "u2") {
      cmd_def->addVariables({"P0", "P1"});
    }

    // std::cout << "CMD_DEF: " << tmpName << "\n";
    auto sequence = (*cmd_def_iter)["sequence"];
    for (auto seq_iter = sequence.begin(); seq_iter != sequence.end();
         ++seq_iter) {
      auto inst_name = (*seq_iter)["name"].get<std::string>();
      auto inst = xacc::getContributedService<Instruction>(inst_name);

      if (inst_name != "acquire") {
        auto channel = (*seq_iter)["ch"].get<std::string>();
        auto t0 = (*seq_iter)["t0"].get<int>();
        inst->setBits(qbits);
        inst->setChannel(channel);
        inst->setStart(t0);

        if ((*seq_iter).find("phase") != (*seq_iter).end()) {
          // we have phase too
          auto p = (*seq_iter)["phase"];
          if (p.is_string()) {
            // this is a variable we have to keep track of
            auto ptmp = p.get<std::string>();
            // get true variable
            ptmp.erase(
                std::remove_if(ptmp.begin(), ptmp.end(),
                               [](char ch) { return ch == '(' || ch == ')'; }),
                ptmp.end());

            InstructionParameter phase(ptmp);
            inst->setParameter(0, phase);

          } else {
            InstructionParameter phase(p.get<double>());
            inst->setParameter(0, phase);
          }
        }
      }
      cmd_def->addInstruction(inst);
    }
    cmd_def->setBits(qbits);

    std::cout << "contributing " << tmpName << "\n";
    xacc::contributeService(tmpName, cmd_def);
  }
  // }
  //   }
}

const std::string RestClient::post(const std::string &remoteUrl,
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

void RestClient::put(const std::string &remoteUrl, const std::string &putStr,
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
    xacc::info("PUT to " + remoteUrl + " with data " + putStr);
  auto r = cpr::Put(cpr::Url{remoteUrl}, cpr::Body(putStr), cprHeaders,
                    cpr::VerifySsl(false));

  if (r.status_code != 200)
    throw std::runtime_error("HTTP POST Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);
  return;
}
const std::string
RestClient::get(const std::string &remoteUrl, const std::string &path,
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

std::string IBMAccelerator::post(const std::string &_url,
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

void IBMAccelerator::put(const std::string &_url, const std::string &postStr,
                         std::map<std::string, std::string> headers) {
  int retries = 10;
  std::exception ex;
  bool succeeded = false;

  // Execute HTTP Post
  do {
    try {
      restClient->put(_url, postStr, headers);
      succeeded = true;
      break;
    } catch (std::exception &e) {
      ex = e;
      xacc::info("Remote Accelerator " + name() +
                 " caught exception while calling restClient->put() "
                 "- " +
                 std::string(e.what()));
      retries--;
      if (retries > 0) {
        xacc::info("Retrying HTTP Put.");
      }
    }
  } while (retries > 0);

  if (!succeeded) {
    cancel();
    xacc::error("Remote Accelerator " + name() +
                " failed HTTP Put for Job Response - " +
                std::string(ex.what()));
  }

  return;
}

std::string
IBMAccelerator::get(const std::string &_url, const std::string &path,
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
} // namespace quantum
} // namespace xacc

// KEEPING THIS PULSE STUFF FOR LATER
// xacc::info("Qobj:\n" + jsonStr);
// exit(0);

// return jsonStr;
//   } else {
//     std::cout << "We are running an OpenPulse job\n";
//     // Create a QObj
//     xacc::ibm_pulse::PulseQObject root;
//     xacc::ibm_pulse::QObject qobj;
//     qobj.set_qobj_id("xacc-qobj-id");
//     qobj.set_schema_version("1.1.0");
//     qobj.set_type("PULSE");
//     xacc::ibm_pulse::QObjectHeader h;
//     h.set_backend_name(backend);
//     h.set_backend_version("1.2.1");
//     qobj.set_header(h);

//     std::vector<xacc::ibm_pulse::Experiment> experiments;
//     // std::vector<xacc::ibm_pulse::PulseLibrary> all_pulses;
//     std::map<std::string, xacc::ibm_pulse::PulseLibrary> all_pulses;
//     for (auto &kernel : circuits) {

//       auto visitor = std::make_shared<OpenPulseVisitor>();

//       InstructionIterator it(kernel);
//       int memSlots = 0;
//       while (it.hasNext()) {
//         auto nextInst = it.next();
//         if (nextInst->isEnabled()) {
//           nextInst->accept(visitor);
//         }
//       }

//       xacc::ibm_pulse::ExperimentHeader hh;
//       hh.set_name(kernel->name());
//       hh.set_memory_slots(
//           chosenBackend.get_specific_configuration().get_n_qubits());

//       xacc::ibm_pulse::Experiment experiment;
//       experiment.set_instructions(visitor->instructions);
//       experiment.set_header(hh);
//       experiments.push_back(experiment);

//       for (auto p : visitor->library) {
//         if (!all_pulses.count(p.get_name())) {
//           all_pulses.insert({p.get_name(), p});
//         }
//       }
//     }

//     qobj.set_experiments(experiments);

//     xacc::ibm_pulse::Config config;

//     std::vector<xacc::ibm_pulse::PulseLibrary> pulses;
//     for (auto &kv : all_pulses) {
//       pulses.push_back(kv.second);
//     }
//     config.set_pulse_library(pulses);
//     config.set_meas_level(2);
//     config.set_memory_slots(
//         chosenBackend.get_specific_configuration().get_n_qubits());
//     config.set_meas_return("avg");
//     config.set_rep_time(1000);
//     config.set_memory_slot_size(100);
//     config.set_memory(false);
//     config.set_shots(shots);
//     config.set_max_credits(10);

//     auto j = json::parse(getBackendPropsResponse);
//     // set meas lo and qubit lo
//     std::vector<double> meas_lo_freq, qubit_lo_freq;
//     std::vector<std::vector<double>> meas_lo_range =
//         *chosenBackend.get_specific_configuration().get_meas_lo_range();
//     for (auto &pair : meas_lo_range) {
//       meas_lo_freq.push_back((pair[0] + pair[1]) / 2.);
//     }
//     std::vector<std::vector<double>> qubit_lo_range =
//         *chosenBackend.get_specific_configuration().get_qubit_lo_range();
//     for (auto &pair : qubit_lo_range) {
//       qubit_lo_freq.push_back((pair[0] + pair[1]) / 2.);
//     }
//     config.set_meas_lo_freq(meas_lo_freq);
//     config.set_qubit_lo_freq(qubit_lo_freq);

//     qobj.set_config(config);

//     root.set_q_object(qobj);

//     xacc::ibm_pulse::Backend b;
//     b.set_name(backend);

//     root.set_backend(b);
//     root.set_shots(shots);

//     nlohmann::json jj;
//     nlohmann::to_json(jj, root);
//     auto jsonStr = jj.dump();

//     std::cout << "JSON:\n" << jsonStr << "\n";
//     exit(0);
//     // return jsonStr;
//   }
