#ifndef XACC_GENERATORS_EXP_HPP_
#define XACC_GENERATORS_EXP_HPP_

#include "IRGenerator.hpp"

namespace xacc {
    class AcceleratorBuffer;
    class Function;
}

namespace xacc {
namespace generators {
class Exp: public xacc::IRGenerator {
public:

	std::shared_ptr<xacc::Function> generate(
			std::map<std::string, xacc::InstructionParameter>& parameters) override;

    bool validateOptions() override;

	const std::string name() const override {
		return "exp_i_theta";
	}

	const std::string description() const override {
		return "";
	}
};
}
}
#endif