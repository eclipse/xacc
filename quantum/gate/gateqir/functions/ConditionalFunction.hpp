#ifndef QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_
#define QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_

#include "GateFunction.hpp"
namespace xacc {
namespace quantum {

class ConditionalFunction: public virtual GateFunction {

protected:

	std::list<InstPtr> conditionalInstructions;

	int qbitIdx;

public:

	ConditionalFunction(int qbit) : GateFunction("conditional_"+std::to_string(qbit)), qbitIdx(qbit) {}

	virtual void addConditionalInstruction(InstPtr inst) {
		conditionalInstructions.push_back(inst);
	}

	const int getConditionalQubit() {
		return qbitIdx;
	}

	void evaluate(const int accBitState) {
		if (accBitState == 1) {
			instructions = conditionalInstructions;
		}
	}

	virtual void accept(std::shared_ptr<InstructionVisitor> visitor) {
	}

	virtual const std::string toString(const std::string& bufferVarName) {
		return "";
	}
};
}
}

#endif /* QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_ */
