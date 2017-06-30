/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#ifndef FUNCTIONALGATEINSTRUCTIONVISITOR_HPP_
#define FUNCTIONALGATEINSTRUCTIONVISITOR_HPP_

#include "AllGateVisitor.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

class FunctionalGateInstructionVisitor: public AllGateVisitor {
protected:
	std::function<void(Hadamard&)> hAction;
	std::function<void(CNOT&)> cnotAction;
	std::function<void(X&)> xAction;
	std::function<void(Y&)> yAction;
	std::function<void(Z&)> zAction;
	std::function<void(Measure&)> measureAction;
	std::function<void(ConditionalFunction&)> condAction;
	std::function<void(Rx&)> rxAction;
	std::function<void(Ry&)> ryAction;
	std::function<void(Rz&)> rzAction;
	std::function<void(CPhase&)> cpAction;
	std::function<void(Swap&)> swAction;

public:
	template<typename HF, typename CNF, typename XF, typename YF, typename ZF,
			typename RXF, typename RYF, typename RZF, typename MF, typename CF,
			typename CPF, typename SWAPF>
	FunctionalGateInstructionVisitor(HF h, CNF cn, XF x, YF y, ZF z, RXF rx,
			RYF ry, RZF rz, MF m, CF c, CPF cp, SWAPF sw) :
			hAction(h), cnotAction(cn), xAction(x), yAction(y), zAction(z), measureAction(
					m), condAction(c), rxAction(rx), ryAction(ry), rzAction(rz), cpAction(
					cp), swAction(sw) {
	}

	void visit(Hadamard& h) {
		hAction(h);
	}
	void visit(CNOT& cn) {
		cnotAction(cn);
	}
	void visit(X& x) {
		xAction(x);
	}

	void visit(Y& y) {
		yAction(y);
	}

	void visit(Z& z) {
		zAction(z);
	}
	void visit(Measure& m) {
		measureAction(m);
	}
	void visit(ConditionalFunction& c) {
		condAction(c);
	}

	void visit(Rx& rx) {
		rxAction(rx);
	}
	void visit(Ry& ry) {
		ryAction(ry);
	}

	void visit(CPhase& cp) {
		cpAction(cp);
	}

	void visit(Rz& rz) {
		rzAction(rz);
	}

	void visit(GateFunction& f) {
		return;
	}

	void visit(Swap& s) {
		swAction(s);
	}

	virtual ~FunctionalGateInstructionVisitor() {
	}
};

}
}

#endif
