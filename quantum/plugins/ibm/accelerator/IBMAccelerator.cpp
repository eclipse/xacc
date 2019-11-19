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

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "Pulse.hpp"

#include "QObject.hpp"
#include "PulseQObject.hpp"

#include <regex>
#include <thread>

namespace xacc {
namespace quantum {

// keeping this here for later...
// curl -X POST
// https://q-console-api.mybluemix.net/api/Network/ibm-q-ornl/Groups/ornl-internal/Projects/algorithms-team/jobs/JOBID/cancel?access_token=TEMPTOKEN

std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

HeterogeneousMap IBMAccelerator::getProperties() {
  HeterogeneousMap m;

  if (backendProperties.count(backend)) {
    auto props = backendProperties[backend];
    json jj;
    to_json(jj, props);
    m.insert("total-json", jj.dump());
    auto qubit_props = props.get_qubits();
    std::vector<double> p01s, p10s;
    for (auto &qp : qubit_props) {
      for (auto &q : qp) {
        if (q.get_name() == xacc::ibm_properties::Name::PROB_MEAS0_PREP1) {
          p01s.push_back(q.get_value());
        } else if (q.get_name() ==
                   xacc::ibm_properties::Name::PROB_MEAS1_PREP0) {
          p10s.push_back(q.get_value());
        }
      }
    }
    m.insert("p01s", p01s);
    m.insert("p10s", p10s);
  }

  return m;
}

void IBMAccelerator::contributeInstructions(
    const std::string &custom_json_config) {
  auto provider = xacc::getIRProvider("quantum");
  json j;
  if (custom_json_config.empty()) {
    j = json::parse(getBackendPropsResponse);
  } else {
    j = json::parse(custom_json_config);
  }

  auto backends = j["backends"];
  for (auto it = backends.begin(); it != backends.end(); ++it) {
    auto t = (*it)["name"].get<std::string>();
    if ((*it)["name"].get<std::string>() == backend) {
        // std::cout << "Contributing analog instructions from " << backend <<
        // "\n";

      auto pulse_library =
          (*it)["specificConfiguration"]["defaults"]["pulse_library"];
      auto cmd_defs = (*it)["specificConfiguration"]["defaults"]["cmd_def"];

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
                ptmp.erase(std::remove_if(
                               ptmp.begin(), ptmp.end(),
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

        xacc::contributeService(tmpName, cmd_def);
      }
    }
  }
}

void IBMAccelerator::initialize(const HeterogeneousMap &params) {
  if (!initialized) {
    updateConfiguration(params);
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

    auto j = json::parse("{\"backends\":" + response + "}");
    from_json(j, backends_root);
    getBackendPropsResponse = "{\"backends\":" + response + "}";

    for (auto &b : backends_root.get_backends()) {

      // If not simulator, store the properties
      if (!b.get_specific_configuration().get_simulator()) {
        std::string getBackendPropertiesPath;
        if (!hub.empty()) {
          getBackendPropertiesPath = "/api/Network/" + hub + "/devices/" +
                                     b.get_name() + "/properties";
        } else {
          getBackendPropertiesPath =
              "/api/Backends/" + b.get_name() + "/properties";
        }

        auto response = handleExceptionRestClientGet(
            url, getBackendPropertiesPath, {},
            {std::make_pair("version", "1"),
             std::make_pair("access_token", currentApiToken)});

        xacc::ibm_properties::Properties props;
        auto j = json::parse(response);
        from_json(j, props);

        backendProperties.insert({b.get_name(), props});
      }

      availableBackends.insert(std::make_pair(b.get_name(), b));
    }

    // Set these for RemoteAccelerator.execute
    postPath = postJobPath + currentApiToken;
    remoteUrl = url;
    initialized = true;
    chosenBackend = availableBackends[backend];
  }
}

const std::string IBMAccelerator::processInput(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  // Local Declarations
  std::string backendName = backend;
  chosenBackend = availableBackends[backend];

  auto connectivity = getConnectivity();

  bool isAnalog = false;
  for (auto &k : functions) {
    if (k->isAnalog()) {
      isAnalog = true;
      break;
    }
  }

  if (!isAnalog) {
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

      auto visitor = std::make_shared<QObjectExperimentVisitor>(
          kernel->name(),
          chosenBackend.get_specific_configuration().get_n_qubits());

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
    config.set_n_qubits(
        chosenBackend.get_specific_configuration().get_n_qubits());

    // Add the experiments and config
    qobj.set_experiments(experiments);
    qobj.set_config(config);

    // Set the Backend
    xacc::ibm::Backend bkend;
    bkend.set_name(backendName);

    xacc::ibm::QObjectHeader qobjHeader;
    qobjHeader.set_backend_version("1.0.0");
    qobjHeader.set_backend_name(backend);
    qobj.set_header(qobjHeader);

    // Create the Root node of the QObject
    xacc::ibm::QObjectRoot root;
    root.set_backend(bkend);
    root.set_q_object(qobj);
    root.set_shots(shots);

    // Create the JSON String to send
    nlohmann::json j;
    nlohmann::to_json(j, root);
    auto jsonStr = j.dump();

    // xacc::info("Qobj:\n" + jsonStr);
    // exit(0);

    return jsonStr;
  } else {
    std::cout << "We are running an OpenPulse job\n";
    // Create a QObj
    xacc::ibm_pulse::PulseQObject root;
    xacc::ibm_pulse::QObject qobj;
    qobj.set_qobj_id("xacc-qobj-id");
    qobj.set_schema_version("1.1.0");
    qobj.set_type("PULSE");
    xacc::ibm_pulse::QObjectHeader h;
    h.set_backend_name(backend);
    h.set_backend_version("1.2.1");
    qobj.set_header(h);

    std::vector<xacc::ibm_pulse::Experiment> experiments;
    // std::vector<xacc::ibm_pulse::PulseLibrary> all_pulses;
    std::map<std::string, xacc::ibm_pulse::PulseLibrary> all_pulses;
    for (auto &kernel : functions) {

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
      hh.set_memory_slots(
          chosenBackend.get_specific_configuration().get_n_qubits());

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
    config.set_memory_slots(
        chosenBackend.get_specific_configuration().get_n_qubits());
    config.set_meas_return("avg");
    config.set_rep_time(1000);
    config.set_memory_slot_size(100);
    config.set_memory(false);
    config.set_shots(shots);
    config.set_max_credits(10);

    auto j = json::parse(getBackendPropsResponse);
    // set meas lo and qubit lo
    std::vector<double> meas_lo_freq, qubit_lo_freq;
    std::vector<std::vector<double>> meas_lo_range =
        *chosenBackend.get_specific_configuration().get_meas_lo_range();
    for (auto &pair : meas_lo_range) {
      meas_lo_freq.push_back((pair[0] + pair[1]) / 2.);
    }
    std::vector<std::vector<double>> qubit_lo_range =
        *chosenBackend.get_specific_configuration().get_qubit_lo_range();
    for (auto &pair : qubit_lo_range) {
      qubit_lo_freq.push_back((pair[0] + pair[1]) / 2.);
    }
    config.set_meas_lo_freq(meas_lo_freq);
    config.set_qubit_lo_freq(qubit_lo_freq);

    qobj.set_config(config);

    root.set_q_object(qobj);

    xacc::ibm_pulse::Backend b;
    b.set_name(backend);

    root.set_backend(b);
    root.set_shots(shots);

    nlohmann::json jj;
    nlohmann::to_json(jj, root);
    auto jsonStr = jj.dump();

    std::cout << "JSON:\n" << jsonStr << "\n";
    exit(0);
    return jsonStr;
  }
}

/**
 * take response and create
 */
void IBMAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
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
              << "curl -X POST " << url << "/api/Network/" << hub << "/Groups/"
              << group << "/Projects/" << project << "/jobs/" << jobId
              << "/cancel?access_token=" << currentApiToken
              << "\n\n-or-\n\nCTRL-C\n\n";
  }

  // Loop until the job is complete,
  // get the JSON response
  std::string msg;
  bool jobCompleted = false;
  while (!jobCompleted) {

    getResponse = handleExceptionRestClientGet(url, getPath);
    // auto jj = json::parse(getResponse);

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

    std::cout << "JOBRESPONSE:\n" << getResponse << "\n";
  jobIsRunning = false;
  currentJobId = "";

  d.Parse(getResponse);

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
  if (!chosenBackend.get_specific_configuration().get_simulator()) {
    // buffer->addExtraInfo("qobject", sb2.GetString());
    // buffer->addExtraInfo("gateSet", chosenBackend.gateSet);
    // buffer->addExtraInfo("basisGates", chosenBackend.basisGates);
    // buffer->addExtraInfo("readoutErrors", chosenBackend.readoutErrors);
    // buffer->addExtraInfo("gateErrors", chosenBackend.gateErrors);
    // buffer->addExtraInfo("multiQubitGates", chosenBackend.multiQubitGates);
    // buffer->addExtraInfo("multiQubitGateErrors",
    //                      chosenBackend.multiQubitGateErrors);
    // buffer->addExtraInfo("frequencies", chosenBackend.frequencies);
    // buffer->addExtraInfo("T1", chosenBackend.T1s);
    // buffer->addExtraInfo("T2", chosenBackend.T2s);
  }

  for (int i = 0; i < resultsArray.size(); i++) {
    auto currentExperiment = experiments[i];
    auto tmpBuffer = std::make_shared<AcceleratorBuffer>(
        currentExperiment.get_header().get_name(), buffer->size());
    auto counts = resultsArray[i].get_data().get_counts();
    for (auto &kv : counts) {

      std::string hexStr = kv.first;
      int nOccurrences = kv.second;
      auto bitStr = hex_string_to_binary_string(hexStr);

      if (resultsArray.size() == 1) {
        buffer->appendMeasurement(bitStr, nOccurrences);
      } else {
        tmpBuffer->appendMeasurement(bitStr, nOccurrences);
        buffer->appendChild(currentExperiment.get_header().get_name(),
                            tmpBuffer);
      }
    }
  }

  return;
}

void IBMAccelerator::cancel() {
  xacc::info("Attempting to cancel this job, " + currentJobId);
  if (!hub.empty() && jobIsRunning && !currentJobId.empty()) {
    xacc::info("Canceling IBM Job " + currentJobId);
    std::map<std::string, std::string> headers{
        {"Content-Type", "application/x-www-form-urlencoded"},
        {"Connection", "keep-alive"},
        {"Content-Length", "0"}};
    auto path = "/api/Network/" + hub + "/Groups/" + group + "/Projects/" +
                project + "/jobs/" + currentJobId +
                "/cancel?access_token=" + currentApiToken;
    auto response = handleExceptionRestClientPost(url, path, "", headers);
    xacc::info("Cancel Response: " + response);
    jobIsRunning = false;
    currentJobId = "";
  }
}

std::vector<std::pair<int, int>> IBMAccelerator::getConnectivity() {
  //   std::string backendName = "ibmq_qasm_simulator";

  //   if (xacc::optionExists("ibm-backend")) {
  //     backendName = xacc::getOption("ibm-backend");
  //   }

  if (!availableBackends.count(backend)) {
    xacc::error(backend + " is not available.");
  }

  auto backend__ = availableBackends[backend];
  //   auto graph = std::make_shared<AcceleratorGraph>(backend.nQubits);
  std::vector<std::pair<int, int>> graph;
  auto couplers = backend__.get_specific_configuration().get_coupling_map();
  auto nq = backend__.get_specific_configuration().get_n_qubits();

  if (!couplers->empty()) {
    for (int i = 0; i < couplers->size(); i++) {
      //   graph->addEdge(es.first, es.second);
      auto first = couplers->operator[](i)[0];
      auto second = couplers->operator[](i)[1];
      graph.push_back({first, second});
    }
  } else {
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
