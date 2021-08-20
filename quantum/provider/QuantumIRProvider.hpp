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
#ifndef QUANTUM_IRPROVIDER_HPP_
#define QUANTUM_IRPROVIDER_HPP_

#include "Identifiable.hpp"

#include "IRProvider.hpp"

namespace xacc {
namespace quantum {

class QuantumIRProvider : public IRProvider {

public:
  QuantumIRProvider() = default;

  const int getNRequiredBits(const std::string name) override;
  std::shared_ptr<Instruction> createInstruction(const std::string name,
                                                 std::size_t bit) override;
  std::shared_ptr<Instruction>
  createInstruction(const std::string name, std::vector<std::size_t> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{},
                    const HeterogeneousMap &analog_options = {}) override;

  std::shared_ptr<CompositeInstruction>
  createComposite(const std::string name,
                  std::vector<std::string> variables = {},
                  const std::string type = "circuit") override;

  std::shared_ptr<IR> createIR(const std::string type = "circuit") override;

  std::vector<std::string> getInstructions() override;

  const std::string name() const override { return "quantum"; }
  const std::string description() const override {
    return "This IRProvider implementation "
           "generates and returns gate and annealing instructions.";
  }
};

} // namespace quantum
} // namespace xacc

#endif
