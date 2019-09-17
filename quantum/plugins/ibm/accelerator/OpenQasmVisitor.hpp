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
#ifndef QUANTUM_GATE_ACCELERATORS_OpenQasmVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_OpenQasmVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "xacc.hpp"

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

	double pi = xacc::constants::pi;

	/**
	 * Reference to the classical memory address indices
	 * where measurements are recorded.
	 */
	std::string classicalAddresses;

	std::map<int, int> qubitToClassicalBitIndex;

	int numAddresses = 0;

	int _nQubits;


    std::string operationsJsonStr = "[";

public:
    bool isIBMAcc = false;

	virtual const std::string name() const {
		return "openqasm-visitor";
	}

	virtual const std::string description() const {
		return "Map XACC IR to OpenQasm.";
	}

	OpenQasmVisitor() : OpenQasmVisitor(16) {
	}

	OpenQasmVisitor(const int nQubits, bool skipPreamble = false) : _nQubits(nQubits) {
		// Create a qubit registry
		if (!skipPreamble) {
			native += "\ninclude \\\"qelib1.inc\\\";\nqreg q[" + std::to_string(nQubits) + "];\n";
		}
	}

	virtual const std::string toString() {
		return native;
	}

	/**
	 * Visit hadamard gates
	 */
	void visit(Hadamard& h) {
		std::stringstream ss, js;
		ss << "u2(" << 0 << ", " << pi << ") q[" << h.bits()[0] << "];\n";
		native += ss.str();

        js << "{\"name\":\"u2\",\"params\": [0.0," << pi << "],\"qubits\":[" << h.bits()[0]<<"]},";
        operationsJsonStr += js.str();
	}

	void visit(Identity& i) {
        std::stringstream js;
		native += "id q[" + std::to_string(i.bits()[0]) + "];\n";
        js << "{\"name\":\"id\",\"params\": [],\"qubits\":[" << i.bits()[0]<<"]},";
        operationsJsonStr += js.str();
	}

	void visit(CZ& cz) {
		xacc::error("cz not supported");
	}

	/**
	 * Visit CNOT gates
	 */
	void visit(CNOT& cn) {
		std::stringstream ss, js;
		ss << "cx q[" << cn.bits()[0] << "], q[" << cn.bits()[1] << "];\n";
		native += ss.str();

        js << "{\"name\":\"cx\",\"params\": [],\"qubits\":[" << cn.bits()[0]<< ", " << cn.bits()[1] << "]},";
        operationsJsonStr += js.str();
	}

	/**
	 * Visit X gates
	 */
	void visit(X& x) {
		std::stringstream ss, js;
		ss << "u3(" << pi << ", " << 0 << ", " << pi << ") q[" << x.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u3\",\"params\": [" << pi << ", " << 0 << ", " << pi << "],\"qubits\":[" << x.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

	/**
	 *
	 */
	void visit(Y& y) {
		std::stringstream ss, js;
		ss << "u3(" << pi << ", " << pi/2.0 << ", " << pi << ") q[" << y.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u3\",\"params\": [" << pi << ", " << pi/2.0 << ", " << pi << "],\"qubits\":[" << y.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

	/**
	 * Visit Z gates
	 */
	void visit(Z& z) {
		std::stringstream ss, js;
		ss << "u1(" << pi << ") q[" << z.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u1\",\"params\": [" << pi << "],\"qubits\":[" << z.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

    void visit(U &u) {
        std::stringstream ss, js;
		auto thetaStr = u.getParameter(0).toString();
        auto phiStr = u.getParameter(1).toString();
		auto lambdaStr = u.getParameter(2).toString();
		ss << "U(" << thetaStr << ", " << phiStr << ", " << lambdaStr << ") q[" << u.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"U\",\"params\": [" << thetaStr << ", " << phiStr << ", " << lambdaStr << "],\"qubits\":[" << u.bits()[0]<< "]},";
        operationsJsonStr += js.str();
    }
	int classicalBitCounter = 0;
	/**
	 * Visit Measurement gates
	 */
	void visit(Measure& m) {
        std::string clbitname = "clbits";
        if (isIBMAcc) clbitname = "memory";
		std::stringstream ss, js;
		ss << "creg c" << classicalBitCounter << "[1];\n";
		ss << "measure q[" << m.bits()[0] << "] -> c" << classicalBitCounter << "[0];\n";
		native += ss.str();
		qubitToClassicalBitIndex.insert(std::make_pair(m.bits()[0], classicalBitCounter));
        js << "{\""+clbitname+"\":[" << classicalBitCounter << "],\"name\":\"measure\",\"qubits\":[" << m.bits()[0]<< "]},";
        operationsJsonStr += js.str();
		classicalBitCounter++;
	}

	void visit(Rx& rx) {
		std::stringstream ss, js;
		auto angleStr = rx.getParameter(0).toString();
		ss << "u3(" << angleStr << ", " << (-pi/2.0) << ", " << (pi/2.0) << ") q[" << rx.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u3\",\"params\": [" << angleStr << ", " << (-pi/2.0) << ", " << pi/2.0 << "],\"qubits\":[" << rx.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

	void visit(Ry& ry) {
		std::stringstream ss, js;
		auto angleStr = ry.getParameter(0).toString();
		ss << "u3(" << angleStr << ", 0, 0) q[" << ry.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u3\",\"params\": [" << angleStr << ", " << 0 << ", " << 0 << "],\"qubits\":[" << ry.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

	void visit(Rz& rz) {
		std::stringstream ss, js;
		auto angleStr = rz.getParameter(0).toString();
		ss << "u1(" << angleStr << ") q[" << rz.bits()[0] << "];\n";
		native += ss.str();
        js << "{\"name\":\"u1\",\"params\": [" << angleStr << "],\"qubits\":[" << rz.bits()[0]<< "]},";
        operationsJsonStr += js.str();
	}

	void visit(CPhase& cp) {
	}

	void visit(Swap& s) {
        CNOT c1(s.bits()), c2(s.bits()[1],s.bits()[0]), c3(s.bits());
        visit(c1);
        visit(c2);
        visit(c3);
	}

	/**
	 * Return the OpenQasm string
	 */
	std::string getOpenQasmString() {
		return native;
	}

    std::string getOperationsJsonString() {
        return operationsJsonStr.substr(0,operationsJsonStr.length()-1)+"]";
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
