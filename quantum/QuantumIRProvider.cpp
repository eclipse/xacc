#include "QuantumIRProvider.hpp"

#include "GateFunction.hpp"
#include "GateIR.hpp"
#include "GateInstruction.hpp"

#include "DWFunction.hpp"
#include "DWQMI.hpp"

#include "xacc_service.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(
    const std::string name, std::vector<int> bits,
    std::vector<InstructionParameter> parameters) {
  std::shared_ptr<Instruction> inst;

  if (name == "qmi") {
    inst = std::make_shared<DWQMI>(bits, parameters[0]);

  } else if (name == "anneal") {
    inst = std::make_shared<Anneal>(parameters);
  } else {
    inst = xacc::getService<GateInstruction>(name);

    inst->setBits(bits);
    int idx = 0;
    for (auto &a : parameters) {
      inst->setParameter(idx, a);
      idx++;
    }
  }

  return inst;
}

std::shared_ptr<Function> QuantumIRProvider::createFunction(
    const std::string name, std::vector<int> bits,
    std::vector<InstructionParameter> parameters) {
  if (parameters.empty()) {
    return std::make_shared<GateFunction>(name);
  } else {
    if (parameters[0].toString() == "anneal") {
      return std::make_shared<DWFunction>(name);
    } else {
      return std::make_shared<GateFunction>(name);
    }
  }
}

std::shared_ptr<IR> QuantumIRProvider::createIR() {
  return std::make_shared<GateIR>();
}

std::vector<std::string> QuantumIRProvider::getInstructions() {
  std::vector<std::string> ret;
  for (auto i : xacc::getRegisteredIds<GateInstruction>()) {
    ret.push_back(i);
  }
  ret.push_back("qmi");
  ret.push_back("anneal");
  return ret;
}

} // namespace quantum
} // namespace xacc
