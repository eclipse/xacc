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
#ifndef OPENPULSEVISITOR_HPP_
#define OPENPULSEVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "Pulse.hpp"
#include "json.hpp"
#include "xacc_service.hpp"
#include "PulseQObject.hpp"

using nlohmann::json;

namespace xacc {
namespace quantum {

class PulseMappingVisitor : public AllGateVisitor, public InstructionVisitor<xacc::quantum::Pulse> {
public:
  std::shared_ptr<CompositeInstruction> pulseComposite;
  PulseMappingVisitor();
  // Gate visit
  void visit(Hadamard &h) override;
  void visit(CNOT &cnot) override;
  void visit(Rz &rz) override;
  void visit(Ry &ry) override;
  void visit(Rx &rx) override;
  void visit(X &x) override;
  void visit(Y &y) override;
  void visit(Z &z) override;
  void visit(CY &cy) override;
  void visit(CZ &cz) override;
  void visit(Swap &s) override;
  void visit(CRZ &crz) override;
  void visit(CH &ch) override;
  void visit(S &s) override;
  void visit(Sdg &sdg) override;
  void visit(T &t) override;
  void visit(Tdg &tdg) override;
  void visit(CPhase &cphase) override;
  void visit(Measure &measure) override; 
  void visit(Identity &i) override;
  void visit(U &u) override;
  void visit(Pulse &pulse) override { pulseComposite->addInstruction(pulse.clone()); }

private:
  std::string constructPulseCommandDef(xacc::quantum::Gate &in_gate);
};

class OpenPulseVisitor : public BaseInstructionVisitor,
                           public InstructionVisitor<xacc::quantum::Pulse> {
protected:

	constexpr static double pi = 3.1415926;

    bool hasAcquire = false;
public:

    std::vector<xacc::ibm_pulse::Instruction> instructions;
    std::vector<xacc::ibm_pulse::PulseLibrary> library;

	const std::string name() const override {
		return "openpulse-visitor";
	}

	const std::string description() const override{
		return "Map XACC IR to OpenPulse.";
	}

	const std::string toString() override {
		return native;
	}


	void visit(Pulse& i) override {
        xacc::ibm_pulse::Instruction inst;

        inst.set_ch(i.channel());
        inst.set_name(i.name());
        inst.set_phase(i.getParameter(0).as<double>());
        inst.set_t0(i.start());

        std::vector<std::string> builtIns {"fc", "acquire", "parametric_pulse", "delay" };
        if (std::find(builtIns.begin(), builtIns.end(), i.name()) == std::end(builtIns)) {
            // add to default libr
            xacc::ibm_pulse::PulseLibrary lib;
            lib.set_name(i.name());
            lib.set_samples(i.getSamples());
            library.push_back(lib);
        }

        if (i.name() == "acquire") {
            std::vector<int64_t> bs;
            for (auto& b: i.bits()) bs.push_back(b);
            inst.set_qubits(bs);
            inst.set_memory_slot(bs);
            inst.set_duration(i.duration());
        }

        if (i.name() == "delay") {
            inst.set_duration(i.duration());
        }

        if (i.channel()[0] == 'm') {
            // this is a measure channel

        }

        if (i.name() == "parametric_pulse") {
          auto pulseParameters = i.getPulseParams();
          inst.set_pulse_shape(pulseParameters.getString("pulse_shape"));
          inst.set_pulse_params(pulseParameters.getString("parameters_json"));
        }

        instructions.push_back(inst);

	}


	virtual ~OpenPulseVisitor() {}
};
}
}
#endif
