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
#ifndef XACC_DWAVE_GENERATORS_RBM_HPP_
#define XACC_DWAVE_GENERATORS_RBM_HPP_

#include "AnnealingProgram.hpp"
#include "Circuit.hpp"

namespace xacc {
namespace dwave {
class RBM : public xacc::quantum::AnnealingProgram {
protected:
  std::vector<InstructionParameter> parameters;

public:
  RBM() : AnnealingProgram("rbm-ap") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
 std::shared_ptr<Instruction> clone() override {
    return std::make_shared<RBM>();
  }

  const InstructionParameter
  getParameter(const std::size_t idx) const override {
    return parameters[idx];
  }
};

class RBMAsCircuitType : public xacc::quantum::Circuit {
protected:
  std::vector<InstructionParameter> parameters;
public:
  RBMAsCircuitType() : quantum::Circuit("rbm") {}
  void applyRuntimeArguments() override;
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override {return false;}
  const std::vector<std::string> requiredKeys() override {return {"nv","nh"};}
  const InstructionParameter
  getParameter(const std::size_t idx) const override {
    return parameters[idx];
  }
  DEFINE_CLONE(RBMAsCircuitType);
};
} // namespace dwave
} // namespace xacc
#endif