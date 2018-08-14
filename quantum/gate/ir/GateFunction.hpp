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

	virtual const std::string getTag() {
		return tag;
	}

	virtual void mapBits(std::vector<int> bitMap) {
		for (auto i : instructions) {
			i->mapBits(bitMap);
		}
	}

	virtual const int nInstructions() {
		return instructions.size();
	}

	virtual InstPtr getInstruction(const int idx) {
		InstPtr i;
		if (instructions.size() > idx) {
			i = *std::next(instructions.begin(), idx);
		} else {
			xacc::error("GateFunction getInstruction invalid instruction index - " + std::to_string(idx) + ".");
		}
		return i;
	}

	virtual std::list<InstPtr> getInstructions() {
		return instructions;
	}

	virtual void removeInstruction(const int idx) {
		instructions.remove(getInstruction(idx));
	}

	/**
	 * Add an instruction to this quantum
	 * intermediate representation.
	 *
	 * @param instruction
	 */
	virtual void addInstruction(InstPtr instruction) {
        if (instruction->isParameterized() && instruction->nParameters() <= 1){
            xacc::InstructionParameter param = instruction->getParameter(0);
            if (param.which() == 3){
                if (std::find(parameters.begin(), parameters.end(), param) != parameters.end()) { }
                else { 
                    parameters.push_back(param);
                }
            }
        }
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
	virtual const std::string name() const {
		return functionName;
	}

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
	virtual const std::vector<int> bits() {
		return std::vector<int> { };
	}

	/**
	 * Return an assembly-like string representation for this function .
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) {
		std::string retStr = "";
		for (auto i : instructions) {
			retStr += i->toString(bufferVarName) + "\n";
		}
		return retStr;
	}

	virtual InstructionParameter getParameter(const int idx) const {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested.");
		}

		return parameters[idx];
	}

	virtual void setParameter(const int idx, InstructionParameter& p) {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested.");
		}

		parameters[idx] = p;
	}

	virtual void addParameter(InstructionParameter instParam) {
		parameters.push_back(instParam);
	}

	virtual std::vector<InstructionParameter> getParameters() {
		return parameters;
	}

	virtual bool isParameterized() {
		return nParameters() > 0;
	}

	virtual const int nParameters() {
		return parameters.size();
	}

	virtual std::shared_ptr<Function> operator()(const Eigen::VectorXd& params) {
		if (params.size() != nParameters()) {
			xacc::error("Invalid GateFunction evaluation: number "
					"of parameters don't match. " + std::to_string(params.size()) +
					", " + std::to_string(nParameters()));
		}
       
        Eigen::VectorXd p = params;
        symbol_table_t symbol_table;
		symbol_table.add_constants();
		std::vector<std::string> variableNames;
        std::vector<double> values;
		for (int i = 0; i < params.size(); i++) {
            auto var = boost::get<std::string>(getParameter(i));
			variableNames.push_back(var);
            symbol_table.add_variable(var, p(i));
		}

        auto compileExpression = [&](InstructionParameter& p) -> double {
            	auto expression = boost::get<std::string>(p);
				expression_t expr;
				expr.register_symbol_table(symbol_table);
				parser_t parser;
				parser.compile(expression, expr);
                return expr.value();
        };
  
		auto gateRegistry = xacc::getService<IRProvider>("gate");
		auto evaluatedFunction = std::make_shared<GateFunction>("evaled_"+name());

		// Walk the IR Tree, handle functions and instructions differently
		for (auto inst : getInstructions()) {
			if (inst->isComposite()) {
				// If a Function, call this method recursively
				auto evaled =
						std::dynamic_pointer_cast<Function>(inst)->operator()(
								params);
				evaluatedFunction->addInstruction(evaled);
			} else {
				// If a concrete GateInstruction, then check that it
				// is parameterized and that it has a string parameter
				if (inst->isParameterized()
						&& inst->getParameter(0).which() == 3) {
                    InstructionParameter p = inst->getParameter(0);
                    std::stringstream s;
                    s << p;
                    auto val = compileExpression(p);
					InstructionParameter pnew(val);
					auto updatedInst = gateRegistry->createInstruction(inst->name(), inst->bits());
                    updatedInst->setParameter(0, pnew);
					evaluatedFunction->addInstruction(updatedInst);
				} else {
					evaluatedFunction->addInstruction(inst);
				}
			}
		}

		return evaluatedFunction;
	}

	DEFINE_VISITABLE()

};

}
}

#endif
