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
#include "QuantumIRProvider.hpp"

#include "GateIR.hpp"
#include "Circuit.hpp"

#include "DWIR.hpp"
#include "AnnealingProgram.hpp"

#include "CompositeInstruction.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"
#include "Pulse.hpp"

namespace xacc {
namespace quantum {
const int QuantumIRProvider::getNRequiredBits(const std::string name) {
  std::shared_ptr<Instruction> inst;
  if (xacc::hasService<Instruction>(name)) {
    inst = xacc::getService<Instruction>(name);
  } else if (xacc::hasContributedService<Instruction>(name)) {
    inst = xacc::getContributedService<Instruction>(name);
  } else if (xacc::hasCompiled(name)) {
    inst = xacc::getCompiled(name);
  } else {
      xacc::error("Invalid instruction name - " + name);
  }
  return inst->nRequiredBits();
}

std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(const std::string name,
                                                         std::size_t bit) {
  return createInstruction(name, std::vector<std::size_t>{bit});
}

std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(
    const std::string name, std::vector<std::size_t> bits,
    std::vector<InstructionParameter> parameters, const HeterogeneousMap& analog_options) {
  // We allow create a new Analog/Pulse instruction even if it wasn't registered/contributed
  const bool isAnalogInst = analog_options.size() > 0;
  std::string iName = name;
  if (name == "CX") {
      iName = "CNOT";
  }
  if (name == "QMI" || name == "qmi") {
      iName = "AnnealingInstruction";
  }
  std::shared_ptr<Instruction> inst;
  if (xacc::hasService<Instruction>(iName)) {
    inst = xacc::getService<Instruction>(iName);
  } else if (xacc::hasContributedService<Instruction>(iName)) {
    inst = xacc::getContributedService<Instruction>(iName);
  } else if (xacc::hasCompiled(iName)) {
    inst = xacc::getCompiled(iName);
  } else {
    // Gate instructions must be contributed before use.
    if (!isAnalogInst) {
      xacc::error("Invalid instruction name - " + iName);
    }
  }

  // We want to create an analog instruction which hasn't been registered.
  if (!inst && isAnalogInst) {
    inst = std::make_shared<Pulse>("pulse::" + iName);
  }

  if (!inst->isComposite() || inst->name() == "ifstmt") {
    inst->setBits(bits);
    int idx = 0;
    for (auto &a : parameters) {
      inst->setParameter(idx, a);
      idx++;
    }
  }

  // Set other analog options:
  if (inst->isAnalog()) {
    // Channel info: key = "channel" or "ch"
    if (analog_options.stringExists("channel")) {
      inst->setChannel(analog_options.getString("channel"));
    } else if (analog_options.stringExists("ch")) {
      inst->setChannel(analog_options.getString("ch"));
    }

    if (analog_options.keyExists<std::vector<std::complex<double>>>(
            "samples")) {
      const auto complexSamples =
          analog_options.get<std::vector<std::complex<double>>>("samples");
      std::vector<std::vector<double>> sampleVec;
      sampleVec.reserve(complexSamples.size());
      for (const auto &sample : complexSamples) {
        sampleVec.emplace_back(
            std::vector<double>{sample.real(), sample.imag()});
      }
      inst->setSamples(sampleVec);
    }

    if (analog_options.keyExists<std::vector<double>>("samples")) {
      const auto complexSamples =
          analog_options.get<std::vector<double>>("samples");
      std::vector<std::vector<double>> sampleVec;
      sampleVec.reserve(complexSamples.size());
      for (const auto &sample : complexSamples) {
        sampleVec.emplace_back(std::vector<double>{sample, 0.0});
      }
      inst->setSamples(sampleVec);
    }

    if (analog_options.keyExists<int>("duration")) {
      inst->setDuration(analog_options.get<int>("duration"));
    }
  }

  return inst;
}

 std::shared_ptr<CompositeInstruction>
 QuantumIRProvider::createComposite(const std::string name,
                  std::vector<std::string> variables, const std::string type) {
  // FIXME, update to handle D-Wave...
  if (type == "circuit") {
  return std::make_shared<xacc::quantum::Circuit>(name,variables);
  } else if (type == "anneal") {
    return std::make_shared<xacc::quantum::AnnealingProgram>(name,variables);
  } else {
      xacc::error("Invalid Composite type, can be circuit or anneal");
      return nullptr;
  }
}

std::shared_ptr<IR> QuantumIRProvider::createIR(const std::string type) {
if (type == "circuit") {
  return std::make_shared<GateIR>();
} else if (type == "anneal") {
    return std::make_shared<DWIR>();
} else {
    xacc::error("Invalid IR type, can be circuit or anneal");
    return nullptr;
}
}

std::vector<std::string> QuantumIRProvider::getInstructions() {
  std::vector<std::string> ret;
  for (auto i : xacc::getRegisteredIds<Instruction>()) {
    ret.push_back(i);
  }
//   ret.push_back("qmi");
//   ret.push_back("anneal");
  return ret;
}

} // namespace quantum
} // namespace xacc
