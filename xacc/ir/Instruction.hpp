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
#include <complex>
#include <boost/variant.hpp>
#include "Utils.hpp"
#include "InstructionVisitor.hpp"

namespace xacc {

/**
 * InstructionParameters can be ints, doubles, floats, strings (for variables), or complex
 */
using InstructionParameter = boost::variant<int, double, float, std::string, std::complex<double>>;

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
 * Instruction can also expose 0 to N InstructionParameters.
 * InstructionParameters can be an int, double, float,
 * or string.
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

	/**
	 * Return this Instruction's parameter at the given index.
	 *
	 * @param idx The index of the parameter.
	 * @return param The InstructionParameter at the given index.
	 */
	virtual InstructionParameter getParameter(const int idx) const = 0;

	/**
	 * Return all of this Instruction's parameters.
	 *
	 * @return params This instructions parameters.
	 */
	virtual std::vector<InstructionParameter> getParameters() = 0;

	/**
	 * Set this Instruction's parameter at the given index.
	 *
	 * @param idx The index of the parameter
	 * @param inst The instruction.
	 */
	virtual void setParameter(const int idx, InstructionParameter& isnt) = 0;

	/**
	 * Return the number of InstructionParameters this Instruction contains.
	 *
	 * @return nInsts The number of instructions.
	 */
	virtual const int nParameters() = 0;

	/**
	 * Return true if this Instruction is parameterized.
	 *
	 * @return parameterized True if this Instruction has parameters.
	 */
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
