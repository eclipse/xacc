/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef XACC_COMPILER_IRPROVIDER_HPP_
#define XACC_COMPILER_IRPROVIDER_HPP_
#include "Instruction.hpp"
#include "IR.hpp"
#include "CompositeInstruction.hpp"

namespace xacc {

// The IRProvider exposes a factory interface for the generation
// of Instructions and CompositeInstructions.
class IRProvider : public Identifiable {
public:
  virtual const int getNRequiredBits(const std::string name) = 0;
  virtual std::shared_ptr<Instruction> createInstruction(const std::string name,
                                                         std::size_t bit) = 0;

  virtual std::shared_ptr<Instruction>
  createInstruction(const std::string name, std::vector<std::size_t> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{},
                    const HeterogeneousMap &analog_options = {}) = 0;

  std::shared_ptr<Instruction>
  createInstruction(const std::string name,
                    // initializer_list overload to disambiguate
                    // empty "{}" argument.
                    std::initializer_list<std::size_t> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{},
                    const HeterogeneousMap &analog_options = {}) {
    return createInstruction(name, std::vector<std::size_t>(bits), parameters,
                             analog_options);
  }

  // Overloads for instruction creation with explicit qubit name binding.
  // i.e. automatically set the buffer names after creation.
  virtual std::shared_ptr<Instruction>
  createInstruction(const std::string name,
                    const std::pair<std::string, std::size_t> &bit) {
    auto inst = createInstruction(name, bit.second);
    inst->setBufferNames({bit.first});
    return inst;
  }

  virtual std::shared_ptr<Instruction> createInstruction(
      const std::string name,
      const std::vector<std::pair<std::string, std::size_t>> &bits,
      std::vector<InstructionParameter> parameters =
          std::vector<InstructionParameter>{},
      const HeterogeneousMap &analog_options = {}) {
    std::vector<std::string> buffer_names;
    std::vector<std::size_t> idxs;

    for (const auto &qb : bits) {
      buffer_names.emplace_back(qb.first);
      idxs.emplace_back(qb.second);
    }

    auto inst = createInstruction(name, idxs, parameters, analog_options);
    inst->setBufferNames(buffer_names);
    return inst;
  }

  virtual std::shared_ptr<CompositeInstruction>
  createComposite(const std::string name,
                  std::vector<std::string> variables = {},
                  const std::string type = "circuit") = 0;

  virtual std::shared_ptr<IR> createIR(const std::string type = "circuit") = 0;

  virtual std::vector<std::string> getInstructions() = 0;
  virtual ~IRProvider() {}
};

} // namespace xacc
#endif
