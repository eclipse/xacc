/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "Gate.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {

Gate::Gate() {}
Gate::Gate(std::string name) : gateName(name) {}
Gate::Gate(std::string name, std::vector<InstructionParameter> params)
    : gateName(name), parameters(params) {}
Gate::Gate(std::string name, std::vector<std::size_t> qubts)
    : gateName(name), qbits(qubts),
      parameters(std::vector<InstructionParameter>{}) {}
Gate::Gate(std::string name, std::vector<std::size_t> qubts,
           std::vector<InstructionParameter> params)
    : gateName(name), qbits(qubts), parameters(params) {}
Gate::Gate(const Gate &inst)
    : gateName(inst.gateName), qbits(inst.qbits), parameters(inst.parameters),
      arguments(inst.arguments), enabled(inst.enabled), buffer_names(inst.buffer_names) {}

const std::string Gate::name() const { return gateName; }
const std::string Gate::description() const {
  return "Gate model Instruction base clase.";
}

std::string Gate::getBufferName(const std::size_t bitIdx) {
  return bitIdx < buffer_names.size() ? buffer_names[bitIdx] : "q";
}

void Gate::setBufferNames(const std::vector<std::string> bufferNamesPerIdx) {
  if (bufferNamesPerIdx.size() != this->nRequiredBits()) {
    xacc::error("Invalid number of buffer names for this instruction: " +
                name() + ", " + std::to_string(bufferNamesPerIdx.size()));
  }
  buffer_names = bufferNamesPerIdx;
}

const std::vector<std::size_t> Gate::bits() { return qbits; }

void Gate::mapBits(std::vector<std::size_t> bitMap) {
  for (int i = 0; i < qbits.size(); i++) {
    if (qbits[i] >= bitMap.size()) {
      xacc::error("Invalid bit mapping for instruction: " + name() +
                  ", bit operand = " + std::to_string(qbits[i]) +
                  ", bit map size = " + std::to_string(bitMap.size()));
    }
    qbits[i] = bitMap[qbits[i]];
  }
}

const std::string Gate::toString() {
  auto bufferVarName = "q";
  auto str = gateName;
  if (!parameters.empty()) {
    str += "(";
    for (auto p : parameters) {
      str += p.toString() + ",";
    }
    str = str.substr(0, str.length() - 1) + ") ";
  } else {
    str += " ";
  }

  int counter = 0;
  for (auto q : bits()) {
    str += (buffer_names.empty() ? "q" : buffer_names[counter]) +
           std::to_string(q) + ",";
    counter++;
  }

  // Remove trailing comma
  str = str.substr(0, str.length() - 1);

  return str;
}

bool Gate::isEnabled() { return enabled; }
void Gate::disable() { enabled = false; }
void Gate::enable() { enabled = true; }

const InstructionParameter Gate::getParameter(const std::size_t idx) const {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error(
        "Invalid Parameter requested from Parameterized Gate (" + name() +
        ") Instruction: " + std::to_string(idx) + ".");
  }

  return parameters[idx];
}

void Gate::setParameter(const std::size_t idx, InstructionParameter &p) {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error(
        "Invalid Parameter being set in Parameterized Gate (" + name() +
        ") Instruction: " + std::to_string(idx) + ".");
  }

  parameters[idx] = p;
}

std::vector<InstructionParameter> Gate::getParameters() { return parameters; }

void Gate::applyRuntimeArguments() {
  if (!parsingUtil) {
    parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  }

  for (auto &kv : arguments) {
    if (kv.second->type.find("std::vector<double>") != std::string::npos) {
      parameters[kv.first] = kv.second->runtimeValue.get<std::vector<double>>(
          INTERNAL_ARGUMENT_VALUE_KEY)[param_idx_to_vector_idx[kv.first]];
    } else {
      double val;
      auto orig = parameters[kv.first].getOriginalExpression();
    //   std::cout << parameters[kv.first].toString() << ", "
    //             << parameters[kv.first].getOriginalExpression() << ", "
    //             << kv.second->name << ", " << kv.second->runtimeValue.get<double>(
    //                                 INTERNAL_ARGUMENT_VALUE_KEY) << "\n";
      if (parsingUtil->evaluate(orig, {kv.second->name},
                                {kv.second->runtimeValue.get<double>(
                                    INTERNAL_ARGUMENT_VALUE_KEY)},
                                val)) {
        parameters[kv.first] = val;
        parameters[kv.first].storeOriginalExpression(orig);
      } else {
        parameters[kv.first] =
            kv.second->runtimeValue.get<double>(INTERNAL_ARGUMENT_VALUE_KEY);
      }
    }
  }
}

bool Gate::isParameterized() { return nParameters() > 0; }
const int Gate::nParameters() { return parameters.size(); }

} // namespace quantum
} // namespace xacc
