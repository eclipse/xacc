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
#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

GateInstruction::GateInstruction() {}
GateInstruction::GateInstruction(std::string name) : gateName(name) {}
GateInstruction::GateInstruction(std::string name,
                                 std::vector<InstructionParameter> params)
    : gateName(name), parameters(params) { describeOptions(); }
GateInstruction::GateInstruction(std::string name, std::vector<int> qubts)
    : gateName(name), qbits(qubts),
      parameters(std::vector<InstructionParameter>{}) { describeOptions(); }
GateInstruction::GateInstruction(std::string name, std::vector<int> qubts,
                                 std::vector<InstructionParameter> params)
    : gateName(name), qbits(qubts), parameters(params) { describeOptions(); }
GateInstruction::GateInstruction(const GateInstruction &inst)
    : gateName(inst.gateName), qbits(inst.qbits), parameters(inst.parameters),
      enabled(inst.enabled) { describeOptions(); }

const std::string GateInstruction::name() const { return gateName; }
const std::string GateInstruction::description() const {
  return "Gate model Instruction base clase.";
}

const std::vector<int> GateInstruction::bits() { return qbits; }

void GateInstruction::mapBits(std::vector<int> bitMap) {
  for (int i = 0; i < qbits.size(); i++) {
    qbits[i] = bitMap[qbits[i]];
  }
}

const std::string GateInstruction::toString() {
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

  for (auto q : bits()) {
    str += bufferVarName + std::to_string(q) + ",";
  }

  // Remove trailing comma
  str = str.substr(0, str.length() - 1);

  return str;
}

const std::string GateInstruction::toString(const std::string &bufferVarName) {
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

  for (auto q : bits()) {
    str += bufferVarName + std::to_string(q) + ",";
  }

  // Remove trailing comma
  str = str.substr(0, str.length() - 1);

  return str;
}

bool GateInstruction::isEnabled() { return enabled; }
void GateInstruction::disable() { enabled = false; }
void GateInstruction::enable() { enabled = true; }

InstructionParameter GateInstruction::getParameter(const int idx) const {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error(
        "Invalid Parameter requested from Parameterized Gate Instruction: " +
        std::to_string(idx) + ".");
  }

  return parameters[idx];
}

void GateInstruction::setParameter(const int idx, InstructionParameter &p) {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error(
        "Invalid Parameter requested from Parameterized Gate Instruction: " +
        std::to_string(idx) + ".");
  }

  parameters[idx] = p;
}

std::vector<InstructionParameter> GateInstruction::getParameters() {
  return parameters;
}

bool GateInstruction::isParameterized() { return nParameters() > 0; }
const int GateInstruction::nParameters() { return parameters.size(); }

bool GateInstruction::hasOptions() { return !options.empty(); }
void GateInstruction::setOption(const std::string optName,
                                InstructionParameter option) {
  if (options.count(optName)) {
    options[optName] = option;
  } else {
    options.insert({optName, option});
  }
  return;
}

InstructionParameter GateInstruction::getOption(const std::string optName) {
  if (!options.count(optName))
    XACCLogger::instance()->error("Invalid Instruction option name: " +
                                  optName);

  return options[optName];
}

std::map<std::string, InstructionParameter> GateInstruction::getOptions() {
  return options;
}
} // namespace quantum
} // namespace xacc
