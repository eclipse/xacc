#ifndef XACC_PROGRAM_KERNEL_HPP_
#define XACC_PROGRAM_KERNEL_HPP_

#include "Function.hpp"

namespace xacc {

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

	std::shared_ptr<Function> function;

	std::shared_ptr<Accelerator> accelerator;

public:

	Kernel(std::shared_ptr<Accelerator> acc, std::shared_ptr<Function> f) :
			function(f), accelerator(acc) {
	}

	Kernel(const Kernel& k) : function(k.function), accelerator(k.accelerator) {}

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

	void operator()(std::shared_ptr<AcceleratorBuffer> buffer, std::vector<InstructionParameter> parameters) {
		function->evaluateVariableParameters(parameters);
		accelerator->execute(buffer, function);
	}

	void evaluateParameters(std::vector<InstructionParameter> parameters) {
		function->evaluateVariableParameters(parameters);
	}

	void prepend(const std::shared_ptr<Function> prependFunction) {
		function->insertInstruction(0, prependFunction);
	}

	const int getNumberOfKernelParameters() {
		return function->nParameters();
	}

	std::shared_ptr<Function> getIRFunction() {
		return function;
	}
};

}

#endif
