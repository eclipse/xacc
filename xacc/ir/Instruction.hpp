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
#include <boost/variant.hpp>
#include "Utils.hpp"
#include "InstructionVisitor.hpp"

namespace xacc {

using InstructionParameter = boost::variant<int, double, float, std::string>;

/**
 * The Instruction interface is the base of all
 * XACC Intermediate Representation Instructions for
 * post-Moore's law accelerated computing. The Instruction, at
 * its core, provides an Instruction name and a set of
 * next-gen bits that the Instruction operates on. Instructions
 * can also be enabled or disabled. Instructions implement
 * BaseInstructionVisitable to enable visitor pattern
 * functionality across all Instruction subclasses.
 *
 */
class Instruction : public BaseInstructionVisitable {

public:

	/**
	 * Return the name of this Instruction
	 *
	 * @return name The name of this Instruction
	 */
	virtual const std::string getName() = 0;

	/**
	 * Persist this Instruction to an assembly-like
	 * string.
	 *
	 * @param bufferVarName The name of the AcceleratorBuffer
	 * @return str The assembly-like string.
	 */
	virtual const std::string toString(const std::string& bufferVarName) = 0;

	/**
	 * Return the indices of the bits that this Instruction
	 * operates on.
	 *
	 * @return bits The bits this Instruction operates on.
	 */
	virtual const std::vector<int> bits() = 0;

	virtual InstructionParameter getParameter(const int idx) = 0;

	virtual void setParameter(const int idx, InstructionParameter& p) = 0;

	virtual const int nParameters() = 0;

	virtual bool isParameterized() {
		return false;
	}
	/**
	 * Returns true if this Instruction is composite,
	 * ie, contains other Instructions.
	 *
	 * @return isComposite True if this is a composite Instruction
	 */
	virtual bool isComposite() { return false; }

	/**
	 * Returns true if this Instruction is enabled
	 *
	 * @return enabled True if this Instruction is enabled.
	 */
	virtual bool isEnabled() { return true; }

	/**
	 * Disable this Instruction
	 */

	virtual void disable() {}

	/**
	 * Enable this Instruction.
	 */
	virtual void enable() {}

	/**
	 * The destructor
	 */
	virtual ~Instruction() {
	}
};

}
#endif
