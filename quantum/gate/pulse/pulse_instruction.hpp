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
#ifndef IBM_PULSE_INSTRUCTION_HPP_
#define IBM_PULSE_INSTRUCTION_HPP_

#include "Instruction.hpp"
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

class PulseInstruction : public xacc::Instruction {

protected:
  std::string _name;
  bool enabled = true;
  std::vector<std::size_t> qbits;
  // parameters[0] corresponds to channel (string)
  // parameters[1] corresponds to time (int)
  // parameters[2] corresponds to phase (double or string)
  // parameters[3] corresponds to samples (vector<complex>)
  std::vector<InstructionParameter> parameters;

public:
  PulseInstruction();
  PulseInstruction(std::string name) : _name(name) {
    parameters = {std::string(""), (int)0, 0.0,
                  std::vector<std::complex<double>>{}};
  }
  PulseInstruction(std::string name, std::vector<InstructionParameter> params)
      : _name(name), parameters(params) {}
  PulseInstruction(std::string name, std::vector<std::size_t> bits,
                   std::vector<InstructionParameter> params)
      : _name(name), parameters(params), qbits(bits) {}
  PulseInstruction(const PulseInstruction &inst)
      : _name(inst._name), parameters(inst.parameters), qbits(inst.qbits) {}

  const std::string name() const override { return _name; }
  const std::string description() const override { return ""; }

  const std::string toString() override {
    return "{\"name\":" + _name + ", \"channel\":" + parameters[0].toString() +
           ", \"time\": " + parameters[1].toString() +
           ", \"phase\": " + parameters[2].toString() + "}";
  }

  const std::vector<std::size_t> bits() override { return qbits; }
  void setBits(const std::vector<std::size_t> bits) override { qbits = bits; }

  const InstructionParameter
  getParameter(const std::size_t idx) const override {
    return parameters[idx];
  }
  std::vector<InstructionParameter> getParameters() override {
    return parameters;
  }
  void setParameter(const std::size_t idx,
                    InstructionParameter &inst) override {
    parameters[idx] = inst;
  }
  const int nParameters() override { return 4; }
  bool isParameterized() override { return true; }

  void mapBits(std::vector<std::size_t> bitMap) override {}
  bool isComposite() override { return false; }

  bool isEnabled() override { return enabled; }
  void disable() override { enabled = false; }
  void enable() override { enabled = true; }
  const bool isAnalog() const override { return true; }

  const int nRequiredBits() const override { return 1; }

  DEFINE_VISITABLE()

  // #define DEFINE_CLONE(CLASS)
  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<PulseInstruction>(_name, qbits, parameters);
  }
};

} // namespace ibm
} // namespace xacc
#endif