/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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
class Instruction : public BaseInstructionVisitable, public Identifiable {

public:

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

	virtual void mapBits(std::vector<int> bitMap) = 0;

	virtual const std::string getTag() = 0;

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

	const std::string getName() const {
		return name();
	}

	/**
	 * The destructor
	 */
	virtual ~Instruction() {
	}
};

}
#endif
