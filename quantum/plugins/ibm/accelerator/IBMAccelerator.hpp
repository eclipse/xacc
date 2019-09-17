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
#ifndef QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "RemoteAccelerator.hpp"
#include <bitset>
#include <type_traits>

using namespace xacc;

namespace xacc {
namespace quantum {

#define IS_INTEGRAL(T)                                                         \
  typename std::enable_if<std::is_integral<T>::value>::type * = 0

template <class T>
std::string integral_to_binary_string(T byte, IS_INTEGRAL(T)) {
  std::bitset<sizeof(T) * 8> bs(byte);
  return bs.to_string();
}

std::string hex_string_to_binary_string(std::string hex);

struct IBMBackend {
  std::string name;
  std::string description;
  int nQubits;
  std::vector<std::pair<int, int>> couplers;
  bool status = true;
  bool isSimulator = true;
  std::string gateSet = "";
  std::string basisGates = "";
  std::vector<double> readoutErrors;
  std::vector<double> gateErrors;
  std::vector<std::string> multiQubitGates;
  std::vector<double> multiQubitGateErrors;
  std::vector<double> T1s;
  std::vector<double> T2s;
  std::vector<double> frequencies;
};

class IBMAccelerator : public RemoteAccelerator {
public:

  void cancel() override;

  std::map<std::string, std::map<int, int>> name2QubitMap;

  void initialize(const HeterogeneousMap& params = {}) override;
 void updateConfiguration(const HeterogeneousMap &config) override {
      if (config.keyExists<int>("shots")) {
          shots = config.get<int>("shots");
      }
     if (config.stringExists("backend")) {
          backend = config.getString("backend");
      }
  }

  const std::vector<std::string> configurationKeys() override {
      return {"shots", "backend"};
  }

  const std::string getSignature() override {return "ibm"+chosenBackend.name;}

  std::vector<std::pair<int, int>> getConnectivity() override;

  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override;

  OptionPairs getOptions() override {
    OptionPairs desc{
        {"ibm-api-key", "Provide the IBM API key. This is used if "
                        "$HOME/.ibm_config is not found"},
        {"ibm-api-url",
         "Specify the IBM Quantum Experience URL, overrides $HOME/.ibm_config "
         "."},
        {"ibm-backend", "Provide the backend name."},
        {"ibm-shots", "Provide the number of shots to execute."},
        {"ibm-list-backends",
         "List the available backends at the IBM Quantum Experience URL."},
        {"ibm-print-queue",
         "Print the status of the queue for the given backend"}};
    return desc;
  }

  bool handleOptions(const std::map<std::string, std::string> &map) override {
    if (map.count("ibm-list-backends")) {
      initialize();
      XACCLogger::instance()->enqueueLog("");
      for (auto s : availableBackends) {
        XACCLogger::instance()->enqueueLog(
            "Available IBM Backend: " + std::string(s.first) + " [" +
            (s.second.status ? "on" : "off") + "]");
      }
      return true;
    } else if (map.count("ibm-print-queue")) {
      initialize();
      auto backend = map.at("ibm-print-queue");
      XACCLogger::instance()->enqueueLog("");
      XACCLogger::instance()->enqueueLog(
          "Queue Status for " + backend + ": " +
          handleExceptionRestClientGet(remoteUrl, "/api/Backends/" + backend +
                                                      "/queue/status"));
      return true;
    }
    return false;
  }

  const std::vector<double> getOneBitErrorRates() override;
  const std::vector<std::pair<std::pair<int, int>, double>>
  getTwoBitErrorRates() override;

  const std::string name() const override { return "ibm"; }
  const std::string description() const override {
    return "The IBM Accelerator interacts with the remote IBM "
           "Quantum Experience to launch XACC quantum kernels.";
  }

  const std::string
  processInput(std::shared_ptr<AcceleratorBuffer> buffer,
               std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  void
  processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                  const std::string &response) override;

  IBMAccelerator() : RemoteAccelerator() {}

  IBMAccelerator(std::shared_ptr<Client> client) : RemoteAccelerator(client) {}

  virtual ~IBMAccelerator() {}

private:

  void searchAPIKey(std::string &key, std::string &url, std::string &hub,
                    std::string &group, std::string &project);
  void findApiKeyInFile(std::string &key, std::string &url, std::string &hub,
                        std::string &group, std::string &project,
                        const std::string &p);
  std::string currentApiToken;

  std::string url;
  std::string hub;
  std::string group;
  std::string project;

  int shots = 1024;
  std::string backend = "ibmq_qasm_simulator";

  bool jobIsRunning = false;
  std::string currentJobId = "";

  std::map<std::string, IBMBackend> availableBackends;

  IBMBackend chosenBackend;

  bool initialized = false;
};

} // namespace quantum
} // namespace xacc

#endif
