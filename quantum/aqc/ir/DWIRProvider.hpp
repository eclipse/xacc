#ifndef QUANTUM_GATE_DWIRPROVIDER_HPP_
#define QUANTUM_GATE_DWIRPROVIDER_HPP_

#include "XACC.hpp"
#include "Identifiable.hpp"
#include "DWQMI.hpp"
#include "DWKernel.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace quantum {
class DWIRProvider : public IRProvider {

public:
  std::shared_ptr<Instruction>
  createInstruction(const std::string name, std::vector<int> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{}) override;

  std::shared_ptr<Function>
  createFunction(const std::string name, std::vector<int> bits,
                 std::vector<InstructionParameter> parameters =
                     std::vector<InstructionParameter>{}) override;

  std::shared_ptr<IR> createIR() override;

  std::vector<std::string> getInstructions() override;

  /**
   * Return the name of this instance.
   *
   * @return name The string name
   */
  const std::string name() const override { return "dwave"; }

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  const std::string description() const override {
    return "This IRProvider implementation "
           "generates and returns DWQMI, DWKernel, and DWIR.";
  }
};

} // namespace quantum
} // namespace xacc

#endif
