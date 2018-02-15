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
#ifndef QUANTUM_AQC_DWKERNEL_HPP_
#define QUANTUM_AQC_DWKERNEL_HPP_

#include "Function.hpp"
#include "DWQMI.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {

/**
 * The DWKernel is an XACC Function that contains
 * DWQMI Instructions.
 */
class DWKernel: public virtual Function {

protected:

	std::list<InstPtr> instructions;

	std::string name;

public:

	/**
	 * The constructor, takes the function unique id and its name.
	 *
	 *
	 * @param id
	 * @param name
	 */
	DWKernel(std::string kernelName) : name(kernelName) {
	}

	virtual const int nInstructions() {
		return instructions.size();
	}

	virtual InstPtr getInstruction(const int idx) {
		InstPtr i;
		if (instructions.size() > idx) {
			i = *std::next(instructions.begin(), idx);
		} else {
			xacc::error("DWKernel getInstruction invalid instruction index - " + std::to_string(idx) + ".");
		}
		return i;
	}

	virtual std::list<InstPtr> getInstructions() {
		return instructions;
	}

	virtual void removeInstruction(const int idx) {
		instructions.remove(getInstruction(idx));
	}

	virtual const std::string getTag() {
		return "";
	}

	virtual void mapBits(std::vector<int> bitMap) {
	}

	/**
	 * Add an instruction to this quantum
	 * intermediate representation.
	 *
	 * @param instruction
	 */
	virtual void addInstruction(InstPtr instruction) {
		instructions.push_back(instruction);
	}

	/**
	 * Replace the given current quantum instruction
	 * with the new replacingInst quantum Instruction.
	 *
	 * @param currentInst
	 * @param replacingInst
	 */
	virtual void replaceInstruction(const int idx, InstPtr replacingInst) {
		std::replace(instructions.begin(), instructions.end(),
				getInstruction(idx), replacingInst);
	}

	virtual void insertInstruction(const int idx, InstPtr newInst) {
		auto iter = std::next(instructions.begin(), idx);
		instructions.insert(iter, newInst);
	}

	/**
	 * Return the name of this function
	 * @return
	 */
	virtual const std::string getName() {
		return name;
	}

	/**
	 * Return the qubits this function acts on.
	 * @return
	 */
	virtual const std::vector<int> bits() {
		return std::vector<int> { };
	}

	/**
	 * Return an assembly-like string representation for this function .
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) {
		std::stringstream ss;
		for (auto i : instructions) {
			ss << i->toString("") << "\n";
		}
		return ss.str();
	}

	std::vector<double> getAllBiases() {
		std::vector<double> biases;
		for (auto i : instructions) {
			if (i->bits()[0] == i->bits()[1]) {
				biases.push_back(boost::get<double>(i->getParameter(0)));
			}
		}

		return biases;
	}

	std::vector<double> getAllCouplers() {
		std::vector<double> weights;
		for (auto i : instructions) {
			if (i->bits()[0] != i->bits()[1]) {
				weights.push_back(boost::get<double>(i->getParameter(0)));
			}
		}

		return weights;
	}

	virtual InstructionParameter getParameter(const int idx) const {
		xacc::error("DWKernel does not contain runtime parameters.");
		return InstructionParameter(0);
	}

	virtual void setParameter(const int idx, InstructionParameter& p) {
		xacc::error("DWKernel does not contain runtime parameters.");
	}

	virtual std::vector<InstructionParameter> getParameters() {
		xacc::error("DWKernel does not contain runtime parameters.");
		return {};
	}

	virtual void addParameter(InstructionParameter instParam) {
		xacc::error("DWKernel does not contain runtime parameters.");
	}

	virtual bool isParameterized() {
		return false;
	}

	virtual const int nParameters() {
		return 0;
	}

	virtual void evaluateVariableParameters(std::vector<InstructionParameter> parameters) {
		xacc::error("DWKernel does not contain runtime parameters.");
	}


	EMPTY_DEFINE_VISITABLE()

};

}
}

#endif
