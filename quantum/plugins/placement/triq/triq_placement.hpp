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
 *   Thien Nguyen
 *******************************************************************************/

#pragma once

#include "xacc.hpp"
#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"

using namespace xacc;
// Forward declarations
class Circuit;
class Machine;

namespace xacc {
namespace quantum {

class TriQPlacement : public xacc::IRTransformation {

public:
  TriQPlacement() {}
  void apply(std::shared_ptr<CompositeInstruction> program,
             const std::shared_ptr<Accelerator> acc,
             const HeterogeneousMap &options = {}) override;
  const IRTransformationType type() const override {
    return IRTransformationType::Placement;
  }
  const std::string name() const override { return "triQ"; }
  const std::string description() const override { return ""; }
private:
  // Returns the QASM source string after placement
  std::string runTriQ(Circuit& program, Machine& machine, int algorithmSelector, double approxFactor) const;
};
} // namespace quantum
} // namespace xacc
