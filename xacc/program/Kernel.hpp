#ifndef XACC_PROGRAM_KERNEL_HPP_
#define XACC_PROGRAM_KERNEL_HPP_

#include "Function.hpp"

namespace xacc {

/**
 * The Kernel represents an functor to be executed
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
			xacc::tuple_for_each(argsTuple, [&](auto value) {
				parameters.push_back(InstructionParameter(value));
			});

			// Evaluate all Variable Parameters
			function->evaluateVariableParameters(parameters);
		}

		// Execute the Kernel on the Accelerator
		accelerator->execute(buffer, function);
	}

	void prepend(const std::shared_ptr<Function> prependFunction) {
		function->insertInstruction(0, prependFunction);
	}
};

}

#endif
