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
#ifndef QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_
#define QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_

#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"
#include "OptionsProvider.hpp"

namespace xacc {
namespace quantum {

class CircuitOptimizer : public IRTransformation, public OptionsProvider {

public:
  CircuitOptimizer() {}

  std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) override;

  const std::string name() const override { return "circuit-optimizer"; }
  const std::string description() const override { return ""; }

  OptionPairs getOptions() override {
    OptionPairs desc {{"circuit-opt-n-tries",
                        "Provide the number of passes to use in optimizing "
                        "this circuit. Default = 2."},{"circuit-opt-silent",
                                                      "Do not print any info"}};
    return desc;
  }

};
} // namespace quantum
} // namespace xacc

#endif
