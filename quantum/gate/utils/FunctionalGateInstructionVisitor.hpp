/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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

	void visit(Identity& i) {}
	void visit(CZ& i) {}

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
