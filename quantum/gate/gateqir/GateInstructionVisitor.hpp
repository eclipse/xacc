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
#ifndef QUANTUM_GATE_GATEQIR_GATEINSTRUCTIONVISITOR_HPP_
#define QUANTUM_GATE_GATEQIR_GATEINSTRUCTIONVISITOR_HPP_

#include "Hadamard.hpp"
#include "Measure.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
#include "Z.hpp"
#include "X.hpp"
#include "ConditionalFunction.hpp"
#include "XaccUtils.hpp"

namespace xacc {
namespace quantum {
class GateInstructionVisitor : public virtual InstructionVisitor {
protected:
	std::function<void(Hadamard&)> hAction;
	std::function<void(CNOT&)> cnotAction;
	std::function<void(X&)> xAction;
	std::function<void(Z&)> zAction;
	std::function<void(Measure&)> measureAction;
	std::function<void(ConditionalFunction&)> condAction;

public:
	template<typename HF, typename CNF, typename XF, typename MF, typename ZF, typename CF>
	GateInstructionVisitor(HF h, CNF cn, XF x, MF m, ZF z, CF c) :
			hAction(h), cnotAction(cn), xAction(x), zAction(z), measureAction(m), condAction(c) {
	}

	virtual void visit(Hadamard& h) {
		hAction(h);
	}
	virtual void visit(CNOT& h) {
		cnotAction(h);
	}
	virtual void visit(X& h) {
		xAction(h);
	}
	virtual void visit(Z& h) {
		zAction(h);
	}
	virtual void visit(Measure& h) {
		measureAction(h);
	}
	virtual void visit(ConditionalFunction& c) {
		condAction(c);
	}

	virtual void visit(GateFunction& f) {
		return;
	}
	virtual void visit(Instruction& i) {
		return;
	}
	virtual ~GateInstructionVisitor() {}
};

template<typename T>
std::function<void (T& t)> nullVisit() {
	return [](T& t) { return; };
}

//template<typename LArg, typename HF, typename CNF, typename XF, typename MF,
//		typename GFF, typename InstF>
//auto createGateInstructionVisitor(LArg l, HF h, CNF cn, XF xf, MF mf, GFF gf,
//		InstF in) {
//	return begin_visitor<GateInstructionVisitor, LArg>(l).template on<Hadamard>(
//			std::ref(h)).template on<CNOT>(std::ref(cn)).template on<X>(
//			std::ref(xf)).template on<Measure>(std::ref(mf)).template on<
//			GateFunction>(std::ref(gf)).template on<xacc::Instruction>(
//			std::ref(in)).end_visitor();
//
//}

}
}

#endif
