#include "GateInstruction.hpp"
namespace xacc {
namespace quantum {


void GateInstruction::setBits(std::vector<int> bits) {
		qbits = bits;
} 

const std::string GateInstruction::name() const {
		return gateName;
}

const std::string GateInstruction::description() const {
		return "";
}

const std::string GateInstruction::getTag() {
		return "";
}

const std::vector<int> GateInstruction::bits() {
		return qbits;
}

void GateInstruction::mapBits(std::vector<int> bitMap) {
		for (int i = 0; i < qbits.size(); i++) {
			qbits[i] = bitMap[qbits[i]];
		}
}

const std::string GateInstruction::toString(const std::string& bufferVarName) {
		auto str = gateName;
		if (!parameters.empty()) {
			str += "(";
			for (auto p : parameters) {
				str += boost::lexical_cast<std::string>(p) + ",";
		}
			str = str.substr(0, str.length() - 1) + ") ";
		} else {
			str += " ";
		}
		for (auto q : bits()) {
			str += bufferVarName + std::to_string(q) + ",";
		}
		// Remove trailing comma
		str = str.substr(0, str.length() - 1);

		return str;
}

bool GateInstruction::isEnabled() {
		return enabled;
}

void GateInstruction::disable() {
		enabled = false;
}

void GateInstruction::enable() {
		enabled = true;
}
    
    
InstructionParameter GateInstruction::getParameter(const int idx) const {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested from Parameterized Gate Instruction.");
		}

		return parameters[idx];
}

void GateInstruction::setParameter(const int idx, InstructionParameter& p) {
		if (idx + 1 > parameters.size()) {
			XACCLogger::instance()->error("Invalid Parameter requested from Parameterized Gate Instruction.");
		}

		parameters[idx] = p;
	}

std::vector<InstructionParameter> GateInstruction::getParameters() {
		return parameters;
}

bool GateInstruction::isParameterized() {
		return nParameters() > 0;
}

const int GateInstruction::nParameters() {
		return parameters.size();
}
}
}
