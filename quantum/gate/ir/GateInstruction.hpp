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
#ifndef QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_
#define QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include <boost/lexical_cast.hpp>
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

class GateIRProvider;

/**
 */
class GateInstruction: public virtual Instruction, public Cloneable<GateInstruction> {

protected:

	friend class GateIRProvider;

	/**
	 * Reference to this instructions name
	 */
	std::string gateName;

	/**
	 * Reference to the qubits this instruction acts on
	 */
	std::vector<int> qbits;

	bool enabled = true;

	std::vector<InstructionParameter> parameters;

	virtual void setBits(std::vector<int> bits) {
		qbits = bits;
	}

public:

	GateInstruction() {}

	GateInstruction(std::string name) : gateName(name) {}
	GateInstruction(std::string name, std::vector<InstructionParameter> params) : gateName(name), parameters(params) {}

	GateInstruction(std::vector<int> qubts) :
			gateName("UNKNOWN"), qbits(qubts), parameters(std::vector<InstructionParameter>{}) {
	}

	/**
	 * The constructor, takes the id, name, layer, and qubits
	 * this instruction acts on.
	 *
	 * @param id
	 * @param layer
	 * @param name
	 * @param qubts
	 */
	GateInstruction(std::string name, std::vector<int> qubts) :
			gateName(name), qbits(qubts), parameters(std::vector<InstructionParameter>{}) {
	}

	GateInstruction(std::string name, std::vector<int> qubts,
			std::vector<InstructionParameter> params) :
			gateName(name), qbits(qubts), parameters(params) {
	}

	GateInstruction(const GateInstruction& inst) :
			gateName(inst.gateName), qbits(inst.qbits), parameters(
					inst.parameters), enabled(inst.enabled) {
	}

	/**
	 * Return the instruction name.
	 * @return
	 */
	virtual const std::string name() const {
		return gateName;
	}

	virtual const std::string description() const {
		return "";
	}
	/**
	 * Return the list of qubits this instruction acts on.
	 * @return
	 */
	virtual const std::vector<int> bits() {
		return qbits;
	}

	virtual const std::string getTag() {
		return "";
	}

	virtual void mapBits(std::vector<int> bitMap) {
		for (int i = 0; i < qbits.size(); i++) {
			qbits[i] = bitMap[qbits[i]];
		}
	}

	/**
	 * Return this instruction's assembly-like string
	 * representation.
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) {
		auto str = gateName;
		if (!parameters.empty()) {
			str += "(";
			for (auto p : parameters) {
				str += boost::lexical_cast<std::string>(p) + ",";
			}
			str = str.substr(0, str.length() - 1) + ") ";
		} else {
			str += " ";
		}

		for (auto q : bits()) {
			str += bufferVarName + std::to_string(q) + ",";
		}

		// Remove trailing comma
		str = str.substr(0, str.length() - 1);

		return str;

	}

	virtual bool isEnabled() {
		return enabled;
	}

	virtual void disable() {
		enabled = false;
	}

	virtual void enable() {
		enabled = true;
	}

	virtual InstructionParameter getParameter(const int idx) const {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested from Parameterized Gate Instruction.");
		}

		return parameters[idx];
	}

	virtual void setParameter(const int idx, InstructionParameter& p) {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested from Parameterized Gate Instruction.");
		}

		parameters[idx] = p;
	}

	virtual std::vector<InstructionParameter> getParameters() {
		return parameters;
	}

	virtual bool isParameterized() {
		return nParameters() > 0;
	}

	virtual const int nParameters() {
		return parameters.size();
	}

	DEFINE_VISITABLE()


	/**
	 * The destructor
	 */
	virtual ~GateInstruction() {
	}
};

}
}

#endif
