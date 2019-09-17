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

namespace xacc {
namespace quantum {

/**
 */
class OpenPulseVisitor: public AllGateVisitor {
protected:

	constexpr static double pi = 3.1415926;
    std::string instructions = "";
public:

	virtual const std::string name() const {
		return "openpulse-visitor";
	}

	virtual const std::string description() const {
		return "Map XACC IR to OpenPulse.";
	}

	OpenPulseVisitor() : OpenPulseVisitor("dummy") {}

	OpenPulseVisitor(const std::string name) {
        native = "{ \"name\": \"" + name + "\", \"instructions\": [";
    }

	const std::string toString() override {
		return native;
	}

	/**
	 * Visit hadamard gates
	 */
	void visit(Hadamard& h) override {
        xacc::error("digital 2 analog not supported for hadamard yet.");
	}

	void visit(Identity& i) override {
	}

	void visit(CZ& cz) override {
		xacc::error("cz not supported");
	}

	/**
	 * Visit CNOT gates
	 */
	void visit(CNOT& cn) override {
        xacc::error("digital 2 analog not supported for CNOT yet.");
	}

	/**
	 * Visit X gates
	 */
	void visit(X& x) override {
        xacc::error("digital 2 analog not supported for x yet.");
	}

	/**
	 *
	 */
	void visit(Y& y) override {
        xacc::error("digital 2 analog not supported for y yet.");
	}

	/**
	 * Visit Z gates
	 */
	void visit(Z& z) override {
        xacc::error("digital 2 analog not supported for z yet.");
	}

    void visit(U &u) override {
        xacc::error("digital 2 analog not supported for u yet.");
    }
	/**

	 * Visit Measurement gates
	 */
	void visit(Measure& m) override {
        xacc::error("digital 2 analog not supported for measure yet.");
	}

	/**
	 * Visit Conditional functions
	 */
	void visit(ConditionalFunction& c) override {
	}

	void visit(Rx& rx) override {
        xacc::error("digital 2 analog not supported for rx yet.");
	}

	void visit(Ry& ry) override {
        xacc::error("digital 2 analog not supported for ry yet.");
	}

	void visit(Rz& rz) override {
        xacc::error("digital 2 analog not supported for rz yet.");
	}

	void visit(CPhase& cp) override {
        xacc::error("digital 2 analog not supported for cphase yet.");
	}

	void visit(Swap& s) override {
        CNOT c1(s.bits()), c2(s.bits()[1],s.bits()[0]), c3(s.bits());
        visit(c1);
        visit(c2);
        visit(c3);
	}

	void visit(GateFunction& f) override {
		return;
	}

    std::string getOpenPulseInstructionsJson() {
        return native.substr(0,native.length()-1) + "]}";
    }
    // append to pulse library function

	/**
	 * The destructor
	 */
	virtual ~OpenPulseVisitor() {}
};


}
}

#endif
