#ifndef QUANTUM_GATE_GATEINSTRUCTIONSERVICE_HPP_
#define QUANTUM_GATE_GATEINSTRUCTIONSERVICE_HPP_

#include "XACC.hpp"
#include "Identifiable.hpp"
#include "GateInstruction.hpp"
#include "InstructionService.hpp"

namespace xacc {
namespace quantum {
class GateInstructionService: public InstructionService {

public:

	virtual std::shared_ptr<Instruction> create(const std::string name,
			std::vector<int> bits,
			std::vector<InstructionParameter> parameters = std::vector<
					InstructionParameter> { }) {

		std::shared_ptr<GateInstruction> inst =
				ServiceRegistry::instance()->getService<GateInstruction>(name);
		inst->setBits(bits);
		int idx = 0;
		for (auto& a : parameters) {
			inst->setParameter(idx, a);
			idx++;
		}

		return inst;
	}

	virtual std::vector<std::string> getInstructions() {
		std::vector<std::string> ret;
		for (auto i : ServiceRegistry::instance()->getRegisteredIds<GateInstruction>()) {
			ret.push_back(i);
		}
		return ret;
	}

	/**
	 * Return the name of this instance.
	 *
	 * @return name The string name
	 */
	virtual const std::string name() const {
		return "gate";
	}

	/**
	 * Return the description of this instance
	 * @return description The description of this object.
	 */
	virtual const std::string description() const {
		return "";
	}
};

}
}

#endif
