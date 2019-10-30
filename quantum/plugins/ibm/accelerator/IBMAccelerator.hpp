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
#include "Properties.hpp"
#include "RemoteAccelerator.hpp"
#include <bitset>
#include <type_traits>
#include "Backends.hpp"
#include "Properties.hpp"

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

class IBMAccelerator : public RemoteAccelerator {
public:
  void cancel() override;

  std::map<std::string, std::map<int, int>> name2QubitMap;

  void initialize(const HeterogeneousMap &params = {}) override;
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

  HeterogeneousMap getProperties() override;

  void
  contributeInstructions(const std::string &custom_json_config = "") override;

  const std::string getSignature() override {
    return "ibm" + chosenBackend.get_name();
  }

  std::vector<std::pair<int, int>> getConnectivity() override;

  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override;

  const std::vector<double> getOneBitErrorRates() override;
  const std::vector<std::pair<std::pair<int, int>, double>>
  getTwoBitErrorRates() override;

  const std::string name() const override { return "ibm"; }
  const std::string description() const override {
    return "The IBM Accelerator interacts with the remote IBM "
           "Quantum Experience to launch XACC quantum kernels.";
  }

  const std::string processInput(
      std::shared_ptr<AcceleratorBuffer> buffer,
      std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  void processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
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

  std::map<std::string, xacc::ibm_backend::Backend> availableBackends;
  xacc::ibm_backend::Backend chosenBackend;
  bool initialized = false;
  xacc::ibm_backend::Backends backends_root;
  std::map<std::string, xacc::ibm_properties::Properties> backendProperties;
  std::string getBackendPropsResponse = "{}";

};

} // namespace quantum
} // namespace xacc

#endif
