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
#ifndef QUANTUM_GATE_IR_QINSTRUCTION_HPP_
#define QUANTUM_GATE_IR_QINSTRUCTION_HPP_

#include <memory>
#include <string>
#include <vector>

class QInstructionVisitor;

namespace xacc {
namespace quantum {

/**
 * The QInstruction interface provides the methods necessary
 * to model a quantum instruction. All quantum instructions have a
 * unique integer id, a name, and a list of qubits that the instruction acts on.
 * Implementations of toString must return a assembly-like string
 * representation of the instruction.
 *
 * QInstructions can be visited by the QInstructionVisitor to
 * provide QInstruction-subtype specific information at runtime.
 *
 * QInstructions, together with QFunctions, are structured as a
 * Composite pattern - a tree of quantum instructions, whereby nodes
 * are QFunctions (which contain QInstructions) and leaves are
 * non-composed QInstructions.
 */
class QInstruction {
public:

	/**
	 * Return the id of this instruction.
	 * @return
	 */
	virtual const int getId() = 0;

	/**
	 * Return the name of this instruction.
	 * @return
	 */
	virtual const std::string getName() = 0;

	/**
	 * Return the qubits that this instruction acts on.
	 * @return
	 */
	virtual const std::vector<int> qubits() = 0;

	/**
	 * Return the assembly-like string representation of this
	 * instruction.
	 * @param bufferVarName The buffer variable name.
	 * @return
	 */
	virtual const std::string toString(const std::string bufferVarName) = 0;

	/**
	 * This method should simply be implemented to invoke the
	 * visit() method on the provided QInstructionVisitor.
	 *
	 * @param visitor
	 */
	virtual void accept(QInstructionVisitor& visitor) = 0;

	/**
	 * The destructor
	 */
	virtual ~QInstruction() {
	}
};


}
}
#endif
