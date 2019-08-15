#ifndef RBM_IR_HPP_
#define RBM_IR_HPP_

#include "IRGenerator.hpp"

namespace xacc {

namespace rbm {

/**
 */
class RBMGenerator: public xacc::IRGenerator {

public:

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