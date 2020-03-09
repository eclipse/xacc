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
#ifndef QUANTUM_PLUGINS_DWACCELERATOR_HPP_
#define QUANTUM_PLUGINS_DWACCELERATOR_HPP_

#include "AcceleratorBuffer.hpp"
#include "AnnealingProgram.hpp"
#include "DWQMI.hpp"
#include "RemoteAccelerator.hpp"

#include "dwave_sapi.h"

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 * Wrapper for information related to the remote
 * D-Wave solver.
 */
struct DWSolver {
  std::string name;
  std::string description;
  double jRangeMin;
  double jRangeMax;
  double hRangeMin;
  double hRangeMax;
  int nQubits;
  std::vector<std::pair<int, int>> edges;
};

class DWAccelerator : public RemoteAccelerator {
public:
  DWAccelerator() : RemoteAccelerator() {}
  DWAccelerator(std::shared_ptr<Client> client) : RemoteAccelerator(client) {}

  std::vector<std::pair<int, int>> getConnectivity() override;

  const std::string getSignature() override { return "dwave-internal" + backend; }

  const std::string processInput(
      std::shared_ptr<AcceleratorBuffer> buffer,
      std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  void processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::string &response) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override {
    int counter = 0;
    std::vector<std::shared_ptr<AcceleratorBuffer>> tmpBuffers;
    for (auto f : functions) {
      auto tmpBuffer = std::make_shared<AcceleratorBuffer>(
          buffer->name() + std::to_string(counter), buffer->size());
      RemoteAccelerator::execute(tmpBuffer, f);
      buffer->appendChild(buffer->name() + std::to_string(counter), tmpBuffer);
      counter++;
    }

    return;
  }

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

  const std::string name() const override { return "dwave-internal"; }

  const std::string description() const override {
    return "The D-Wave Accelerator executes Ising Hamiltonian parameters "
           "on a remote D-Wave QPU.";
  }

  virtual ~DWAccelerator() {}

protected:
  int shots = 1000;
  std::string backend = "DW_2000Q_5";

  std::string apiKey;
  std::string url;
  std::map<std::string, DWSolver> availableSolvers;

  sapi_Connection *connection = nullptr;
  sapi_Solver *solver = nullptr;
  const sapi_SolverProperties *solver_properties = nullptr;

  void searchAPIKey(std::string &key, std::string &url);
  void findApiKeyInFile(std::string &key, std::string &url,
                        const std::string &p);
};

} // namespace quantum
} // namespace xacc

#endif
