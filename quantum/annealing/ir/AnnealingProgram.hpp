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
#ifndef QUANTUM_AQC_DWFUNCTION_HPP_
#define QUANTUM_AQC_DWFUNCTION_HPP_

#include "CompositeInstruction.hpp"
#include "DWQMI.hpp"
#include "xacc.hpp"
#include "expression_parsing_util.hpp"

namespace xacc {
namespace quantum {

class AnnealingProgram : public CompositeInstruction,
                         public std::enable_shared_from_this<AnnealingProgram> {

protected:
  std::vector<InstPtr> instructions;

  std::vector<std::string> variables{};
  std::shared_ptr<ExpressionParsingUtil> parsingUtil;

  std::string _name;
  std::string tag = "ising";

  void throwIfInvalidInstructionParameter(InstPtr instruction);
  void validateInstructionIndex(const std::size_t idx) {
    if (idx >= instructions.size()) {
      xacc::XACCLogger::instance()->error(
          "\nInvalid instruction index on AnnealingProgram\n"
          "CompositeInstruction:\nnInstructions() = " +
          std::to_string(nInstructions()) + "\nidx = " + std::to_string(idx));
      print_backtrace();
      exit(0);
    }
  }
  void validateInstructionPtr(InstPtr inst) {
    if (std::find(instructions.begin(), instructions.end(), inst) !=
        std::end(instructions)) {
      xacc::XACCLogger::instance()->error(
          "\nInvalid instruction:\nThis instruction pointer already added to "
          "AnnealingProgram.");
      print_backtrace();
      exit(0);
    }
  }

  void errorCircuitParameter() const {
    xacc::XACCLogger::instance()->error(
        "AnnealingProgram Instruction parameter API not implemented.");
    print_backtrace();
    exit(0);
  }

public:
  AnnealingProgram(std::string kernelName) : _name(kernelName) {}

  AnnealingProgram(std::string kernelName, std::vector<std::string> p)
      : _name(kernelName), variables(p) {}

  void applyRuntimeArguments() override {
    for (auto &i : instructions) {
      i->applyRuntimeArguments();
    }
  }

  std::shared_ptr<CompositeInstruction> enabledView() override {
    auto newF = std::make_shared<AnnealingProgram>(_name, variables);
    for (int i = 0; i < nInstructions(); i++) {
      auto inst = getInstruction(i);
      if (inst->isEnabled()) {
        newF->addInstruction(inst);
      }
    }
    return newF;
  }

  // ising or qubo
  const std::string getTag() override { return tag; }
  void setTag(const std::string &t) override {
    tag = t;
    return;
  }

  const int nInstructions() override { return instructions.size(); }
  const int nChildren() override { return instructions.size(); }

  void mapBits(std::vector<std::size_t> bitMap) override {
    xacc::error("AnnealingProgrma.mapBits not implemented");
  }

  void removeDisabled() override {}

  void setBitExpression(const std::size_t bit_idx,
                        const std::string expr) override {}
  std::string getBitExpression(const std::size_t bit_idx) override {
    return "";
  }
  const int nRequiredBits() const override { return 0; }
  InstPtr getInstruction(const std::size_t idx) override {
    validateInstructionIndex(idx);
    return instructions[idx];
  }
  std::vector<InstPtr> getInstructions() override { return instructions; }
  void removeInstruction(const std::size_t idx) override {
    validateInstructionIndex(idx);
    instructions.erase(instructions.begin() + idx);
  }
  void replaceInstruction(const std::size_t idx, InstPtr newInst) override {
    validateInstructionIndex(idx);
    throwIfInvalidInstructionParameter(newInst);
    instructions[idx] = newInst;
  }
  void insertInstruction(const std::size_t idx, InstPtr newInst) override {
    validateInstructionIndex(idx);
    throwIfInvalidInstructionParameter(newInst);
    instructions.insert(instructions.begin() + idx, newInst);
  }

  void addInstruction(InstPtr instruction) override {
    throwIfInvalidInstructionParameter(instruction);
    validateInstructionPtr(instruction);
    instructions.push_back(instruction);
  }
  void addInstructions(std::vector<InstPtr> &insts) override {
    for (auto &i : insts)
      addInstruction(i);
  }
  void addInstructions(const std::vector<InstPtr> &insts) override {
    for (auto &i : insts)
      addInstruction(i);
  }

  const int depth() override {
    xacc::error("AnnealingProgram graph is undirected, cannot compute depth.");
    return 0;
  }
  void clear() override { instructions.clear(); }

  const std::string persistGraph() override {
    std::stringstream s;
    toGraph()->write(s);
    return s.str();
  }

  std::shared_ptr<Graph> toGraph() override;

  const std::string name() const override { return _name; }
  const std::string description() const override { return ""; }
  void setName(const std::string name) override {}
  const std::vector<std::size_t> bits() override {
    return std::vector<std::size_t>{};
  }

  const std::string toString() override {
    std::stringstream ss;
    for (auto i : instructions) {
      ss << i->toString() << ";\n";
    }
    return ss.str();
  }

  const std::size_t nLogicalBits() override { return 0; }
  const std::size_t nPhysicalBits() override { return 0; }
  const std::set<std::size_t> uniqueBits() override { return {}; }

  std::vector<double> getAllBiases() {
    std::vector<double> biases;
    for (auto i : instructions) {
      if (i->bits()[0] == i->bits()[1]) {
        biases.push_back(mpark::get<double>(i->getParameter(0)));
      }
    }

    return biases;
  }

  std::string getBufferName(const std::size_t bitIdx) override { return ""; }
  void
  setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override {}
  std::vector<std::string> getBufferNames() override { return {}; }

  std::vector<double> getAllCouplers() {
    std::vector<double> weights;
    for (auto i : instructions) {
      if (i->bits()[0] != i->bits()[1]) {
        weights.push_back(mpark::get<double>(i->getParameter(0)));
      }
    }

    return weights;
  }

  void persist(std::ostream &outStream) override;
  void load(std::istream &inStream) override;

  const InstructionParameter
  getParameter(const std::size_t idx) const override {
    errorCircuitParameter();
    return InstructionParameter(0);
  }
  void setParameter(const std::size_t idx, InstructionParameter &p) override {
    errorCircuitParameter();
  }
  std::vector<InstructionParameter> getParameters() override {
    errorCircuitParameter();
    return {};
  }
  const int nParameters() override {
    errorCircuitParameter();
    return 0;
  }

  bool isParameterized() override { return nParameters() > 0; }

  std::shared_ptr<CompositeInstruction>
  operator()(const std::vector<double> &params) override;

  void setCoefficient(const std::complex<double> c) override {}
  const std::complex<double> getCoefficient() override { return 1.0; }
  const std::vector<std::string> requiredKeys() override { return {}; }
  void setBits(const std::vector<std::size_t> bits) override {}

  bool hasChildren() const override { return !instructions.empty(); }
  bool expand(const HeterogeneousMap &runtimeOptions) override { return true; }
  void addVariable(const std::string variableName) override {
    variables.push_back(variableName);
  }
  void addVariables(const std::vector<std::string> &vars) override {
    variables.insert(variables.end(), vars.begin(), vars.end());
  }
  const std::vector<std::string> getVariables() override { return variables; }
  void replaceVariable(const std::string variable,
                       const std::string newVariable) override {
    std::replace_if(
        variables.begin(), variables.end(),
        [&](const std::string var) { return var == variable; }, newVariable);
  }

  const std::size_t nVariables() override { return variables.size(); }

  const std::string accelerator_signature() override { return "dwave"; }
  void set_accelerator_signature(const std::string signature) override {}

  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<AnnealingProgram>(*this);
  }
  EMPTY_DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc

#endif
