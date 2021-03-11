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
#include "Accelerator.hpp"
#include <bitset>
#include <type_traits>
#include "Backends.hpp"
#include "Properties.hpp"
#include "IRTransformation.hpp"
#include "QObjGenerator.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {
class QasmQObjGenerator : public QObjGenerator {
public:
  std::string
  getQObjJsonStr(std::vector<std::shared_ptr<CompositeInstruction>> composites,
                 const int &shots, const nlohmann::json &backend,
                 const std::string getBackendPropsResponse,
                 std::vector<std::pair<int, int>> &connectivity,
                 const nlohmann::json &backendDefaults) override;
  const std::string name() const override { return "qasm"; }
  const std::string description() const override { return ""; }
};

class PulseQObjGenerator : public QObjGenerator {
public:
  std::string
  getQObjJsonStr(std::vector<std::shared_ptr<CompositeInstruction>> composites,
                 const int &shots, const nlohmann::json &backend,
                 const std::string getBackendPropsResponse,
                 std::vector<std::pair<int, int>> &connectivity,
                 const nlohmann::json &backendDefaults) override;
  const std::string name() const override { return "pulse"; }
  const std::string description() const override { return ""; }
};

class RestClient {

protected:
  bool verbose = false;

public:
  void setVerbose(bool v) { verbose = v; }

  virtual const std::string post(const std::string &remoteUrl,
                                 const std::string &path,
                                 const std::string &postStr,
                                 std::map<std::string, std::string> headers =
                                     std::map<std::string, std::string>{});
  virtual void put(const std::string &remoteUrl, const std::string &putStr,
                   std::map<std::string, std::string> headers =
                       std::map<std::string, std::string>{});
  virtual const std::string
  get(const std::string &remoteUrl, const std::string &path,
      std::map<std::string, std::string> headers =
          std::map<std::string, std::string>{},
      std::map<std::string, std::string> extraParams = {});

  virtual ~RestClient() {}
};

#define IS_INTEGRAL(T)                                                         \
  typename std::enable_if<std::is_integral<T>::value>::type * = 0

template <class T>
std::string integral_to_binary_string(T byte, IS_INTEGRAL(T)) {
  std::bitset<sizeof(T) * 8> bs(byte);
  return bs.to_string();
}

std::string hex_string_to_binary_string(std::string hex);

class IBMAccelerator : public Accelerator {
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
    if (config.keyExists<bool>("http-verbose")) {
      restClient->setVerbose(config.get<bool>("http-verbose"));
    }
    // Specify a mode: "qasm" or "pulse"
    if (config.stringExists("mode")) {
      mode = config.getString("mode");
    }
  }

  const std::vector<std::string> configurationKeys() override {
    return {"shots", "backend"};
  }

  HeterogeneousMap getProperties() override;

  void
  contributeInstructions(const std::string &custom_json_config = "") override;

  const std::string getSignature() override {
    return "ibm:" + chosenBackend["backend_name"].get<std::string>();
  }

  std::vector<std::pair<int, int>> getConnectivity() override;

  // Return the name of an IRTransformation of type Placement that is
  // preferred for this Accelerator
  const std::string defaultPlacementTransformation() override {
    return "swap-shortest-path";
  }

  const std::string name() const override { return "ibm"; }
  const std::string description() const override {
    return "The IBM Accelerator interacts with the remote IBM "
           "Quantum Experience to launch XACC quantum kernels.";
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> circuit) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   circuits) override;

  bool isRemote() override { return true; }

  IBMAccelerator()
      : Accelerator(), restClient(std::make_shared<RestClient>()) {}

  virtual ~IBMAccelerator() {}

private:
  void searchAPIKey(std::string &key, std::string &hub, std::string &group,
                    std::string &project);
  void findApiKeyInFile(std::string &key, std::string &hub, std::string &group,
                        std::string &project, const std::string &p);
  std::shared_ptr<RestClient> restClient;

  static const std::string IBM_AUTH_URL;
  static const std::string IBM_API_URL;
  static const std::string DEFAULT_IBM_BACKEND;
  static const std::string IBM_LOGIN_PATH;
  std::string IBM_CREDENTIALS_PATH = "";

  std::string currentApiToken;

  std::string hub;
  std::string group;
  std::string project;

  int shots = 1024;
  std::string backend = DEFAULT_IBM_BACKEND;

  bool jobIsRunning = false;
  std::string currentJobId = "";

  std::map<std::string, nlohmann::json> availableBackends;
  nlohmann::json chosenBackend;
  bool multi_meas_enabled = false;
  bool initialized = false;
  nlohmann::json backends_root;
  std::map<std::string, nlohmann::json> backendProperties;
  std::string getBackendPropsResponse = "{}";
  std::string defaults_response = "{}";
  std::string mode = "qasm";
  std::string post(const std::string &_url, const std::string &path,
                   const std::string &postStr,
                   std::map<std::string, std::string> headers = {});
  void put(const std::string &_url, const std::string &postStr,
           std::map<std::string, std::string> headers = {});

  std::string get(const std::string &_url, const std::string &path,
                  std::map<std::string, std::string> headers =
                      std::map<std::string, std::string>{},
                  std::map<std::string, std::string> extraParams = {});
};

// IBM Pulse Transformation (gate-pulse lowering)
class IBMPulseTransform : public IRTransformation {
public:
  IBMPulseTransform() {}
  void apply(std::shared_ptr<CompositeInstruction> program,
             const std::shared_ptr<Accelerator> accelerator,
             const HeterogeneousMap &options = {}) override;

  const IRTransformationType type() const override {
    return IRTransformationType::Optimization;
  }

  const std::string name() const override { return "ibm-pulse"; }
  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc

#endif
