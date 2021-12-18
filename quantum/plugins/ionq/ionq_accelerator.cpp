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
#include "ionq_accelerator.hpp"
#include <cctype>
#include <fstream>
#include "xacc.hpp"
#include "json.hpp"

#include "ionq_program.hpp"
#include "ionq_program_visitor.hpp"

#include <regex>
#include <thread>

namespace xacc {
namespace quantum {

HeterogeneousMap IonQAccelerator::getProperties() {
  HeterogeneousMap m;

  return m;
}

void IonQAccelerator::initialize(const HeterogeneousMap &params) {
  if (!initialized) {
    updateConfiguration(params);
    std::string jsonStr = "", apiKey = "";
    searchAPIKey(apiKey, url);

    headers.clear();
    headers.insert({"Authorization", "apiKey " + apiKey});
    headers.insert({"Content-Type", "application/json"});

    auto calibrations = restClient->get(url, "/calibrations", headers);
    auto j = nlohmann::json::parse(calibrations);
    m_connectivity = j["calibrations"][0]["connectivity"].get<std::vector<std::pair<int,int>>>();
    
    remoteUrl = url;
    postPath = "/jobs";
  }
}

// Note: IonQ don't support batching. 
void IonQAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> circuits) {
  for (const auto &f : circuits) {
    auto tmpBuffer =
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
    // Run each circuit
    RemoteAccelerator::execute(tmpBuffer, f);
    // tmpBuffer->print();
    buffer->appendChild(f->name(), tmpBuffer);
  }
}

const std::string IonQAccelerator::processInput(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  xacc::ionq::IonQProgram prog;
  prog.set_shots(shots);
  prog.set_target(backend);

  auto visitor = std::make_shared<IonQProgramVisitor>();

  InstructionIterator it(functions[0]);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  // After calling getExperiment, maxMemorySlots should be
  // maxClassicalBit + 1
  auto insts = visitor->getCircuitInstructions();

  xacc::ionq::Body b;
  b.set_circuit(insts);
  b.set_qubits(buffer->size());

  prog.set_body(b);
  using json = nlohmann::json;
  json jj;
  to_json(jj, prog);
  xacc::info("IonQ Job JSON:\n" + jj.dump(4) + "\n");
  return jj.dump();
}

void IonQAccelerator::processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                                      const std::string &response) {

  using json = nlohmann::json;
  xacc::ionq::IonQProgram root;
  auto j = json::parse(response);

  std::string jobId = j["id"].get<std::string>();
  std::string msg;
  bool jobCompleted = false;
  int dots = 1;
  while (!jobCompleted) {

    msg = handleExceptionRestClientGet(url, "/jobs/" + jobId, headers);
    j = json::parse(msg);
    if (j["status"].get<std::string>() == "completed") {
      jobCompleted = true;
    }

    // Add a status message...
    if (dots > 4) {
      dots = 1;
    }
    std::stringstream ss;
    ss << "\033[0;32m"
       << "IonQ Job "
       << "\033[0;36m" << jobId
       << "\033[0;32m"
       // IonQ use the word "ready" to denote the submitted job status
       << " Status: "
       << (j["status"].get<std::string>() == "ready"
               ? "submitted"
               : j["status"].get<std::string>());
    for (int i = 0; i < dots; i++) {
      ss << '.';
    }

    dots++;
    std::cout << '\r' << ss.str() << std::setw(20) << std::setfill(' ')
              << std::flush;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  // End the color log
  std::cout << "\033[0m" << "\n";

  std::map<std::string, double> histogram =
      j["data"]["histogram"].get<std::map<std::string, double>>();

  int n = buffer->size();
  auto getBitStrForInt = [&](std::uint64_t i) {
    std::stringstream s;
    for (int k = n - 1; k >= 0; k--)
      s << ((i >> k) & 1);
    return s.str();
  };

  for (auto &kv : histogram) {
    buffer->appendMeasurement(getBitStrForInt(std::stoi(kv.first)),
                              std::ceil(kv.second * shots));
  }

  xacc::info("IonQ Result JSON:\n" + j.dump(4) + "\n");
  return;
}

void IonQAccelerator::cancel() {}

std::vector<std::pair<int, int>> IonQAccelerator::getConnectivity() {
  return m_connectivity;
}

void IonQAccelerator::searchAPIKey(std::string &key, std::string &url) {

  // Search for the API Key in $HOME/.ibm_config,
  // $IBM_CONFIG, or in the command line argument --ibm-api-key
  std::string ionqConfig(std::string(getenv("HOME")) + "/.ionq_config");
  if (xacc::fileExists(ionqConfig)) {
    findApiKeyInFile(key, url, ionqConfig);
  } else if (const char *nonStandardPath = getenv("IONQ_CONFIG")) {
    std::string nonStandardIonqConfig(nonStandardPath);
    findApiKeyInFile(key, url, nonStandardIonqConfig);
  }

  // If its still empty, then we have a problem
  if (key.empty()) {
    xacc::error("Error. The API Key is empty. Please place it "
                "in your $HOME/.ionq_config file, $IONQ_CONFIG env var.");
  }
}

void IonQAccelerator::findApiKeyInFile(std::string &apiKey, std::string &url,
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
      if (split.size() > 2) {
        key += ":" + split[2];
      }
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
} // namespace quantum
} // namespace xacc
