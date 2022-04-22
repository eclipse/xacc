#pragma once
#include "Instruction.hpp"
// Common interface for modifier circuit class:
// helping other plugins (e.g., Accelerator) to access information
// specific to these modifier block
namespace xacc {
namespace quantum {
struct GateModifier {
  virtual std::shared_ptr<Instruction> getBaseInstruction() const = 0;
};
// We only have control modifier at the moment...
struct ControlModifier : public GateModifier {
  // Returns list of qubits as <qreg name, index> pairs
  virtual std::vector<std::pair<std::string, size_t>>
  getControlQubits() const = 0;
};
} // namespace quantum
} // namespace xacc
