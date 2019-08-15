/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
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
