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
#include <cassert>
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

std::vector<xacc::ibm_pulse::Instruction> alignMeasurePulseInstructions(
    const std::vector<xacc::ibm_pulse::Instruction> &in_originalPulseSchedule) {
  std::vector<xacc::ibm_pulse::Instruction> result;
  std::vector<xacc::ibm_pulse::Instruction> acquireInsts;
  // Align stimulus measure pulses and combine acquire instructions.
  // IBM can only handle 1 acquire instruction at the momemt.
  std::optional<int> firstMeasureT0;
  // A measurement pulse can contain multiple pulses!!!
  std::string firstMeasChannel;
  std::unordered_map<std::string, int> mChannelOffset;
  for (const auto &ibmInst : in_originalPulseSchedule) {
    if (ibmInst.get_name() != "acquire") {
      if (!ibmInst.get_ch().empty() && ibmInst.get_ch()[0] == 'm') {
        if (!firstMeasureT0.has_value()) {
          firstMeasureT0 = ibmInst.get_t0();
          firstMeasChannel = ibmInst.get_ch();
          // This is the first one, hence offset = 0
          mChannelOffset.emplace(ibmInst.get_ch(), 0);
        }

        auto offsetIter = mChannelOffset.find(ibmInst.get_ch());
        if (offsetIter == mChannelOffset.end()) {
          const auto offset = ibmInst.get_t0() - firstMeasureT0.value();
          mChannelOffset.emplace(ibmInst.get_ch(), offset);
        }
        const auto t0_offset = mChannelOffset[ibmInst.get_ch()];
        auto alignedMeasPulse = ibmInst;
        // A measure composite is a set of pulses on the meas channel,
        // we need to shift all of them according to the offset.
        if (ibmInst.get_ch() != firstMeasChannel) {
          const auto shiftT0 = ibmInst.get_t0() - t0_offset;
          alignedMeasPulse.set_t0(shiftT0);
        }
        result.emplace_back(alignedMeasPulse);
      } else {
        result.emplace_back(ibmInst);
      }
    } else {
      acquireInsts.emplace_back(ibmInst);
    }
  }

  std::vector<int64_t> acquiredBits;
  for (const auto &aqInst : acquireInsts) {
    assert(aqInst.get_qubits().size() == 1);
    if (!xacc::container::contains(acquiredBits, aqInst.get_qubits()[0])) {
      acquiredBits.emplace_back(aqInst.get_qubits()[0]);
    }
  }

  if (!acquiredBits.empty()) {
    auto mergedAcquire = acquireInsts.front();
    mergedAcquire.set_qubits(acquiredBits);
    // Use the same logic as QASM-based measurements:
    // i.e. use memory slots from 0 onward.
    // This is important for the AcceleratorBuffer to summarize the bit count
    // later.
    // e.g. when only measure qubit 1 (acquire pulse), we need to map the result
    // to memory slot 0, not 1.
    std::vector<int64_t> memory_slots(acquiredBits.size());
    std::iota(std::begin(memory_slots), std::end(memory_slots), 0);
    mergedAcquire.set_memory_slot(memory_slots);
    result.emplace_back(mergedAcquire);
  }

  return result;
}

std::vector<xacc::ibm_pulse::Instruction> orderFrameChangeInsts(
    const std::vector<xacc::ibm_pulse::Instruction> &in_originalPulseSchedule) {
  std::vector<xacc::ibm_pulse::Instruction> result;
  std::vector<xacc::ibm_pulse::Instruction> fcInsts;

  const auto sortFcInst =
      [](std::vector<xacc::ibm_pulse::Instruction> &io_inst) {
        for (size_t i = 1; i < io_inst.size(); ++i) {
          assert(io_inst[i].get_t0() == io_inst[0].get_t0());
        }
        std::sort(io_inst.begin(), io_inst.end(),
                  [](const auto &lhs, const auto &rhs) {
                    return lhs.get_ch() < rhs.get_ch();
                  });
      };

  for (const auto &ibmInst : in_originalPulseSchedule) {
    if (ibmInst.get_name() == "fc") {
      if (fcInsts.empty()) {
        fcInsts.emplace_back(ibmInst);
      } else {
        if (ibmInst.get_t0() == fcInsts.back().get_t0()) {
          fcInsts.emplace_back(ibmInst);
        } else {
          // Sort the list and add
          sortFcInst(fcInsts);
          for (auto &fcInst : fcInsts) {
            result.emplace_back(fcInst);
          }
          fcInsts.clear();
          fcInsts.emplace_back(ibmInst);
        }
      }
    } else {
      if (!fcInsts.empty()) {
        sortFcInst(fcInsts);
        for (auto &fcInst : fcInsts) {
          result.emplace_back(fcInst);
        }
        fcInsts.clear();
      }
      result.emplace_back(ibmInst);
    }
  }
  assert(result.size() == in_originalPulseSchedule.size());
  return result;
}

bool hasMidCircuitMeasurement(
    const std::shared_ptr<CompositeInstruction> &in_circuit) {
  InstructionIterator it(in_circuit);
  bool measureEncountered = false;
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      if (nextInst->name() == "Measure") {
        // Flag that we have seen a Measure gate.
        measureEncountered = true;
      }

      // We have seen a Measure gate but this one is not another Measure gate.
      if (measureEncountered && nextInst->name() != "Measure") {
        // This circuit has mid-circuit measurement
        return true;
      }
    }
  }
  return false;
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

    if (hub.empty() && group.empty() && project.empty()) {
      // Fallback to public API credentials
      hub = "ibm-q";
      group = "open";
      project = "main";
    }

    IBM_CREDENTIALS_PATH =
        "/api/Network/" + hub + "/Groups/" + group + "/Projects/" + project;
    getBackendPath = IBM_CREDENTIALS_PATH + "/devices?access_token=";
    getBackendPropertiesPath = "/api/Network/" + hub + "/Groups/" + group +
                               "/Projects/" + project + "/devices/" + backend +
                               "/properties";

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
    auto backend_props_response =
        get(IBM_API_URL, getBackendPropertiesPath, {},
            {std::make_pair("version", "1"),
             std::make_pair("access_token", currentApiToken)});
    xacc::info("Backend property:\n" + backend_props_response);
    auto props = json::parse(backend_props_response);
    backendProperties.insert({backend, props});
    std::vector<std::string> your_available_backends;
    for (auto &b : backends_root["backends"]) {
      if (b.count("backend_name") &&
          b["backend_name"].get<std::string>() == backend) {
        availableBackends.insert(std::make_pair(backend, b));
      }
      if (b.count("backend_name")) {
        your_available_backends.push_back(b["backend_name"].get<std::string>());
      }
    }

    if (!xacc::container::contains(your_available_backends, backend)) {
      std::stringstream error_ss;
      error_ss << "IBM Initialization Error:\n";
      error_ss << "Hub: " << hub << "\n";
      error_ss << "Group: " << group << "\n";
      error_ss << "Project: " << project << "\n";
      error_ss << "The requested backend (" << backend
               << ") is not available in this allocation.";
      error_ss << "\n\nAvailable backends are:\n";
      for (int i = 0; i < your_available_backends.size(); i++) {
        error_ss << your_available_backends[i] << ( i < your_available_backends.size()-1 ? ", " : "");
        if (i % 4 == 0) error_ss << "\n";
      }
      xacc::error(error_ss.str());
    }

    chosenBackend = availableBackends[backend];
    xacc::info("Backend config:\n" + chosenBackend.dump());
    multi_meas_enabled = chosenBackend.value("multi_meas_enabled", false);
    defaults_response =
        get(IBM_API_URL,
            IBM_CREDENTIALS_PATH + "/devices/" + backend + "/defaults", {},
            {std::make_pair("version", "1"),
             std::make_pair("access_token", currentApiToken)});
    xacc::info("Backend default:\n" + defaults_response);

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
    std::vector<std::pair<int, int>> &connectivity,
    const nlohmann::json &backendDefaults) {

  // Create a QObj
  xacc::ibm::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.1.0");
  qobj.set_type("QASM");
  qobj.set_header(QObjectHeader());

  const auto basis_gates =
      backend["basis_gates"].get<std::vector<std::string>>();
  // If the gate set has "u3" -> old gateset.
  const auto gateSet = (xacc::container::contains(basis_gates, "u3"))
                           ? QObjectExperimentVisitor::GateSet::U_CX
                           : QObjectExperimentVisitor::GateSet::RZ_SX_CX;
  // Create the Experiments
  std::vector<xacc::ibm::Experiment> experiments;
  int maxMemSlots = 0;
  for (auto &kernel : circuits) {

    auto visitor = std::make_shared<QObjectExperimentVisitor>(
        kernel->name(), backend["n_qubits"].get<int>(), gateSet);

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
    std::vector<std::pair<int, int>> &connectivity,
    const nlohmann::json &backendDefaults) {
  xacc::info("Backend Info: \n" + backend.dump());
  xacc::ibm_pulse::PulseQObject root;
  xacc::ibm_pulse::QObject qobj;
  qobj.set_qobj_id("xacc-qobj-id");
  qobj.set_schema_version("1.2.0");
  qobj.set_type("PULSE");
  xacc::ibm_pulse::QObjectHeader h;
  h.set_backend_name(backend["backend_name"].get<std::string>());
  h.set_backend_version(backend["backend_version"].get<std::string>());
  qobj.set_header(h);
  const bool pulseSupported = backend["open_pulse"].get<bool>();
  if (!pulseSupported) {
    xacc::error("Backend named '" + h.get_backend_name() +
                "' doesn't support OpenPulse.");
    return "";
  }
  auto scheduler = xacc::getService<Scheduler>("pulse");
  std::vector<xacc::ibm_pulse::Experiment> experiments;
  // std::vector<xacc::ibm_pulse::PulseLibrary> all_pulses;
  std::map<std::string, xacc::ibm_pulse::PulseLibrary> all_pulses;

  // Using the Pulse instruction assembler: lower gate->pulse + schedule.
  auto ibmPulseAssembler = xacc::getService<IRTransformation>("ibm-pulse");
  for (auto &gateKernel : circuits) {
    auto kernel = xacc::ir::asComposite(gateKernel->clone());
    // Assemble pulse composite from the input kernel.
    ibmPulseAssembler->apply(kernel, nullptr);

    // Construct the Pulse QObj
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
    experiment.set_instructions(alignMeasurePulseInstructions(
        orderFrameChangeInsts(visitor->instructions)));
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
  config.set_memory_slots(backend["n_qubits"].get<int>());
  // For now, we always use measurement level 2 (qubit 0/1 measurement)
  // We can support level 1 if required (IQ measurement values)
  config.set_meas_level(
      2); // Possible values: 1 (IQ raw values); 2 (digital values)
  config.set_meas_return("avg"); // Possible values: "avg", "single"
  config.set_rep_time(1000);
  config.set_memory_slot_size(100);
  config.set_memory(false);
  config.set_shots(shots);
  // The list of intrinsic parametric pulses supported by the backend.
  config.set_parametric_pulses(
      backend["parametric_pulses"].get<std::vector<std::string>>());

  // auto j = json::parse(getBackendPropsResponse);
  // Set meas lo and qubit lo
  // We always use the frequency estimates provided by the backend defaults.
  // This will guarantee best on-resonance drive.
  // TODO: we can support changing the drive freq. if necessary from
  // higher-level.
  config.set_meas_lo_freq(
      backendDefaults["meas_freq_est"].get<std::vector<double>>());
  config.set_qubit_lo_freq(
      backendDefaults["qubit_freq_est"].get<std::vector<double>>());

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
  std::string qobj_type = mode;
  for (auto &c : circuits) {
    if (hasMidCircuitMeasurement(c) && !multi_meas_enabled) {
      xacc::error("Circuit '" + c->name() +
                  "' has mid-circuit measurement instructions but the backend "
                  "doesn't support multiple measurement");
    }
    // If there is an analog instruction (pulse),
    // always use 'pulse' mode.
    if (c->isAnalog()) {
      qobj_type = "pulse";
      break;
    }
  }
  if (qobj_type == "pulse") {
    // if we need to use pulse mode, contribute the pulse library of the
    // backend (if any)
    if (!defaults_response.empty()) {
      // Make sure we only contribute pulse cmd-def once
      static bool contributed;
      if (!contributed) {
        contributed = true;
        contributeInstructions();
      }
    }
  }
  // Get the correct QObject Generator
  auto qobjGen = xacc::getService<QObjGenerator>(qobj_type);

  // Generate the QObject JSON
  auto jsonStr = qobjGen->getQObjJsonStr(circuits, shots, chosenBackend,
                                         getBackendPropsResponse, connectivity,
                                         json::parse(defaults_response));

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
    m.insert("config-json", chosenBackend.dump());
    m.insert("defaults-json", defaults_response);

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
    m.insert("multi_meas_enabled", multi_meas_enabled);
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
  auto dl = std::make_shared<Pulse>("delay");
  xacc::contributeService("delay", dl);

  // Add "parametric_pulse"
  auto parametricPulse = std::make_shared<Pulse>("parametric_pulse");
  xacc::contributeService("parametric_pulse", parametricPulse);

  for (auto cmd_def_iter = cmd_defs.begin(); cmd_def_iter != cmd_defs.end();
       ++cmd_def_iter) {
    auto cmd_def_name = (*cmd_def_iter)["name"].get<std::string>();
    auto qbits = (*cmd_def_iter)["qubits"].get<std::vector<std::size_t>>();

    std::string tmpName;
    tmpName = "pulse::" + cmd_def_name + "_" + std::to_string(qbits[0]);

    // Note: some pulse lib contains a special pulse composite a *measure all*
    // instruction, i.e. the list of qubits contains more than 2 qubits.
    if (qbits.size() >= 2) {
      for (size_t qIdx = 1; qIdx < qbits.size(); ++qIdx) {
        tmpName += "_" + std::to_string(qbits[qIdx]);
      }
    }
    auto cmd_def = provider->createComposite(tmpName);

    if (cmd_def_name == "u3") {
      cmd_def->addVariables({"P0", "P1", "P2"});
    } else if ((cmd_def_name == "u1") || (cmd_def_name == "rz")) {
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
      // std::cout << "inst_name = " << inst_name << "\n";
      if (inst_name != "acquire") {
        auto channel = (*seq_iter)["ch"].get<std::string>();
        auto t0 = (*seq_iter)["t0"].get<int>();
        inst->setBits(qbits);
        inst->setChannel(channel);
        inst->setStart(t0);

        // Handle parametric pulse
        if (inst_name == "parametric_pulse") {
          auto pulseParams = (*seq_iter)["parameters"];
          const std::string pulseShape =
              (*seq_iter)["pulse_shape"].get<std::string>();
          if (pulseParams.find("amp") != pulseParams.end()) {
            // Handle a potential *IBM* bug whereby it has *Internal Error
            // (9999)* when the amplitude vector contains a zero (0) entry. It
            // is expecting a "0.0" not a "0".
            // We need to explicitly cast this as a vector<double>
            // so that nlohmann will serialize accordingly.
            std::vector<double> ampVec =
                pulseParams["amp"].get<std::vector<double>>();
            const double EPS = 1e-24;
            for (auto &ampVal : ampVec) {
              if (std::abs(ampVal) < EPS) {
                ampVal = 0.0;
              }
            }
            pulseParams["amp"] = ampVec;
          }

          const std::string paramJson = pulseParams.dump();
          inst->setPulseParams(
              {{"pulse_shape", pulseShape}, {"parameters_json", paramJson}});

          const int parametricPulseDuration =
              pulseParams["duration"].get<int>();
          inst->setDuration(parametricPulseDuration);
        }
        
        // Delay pulse has a duration
        if (inst_name == "delay") {
          inst->setDuration((*seq_iter)["duration"].get<int>());
        }

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
      } else {
        // Acquire instruction
        const int duration = (*seq_iter)["duration"].get<int>();
        inst->setDuration(duration);
        inst->setBits(qbits);
      }
      cmd_def->addInstruction(inst);
    }
    cmd_def->setBits(qbits);
    // The sequence data in the cmd-def JSON is out-of-order in terms of t0,
    // i.e. it seems to be sorted by 'channel name'.
    // It may cause trouble later when assemble pulses by these cmd-defs and
    // submit an out-of-order pulse sequence .
    // Hence, we sort the pulse by t0 here.
    if (cmd_def->nInstructions() > 1) {
      auto instructionList = cmd_def->getInstructions();
      std::sort(instructionList.begin(), instructionList.end(),
                [](const auto &lhs, const auto &rhs) {
                  if (lhs->start() == rhs->start()) {
                    if (lhs->duration() == rhs->duration()) {
                      return lhs->channel() < rhs->channel();
                    }
                    return lhs->duration() < rhs->duration();
                  }
                  return lhs->start() < rhs->start();
                });
      cmd_def->clear();
      cmd_def->addInstructions(instructionList);
    }

    xacc::info("Contributing " + tmpName + " pulse composite.");
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

void IBMPulseTransform::apply(std::shared_ptr<CompositeInstruction> program,
                              const std::shared_ptr<Accelerator> accelerator,
                              const HeterogeneousMap &options) {

  auto scheduler = xacc::getService<Scheduler>("pulse");
  auto pulseMapper = std::make_shared<PulseMappingVisitor>();
  {
    InstructionIterator it(program);
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled()) {
        nextInst->accept(pulseMapper);
      }
    }
  }
  auto loweredKernel = pulseMapper->pulseComposite;
  xacc::info("Pulse-level kernel: \n" + loweredKernel->toString());
  // Schedule the pulses
  scheduler->schedule(loweredKernel);
  program->clear();
  program->addInstructions(loweredKernel->getInstructions());
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
