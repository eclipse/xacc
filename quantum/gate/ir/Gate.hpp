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
#ifndef QUANTUM_GATE_IR_GATEINSTRUCTION_HPP_
#define QUANTUM_GATE_IR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

/**
 */
class Gate : public Instruction, public Cloneable<Instruction> {

protected:
  std::string gateName;
  std::vector<std::size_t> qbits;
  bool enabled = true;
  std::vector<InstructionParameter> parameters;

public:
  Gate();
  Gate(std::string name);
  Gate(std::string name, std::vector<InstructionParameter> params);
  Gate(std::string name, std::vector<std::size_t> qubts);
  Gate(std::string name, std::vector<std::size_t> qubts,
       std::vector<InstructionParameter> params);

  Gate(const Gate &inst);

  const std::string name() const override;
  const std::string description() const override;

  const std::string toString() override;

  const std::vector<std::size_t> bits() override;
  void setBits(const std::vector<std::size_t> bits) override { qbits = bits; }

  const InstructionParameter getParameter(const std::size_t idx) const override;
  std::vector<InstructionParameter> getParameters() override;

  void setParameter(const std::size_t idx, InstructionParameter &inst) override;

  const int nParameters() override;

  bool isParameterized() override;

  void mapBits(std::vector<std::size_t> bitMap) override;

  bool isComposite() override { return false; }

  bool isEnabled() override;
  void disable() override;
  void enable() override;

#define DEFINE_CLONE(CLASS)                                                    \
  std::shared_ptr<Instruction> clone() override {                              \
    return std::make_shared<CLASS>();                                          \
  }
};

} // namespace quantum
} // namespace xacc
#endif