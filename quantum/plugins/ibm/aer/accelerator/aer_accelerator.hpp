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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma one
#include "xacc.hpp"
#include <nlohmann/json.hpp>
namespace xacc {
namespace quantum {

class AerAccelerator : public Accelerator {
public:
  // Identifiable interface impls
  const std::string name() const override { return "aer"; }
  const std::string description() const override {
    return "XACC Simulation Accelerator based on aer library.";
  }

  // Accelerator interface impls
  void initialize(const HeterogeneousMap &params = {}) override;
  void updateConfiguration(const HeterogeneousMap &config) override {
    initialize(config);
  };
  const std::vector<std::string> configurationKeys() override { return {}; }
  BitOrder getBitOrder() override { return BitOrder::MSB; }
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> compositeInstruction)
      override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   compositeInstructions) override;

private:
  static double calcExpectationValueZ(
      const std::vector<std::pair<double, double>> &in_stateVec,
      const std::vector<std::size_t> &in_bits);

  std::shared_ptr<Compiler> xacc_to_qobj;
  int m_shots = 1024;
  std::string m_simtype = "qasm";
  nlohmann::json noise_model;
};
} // namespace quantum
} // namespace xacc
