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

class IRProvider : public Identifiable {
public:
  virtual const int getNRequiredBits(const std::string name) = 0;
  virtual std::shared_ptr<Instruction> createInstruction(const std::string name,
                                                         std::size_t bit) = 0;

  virtual std::shared_ptr<Instruction>
  createInstruction(const std::string name, std::vector<std::size_t> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{}) = 0;

  virtual std::shared_ptr<CompositeInstruction>
  createComposite(const std::string name,
                  std::vector<std::string> variables = {}) = 0;

  virtual std::shared_ptr<IR> createIR() = 0;

  virtual std::vector<std::string> getInstructions() = 0;
  virtual ~IRProvider() {}
};

} // namespace xacc
#endif
