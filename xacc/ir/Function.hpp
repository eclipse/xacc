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
#ifndef XACC_COMPILER_FUNCTION_HPP_
#define XACC_COMPILER_FUNCTION_HPP_
#include <iostream>
#include <list>
#include "Instruction.hpp"
#include <Eigen/Dense>

namespace xacc {

using InstPtr = std::shared_ptr<Instruction>;

/**
 * The Function is an Instruction that contains further
 * child Instructions.
 *
 * @author Alex McCaskey
 */
class Function : public Instruction {
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
	 * Insert a new Instruction at the given index. All previous
	 * instructions are pushed back, ie their new indices are
	 * currentIndex + 1.
	 *
	 * @param idx The index where the new instruction should be inserted
	 * @param newInst The new Instruction to insert.
	 */
	virtual void insertInstruction(const int idx, InstPtr newInst) = 0;

	/**
	 * Add an Instruction to this Function.
	 *
	 * @param instruction The instruction to add.
	 */
	virtual void addInstruction(InstPtr instruction) = 0;

	virtual void addParameter(InstructionParameter instParam) = 0;

	/**
	 * Return true always to indicate that the
	 * Function is composite.
	 *
	 * @return composite True indicating this is a composite Instruction.
	 */
	virtual bool isComposite() { return true; }

//	/**
//	 * This method is used to evaluate this Function's parameterized
//	 * Instructions that have string variable InstructionParameters.
//	 * These parameters are updated with the given runtime parameters.
//	 *
//	 * @param parameters The runtime parameters
//	 */
//	virtual void evaluateVariableParameters(std::vector<InstructionParameter> parameters) = 0;

	virtual std::shared_ptr<Function> operator()(const Eigen::VectorXd& params) = 0;

	/**
	 * The destructor
	 */
	virtual ~Function() {}
};

}
#endif
