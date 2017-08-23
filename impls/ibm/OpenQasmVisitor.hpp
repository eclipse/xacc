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
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTI_OpenQasmVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTI_OpenQasmVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include <boost/math/constants/constants.hpp>

namespace xacc {
namespace quantum {

/**
 * The OpenQasmVisitor is an InstructionVisitor that visits
 * quantum gate instructions and creates an equivalent
 * OpenQasm string that can be executed by the Rigetti
 * superconducting quantum computer.
 *
 */
class OpenQasmVisitor: public AllGateVisitor {
protected:

	constexpr static double pi = boost::math::constants::pi<double>();

	/**
	 * Reference to the OpenQasm string
	 * this visitor is trying to construct
	 */
	std::string OpenQasmStr;

	/**
	 * Reference to the classical memory address indices
	 * where measurements are recorded.
	 */
	std::string classicalAddresses;

	std::map<int, int> qubitToClassicalBitIndex;


	int numAddresses = 0;

	int _nQubits;
public:

	OpenQasmVisitor(const int nQubits, bool skipPreamble = false) : _nQubits(nQubits) {
		// Create a qubit registry
		if (!skipPreamble) {
			OpenQasmStr += "\ninclude \\\"qelib1.inc\\\";\nqreg q[" + std::to_string(nQubits) + "];\n";
		}
	}

	/**
	 * Visit hadamard gates
	 */
	void visit(Hadamard& h) {
		std::stringstream ss;
		ss << "h q[" << h.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	/**
	 * Visit CNOT gates
	 */
	void visit(CNOT& cn) {
		std::stringstream ss;
		ss << "cx q[" << cn.bits()[0] << "], q[" << cn.bits()[1] << "];\n";
		OpenQasmStr += ss.str();
	}
	/**
	 * Visit X gates
	 */
	void visit(X& x) {
		std::stringstream ss;
		ss << "x q[" << x.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	/**
	 *
	 */
	void visit(Y& y) {
		std::stringstream ss;
		ss << "y q[" << y.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	/**
	 * Visit Z gates
	 */
	void visit(Z& z) {
		std::stringstream ss;
		ss << "z q[" << z.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	int classicalBitCounter = 0;
	/**
	 * Visit Measurement gates
	 */
	void visit(Measure& m) {
		std::stringstream ss;
		ss << "creg c" << classicalBitCounter << "[1];\n";
		ss << "measure q[" << m.bits()[0] << "] -> c" << classicalBitCounter << "[0];\n";
		OpenQasmStr += ss.str();
		qubitToClassicalBitIndex.insert(std::make_pair(m.bits()[0], classicalBitCounter));
		classicalBitCounter++;
	}

	/**
	 * Visit Conditional functions
	 */
	void visit(ConditionalFunction& c) {
		std::stringstream ss;
		auto visitor = std::make_shared<OpenQasmVisitor>(_nQubits, true);
		auto classicalBitIdx = qubitToClassicalBitIndex[c.getConditionalQubit()];

		ss << "if (c" << classicalBitIdx << " == 1) ";

		if (c.nInstructions() > 1) XACCError("IBM only supports single conditional operations.");

		for (auto inst : c.getInstructions()) {
			inst->accept(visitor);
		}

		ss << visitor->getOpenQasmString();

		OpenQasmStr += ss.str();
	}

	void visit(Rx& rx) {
		std::stringstream ss;
		auto angleStr = boost::lexical_cast<std::string>(rx.getParameter(0));
		ss << "u3(" << angleStr << ", " << (-pi/2.0) << ", " << (pi/2.0) << ") q[" << rx.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	void visit(Ry& ry) {
		std::stringstream ss;
		auto angleStr = boost::lexical_cast<std::string>(ry.getParameter(0));
		ss << "u3(" << angleStr << ", 0, 0) q[" << ry.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	void visit(Rz& rz) {
		std::stringstream ss;
		auto angleStr = boost::lexical_cast<std::string>(rz.getParameter(0));
		ss << "u1(" << angleStr << ") q[" << rz.bits()[0] << "];\n";
		OpenQasmStr += ss.str();
	}

	void visit(CPhase& cp) {
	}

	void visit(Swap& s) {
//		OpenQasmStr += "SWAP " + std::to_string(s.bits()[0]) + " " + std::to_string(s.bits()[1]) + "\n";
	}

	void visit(GateFunction& f) {
		return;
	}
	/**
	 * Return the OpenQasm string
	 */
	std::string getOpenQasmString() {
		return OpenQasmStr;
	}

	/**
	 * Return the classical measurement indices
	 * as a json int array represented as a string.
	 */
	std::string getClassicalAddresses() {
		auto retStr = classicalAddresses.substr(0, classicalAddresses.size() - 2);
		return "[" + retStr + "]";
	}

	int getNumberOfAddresses() {
		return classicalBitCounter-1;
	}
	/**
	 * The destructor
	 */
	virtual ~OpenQasmVisitor() {}
};


}
}

#endif
