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
#ifndef QUANTUM_AQC_DWKERNEL_HPP_
#define QUANTUM_AQC_DWKERNEL_HPP_

#include "Function.hpp"
#include "DWQMI.hpp"
#include "XACC.hpp"
#include "exprtk.hpp"
#include <boost/math/constants/constants.hpp>

static constexpr double pi = boost::math::constants::pi<double>();

using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

namespace xacc {
namespace quantum {

/**
 * The DWKernel is an XACC Function that contains
 * DWQMI Instructions.
 */
class DWKernel : public virtual Function,
                 public std::enable_shared_from_this<DWKernel> {

protected:
  std::list<InstPtr> instructions;

  std::vector<InstructionParameter> parameters;

  std::string _name;

public:
  /**
   * The constructor, takes the function unique id and its name.
   *
   *
   * @param id
   * @param name
   */
  DWKernel(std::string kernelName)
      : _name(kernelName), parameters(std::vector<InstructionParameter>{}) {}

  DWKernel(std::string kernelName, std::vector<InstructionParameter> p)
      : _name(kernelName), parameters(p) {}

  std::shared_ptr<Function> enabledView() override {
    auto newF = std::make_shared<DWKernel>(_name, parameters);
    for (int i = 0; i < nInstructions(); i++) {
      auto inst = getInstruction(i);
      if (inst->isEnabled()) {
        newF->addInstruction(inst);
      }
    }
    return newF;
  }

  virtual const int nInstructions() { return instructions.size(); }

  virtual InstPtr getInstruction(const int idx) {
    InstPtr i;
    if (instructions.size() > idx) {
      i = *std::next(instructions.begin(), idx);
    } else {
      xacc::error("DWKernel getInstruction invalid instruction index - " +
                  std::to_string(idx) + ".");
    }
    return i;
  }

  virtual std::list<InstPtr> getInstructions() { return instructions; }

  virtual void removeInstruction(const int idx) {
    instructions.remove(getInstruction(idx));
  }

  virtual const std::string getTag() { return ""; }

  virtual void mapBits(std::vector<int> bitMap) {}

  /**
   * Add an instruction to this quantum
   * intermediate representation.
   *
   * @param instruction
   */
  virtual void addInstruction(InstPtr instruction) {
    instructions.push_back(instruction);
  }

  const int depth() override {
    xacc::error("DWKernel.depth() not implemented.");
  }

  const std::string persistGraph() override { return ""; }
  /**
   * Replace the given current quantum instruction
   * with the new replacingInst quantum Instruction.
   *
   * @param currentInst
   * @param replacingInst
   */
  virtual void replaceInstruction(const int idx, InstPtr replacingInst) {
    std::replace(instructions.begin(), instructions.end(), getInstruction(idx),
                 replacingInst);
  }

  virtual void insertInstruction(const int idx, InstPtr newInst) {
    auto iter = std::next(instructions.begin(), idx);
    instructions.insert(iter, newInst);
  }

  /**
   * Return the name of this function
   * @return
   */
  virtual const std::string name() const { return _name; }

  virtual const std::string description() const { return ""; }
  /**
   * Return the qubits this function acts on.
   * @return
   */
  virtual const std::vector<int> bits() { return std::vector<int>{}; }

  /**
   * Return an assembly-like string representation for this function .
   * @param bufferVarName
   * @return
   */
  virtual const std::string toString(const std::string &bufferVarName) {
    std::stringstream ss;
    for (auto i : instructions) {
      ss << i->toString("") << "\n";
    }
    return ss.str();
  }

  std::vector<double> getAllBiases() {
    std::vector<double> biases;
    for (auto i : instructions) {
      if (i->bits()[0] == i->bits()[1]) {
        biases.push_back(boost::get<double>(i->getParameter(0)));
      }
    }

    return biases;
  }

  std::vector<double> getAllCouplers() {
    std::vector<double> weights;
    for (auto i : instructions) {
      if (i->bits()[0] != i->bits()[1]) {
        weights.push_back(boost::get<double>(i->getParameter(0)));
      }
    }

    return weights;
  }

  virtual InstructionParameter getParameter(const int idx) const {
    return parameters[idx];
  }

  virtual void setParameter(const int idx, InstructionParameter &p) {
    if (idx + 1 > parameters.size()) {
      XACCLogger::instance()->error(
          "DWKernel.setParameter: Invalid Parameter requested.");
    }

    parameters[idx] = p;
  }

  virtual std::vector<InstructionParameter> getParameters() {
    return parameters;
  }

  virtual void addParameter(InstructionParameter instParam) {
    parameters.push_back(instParam);
  }

  virtual bool isParameterized() { return nParameters() > 0; }

  virtual const int nParameters() { return parameters.size(); }

  virtual std::shared_ptr<Function> operator()(const Eigen::VectorXd &params) {
    if (params.size() != nParameters()) {
      xacc::error("Invalid DWKernel evaluation: number "
                  "of parameters don't match. " +
                  std::to_string(params.size()) + ", " +
                  std::to_string(nParameters()));
    }

    Eigen::VectorXd p = params;
    symbol_table_t symbol_table;
    symbol_table.add_constants();
    std::vector<std::string> variableNames;
    std::vector<double> values;
    for (int i = 0; i < params.size(); i++) {
      auto var = boost::get<std::string>(getParameter(i));
      variableNames.push_back(var);
      symbol_table.add_variable(var, p(i));
    }

    auto compileExpression = [&](InstructionParameter &p) -> double {
      auto expression = boost::get<std::string>(p);
      expression_t expr;
      expr.register_symbol_table(symbol_table);
      parser_t parser;
      parser.compile(expression, expr);
      return expr.value();
    };

    auto evaluatedFunction = std::make_shared<DWKernel>("evaled_" + name());
    for (auto inst : getInstructions()) {
      if (inst->isComposite()) {
        // If a Function, call this method recursively
        auto evaled =
            std::dynamic_pointer_cast<Function>(inst)->operator()(params);
        evaluatedFunction->addInstruction(evaled);
      } else {
        if (inst->name() == "dw-qmi") {
          if (inst->getParameter(0).which() == 3) {
            InstructionParameter p = inst->getParameter(0);
            auto val = compileExpression(p);
            InstructionParameter pnew(val);
            auto updatedInst =
                std::make_shared<DWQMI>(inst->bits()[0], inst->bits()[1], pnew);
            evaluatedFunction->addInstruction(updatedInst);
          } else {
            evaluatedFunction->addInstruction(inst);
          }
        } else if (inst->name() == "anneal") {
          std::vector<InstructionParameter> newParams;
          for (int i = 0; i < inst->nParameters(); i++) {
            InstructionParameter p = inst->getParameter(i);
            if (inst->getParameter(i).which() == 3) {
              if (boost::get<std::string>(p) == "forward" |
                  boost::get<std::string>(p) == "reverse") {
                newParams.push_back(p);
              } else {
                auto val = compileExpression(p);
                newParams.push_back(InstructionParameter(val));
              }
            } else {
              newParams.push_back(p);
            }
          }
          auto newAnneal = std::make_shared<Anneal>(newParams);
          evaluatedFunction->addInstruction(newAnneal);
        } else {
          xacc::error("Invalid DW QMI during kernel evaluation - " +
                      inst->name() + ".");
        }
      }
    }

    return evaluatedFunction;
  }

  EMPTY_DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc

#endif
