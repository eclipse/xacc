#include "ConditionalFunction.hpp"
namespace xacc {
namespace quantum {

ConditionalFunction::ConditionalFunction(int qbit) :
		GateFunction("conditional_" + std::to_string(qbit)), qbitIdx(qbit) {
}

void ConditionalFunction::addInstruction(InstPtr instruction) {
	instruction->disable();
	instructions.push_back(instruction);
}

const int ConditionalFunction::getConditionalQubit() {
	return qbitIdx;
}

void ConditionalFunction::evaluate(const int accBitState) {
	if (accBitState == 1) {
		for (auto i : instructions) {
			i->enable();
		}
	}
}

const std::string ConditionalFunction::toString(
		const std::string& bufferVarName) {
	return "";
}

//RegisterGateFunction<ConditionalFunction, int> CONDTEMP("Conditional");

}
}
