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
#ifndef QUANTUM_GATEQIR_QFUNCTION_HPP_
#define QUANTUM_GATEQIR_QFUNCTION_HPP_

#include "QFunction.hpp"
#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

/**
 * The GateFunction is a QFunction for gate-model
 * quantum computing. It is composed of QInstructions that
 * are themselves derivations of the GateInstruction class.
 */
class GateFunction: public virtual QFunction {

protected:

	/**
	 * The list of QInstructions that this GateFunction
	 * contains.
	 */
	std::vector<std::shared_ptr<QInstruction>> instructions;

	/**
	 * The name of this function
	 */
	std::string functionName;

	/**
	 * The unique id integer for this function
	 */
	int functionId;

	/**
	 * The list of qubits this function acts on.
	 */
	std::vector<int> qbits;

public:

	/**
	 * The nullary constructor.
	 */
	GateFunction();

	/**
	 * The constructor, takes the function unique id and its name.
	 *
	 * @param id
	 * @param name
	 */
	GateFunction(int id, const std::string name);

	/**
	 * Add an instruction to this quantum
	 * intermediate representation.
	 *
	 * @param instruction
	 */
	virtual void addInstruction(InstPtr instruction);

	/**
	 * Replace the given current quantum instruction
	 * with the new replacingInst quantum Instruction.
	 *
	 * @param currentInst
	 * @param replacingInst
	 */
	virtual void replaceInstruction(InstPtr currentInst,
			InstPtr replacingInst);

	/**
	 * Replace the given current quantum instruction
	 * with the new replacingInst quantum Instruction.
	 *
	 * @param currentInst
	 * @param replacingInst
	 */
	virtual void replaceInstruction(int instId,
			InstPtr replacingInst);

	/**
	 * Return the id of this function
	 * @return
	 */
	virtual const int getId();

	/**
	 * Return the name of this function
	 * @return
	 */
	virtual const std::string getName();

	/**
	 * Return the qubits this function acts on.
	 * @return
	 */
	virtual const std::vector<int> qubits();

	/**
	 * Return an assembly-like string representation for this function .
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string bufferVarName);

	/**
	 * This method should simply be implemented to invoke the
	 * visit() method on the provided QInstructionVisitor.
	 *
	 * @param visitor
	 */
	virtual void accept(QInstructionVisitor& visitor);

	/**
	 * Return the number of instructions in this function.
	 * @return
	 */
	const int nInstructions();
};
}
}
#endif /* QUANTUM_GATE_IR_QFUNCTION_HPP_ */
