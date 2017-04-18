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
#ifndef QUANTUM_GATE_GATEQIR_PARAMETERIZEDGATEINSTRUCTION_HPP_
#define QUANTUM_GATE_GATEQIR_PARAMETERIZEDGATEINSTRUCTION_HPP_

#include "GateInstruction.hpp"
#include "XACCError.hpp"
#include "XaccUtils.hpp"

namespace xacc {
namespace quantum {

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
			params(std::make_tuple(pars...)) {
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
	virtual const std::string toString(const std::string bufferVarName) {
		auto str = gateName;
		str += "(";
		xacc::tuple_for_each(params, [&](auto element) {
			str += std::to_string(element) + ",";
		});
		str = str.substr(0, str.length() - 1) + ") ";

		for (auto q : qubits()) {
			str += bufferVarName + std::to_string(q) + ",";
		}

		// Remove trailing comma
		str = str.substr(0, str.length() - 1);

		return str;
	}

};
/**
 */
template<typename... Params>
using ParameterizedGateInstructionRegistry = Registry<ParameterizedGateInstruction<Params...>, int, int, std::vector<int>, Params...>;

/**
 */
template<typename T, typename... Params>
class RegisterParameterizedGateInstruction {
public:
	RegisterParameterizedGateInstruction(const std::string& name) {
		ParameterizedGateInstructionRegistry<Params...>::instance()->add(name,
				(std::function<
						std::shared_ptr<xacc::quantum::ParameterizedGateInstruction<Params...>>(int,
								int, std::vector<int>, Params...)>) ([](int id, int layer, std::vector<int> qubits, Params... args) {
					return std::make_shared<T>(id, layer, qubits, args...);
				}));
	}
};

}
}

#endif
