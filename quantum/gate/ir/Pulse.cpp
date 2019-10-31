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
#include "Pulse.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
Pulse::Pulse()
    : gateName("null"), ch(""), parameters({InstructionParameter(0.0)}) {}
Pulse::Pulse(std::string name)
    : gateName(name), ch(""), parameters({InstructionParameter(0.0)}) {}
Pulse::Pulse(std::string name, std::string channel)
    : gateName(name), ch(channel), parameters({InstructionParameter(0.0)}) {}
Pulse::Pulse(std::string name, std::string channel, InstructionParameter &phase,
             std::vector<std::size_t> qubts)
    : gateName(name), ch(channel), parameters({phase}), qbits(qubts) {}

Pulse::Pulse(std::string name, std::string channel, double phase,
             std::vector<std::size_t> qubts)
    : gateName(name), ch(channel), parameters({InstructionParameter(phase)}),
      qbits(qubts) {}

Pulse::Pulse(const Pulse &inst)
    : gateName(inst.gateName), ch(inst.ch), qbits(inst.qbits),
      parameters(inst.parameters), enabled(inst.enabled), samples(inst.samples),
      t0(inst.t0), _duration(inst._duration) {}

const std::string Pulse::name() const { return gateName; }
const std::string Pulse::description() const {
  return "Pulse Instruction base clase.";
}

const std::vector<std::size_t> Pulse::bits() { return qbits; }

void Pulse::mapBits(std::vector<std::size_t> bitMap) {
  xacc::error("Pulse::mapBits not implemented.");
}

const std::string Pulse::toString() {
  return "{\"name\":" + gateName + ", \"channel\":" + ch +
         ", \"time\": " + std::to_string(t0) + ", \"duration\": " + std::to_string(_duration) +
         ", \"phase\": " + parameters[0].toString() + "}";
}

bool Pulse::isEnabled() { return enabled; }
void Pulse::disable() { enabled = false; }
void Pulse::enable() { enabled = true; }

const InstructionParameter Pulse::getParameter(const std::size_t idx) const {
  if (idx > 0) {
    xacc::error(
        "Invalid Parameter requested from Parameterized Pulse Instruction: " +
        std::to_string(idx) + ".");
  }

  return parameters[idx];
}

void Pulse::setParameter(const std::size_t idx, InstructionParameter &p) {
  if (idx > 0) {
    xacc::error(
        "Invalid Parameter requested from Parameterized Pulse Instruction: " +
        std::to_string(idx) + ".");
  }

  parameters[idx] = p;
}

std::vector<InstructionParameter> Pulse::getParameters() { return parameters; }

bool Pulse::isParameterized() { return nParameters() > 0; }
const int Pulse::nParameters() { return parameters.size(); }

} // namespace quantum
} // namespace xacc
