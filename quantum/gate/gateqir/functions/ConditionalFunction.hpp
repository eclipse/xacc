#ifndef QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_
#define QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_

#include "GateFunction.hpp"

namespace xacc {
namespace quantum {

class ConditionalFunction: public virtual GateFunction {

protected:

	int qbitIdx;

public:

	ConditionalFunction(int qbit);

	virtual void addInstruction(InstPtr instruction);

	const int getConditionalQubit();
	void evaluate(const int accBitState);
	virtual void accept(std::shared_ptr<InstructionVisitor> visitor);

	virtual const std::string toString(const std::string& bufferVarName);
};
}
}

#endif
