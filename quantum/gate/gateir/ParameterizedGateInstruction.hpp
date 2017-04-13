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
#ifndef QUANTUM_GATE_GATEIR_PARAMETERIZEDGATEINSTRUCTION_HPP_
#define QUANTUM_GATE_GATEIR_PARAMETERIZEDGATEINSTRUCTION_HPP_

#include "GateInstruction.hpp"
#include "XACCError.hpp"

namespace xacc {
namespace quantum {
template<typename ... InstructionParameter>
class ParameterizedGateInstruction: public virtual GateInstruction {
protected:
	std::tuple<InstructionParameter...> params;

public:
	ParameterizedGateInstruction(int id, int layer, std::string name,
			std::vector<int> qubts, InstructionParameter ... pars) :
			GateInstruction(id, layer, name, qubts), params(
					std::make_tuple(pars...)) {
	}

	auto getParameter(const std::size_t idx) {
		if(idx + 1 > sizeof...(InstructionParameter)) {
			XACCError("Invalid Parameter requested from Parameterized Gate Instruction.");
		}
		return xacc::runtime_get(params, idx);
	}

	virtual const std::string toString(const std::string bufferVarName) {
		auto str = gateName;
		str += "(";
		xacc::for_each(params, [&](auto element) {
			str += std::to_string(element) + ",";
		});
		str += str.substr(0, str.length() - 1) + ") ";

		for (auto q : qubits()) {
			str += bufferVarName + std::to_string(q) + ",";
		}

		// Remove trailing comma
		str = str.substr(0, str.length() - 1);

		return str;
	}

};
}
}

#endif /* QUANTUM_GATE_GATEIR_PARAMETERIZEDGATEINSTRUCTION_HPP_ */
