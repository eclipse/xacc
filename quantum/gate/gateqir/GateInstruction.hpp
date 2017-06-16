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

public:

	GateInstruction() = delete;

	GateInstruction(std::vector<int> qubts) :
			gateName("UNKNOWN"), qbits(qubts) {
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
			gateName(name), qbits(qubts) {
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
		auto str = gateName + " ";
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

	DEFINE_VISITABLE()


	/**
	 * The destructor
	 */
	virtual ~GateInstruction() {
	}
};

/**
 * The ParamaterizedGateInstruction is a GateInstruction that is
 * templated on a list of variadic parameters that model the
 * instructions gate parameters. For example, this class could be
 * subclassed to provide a rotation gate with an angle theta
 * (ParamaterizedGateInstruction<double>).
 */
template<typename ... InstructionParameter>
class ParameterizedGateInstruction: public virtual GateInstruction {

protected:

	/**
	 * The paramaters that this gate instruction requires.
	 */
	std::tuple<InstructionParameter...> params;

public:

	/**
	 * The constructor, takes the parameters
	 * @param pars
	 */
	ParameterizedGateInstruction(InstructionParameter ... pars) :
			params(std::make_tuple(pars...)), GateInstruction(std::vector<int>{}) {
	}

	/**
	 * Return the gate parameter at the given index.
	 *
	 * @param idx
	 * @return
	 */
	auto getParameter(const std::size_t idx) {
		if (idx + 1 > sizeof...(InstructionParameter)) {
			XACCError("Invalid Parameter requested from Parameterized Gate Instruction.");
		}
		return xacc::tuple_runtime_get(params, idx);
	}

	/**
	 * Return an assembly-like string representation for this
	 * instruction.
	 *
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) {
		auto str = gateName;
		str += "(";
		xacc::tuple_for_each(params, [&](auto element) {
			str += std::to_string(element) + ",";
		});
		str = str.substr(0, str.length() - 1) + ") ";

		for (auto q : bits()) {
			str += bufferVarName + std::to_string(q) + ",";
		}

		// Remove trailing comma
		str = str.substr(0, str.length() - 1);

		return str;
	}

	DEFINE_VISITABLE()


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
		GateInstructionRegistry::instance()->add(name,
				(std::function<
						std::shared_ptr<xacc::quantum::GateInstruction>(
								std::vector<int>)>) ([](std::vector<int> qubits) {
					return std::make_shared<T>(qubits);
				}));
	}
};

/**
 */
template<typename ... Params>
using ParameterizedGateInstructionRegistry = Registry<ParameterizedGateInstruction<Params...>, std::vector<int>, Params...>;

/**
 */
template<typename T, typename ... Params>
class RegisterParameterizedGateInstruction {
public:
	RegisterParameterizedGateInstruction(const std::string& name) {
		ParameterizedGateInstructionRegistry<Params...>::instance()->add(name,
				(std::function<
						std::shared_ptr<
								xacc::quantum::ParameterizedGateInstruction<
										Params...>>(std::vector<int>, Params...)>) ([](std::vector<int> qubits, Params... args) {
					return std::make_shared<T>(qubits, args...);
				}));
	}
};

}
}

#endif
