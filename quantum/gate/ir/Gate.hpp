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
#include "Utils.hpp"
#include "expression_parsing_util.hpp"

namespace xacc {
namespace quantum {

/**
 */
class Gate : public Instruction {

protected:
  std::shared_ptr<ExpressionParsingUtil> parsingUtil;

  std::string gateName;
  std::vector<std::size_t> qbits;
  bool enabled = true;
  std::vector<InstructionParameter> parameters;
  std::vector<std::string> buffer_names;

  std::map<std::size_t, std::string> bitIdxExpressions;

  // map InstructionParameter index to corresponding CompositeArgument
  std::map<int, std::shared_ptr<CompositeArgument>> arguments;

  // Map index of InstructionParameters to corresponding
  // std::vector<double> index for instructions like this
  // foo ( std::vector<double> x ) {
  //   U(x[0], x[1], x[2]);
  //}
  std::map<int, int> param_idx_to_vector_idx;

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

  void addArgument(std::shared_ptr<CompositeArgument> arg,
                   const int idx_of_inst_param) override {
    arguments.insert({idx_of_inst_param, arg});
  }
  void addIndexMapping(const int idx_1, const int idx_2) override {
    param_idx_to_vector_idx.insert({idx_1, idx_2});
  }

  void applyRuntimeArguments() override;
  const std::vector<std::size_t> bits() override;
  void setBits(const std::vector<std::size_t> bits) override { qbits = bits; }
  std::string getBufferName(const std::size_t bitIdx) override;
  void
  setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override;
  std::vector<std::string> getBufferNames() override { return buffer_names; }
  void setBitExpression(const std::size_t bit_idx,
                        const std::string expr) override {
    bitIdxExpressions.insert({bit_idx, expr});
  }
  std::string getBitExpression(const std::size_t bit_idx) override {
    return bitIdxExpressions.count(bit_idx) ? bitIdxExpressions[bit_idx] : "";
  }

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
    return std::make_shared<CLASS>(*this);                                     \
  }
};

} // namespace quantum
} // namespace xacc
#endif