/*******************************************************************************
 * Copyright (c) 2024 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#pragma once
#include "xacc.hpp"
#include "CuTensorNetHelper.hpp"
#include "PauliOperator.hpp"

namespace xacc {
namespace quantum {

class CuTensorNetAccelerator : public Accelerator {
public:
  // Identifiable interface impls
  virtual const std::string name() const override { return "cutensornet"; }
  virtual const std::string description() const override {
    return "XACC Simulation Accelerator based on cutensor library.";
  }

  // Accelerator interface impls
  virtual void initialize(const HeterogeneousMap &params = {}) override;
  virtual void updateConfiguration(const HeterogeneousMap &config) override {
    return;
  };
  virtual const std::vector<std::string> configurationKeys() override {
    return {};
  }
  virtual HeterogeneousMap getProperties() override { return {}; }
  virtual BitOrder getBitOrder() override { return BitOrder::LSB; }
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction>
                           compositeInstruction) override;
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           compositeInstructions) override;
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction> baseCircuit,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           basisRotations) override;
  virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer,
                     std::shared_ptr<Instruction> inst) override {
    return;
  };
  // ExecutionInfo implementation:
  virtual xacc::HeterogeneousMap getExecutionInfo() const override {
    return {};
  }

  ~CuTensorNetAccelerator() {
    HANDLE_CUTN_ERROR(cutensornetDestroy(cutnHandle));
  }

private:
  cutensornetHandle_t cutnHandle;
  int64_t bondDimension = 2;
  PauliOperator *observable;
};

} // namespace quantum
} // namespace xacc