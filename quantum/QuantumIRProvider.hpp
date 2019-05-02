#ifndef QUANTUM_IRPROVIDER_HPP_
#define QUANTUM_IRPROVIDER_HPP_

#include "Identifiable.hpp"

#include "IRProvider.hpp"

namespace xacc {
namespace quantum {

class QuantumIRProvider : public IRProvider {

public:
  virtual std::shared_ptr<Instruction>
  createInstruction(const std::string name, std::vector<int> bits,
                    std::vector<InstructionParameter> parameters =
                        std::vector<InstructionParameter>{});

  virtual std::shared_ptr<Function>
  createFunction(const std::string name, std::vector<int> bits,
                 std::vector<InstructionParameter> parameters =
                     std::vector<InstructionParameter>{});

  virtual std::shared_ptr<IR> createIR();

  virtual std::vector<std::string> getInstructions();

  /**
   * Return the name of this instance.
   *
   * @return name The string name
   */
  virtual const std::string name() const { return "quantum"; }

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  virtual const std::string description() const {
    return "This IRProvider implementation "
           "generates and returns gate and annealing instructions.";
  }
};

} // namespace quantum
} // namespace xacc

#endif
