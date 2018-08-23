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
#ifndef QUANTUM_GATEQIR_QFUNCTION_HPP_
#define QUANTUM_GATEQIR_QFUNCTION_HPP_

#include "Function.hpp"
#include "XACC.hpp"
#include <boost/math/constants/constants.hpp>
#include "exprtk.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace quantum {

static constexpr double pi = boost::math::constants::pi<double>();

using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

/**
 * The GateFunction is a realization of Function for gate-model
 * quantum computing. It is composed of QInstructions that
 * are themselves derivations of the GateInstruction class.
 */
class GateFunction: public virtual Function {

protected:

	/**
	 * The name of this function
	 */
	std::string functionName;

	std::list<InstPtr> instructions;

	std::vector<InstructionParameter> parameters;

	std::string tag = "";

	/**
	 * Map of Instruction Index to ( Instruction's Runtime Parameter Index, Dependent Variable name)
	 */
	std::map<int, std::pair<int, std::string>> cachedVariableInstructions;

public:

	/**
	 * The constructor, takes the function unique id and its name.
	 *
	 * @param id
	 * @param name
	 */
	GateFunction(const std::string& name) :
			functionName(name), parameters(
					std::vector<InstructionParameter> { }) {
	}
	GateFunction(const std::string& name,
			std::vector<InstructionParameter> params) :
			functionName(name), parameters(params) {
	}

	GateFunction(const std::string& name, const std::string& _tag) :
			functionName(name), parameters(
					std::vector<InstructionParameter> { }), tag(_tag) {
	}
	GateFunction(const std::string& name, const std::string& _tag,
			std::vector<InstructionParameter> params) :
			functionName(name), parameters(params), tag(_tag) {
	}

	GateFunction(const GateFunction& other) :
			functionName(other.functionName), parameters(other.parameters) {
	}

	virtual const std::string getTag();

	virtual void mapBits(std::vector<int> bitMap);

	virtual const int nInstructions();

	virtual InstPtr getInstruction(const int idx);

	virtual std::list<InstPtr> getInstructions();
    
    /**
    * Remove an instruction from this
    * quantum intermediate representation
    * 
    * @param instructionID
    */
	virtual void removeInstruction(const int idx);

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
	virtual void replaceInstruction(const int idx, InstPtr replacingInst);

	virtual void insertInstruction(const int idx, InstPtr newInst);

	/**
	 * Return the name of this function
	 * @return
	 */
	virtual const std::string name() const;

	/**
	 * Return the description of this instance
	 * @return description The description of this object.
	 */
	virtual const std::string description() const {
		return "";
	}

	/**
	 * Return the qubits this function acts on.
	 * @return
	 */
	virtual const std::vector<int> bits();

	/**
	 * Return an assembly-like string representation for this function .
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName);

	virtual InstructionParameter getParameter(const int idx) const;

	virtual void setParameter(const int idx, InstructionParameter& p);

	virtual void addParameter(InstructionParameter instParam);

	virtual std::vector<InstructionParameter> getParameters();

	virtual bool isParameterized();

	virtual const int nParameters();

	virtual std::shared_ptr<Function> operator()(const Eigen::VectorXd& params);

	DEFINE_VISITABLE()

};

}
}

#endif
