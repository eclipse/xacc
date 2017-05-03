#include "ConditionalFunction.hpp"
#include "GateInstructionVisitor.hpp"
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

void ConditionalFunction::accept(std::shared_ptr<InstructionVisitor> visitor) {
	auto v = std::dynamic_pointer_cast<GateInstructionVisitor>(visitor);
	if (v) {
		v->visit(*this);
	} else {
		visitor->visit(*this);
	}
}

const std::string ConditionalFunction::toString(
		const std::string& bufferVarName) {
	return "";
}
}
}
