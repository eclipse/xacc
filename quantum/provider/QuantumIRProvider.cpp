#include "QuantumIRProvider.hpp"

#include "GateIR.hpp"
#include "Circuit.hpp"

#include "DWIR.hpp"
#include "AnnealingProgram.hpp"

#include "CompositeInstruction.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
const int QuantumIRProvider::getNRequiredBits(const std::string name) {
  std::shared_ptr<Instruction> inst;
  if (xacc::hasService<Instruction>(name)) {
    inst = xacc::getService<Instruction>(name);
  } else if (xacc::hasContributedService<Instruction>(name)) {
    inst = xacc::getContributedService<Instruction>(name);
  } else if (xacc::hasCompiled(name)) {
    inst = xacc::getCompiled(name);
  } else {
      xacc::error("Invalid instruction name - " + name);
  }
  return inst->nRequiredBits();
}

std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(const std::string name,
                                                         std::size_t bit) {
  return createInstruction(name, std::vector<std::size_t>{bit});
}

std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(
    const std::string name, std::vector<std::size_t> bits,
    std::vector<InstructionParameter> parameters) {

  std::string iName = name;
  if (name == "CX") {
      iName = "CNOT";
  }
  std::shared_ptr<Instruction> inst;
  if (xacc::hasService<Instruction>(iName)) {
    inst = xacc::getService<Instruction>(iName);
  } else if (xacc::hasContributedService<Instruction>(iName)) {
    inst = xacc::getContributedService<Instruction>(iName);
  } else if (xacc::hasCompiled(iName)) {
    inst = xacc::getCompiled(iName);
  } else {
      xacc::error("Invalid instruction name - " + iName);
  }

  if (!inst->isComposite()) {
    inst->setBits(bits);
    int idx = 0;
    for (auto &a : parameters) {
      inst->setParameter(idx, a);
      idx++;
    }
  }

  return inst;
}

 std::shared_ptr<CompositeInstruction>
 QuantumIRProvider::createComposite(const std::string name,
                  std::vector<std::string> variables, const std::string type) {
  // FIXME, update to handle D-Wave...
  if (type == "circuit") {
  return std::make_shared<xacc::quantum::Circuit>(name,variables);
  } else if (type == "anneal") {
    return std::make_shared<xacc::quantum::AnnealingProgram>(name,variables);
  } else {
      xacc::error("Invalid Composite type, can be circuit or anneal");
      return nullptr;
  }
}

std::shared_ptr<IR> QuantumIRProvider::createIR(const std::string type) {
if (type == "circuit") {
  return std::make_shared<GateIR>();
} else if (type == "anneal") {
    return std::make_shared<DWIR>();
} else {
    xacc::error("Invalid IR type, can be circuit or anneal");
    return nullptr;
}
}

std::vector<std::string> QuantumIRProvider::getInstructions() {
  std::vector<std::string> ret;
  for (auto i : xacc::getRegisteredIds<Instruction>()) {
    ret.push_back(i);
  }
//   ret.push_back("qmi");
//   ret.push_back("anneal");
  return ret;
}

} // namespace quantum
} // namespace xacc
