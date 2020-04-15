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
#ifndef QUANTUM_AQC_IR_DWQMI_HPP_
#define QUANTUM_AQC_IR_DWQMI_HPP_

#include "Instruction.hpp"

namespace xacc {

namespace quantum {

class DWQMI : public Instruction {

protected:
  std::vector<std::size_t> qubits;

  InstructionParameter parameter;

  bool enabled = true;

public:
  DWQMI() = default;
  DWQMI(std::size_t qbit) : qubits(std::vector<std::size_t>{qbit, qbit}), parameter(0.0) {}
  DWQMI(std::size_t qbit, double param)
      : qubits(std::vector<std::size_t>{qbit, qbit}), parameter(param) {}
  DWQMI(std::size_t qbit1, std::size_t qbit2, double param)
      : qubits(std::vector<std::size_t>{qbit1, qbit2}), parameter(param) {}
  DWQMI(std::size_t qbit1, std::size_t qbit2, InstructionParameter param)
      : qubits(std::vector<std::size_t>{qbit1, qbit2}), parameter(param) {}

  DWQMI(std::vector<std::size_t> bits, InstructionParameter param)
      : qubits(bits), parameter(param) {}

  const std::string name() const override { return "dwqmi"; }
  const std::string description() const override { return ""; }


  void addArgument(std::shared_ptr<CompositeArgument> arg,
                   const int idx_of_inst_param) override {
  }

  void applyRuntimeArguments() override {
  }

  void mapBits(std::vector<std::size_t> bitMap) override {}
  const int nRequiredBits() const override {return 2;}
  void setBits(const std::vector<std::size_t> bits) override {
      qubits = bits;
  }

  const std::string toString() override {
    std::stringstream ss;
    ss << bits()[0] << " " << bits()[1] << " " << getParameter(0).toString();
    return ss.str();
  }
 void setBitExpression(const std::size_t bit_idx, const std::string expr) override {}
 std::string getBitExpression(const std::size_t bit_idx) override {return "";}
  const std::vector<std::size_t> bits() override { return qubits; }

  const InstructionParameter getParameter(const std::size_t idx) const override {
    return parameter;
  }
  std::vector<InstructionParameter> getParameters() override {
    return std::vector<InstructionParameter>{parameter};
  }
  void setParameter(const std::size_t idx, InstructionParameter &inst) override {
    parameter = inst;
  }

  const int nParameters() override { return 1; }
  bool isParameterized() override { return true; }

  bool isComposite() override { return false; }

  bool isEnabled() override { return enabled; }
  void disable() override { enabled = false; }
  void enable() override { enabled = true; }

  std::string getBufferName(const std::size_t bitIdx) override {return "";}
  void setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override {}
  std::vector<std::string> getBufferNames() override {return {};}

  std::shared_ptr<Instruction> clone() override {
      return std::make_shared<DWQMI>();
  }
  EMPTY_DEFINE_VISITABLE()
};

} // namespace quantum

} // namespace xacc

#endif
