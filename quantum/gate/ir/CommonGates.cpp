#include "CommonGates.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {
bool IfStmt::expand(const HeterogeneousMap &runtimeOptions) {
  auto buffer = xacc::getClassicalRegHostBuffer(bufferName);
  // Use the generic CReg deref.
  // this will decay to qreg->operator[] if the bufferName is the buffer name.
  if (buffer->getCregValue(bufferName, bitIdx)) {
    for (auto &i : instructions) {
      i->enable();
    }
  } else {
    // Note: although sub-instructions are initially disabled,
    // we need to disable here as well just in case we run multiple shots
    // and they may be enabled in the previous run.
    disable();
  }
  return true;
}

const std::string IfStmt::toString() {
    std::stringstream retStr;
    retStr << "if (" << bufferName << "[" << bitIdx <<  "]) {\n";

    for (auto i : instructions) {
      if (i->isComposite() &&
          !std::dynamic_pointer_cast<CompositeInstruction>(i)->hasChildren()) {
        retStr << " " << i->name() << "()\n";
      } else {
        retStr << " " << i->toString() << "\n";
      }
    }
    retStr << "}\n";
    return retStr.str();
}
void Measure::setBufferNames(
    const std::vector<std::string> bufferNamesPerIdx) {
  if (bufferNamesPerIdx.size() > nRequiredBits() + 1) {
    xacc::error("Invalid number of buffer names for this instruction: " +
                name() + ", " + std::to_string(bufferNamesPerIdx.size()));
  }
  buffer_names = bufferNamesPerIdx;
}
} // namespace quantum
} // namespace xacc