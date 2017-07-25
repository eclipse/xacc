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
#ifndef QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_
#define QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include "Registry.hpp"
#include <boost/lexical_cast.hpp>

namespace xacc {
namespace quantum {

/**
 */
class GateInstruction: public virtual Instruction {

protected:

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

public:

	GateInstruction() = delete;

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

	/**
	 * Return the instruction name.
	 * @return
	 */
	virtual const std::string getName() {
		return gateName;
	}

	/**
	 * Return the list of qubits this instruction acts on.
	 * @return
	 */
	virtual const std::vector<int> bits() {
		return qbits;
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
			XACCError("Invalid Parameter requested from Parameterized Gate Instruction.");
		}

		return parameters[idx];
	}

	virtual void setParameter(const int idx, InstructionParameter& p) {
		if (idx + 1 > parameters.size()) {
			XACCError("Invalid Parameter requested from Parameterized Gate Instruction.");
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

/**
 */
using GateInstructionRegistry = Registry<GateInstruction, std::vector<int>>;

/**
 */
template<typename T>
class RegisterGateInstruction {
public:
	RegisterGateInstruction(const std::string& name) {
		GateInstructionRegistry::CreatorFunctionPtr f = std::make_shared<
				GateInstructionRegistry::CreatorFunction>(
				[](std::vector<int> qubits) {
					return std::make_shared<T>(qubits);
				});

		GateInstructionRegistry::instance()->add(name, f);
	}
};


}
}

#endif
