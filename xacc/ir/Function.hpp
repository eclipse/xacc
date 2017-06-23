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
#ifndef XACC_COMPILER_FUNCTION_HPP_
#define XACC_COMPILER_FUNCTION_HPP_
#include <iostream>
#include <list>
#include "Instruction.hpp"

namespace xacc {

using InstPtr = std::shared_ptr<Instruction>;

/**
 * The Function is an Instruction that contains further
 * child Instructions.
 */
class Function : public virtual Instruction {
public:

	/**
	 * Return the number of Instructions that this Function contains.
	 *
	 * @return nInst The number of instructions
	 */
	virtual const int nInstructions() = 0;

	/**
	 * Return the Instruction at the given index.
	 *
	 * @param idx The desired Instruction index
	 * @return inst The instruction at the given index.
	 */
	virtual InstPtr getInstruction(const int idx) = 0;

	/**
	 * Return all Instructions in this Function
	 *
	 * @return insts The list of this Function's Instructions
	 */
	virtual std::list<InstPtr> getInstructions() = 0;

	/**
	 * Remove the Instruction at the given index.
	 *
	 * @param idx The index of the Instruction to remove.
	 */
	virtual void removeInstruction(const int idx) = 0;

	/**
	 * Replace the Instruction at the given index with the given new Instruction.
	 *
	 * @param idx The index of the Instruction to replace.
	 * @param newInst The new Instruction to replace with.
	 */
	virtual void replaceInstruction(const int idx, InstPtr newInst) = 0;

	/**
	 * Add an Instruction to this Function.
	 *
	 * @param instruction The instruction to add.
	 */
	virtual void addInstruction(InstPtr instruction) = 0;

	/**
	 * Return true always to indicate that the
	 * Function is composite.
	 *
	 * @return composite True indicating this is a composite Instruction.
	 */
	virtual bool isComposite() { return true; }

	virtual void evaluateVariableParameters(std::vector<InstructionParameter> parameters) = 0;

	/**
	 * The destructor
	 */
	virtual ~Function() {}
};

}
#endif
