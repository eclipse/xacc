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
#ifndef IBM_PULSE_COMPOSITE_INSTRUCTION_HPP_
#define IBM_PULSE_COMPOSITE_INSTRUCTION_HPP_

#include "CompositeInstruction.hpp"
#include "pulse_instruction.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "expression_parsing_util.hpp"
#include "xacc_service.hpp"

#include <limits>
#include <memory>
#include <stdexcept>
#include <unordered_set>

namespace xacc {
namespace quantum {

class PulseComposite : public CompositeInstruction,
                       public std::enable_shared_from_this<PulseComposite> {

private:
  void validateInstructionIndex(const std::size_t idx) {
    if (idx >= instructions.size()) {
      xacc::XACCLogger::instance()->error(
          "\nInvalid instruction index on CMD_DEF\n"
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
          "CMD_DEF.");
      print_backtrace();
      exit(0);
    }
  }

  void errorCircuitParameter() const {
    xacc::XACCLogger::instance()->error(
        "CMD_DEF Instruction parameter API not implemented.");
    print_backtrace();
    exit(0);
  }

protected:
  std::vector<InstPtr> instructions{};
  std::vector<std::string> variables{};
  std::vector<std::string> _requiredKeys{};

  std::vector<std::size_t> qbits;
  std::string cmdDefName = "";

  std::shared_ptr<ExpressionParsingUtil> parsingUtil;

  std::complex<double> coefficient = 1.0;
  std::string acc_signature = "";

public:
  PulseComposite(const std::string &name) : cmdDefName(name) {}
  PulseComposite(const std::string &name, std::vector<std::string> &vars)
      : cmdDefName(name), variables(vars) {}
  PulseComposite(const std::string &name, std::vector<std::string> &&vars)
      : cmdDefName(name), variables(vars) {}

  PulseComposite(const PulseComposite &other)
      : cmdDefName(other.cmdDefName), variables(other.variables),
        instructions(other.instructions), parsingUtil(other.parsingUtil) {}

  const std::string name() const override { return cmdDefName; }
  const std::string description() const override { return ""; }
  void setName(const std::string name) override { cmdDefName = name; }

  void mapBits(std::vector<std::size_t> bitMap) override {
    for (auto &inst : instructions) {
      inst->mapBits(bitMap);
    }
  }
  void setBits(const std::vector<std::size_t> bits) override { qbits = bits; }
  const std::vector<std::size_t> bits() override { return qbits; }

  const std::string toString() override {
    std::string retStr = "";
    std::stringstream vs;
    vs << getVariables();
    retStr += vs.str() + "\n";
    for (auto i : instructions) {
      if (i->isEnabled()) {
        if (i->isComposite() &&
            !std::dynamic_pointer_cast<CompositeInstruction>(i)
                 ->hasChildren()) {
          retStr += i->name() + "()\n";
        } else {
          retStr += i->toString() + "\n";
        }
      }
    }
    return retStr;
  }

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

  const int nRequiredBits() const override { return 0; }

  void enable() override {
    for (int i = 0; i < nInstructions(); i++) {
      getInstruction(i)->enable();
    }
  }

  void disable() override {
    for (int i = 0; i < nInstructions(); i++) {
      getInstruction(i)->disable();
    }
  }

  const bool isAnalog() const override { return true; }

  void persist(std::ostream &outStream) override {}
  void load(std::istream &inStream) override {}

  const int nInstructions() override { return instructions.size(); }

  const int nChildren() override { return instructions.size(); }

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
    // throwIfInvalidInstructionParameter(newInst);
    instructions[idx] = newInst;
  }
  void insertInstruction(const std::size_t idx, InstPtr newInst) override {
    validateInstructionIndex(idx);
    // throwIfInvalidInstructionParameter(newInst);
    instructions.insert(instructions.begin() + idx, newInst);
  }

  void addInstruction(InstPtr instruction) override {
    // throwIfInvalidInstructionParameter(instruction);
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

  bool hasChildren() const override { return !instructions.empty(); }
  bool expand(const HeterogeneousMap &runtimeOptions) override { return true; }

  const std::vector<std::string> requiredKeys() override {
    return _requiredKeys;
  }

  void addVariable(const std::string variableName) override {
    variables.push_back(variableName);
  }
  void addVariables(const std::vector<std::string> &vars) override {
    variables.insert(variables.end(), vars.begin(), vars.end());
  }
  const std::vector<std::string> getVariables() override {
    // return this guys variables + all sub-node variables
    std::vector<std::string> ret = variables;
    for (auto &i : instructions) {
      if (i->isComposite()) {
        auto vars =
            std::dynamic_pointer_cast<CompositeInstruction>(i)->getVariables();
        ret.insert(ret.end(), vars.begin(), vars.end());
      }
    }
    std::unordered_set<std::string> s(ret.begin(), ret.end());
    ret.assign(s.begin(), s.end());
    return ret;
  }
  void replaceVariable(const std::string variable,
                       const std::string newVariable) override {
    std::replace_if(
        variables.begin(), variables.end(),
        [&](const std::string var) { return var == variable; }, newVariable);
  }

  const std::size_t nVariables() override { return getVariables().size(); }

  const int depth() override { return 0; }
  const std::string persistGraph() override { return ""; }
  std::shared_ptr<Graph> toGraph() override { return nullptr; }

  void flatten() {}

  const std::size_t nLogicalBits() override {
    // n logical should just be the number of
    // unique qbit integers
    std::set<std::size_t> bits;
    InstructionIterator iter(shared_from_this());
    while (iter.hasNext()) {
      auto inst = iter.next();
      if (!inst->isComposite()) {
        for (auto &b : inst->bits()) {
          bits.insert(b);
        }
      }
    }
    return bits.size();
  }

  const std::size_t nPhysicalBits() override {
    // n physical bits should be the largest bit
    // plus 1
    std::size_t maxBit = 0;
    InstructionIterator iter(shared_from_this());
    while (iter.hasNext()) {
      auto inst = iter.next();
      if (!inst->isComposite()) {
        for (auto &b : inst->bits()) {
          if (b > maxBit) {
            maxBit = b;
          }
        }
      }
    }
    return maxBit + 1;
  }

  const std::set<std::size_t> uniqueBits() override {
    std::set<std::size_t> uniqueBits;
    InstructionIterator iter(shared_from_this());
    while (iter.hasNext()) {
      auto next = iter.next();
      if (!next->isComposite()) {
        for (auto &b : next->bits()) {
          uniqueBits.insert(b);
        }
      }
    }

    return uniqueBits;
  }

  std::shared_ptr<CompositeInstruction> enabledView() override {
    auto newF = std::make_shared<PulseComposite>(this->cmdDefName);
    for (int i = 0; i < nInstructions(); i++) {
      auto inst = getInstruction(i);
      if (inst->isEnabled()) {
        newF->addInstruction(inst);
      }
    }
    return newF;
  }

  void setCoefficient(const std::complex<double> c) override {
    coefficient = c;
  }
  const std::complex<double> getCoefficient() override { return coefficient; }

  std::shared_ptr<CompositeInstruction>
  operator()(const std::vector<double> &params) override {
      if (!parsingUtil) {
    parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  }
    variables.erase( std::unique( variables.begin(), variables.end() ), variables.end() );
    std::vector<InstPtr> flatten;
    InstructionIterator iter(shared_from_this());
    while (iter.hasNext()) {
      auto inst = iter.next();
      if (!inst->isComposite()) {
        flatten.emplace_back(inst);
      }
    }

    auto evaluatedCircuit = std::make_shared<PulseComposite>("evaled_" + name());

    // Walk the IR Tree, handle functions and instructions differently
    for (auto inst : flatten) {
      // if (inst->isParameterized()) {
      auto updatedInst =
          std::dynamic_pointer_cast<PulseInstruction>(inst)->clone();

      // Really we only care about the phase...
      // In future we might care about functional form params
      for (int i = 0; i < inst->nParameters(); i++) {
        if (inst->getParameter(i).isVariable()) {
          InstructionParameter p = inst->getParameter(i);
          double val;
          if (parsingUtil->evaluate(p.toString(), variables, params, val)) {
            updatedInst->setParameter(i, val);
          } else {
              updatedInst->setParameter(i, p);
          }
        } else {
          auto a = inst->getParameter(i);
          updatedInst->setParameter(i, a);
        }
        updatedInst->setBits(inst->bits());
      }


      evaluatedCircuit->addInstruction(updatedInst);
    }

    // else {
    //   evaluatedCircuit->addInstruction(inst);
    // }
    return evaluatedCircuit;
  }

  const std::string accelerator_signature() override { return acc_signature; }
  void set_accelerator_signature(const std::string signature) override {
    acc_signature = signature;
  }

  DEFINE_VISITABLE()
  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<PulseComposite>(*this);
  }
  virtual ~PulseComposite() {}
};

} // namespace quantum
} // namespace xacc
#endif