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
#ifndef QUANTUM_GATE_COMPILER_DEFAULT_PLACEMENT_HPP_
#define QUANTUM_GATE_COMPILER_DEFAULT_PLACEMENT_HPP_

#include "IRTransformation.hpp"
namespace xacc {
namespace quantum {

class DefaultPlacement : public IRTransformation {

public:
  DefaultPlacement() {}
  void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap& options = {}) override {
     if (options.keyExists<std::vector<int>>("qubit-map")) {
         auto map = options.get<std::vector<int>>("qubit-map");
         std::vector<std::size_t> tmp;
         for (auto& m : map) tmp.push_back(m);
         program->mapBits(tmp);
     }
     return;
  }
  const IRTransformationType type() const override {return IRTransformationType::Placement;}

  const std::string name() const override { return "default-placement"; }
  const std::string description() const override { return ""; }


};
} // namespace quantum
} // namespace xacc

#endif
