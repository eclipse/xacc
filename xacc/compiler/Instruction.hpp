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
#ifndef XACC_COMPILER_INSTRUCTION_HPP_
#define XACC_COMPILER_INSTRUCTION_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace xacc {

class Instruction;

/**
 * This class is to be subclassed. Subclasses
 * should add appropriate derived instruction
 * visit classes.
 */
class InstructionVisitor {
public:
	virtual void visit(Instruction& inst) = 0;
	virtual ~InstructionVisitor() {}
};

/**
 *
 */
class Instruction {

public:

	/**
	 *
	 * @return
	 */
	virtual const std::string getName() = 0;

	/**
	 *
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) = 0;

	/**
	 *
	 * @return
	 */
	virtual const std::vector<int> bits() = 0;

	virtual bool isComposite() { return false; }

	virtual bool isEnabled() { return true; }

	virtual void disable() {}
	virtual void enable() {}

	/**
	 *
	 * @param visitor
	 */
	virtual void accept(std::shared_ptr<InstructionVisitor> visitor) = 0;

	/**
	 * The destructor
	 */
	virtual ~Instruction() {
	}
};

}
#endif
