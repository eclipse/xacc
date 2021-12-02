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
#ifndef QUANTUM_GATE_ACCELERATORS_IONQACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_IONQACCELERATOR_HPP_

#include "RemoteAccelerator.hpp"

#include <bitset>
#include <type_traits>

namespace xacc {
namespace quantum {

class IonQAccelerator : public RemoteAccelerator {
public:
  void cancel() override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   circuits) override;
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

  const std::string getSignature() override { return "ionq:" + backend; }

  std::vector<std::pair<int, int>> getConnectivity() override;

  const std::string name() const override { return "ionq"; }
  const std::string description() const override { return ""; }

  const std::string processInput(
      std::shared_ptr<AcceleratorBuffer> buffer,
      std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  void processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::string &response) override;

  IonQAccelerator() : RemoteAccelerator() {}

  IonQAccelerator(std::shared_ptr<Client> client) : RemoteAccelerator(client) {}

  virtual ~IonQAccelerator() {}

private:
  void searchAPIKey(std::string &key, std::string &url);
  void findApiKeyInFile(std::string &key, std::string &url,
                        const std::string &p);

  std::string currentApiToken;

  std::string url;

  int shots = 1024;
  std::string backend = "simulator";
  std::vector<std::pair<int,int>> m_connectivity;

  bool jobIsRunning = false;
  std::string currentJobId = "";

  bool initialized = false;
};

} // namespace quantum
} // namespace xacc

#endif
