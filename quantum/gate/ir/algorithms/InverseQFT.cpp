#include "InverseQFT.hpp"
#include "QFT.hpp"
#include "GateFunction.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<Function> InverseQFT::generateAlgorithm(std::vector<int> qubits) {

	// Create a function to return
	auto retFunction = std::make_shared<GateFunction>("inverse_qft");

	// Generate the QFT algorithm and get the
	// individual instructions
	QFT qft;
	auto qftFunction = qft.generateAlgorithm(qubits);
	auto instructions = qftFunction->getInstructions();

	// Reverse those instructions
	std::reverse(instructions.begin(), instructions.end());

	// Add them to the return function
	for (auto i : instructions) {
		retFunction->addInstruction(i);
	}

	return retFunction;
}

}
}

