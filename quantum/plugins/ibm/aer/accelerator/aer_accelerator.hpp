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

namespace AER {
namespace Noise {
class NoiseModel;
}
} // namespace AER

namespace xacc {
namespace quantum {

class AerAccelerator : public Accelerator {
public:
  // Identifiable interface impls
  const std::string name() const override { return "aer"; }
  const std::string description() const override {
    return "XACC Simulation Accelerator based on aer library.";
  }
  HeterogeneousMap getProperties() override;

  // Accelerator interface impls
  void initialize(const HeterogeneousMap &params = {}) override;
  void updateConfiguration(const HeterogeneousMap &config) override {
    if (config.keyExists<int>("shots")) {
        m_shots = config.get<int>("shots");
        m_options.insert("shots", m_shots);
    }
    if (config.stringExists("backend")) {
        m_options.insert("backend", config.getString("backend"));
        // backend changed so reinit
        initialize(m_options);
    }
  };
  const std::vector<std::string> configurationKeys() override { return {}; }
  BitOrder getBitOrder() override { return BitOrder::MSB; }
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> compositeInstruction)
      override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   compositeInstructions) override;
  std::vector<std::pair<int, int>> getConnectivity() override {
    return connectivity;
  }

  void apply(std::shared_ptr<AcceleratorBuffer> buffer,
             std::shared_ptr<Instruction> inst) override;
  bool isInitialized() const { return initialized; }

private:
  static double calcExpectationValueZ(
      const std::vector<std::pair<double, double>> &in_stateVec,
      const std::vector<std::size_t> &in_bits);

  static double calcExpectationValueZFromDensityMatrix(
      const std::vector<std::vector<std::pair<double, double>>> &in_densityMat,
      const std::vector<std::size_t> &in_bits);

  std::shared_ptr<Compiler> xacc_to_qobj;
  int m_shots = 1024;
  std::string m_simtype = "qasm";
  nlohmann::json noise_model;
  std::vector<std::pair<int, int>> connectivity;
  HeterogeneousMap m_options;
  bool initialized = false;
  std::shared_ptr<AER::Noise::NoiseModel> noiseModelObj;
  HeterogeneousMap physical_backend_properties;

};
} // namespace quantum
} // namespace xacc
