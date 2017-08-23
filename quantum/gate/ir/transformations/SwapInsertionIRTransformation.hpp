#ifndef QUANTUM_GATE_IR_TRANSFORMATIONS_SWAPINSERTIONIRTRANSFORMATION_HPP_
#define QUANTUM_GATE_IR_TRANSFORMATIONS_SWAPINSERTIONIRTRANSFORMATION_HPP_

#include "Accelerator.hpp"
#include "IRTransformation.hpp"
#include "GateQIR.hpp"

namespace xacc {
namespace quantum {

class SwapInsertionIRTransformation : public IRTransformation {

protected:

	std::shared_ptr<AcceleratorGraph> graph;

public:

	SwapInsertionIRTransformation(std::shared_ptr<AcceleratorGraph> g) :graph(g) {}

	virtual std::shared_ptr<IR> transform(std::shared_ptr<IR> ir);

	virtual const std::string name() const {
		return "swap-insertion";
	}

	virtual const std::string description() const {
		return "";
	}
};
}
}



#endif
