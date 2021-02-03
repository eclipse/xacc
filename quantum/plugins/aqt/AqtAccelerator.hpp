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
#include <pybind11/embed.h>

namespace xacc {
namespace quantum {
class AqtAccelerator : public Accelerator {
public:
  // Identifiable interface impls
  virtual const std::string name() const override { return "aqt"; }
  virtual const std::string description() const override {
    return "XACC Accelerator for AQT platform.";
  }

  // Accelerator interface impls
  virtual void initialize(const HeterogeneousMap &params = {}) override;
  virtual void updateConfiguration(const HeterogeneousMap &config) override {
    initialize(config);
  }
  virtual const std::vector<std::string> configurationKeys() override {
    return {};
  }

  virtual std::vector<std::pair<int, int>> getConnectivity() override;

  virtual BitOrder getBitOrder() override { return BitOrder::MSB; }
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction>
                           compositeInstruction) override;
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           compositeInstructions) override;
  virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer,
                     std::shared_ptr<Instruction> inst) override{};

private:
  pybind11::object createQtrlCircuit(
      std::shared_ptr<AcceleratorBuffer> buffer,
      const std::shared_ptr<CompositeInstruction> compositeInstruction);

private:
  int m_shots;
  pybind11::object m_openQASM2qtrl;
  pybind11::object m_config;
  pybind11::object m_qpu;
  pybind11::object m_circuitCtor;
  pybind11::object m_circuitColectionCtor;
};
} // namespace quantum
} // namespace xacc
