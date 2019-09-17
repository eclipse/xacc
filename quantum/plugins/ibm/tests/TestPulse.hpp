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
#ifndef IMPLS_TESTPULSE_HPP
#define IMPLS_TESTPULSE_HPP

#include "GateInstruction.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
class TestPulse : public GateInstruction {
protected:
  void describeOptions() override {
    if (qbits.empty()) {
      xacc::error("TestPulse Error, invalid list of qubits (empty).");
    }

    // runs pulse from pulse dictionary
    options.insert({"pulse_id", InstructionParameter("null")});
    // run at time t0
    options.insert({"t0", InstructionParameter(0.0)});

    std::string di = "d" + std::to_string(qbits[0]);
    options.insert({"ch", InstructionParameter(di)});
  }

  bool visited = false;

public:
  TestPulse()
      : GateInstruction(name(), std::vector<InstructionParameter>{
                                    InstructionParameter(0.0)}) {}
  TestPulse(std::vector<int> qbits)
      : GateInstruction(
            name(), qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {
    describeOptions();
  }

  bool wasVisited() { return visited; }

  DEFINE_CLONE(TestPulse)
  DEFINE_VISITABLE()

  const int nRequiredBits() const override {return 1;}

  const std::string name() const override {
    // Name used to construct IR instances,
    return "TestPulse";
  }

  const std::string description() const override {
    // Pulse waveform
    return "";
  }
  const bool isAnalog() const override { return true; }

  void customVisitAction(BaseInstructionVisitor &iv) override {

    visited = true;
    // CACHING IMPLEMENTED, need to create qcor::getPulseLibrary function
    std::map<std::string, InstructionParameter>
        pulseLibrary = xacc::getCache("qcor_pulse_library.json"); //= qcor::getPulseLibrary();

    if (!options.count("pulse_id")) {
      // No samples provided, but we have an angle
      // so generate them based on angle

      // GENERATE SAMPLES FROM THETA
      auto theta = getParameter(0).as<double>();
      // FIXME GENERATE new_name_we_create samples
      options.insert({"pulse_id", InstructionParameter("new_name_we_create")});

      // qcor::appendPulse(new_name_we_create_samples);

    } else {
      // user did provide pulse id, so just make
      // sure it exists in the pulseLibrary
      auto pulseId = options["pulse_id"].as<std::string>();
      if (!pulseLibrary.count(pulseId)) {
        xacc::error("This pulse_id (" + pulseId +
                    ") is not in the pulse library cache.");
      }
    }

    // We now have valid ch, t0, and pulse_id,
    // so build up instruction json
    std::stringstream ss;
    ss << "\"ch\": \"" << options["ch"].as<std::string>() << "\", \"name\": \""
       << options["pulse_id"].as<std::string>()
       << "\", \"t0\":" << options["t0"].as<double>();
    iv.getNativeAssembly() += "{ " + ss.str() + " },";
  }

  virtual ~TestPulse() {}
};
} // namespace quantum
} // namespace xacc

#endif
