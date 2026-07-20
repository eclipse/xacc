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
 *   Daniel Claudino - Update to Qiskit Runtime (Qiskit 1.x)
 *******************************************************************************/
#include "IBMAccelerator.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include "Utils.hpp"

#ifndef REMOTE_DISABLED
#include <cpr/cpr.h>
#endif

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Scheduler.hpp"
#include "QObject.hpp"

#include <thread>
#include <cassert>

namespace xacc {
namespace quantum {

const std::string IBMAccelerator::IBM_API_URL =
  "https://api.quantum-computing.ibm.com";
const std::string IBMAccelerator::IBM_TRANSPILER_URL =
  "https://cloud-transpiler.quantum.ibm.com";

std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

bool caseInsensitiveCompare(const std::string& str1, const std::string& str2) {
    return str1.size() == str2.size() &&
           std::equal(str1.begin(), str1.end(), str2.begin(), [](unsigned char c1, unsigned char c2) {
               return std::tolower(c1) == std::tolower(c2);
           });
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

bool IBMAccelerator::verifyJobsLimit(std::string& curr_backend) {

  // Get backend jobs limit
  std::string getJobsLimitPath = "/api/Network/" + hub + "/Groups/" + group +
                        "/Projects/" + project + "/devices/" + curr_backend +
                        "/jobsLimit";
  auto backend_jobslimit_response = get(IBM_API_URL, getJobsLimitPath, {},
        {std::make_pair("version", "1"),
        std::make_pair("access_token", currentApiToken)});
  auto backend_jobslimit_response_json = json::parse(backend_jobslimit_response);

  auto maximumJobs = backend_jobslimit_response_json["maximumJobs"].get<int>();
  auto runningJobs = backend_jobslimit_response_json["runningJobs"].get<int>();

  return maximumJobs < 0 || runningJobs < maximumJobs;
}

void IBMAccelerator::processBackendCandidate(const nlohmann::json& backend_json) {

  // First of all filter by count of qubits
  if (requested_n_qubits > 0) {
    if( backend_json.count("n_qubits") ) {
      int nqubits = backend_json["n_qubits"].get<int>();
      if(nqubits < requested_n_qubits) {
        return;
      }
    } else {
      return;
    }
  }
  std::string curr_backend = backend_json["backend_name"].get<std::string>();

// Get current backend status
  const std::string path("/runtime/backends/" + curr_backend + "/status");
  auto status_json = json::parse(get(IBM_API_URL, path, headers, {{"name", curr_backend}}));

  auto queue_length = status_json["length_queue"].get<int>();
  auto state = status_json["state"].get<bool>();

  if (state && (backendQueueLength < 0 || backendQueueLength > queue_length)) {
    if (filterByJobsLimit && !verifyJobsLimit(curr_backend)) {
      return;
    }
    backendQueueLength = queue_length;
    auto old_backend = backend;
    backend = curr_backend;
    //availableBackends.clear();
    availableBackends.push_back(curr_backend);
  }
}

void IBMAccelerator::selectBackend(std::vector<std::string>& all_available_backends) {
  bool lowest_queue_backend = false;
  if (backend == "lowest-queue-count") {
    lowest_queue_backend = true;
  }

  for (std::string b : backends_root["devices"]) {

    // Simple case: select by backend_name
    if (!lowest_queue_backend) {

      if (b == backend) {

        const std::string path("/runtime/backends/" + b + "/status");
        auto backend_status_json =
            json::parse(get(IBM_API_URL, path, headers, {{"name", b}}));
        if (caseInsensitiveCompare(
                backend_status_json["status"].get<std::string>(), "active")) {
          availableBackends.push_back(b);
        } else {
          xacc::error(backend + "not active at the moment.");
        }
      }
    } else {
      const std::string path("/runtime/backends/" + b + "/configuration");
      auto backend_json =
          json::parse(get(IBM_API_URL, path, headers, {{"name", b}}));
      // Select backend by job queue size and by parameters (optional)
      processBackendCandidate(backend_json);
    }
    all_available_backends.push_back(b);
  }
  if (lowest_queue_backend) {
    xacc::info("Backend with lowest queue count: " + backend);
  }
}

void IBMAccelerator::updateConfiguration(const HeterogeneousMap &config) {

  if (config.keyExists<int>("shots")) {
    shots = config.get<int>("shots");
  }
  if (config.stringExists("backend")) {
    backend = config.getString("backend");
  }
  if (config.keyExists<int>("n-qubits")) {
    requested_n_qubits = config.get<int>("n-qubits");
  }
  if (config.keyExists<bool>("check-jobs-limit")) {
    filterByJobsLimit = config.get<bool>("check-jobs-limit");
  }
  if (config.keyExists<bool>("http-verbose")) {
    restClient->setVerbose(config.get<bool>("http-verbose"));
  }
  // Specify a mode: "qasm" or "pulse"
  if (config.stringExists("mode")) {
    mode = config.getString("mode");
  }

  if (config.keyExists<bool>("cloud-transpiler")) {
    useCloudTranspiler = config.get<bool>("cloud-transpiler");
  }

  if (config.stringExists("primitive")) {
    primitiveId = config.getString("primitive");
    if (primitiveId != "sampler" && primitiveId != "estimator") {
      xacc::error("Primitive can only be sampler or estimator.");
    }
  }

  if (config.stringExists("dynamical-decoupling-type")) {
    ddType = config.getString("dynamical-decoupling-type");
    if(!xacc::container::contains(DDTYPES, ddType)) {
      xacc::error("Chosen dynamical decoupling type invalid. Valid sequences are XX, XpXm, and XY4");
    }
    enableDD = true;
  } else if (config.keyExists<bool>("dynamical-decoupling")) {
    enableDD = config.get<bool>("dynamical-decoupling");
    if (enableDD) {
      xacc::warning(
          "Using dynamical decoupling default XX. You can change it by passing "
          "the \"dynamical-decoupling-type\" key with arguments XX, XpXm, XY4");
      ddType = "XX";
    }
  }
}

void IBMAccelerator::initialize(const HeterogeneousMap &params) {
  if (!initialized) {

    // Set backend, shots, etc.
    // and get the apikey, hub, group, and project
    updateConfiguration(params);
    searchAPIKey(currentApiToken, hub, group, project);

    // We should have backend set by now

    if (hub.empty() && group.empty() && project.empty()) {
      // Fallback to public API credentials
      hub = "ibm-q";
      group = "open";
      project = "main";
    }

    headers = {
        {"Accept", "application/json"},
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + currentApiToken}
        };

    auto getUser = get(IBM_API_URL, "/runtime/users/me", headers);
    auto provider = json::parse(getUser)["instances"][0]["name"].get<std::string>();

    backends_root = json::parse(get(IBM_API_URL, "/runtime/backends", headers, {{"provider", provider}}));

    std::vector<std::string> your_available_backends;
    selectBackend(your_available_backends);

    std::string getBackendPropertiesPath = "/runtime/backends/" + backend + "/properties";
    // Get current backend properties
    auto backend_props_response =
        get(IBM_API_URL, getBackendPropertiesPath, headers, {{"name", backend}});

    xacc::info("Backend property:\n" + backend_props_response);
    auto props = json::parse(backend_props_response);
    backendProperties.insert({backend, props});

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

    const std::string path("/runtime/backends/" + backend + "/configuration");
    chosenBackend = json::parse(get(IBM_API_URL, path, headers, {{"name", backend}}));
    xacc::info("Backend config:\n" + chosenBackend.dump());

    multi_meas_enabled = chosenBackend.value("multi_meas_enabled", false);

    auto defaultsPath = "/runtime/backends/" + backend + "/defaults";
    defaults_response = get(IBM_API_URL, defaultsPath, headers);
    xacc::info("Backend default:\n" + defaults_response);

    // check if cloud transpiler is working in case it will be used
    if (useCloudTranspiler) {
      auto checkTranspilerHealth = get(IBM_TRANSPILER_URL, "/health", headers);
      auto transpilerHealth = json::parse(checkTranspilerHealth)["message"].get<std::string>();
      if (transpilerHealth != "Health OK") {
        xacc::warning("IBM cloud transpiler is not working. Unless your program is already transpiled, it will not run.");
      }
    }

    initialized = true;
  }

}

void IBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> circuit) {
  execute(buffer, std::vector<std::shared_ptr<CompositeInstruction>>{circuit});
}

void IBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> circuits) {

    auto qasmCompiler = xacc::getCompiler("staq");
    std::vector<std::string> qasmCircuits;
    for (auto c : circuits) {
      qasmCircuits.push_back(qasmCompiler->translate(c));
    }

    // transpilation
     json transpilerBodyParams = {
        {"qasm_circuits", qasmCircuits}
    };

    json transpilerQueryParams = {{"backend", backend}};


    auto transpilerResponse = post(IBM_TRANSPILER_URL, "/transpile", transpilerBodyParams.dump(), headers, transpilerQueryParams.dump());

    auto task_id = json::parse(transpilerResponse)["task_id"].get<std::string>();

  int dots = 1;
  std::string state;
  while (true) {
    
    auto get_job_status = get(IBM_TRANSPILER_URL, "/transpile/" + task_id, headers);
    json get_job_status_json;

    try {
      get_job_status_json = json::parse(get_job_status);
    } catch (json::exception const &) {
      std::cout << "Failed to parse response '" << get_job_status
                << "' from job status " << task_id << std::endl;
      throw;
    }
    state = get_job_status_json["state"].get<std::string>();

    if (state.find("SUCCESS") == std::string::npos && state.find("PENDING") == std::string::npos) {
      xacc::error("Transpiler failed: " + get_job_status_json.dump(4));
    }

    if (state == "SUCCESS") {
      std::cout << std::endl;
      break;
    }

    if (dots > 4)
      dots = 1;
    std::stringstream ss;
    ss << "\033[0;32m"
        << "IBM Transpiler Job "
        << "\033[0;36m" << task_id << "\033[0;32m"
        << " Status: " << state;
    for (int i = 0; i < dots; i++)
      ss << '.';
    dots++;
    ss << "\033[0m";
    std::cout << '\r' << ss.str() << std::setw(20) << std::setfill(' ')
              << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  auto transpiled = json::parse(get(IBM_TRANSPILER_URL, "/transpile/" + task_id, headers));

  json body;
  body["program_id"] = primitiveId;
  body["hub"] = hub;
  body["group"] = group;
  body["project"] = project;
  body["backend"] = backend;

  // Initialize the `params` section
  body["params"]["pubs"] = json::array();
  body["params"]["support_qiskit"] = false;
  body["params"]["version"] = 2;

  // Add the nested `options` object
  if (enableDD) {
    body["params"]["options"]["dynamical_decoupling"]["enable"] = enableDD;
    body["params"]["options"]["dynamical_decoupling"]["sequence_type"] = ddType;
  }

  for (const auto & c : transpiled["result"]) {
    body["params"]["pubs"].push_back(
      {c["qasm"], json::array(), shots}
    );
  }

  auto reserve_response = post(IBM_API_URL, "/runtime/jobs", body.dump(), headers);

  xacc::info("Current job ID: " + json::parse(reserve_response)["id"].get<std::string>());

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
    auto response = post(IBM_API_URL, path, std::string(), headers);
    xacc::info("Cancel Response: " + response);
    jobIsRunning = false;
    currentJobId = "";
  }
}

void IBMAccelerator::retrieve(const std::string jobId, std::shared_ptr<xacc::AcceleratorBuffer> buffer) {

  const std::string path("/runtime/jobs/" + jobId + "/results");
  auto resultsJson = get(IBM_API_URL, path, headers);
  auto hexs = json::parse(resultsJson)["results"][0]["data"]["q_c"]["samples"];

  std::vector<int> samples;
  for (const auto& sample_hex : hexs) {
            int sample_int = std::stoi(sample_hex.get<std::string>(), nullptr, 16);
            // Convert the integer to a binary string
            samples.push_back(sample_int);
        }
        buffer->setSamples(samples);
  return;
}

std::vector<std::pair<int, int>> IBMAccelerator::getConnectivity() {

  if (!xacc::container::contains(availableBackends, backend)) {
    xacc::error(backend + " is not available.");
  }

  const std::string path("/runtime/backends/" + backend + "/configuration");
  auto backend_json = json::parse(get(IBM_API_URL, path, headers, {{"name", backend}}));
  std::vector<std::pair<int, int>> graph;
  graph = backend_json["coupling_map"].get<std::vector<std::pair<int, int>>>();
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
          p10s.push_back(q["value"].get<double>());
        }
      }
    }
    assert(std::all_of(p01s.begin(), p01s.end(),
                       [](double x) { return x >= 0.0 && x < 1.0; }));
    assert(std::all_of(p10s.begin(), p10s.end(),
                       [](double x) { return x >= 0.0 && x < 1.0; }));

    m.insert("p01s", p01s);
    m.insert("p10s", p10s);
    m.insert("multi_meas_enabled", multi_meas_enabled);
  }

  return m;
} // namespace quantum


const std::string RestClient::post(const std::string &remoteUrl,
                                   const std::string &path,
                                   const std::string &postStr,
                                   std::map<std::string, std::string> headers,
                                   const std::string &queryParams) {
#ifndef REMOTE_DISABLED
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

  cpr::Response r;
  if (queryParams.empty()) {
    r = cpr::Post(cpr::Url{remoteUrl + path}, cpr::Body(postStr), cprHeaders,
                     cpr::VerifySsl(false));
  } else {
    json jsonQueryParams = json::parse(queryParams);
    cpr::Parameters cprQueryParams{};
    for (const auto& [key, value] : jsonQueryParams.items()) {
        cprQueryParams.Add({key, value});
    }
        r = cpr::Post(cpr::Url{remoteUrl + path}, cpr::Body(postStr), cprHeaders,
                     cprQueryParams, cpr::VerifySsl(false));

  }

  if (r.status_code != 200)
    throw std::runtime_error("HTTP POST Error - status code " +
                             std::to_string(r.status_code) + ": " +
                             r.error.message + ": " + r.text);

  return r.text;
#else
  return "";
#endif
}

void RestClient::put(const std::string &remoteUrl, const std::string &putStr,
                     std::map<std::string, std::string> headers) {
#ifndef REMOTE_DISABLED
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
#endif
  return;
}
const std::string
RestClient::get(const std::string &remoteUrl, const std::string &path,
                std::map<std::string, std::string> headers,
                std::map<std::string, std::string> extraParams) {
#ifndef REMOTE_DISABLED
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
    cprParams.Add({kv.first, kv.second});
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
#else
  return "";
#endif
}

std::string IBMAccelerator::post(const std::string &_url,
                                 const std::string &path,
                                 const std::string &postStr,
                                 std::map<std::string, std::string> headers,
                                 const std::string& queryParams) {
  std::string postResponse;
  int retries = 10;
  std::exception ex;
  bool succeeded = false;

  // Execute HTTP Post
  do {
    try {
      postResponse = restClient->post(_url, path, postStr, headers, queryParams);
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

// TODO
std::string
IBMAccelerator::getNativeCode(std::shared_ptr<CompositeInstruction> program,
                              const HeterogeneousMap &config) {
  return "";
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