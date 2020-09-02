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
#ifndef QUANTUM_PULSE_IR_GATEINSTRUCTION_HPP_
#define QUANTUM_PULSE_IR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

class Pulse : public Instruction {

protected:
  std::string gateName;
  std::vector<std::size_t> qbits;
  bool enabled = true;
  std::vector<InstructionParameter> parameters;

  std::string ch = "";
  std::size_t t0 = 0;
  std::size_t _duration = 0;

  std::vector<std::vector<double>> samples = {};
  HeterogeneousMap pulseParameters;
public:
  Pulse();
  Pulse(std::string name);
  Pulse(std::string name, std::string channel);
  Pulse(std::string name, std::string channel, InstructionParameter &phase,
        std::vector<std::size_t> qubts);
  Pulse(std::string name, std::string channel, double phase,
        std::vector<std::size_t> qubts);

  Pulse(const Pulse &inst);

  void addArgument(std::shared_ptr<CompositeArgument> arg, const int idx_for_param) override {
  }


  void applyRuntimeArguments() override {
     // by default do nothing
  }

  std::string channel() override { return ch; }
  void setChannel(const std::string c) override { ch = c; }
  std::size_t start() override { return t0; }
  void setStart(const std::size_t s) override { t0 = s; }
  std::size_t duration() override { return _duration; }
  void setDuration(const std::size_t d) override { _duration = d; }
  void setSamples(const std::vector<std::vector<double>> s) override {
    samples = s;
    _duration = samples.size();
  }
  std::vector<std::vector<double>> getSamples() override { return samples; }
  void setPulseParams(const HeterogeneousMap &in_pulseParams) override {
    pulseParameters = in_pulseParams;
  }
  HeterogeneousMap getPulseParams() override { return pulseParameters; }

  const std::string name() const override;
  const std::string description() const override;

  const std::string toString() override;

  const std::vector<std::size_t> bits() override;
  void setBits(const std::vector<std::size_t> bits) override { qbits = bits; }
  void setBitExpression(const std::size_t bit_idx, const std::string expr) override {}
  std::string getBitExpression(const std::size_t bit_idx) override {return "";}

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

  const bool isAnalog() const override { return true; }
  const int nRequiredBits() const override { return qbits.size(); }

  std::string getBufferName(const std::size_t bitIdx) override {
      return "";
  }
  std::vector<std::string> getBufferNames() override {return {};}

  void setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override {
      return;
  }
  DEFINE_VISITABLE()

  std::shared_ptr<Instruction> clone() override {
    auto inst = std::make_shared<Pulse>(gateName, ch, parameters[0], qbits);
    inst->setSamples(samples);
    inst->setStart(t0);
    inst->setDuration(_duration);
    inst->setPulseParams(pulseParameters);
    return inst;
  }
};

} // namespace quantum
} // namespace xacc
#endif