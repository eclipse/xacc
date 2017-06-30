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
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTI_SCAFFOLDIRTOSRCVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTI_SCAFFOLDIRTOSRCVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

namespace xacc {
namespace quantum {

/**
 * The QuilVisitor is an InstructionVisitor that visits
 * quantum gate instructions and creates an equivalent
 * Quil string that can be executed by the Rigetti
 * superconducting quantum computer.
 *
 */
class ScaffoldIRToSrcVisitor: public AllGateVisitor {
protected:

	/**
	 * Reference to the Quil string
	 * this visitor is trying to construct
	 */
	std::string scaffoldStr;

	std::string qubitVarName;

	int nMeasurements = 0;

	std::map<int, int> qubitToClassicalBitIndex;

	void baseInstruction(std::string gateName, std::vector<int> qubits) {
		std::string qubitArgs = "";
		for (auto i : qubits) {
			qubitArgs += qubitVarName + "[" + std::to_string(i) + "],";
		}
		qubitArgs = qubitArgs.substr(0, qubitArgs.size() - 1);
		scaffoldStr += gateName + "(" + qubitArgs + ");\n";
	}

	void baseParameterizedInstruction(std::string gateName, std::vector<int> qubits, std::vector<InstructionParameter> params) {
		std::string qubitArgs = "", paramsStr = "";
		for (auto p : params) {
			paramsStr += boost::lexical_cast<std::string>(p) + ",";
		}
		paramsStr = paramsStr.substr(0, paramsStr.size() - 1);
		for (auto i : qubits) {
			qubitArgs += qubitVarName + "[" + std::to_string(i) + "],";
		}
		qubitArgs = qubitArgs.substr(0, qubitArgs.size() - 1);

		scaffoldStr += gateName + "(" + qubitArgs + "," + paramsStr + ");\n";
	}

public:

	ScaffoldIRToSrcVisitor(std::string var) : qubitVarName(var) {}

	/**
	 * Visit hadamard gates
	 */
	void visit(Hadamard& h) {
		baseInstruction("H", h.bits());
	}

	/**
	 * Visit CNOT gates
	 */
	void visit(CNOT& cn) {
		baseInstruction("CNOT", cn.bits());
	}
	/**
	 * Visit X gates
	 */
	void visit(X& x) {
		baseInstruction("X", x.bits());
	}

	/**
	 *
	 */
	void visit(Y& y) {
		baseInstruction("Y", y.bits());
	}

	/**
	 * Visit Z gates
	 */
	void visit(Z& z) {
		baseInstruction("Z", z.bits());
	}

	/**
	 * Visit Measurement gates
	 */
	void visit(Measure& m) {
		int classicalBitIdx = m.getClassicalBitIndex();
		nMeasurements++;
		scaffoldStr += "creg[" + std::to_string(classicalBitIdx) + "] = ";
		baseInstruction("MeasZ", m.bits());
		qubitToClassicalBitIndex.insert(std::make_pair(m.bits()[0], classicalBitIdx));
	}

	/**
	 * Visit Conditional functions
	 */
	void visit(ConditionalFunction& c) {
		auto visitor = std::make_shared<ScaffoldIRToSrcVisitor>(qubitVarName);
		auto classicalBitIdx = qubitToClassicalBitIndex[c.getConditionalQubit()];
		scaffoldStr += "if (creg[" + std::to_string(classicalBitIdx) + "] == 1) {\n";
		for (auto inst : c.getInstructions()) {
			scaffoldStr += "   ";
			inst->accept(visitor);
		}
		scaffoldStr += visitor->getScaffoldString() + "}\n";
	}

	void visit(Rx& rx) {
		auto params = rx.getParameters();
		baseParameterizedInstruction("Rx", rx.bits(), params);
	}

	void visit(Ry& ry) {
		auto params = ry.getParameters();
		baseParameterizedInstruction("Ry", ry.bits(), params);
	}

	void visit(Rz& rz) {
		auto params = rz.getParameters();
		baseParameterizedInstruction("Rz", rz.bits(), params);
	}

	void visit(CPhase& cp) {
		auto srcQubit = cp.bits()[0];
		auto targetQubit = cp.bits()[1];
		auto params = cp.getParameters();

		// Check if this is a string
		if (params[0].which() == 3) {
			params[0] = "0.5 * " + boost::get<std::string>(params[0]);
		} else {
			params[0] = 0.5 * boost::get<double>(params[0]);
		}

		scaffoldStr += "// BEGIN CPHASE GATE\n";
		baseParameterizedInstruction("Rz", std::vector<int>{targetQubit}, params);
		baseInstruction("CNOT", std::vector<int>{targetQubit, srcQubit});
		// Check if this is a string
		if (params[0].which() == 3) {
			params[0] = "-1 * " + boost::get<std::string>(params[0]);
		} else {
			params[0] = -1 * boost::get<double>(params[0]);
		}

		baseParameterizedInstruction("Rz", std::vector<int>{targetQubit}, params);
		baseInstruction("CNOT", std::vector<int>{targetQubit, srcQubit});

		scaffoldStr += "// END CPHASE GATE\n";

	}

	void visit(Swap& s) {

		auto srcQubit = s.bits()[0];
		auto targetQubit = s.bits()[1];

		scaffoldStr += "// BEGIN SWAP " + std::to_string(srcQubit) + " " + std::to_string(targetQubit) + "\n";
		baseInstruction("CNOT", std::vector<int>{targetQubit, srcQubit});
		baseInstruction("CNOT", std::vector<int>{srcQubit, targetQubit});
		baseInstruction("CNOT", std::vector<int>{targetQubit, srcQubit});
		scaffoldStr += "// END SWAP " + std::to_string(srcQubit) + " " + std::to_string(targetQubit) + "\n";
	}

	void visit(GateFunction& f) {
		return;
	}
	/**
	 * Return the quil string
	 */
	std::string getScaffoldString() {
		if (nMeasurements > 0) {
			scaffoldStr = "cbit creg[" + std::to_string(nMeasurements) + "];\n" + scaffoldStr;
		}

		return scaffoldStr;
	}

	/**
	 * The destructor
	 */
	virtual ~ScaffoldIRToSrcVisitor() {}
};


}
}

#endif
