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

#include "PulseQObject.hpp"

using nlohmann::json;

namespace xacc {
namespace quantum {

class OpenPulseVisitor: public BaseInstructionVisitor,
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

        std::vector<std::string> builtIns {"fc", "acquire"};
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

        if (i.channel()[0] == 'm') {
            // this is a measure channel

        }

        instructions.push_back(inst);

	}


	virtual ~OpenPulseVisitor() {}
};


}
}
#endif
