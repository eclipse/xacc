#ifndef RBM_IR_HPP_
#define RBM_IR_HPP_

#include "IRGenerator.hpp"

namespace xacc {

namespace rbm {

/**
 */
class RBMGenerator: public xacc::IRGenerator {

public:

	/**
	 * Implementations of this method generate a Function IR
	 * instance corresponding to the implementation's modeled
	 * algorithm. The algorithm is specified to operate over the
	 * provided AcceleratorBuffer and can take an optional
	 * vector of InstructionParameters.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	std::shared_ptr<Function> generate(
			std::shared_ptr<AcceleratorBuffer> buffer,
			std::vector<InstructionParameter> parameters = std::vector<
					InstructionParameter> { }) override;

	std::shared_ptr<Function> generate(
			std::map<std::string, InstructionParameter>& parameters) override ;


	const std::string name() const override {
		return "rbm";
	}

	const std::string description() const override {
		return "";
	}
};

}

}

#endif