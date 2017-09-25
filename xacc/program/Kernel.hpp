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
#ifndef XACC_PROGRAM_KERNEL_HPP_
#define XACC_PROGRAM_KERNEL_HPP_

#include "Function.hpp"

namespace xacc {

/**
 * Utility class used to map variadic arguments to
 * a vector of InstructionParameters.
 */
class GetInstructionParametersFunctor {
protected:
	std::vector<InstructionParameter>& params;
public:
	GetInstructionParametersFunctor(std::vector<InstructionParameter>& p) :
			params(p) {
	}
	template<typename T>
	void operator()(const T& t) {
		params.push_back(InstructionParameter(t));
	}
};

/**
 * The Kernel represents a functor to be executed
 * on the attached Accelerator.
 */
template<typename... RuntimeArgs>
class Kernel {

protected:

	/**
	 * The IR Function that this Kernel is executing.
	 */
	std::shared_ptr<Function> function;

	/**
	 * The Accelerator to execute this Kernel on
	 */
	std::shared_ptr<Accelerator> accelerator;

public:

	/**
	 * The Constructor
	 * @param acc Accelerator to execute on
	 * @param f Function to execute
	 */
	Kernel(std::shared_ptr<Accelerator> acc, std::shared_ptr<Function> f) :
			function(f), accelerator(acc) {
	}

	/**
	 * The copy constructor
	 * @param k Other kernel to copy
	 */
	Kernel(const Kernel& k) : function(k.function), accelerator(k.accelerator) {}

	/**
	 * Execute this Kernel on the given AcceleratorBuffer and
	 * with the provided list of runtime arguments.
	 *
	 * @param buffer AcceleratorBuffer to execute on.
	 * @param args Runtime parameters
	 */
	void operator()(std::shared_ptr<AcceleratorBuffer> buffer,
			RuntimeArgs ... args) {
		if (sizeof...(RuntimeArgs) > 0) {
			// Store the runtime parameters in a tuple
			auto argsTuple = std::make_tuple(args...);

			// Loop through the tuple, and add InstructionParameters
			// to the parameters vector.
			std::vector<InstructionParameter> parameters;
			GetInstructionParametersFunctor f(parameters);
			xacc::tuple_for_each(argsTuple, f);

			// Evaluate all Variable Parameters
			function->evaluateVariableParameters(parameters);
		}

		// Execute the Kernel on the Accelerator
		accelerator->execute(buffer, function);
	}

	/**
	 * Execute this Kernel on the given AcceleratorBuffer and with
	 * the provided vector of InstructionParameters.
	 *
	 * @param buffer The AcceleratorBuffer
	 * @param parameters The runtime parameters
	 */
	void operator()(std::shared_ptr<AcceleratorBuffer> buffer, std::vector<InstructionParameter> parameters) {
		function->evaluateVariableParameters(parameters);
		accelerator->execute(buffer, function);
	}

	/**
	 * Evaluate any string variable runtime parameters in
	 * this Kernel's Function.
	 *
	 * @param parameters The concrete runtime values.
	 */
	void evaluateParameters(std::vector<InstructionParameter> parameters) {
		function->evaluateVariableParameters(parameters);
	}

	/**
	 * Return the number of Kernel runtime parameters
	 * @return nParams The number of Parameters
	 */
	const int getNumberOfKernelParameters() {
		return function->nParameters();
	}

	/**
	 * Return the IR Function wrapped by this Kernel.
	 *
	 * @return function The IR Function
	 */
	std::shared_ptr<Function> getIRFunction() {
		return function;
	}
};

}

#endif
