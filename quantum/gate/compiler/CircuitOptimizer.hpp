#ifndef QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_
#define QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_

#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"
#include "GateFunction.hpp"

namespace xacc {
namespace quantum {

class CircuitOptimizer : public IRTransformation {

public:

	CircuitOptimizer() {}

	virtual std::shared_ptr<IR> transform(std::shared_ptr<IR> ir);

	virtual const std::string name() const {
		return "circuit-optimizer";
	}

	virtual const std::string description() const {
		return "";
	}
};
}
}



#endif
