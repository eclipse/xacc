#include "QuantumIRProvider.hpp"

// #include "GateFunction.hpp"
#include "GateIR.hpp"
#include "Circuit.hpp"

// #include "GateInstruction.hpp"

// #include "DWFunction.hpp"
// #include "DWQMI.hpp"

#include "CompositeInstruction.hpp"
#include "xacc_service.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(const std::string name,
                                                         std::size_t bit) {
  return createInstruction(name, std::vector<std::size_t>{bit});
}

std::shared_ptr<Instruction> QuantumIRProvider::createInstruction(
    const std::string name, std::vector<std::size_t> bits,
    std::vector<InstructionParameter> parameters) {

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
                  std::vector<std::string> variables) {
  // FIXME, update to handle D-Wave...
  auto f = std::make_shared<xacc::quantum::Circuit>(name,variables);
  return f;
}

std::shared_ptr<IR> QuantumIRProvider::createIR() {
  return std::make_shared<GateIR>();
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
