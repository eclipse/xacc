#include "DWIRProvider.hpp"
#include "DWIR.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<Instruction>
DWIRProvider::createInstruction(const std::string name, std::vector<int> bits,
                                std::vector<InstructionParameter> parameters) {
  return std::make_shared<DWQMI>(bits, parameters[0]);
}

std::shared_ptr<Function>
DWIRProvider::createFunction(const std::string name, std::vector<int> bits,
                             std::vector<InstructionParameter> parameters) {
  return std::make_shared<DWKernel>(name, parameters);
}

std::shared_ptr<IR> DWIRProvider::createIR() {
  return std::make_shared<DWIR>();
}

std::vector<std::string> DWIRProvider::getInstructions() { return {"dw-qmi"}; }

} // namespace quantum
} // namespace xacc
