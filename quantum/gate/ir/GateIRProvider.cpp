#include "GateIRProvider.hpp"
#include "GateIR.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<Instruction> GateIRProvider::createInstruction(const std::string name,
		std::vector<int> bits, std::vector<InstructionParameter> parameters) {

	std::shared_ptr<GateInstruction> inst =
			xacc::getService<GateInstruction>(name);
	inst->setBits(bits);
	int idx = 0;
	for (auto& a : parameters) {
		inst->setParameter(idx, a);
		idx++;
	}

	return inst;
}

std::shared_ptr<Function> GateIRProvider::createFunction(const std::string name,
			std::vector<int> bits,
			std::vector<InstructionParameter> parameters) {
	 return std::make_shared<GateFunction>(name, parameters);
}

std::shared_ptr<IR> GateIRProvider::createIR() {
	return std::make_shared<GateIR>();
}

std::vector<std::string> GateIRProvider::getInstructions() {
	std::vector<std::string> ret;
	for (auto i : xacc::getRegisteredIds<GateInstruction>()) {
		ret.push_back(i);
	}
	return ret;
}

}
}
