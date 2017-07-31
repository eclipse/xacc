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
#ifndef QUANTUM_AQC_IR_DWQMI_HPP_
#define QUANTUM_AQC_IR_DWQMI_HPP_

#include "Instruction.hpp"

namespace xacc {

namespace quantum {

/**
 * The DWQMI (D=Wave Quantum Machine Instruction) class is an XACC
 * Instruction that models a logical problem or hardware bias or
 * connection for an optimization problem to be solved on
 * the D-Wave QPU. It keeps track of 2 bits indices, if both
 * are the same index then this DWQMI Instruction models
 * a bias value, and if they are different indices,
 * then this Instruction models a logical coupling.
 *
 * Note that this class can model both problem bias/couplings and
 * hardware bias/couplings. The hardware bias/couplings result from
 * a minor graph embedding computation.
 *
 */
class DWQMI: public Instruction {

protected:

	/**
	 * The qubits involved in this Instruction
	 */
	std::vector<int> qubits;

	/**
	 * The bias or coupling value.
	 */
	InstructionParameter parameter;

	/**
	 * Is this Instruction enabled or disabled.
	 *
	 */
	bool enabled = true;

public:

	/**
	 * The Constructor, takes one qubit
	 * indicating this is a bias value, initialized to 0.0
	 *
	 * @param qbit The bit index
	 */
	DWQMI(int qbit) :
			qubits(std::vector<int> { qbit, qbit }), parameter(0.0) {
	}

	/**
	 * The Constructor, takes one qubit
	 * indicating this is a bias value, but
	 * set to the provided bias.
	 *
	 * @param qbit The bit index
	 * @param param The bias value
	 */
	DWQMI(int qbit, double param) :
			qubits(std::vector<int> { qbit, qbit }), parameter(param) {
	}

	/**
	 * The Constructor, takes two qubit indices
	 * to indicate that this is a coupler, with
	 * value given by the provided parameter.
	 *
	 * @param qbit1 The bit index
	 * @param qbit2 The bit index
	 * @param param The coupling weight
	 */
	DWQMI(int qbit1, int qbit2, double param) :
			qubits(std::vector<int> { qbit1, qbit2 }), parameter(param) {
	}

	/**
	 * Return the name of this Instruction
	 *
	 * @return name The name of this Instruction
	 */
	virtual const std::string getName() {
		return "dw-qmi";
	}
	;

	/**
	 * Persist this Instruction to an assembly-like
	 * string.
	 *
	 * @param bufferVarName The name of the AcceleratorBuffer
	 * @return str The assembly-like string.
	 */
	virtual const std::string toString(const std::string& bufferVarName) {
		std::stringstream ss;
		ss << bits()[0] << " " << bits()[1] << " " << getParameter(0);
		return ss.str();
	}

	/**
	 * Return the indices of the bits that this Instruction
	 * operates on.
	 *
	 * @return bits The bits this Instruction operates on.
	 */
	virtual const std::vector<int> bits() {
		return qubits;
	}

	/**
	 * Return this Instruction's parameter at the given index.
	 *
	 * @param idx The index of the parameter.
	 * @return param The InstructionParameter at the given index.
	 */
	virtual InstructionParameter getParameter(const int idx) const {
		return parameter;
	}

	/**
	 * Return all of this Instruction's parameters.
	 *
	 * @return params This instructions parameters.
	 */
	virtual std::vector<InstructionParameter> getParameters() {
		return std::vector<InstructionParameter> { parameter };
	}

	/**
	 * Set this Instruction's parameter at the given index.
	 *
	 * @param idx The index of the parameter
	 * @param inst The instruction.
	 */
	virtual void setParameter(const int idx, InstructionParameter& inst) {
		parameter = inst;
	}

	/**
	 * Return the number of InstructionParameters this Instruction contains.
	 *
	 * @return nInsts The number of instructions.
	 */
	virtual const int nParameters() {
		return 1;
	}

	/**
	 * Return true if this Instruction is parameterized.
	 *
	 * @return parameterized True if this Instruction has parameters.
	 */
	virtual bool isParameterized() {
		return true;
	}
	/**
	 * Returns true if this Instruction is composite,
	 * ie, contains other Instructions.
	 *
	 * @return isComposite True if this is a composite Instruction
	 */
	virtual bool isComposite() {
		return false;
	}

	/**
	 * Returns true if this Instruction is enabled
	 *
	 * @return enabled True if this Instruction is enabled.
	 */
	virtual bool isEnabled() {
		return enabled;
	}

	/**
	 * Disable this Instruction
	 */

	virtual void disable() {
		enabled = false;
	}

	/**
	 * Enable this Instruction.
	 */
	virtual void enable() {
		enabled = true;
	}

	EMPTY_DEFINE_VISITABLE()
};
}

}

#endif
