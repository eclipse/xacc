#ifndef QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_
#define QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_

#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"
#include "OptionsProvider.hpp"

namespace xacc {
namespace quantum {

class CircuitOptimizer : public IRTransformation, public OptionsProvider {

public:
  CircuitOptimizer() {}

  std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) override;

  const std::string name() const override { return "circuit-optimizer"; }
  const std::string description() const override { return ""; }

  OptionPairs getOptions() override {
    OptionPairs desc {{"circuit-opt-n-tries",
                        "Provide the number of passes to use in optimizing "
                        "this circuit. Default = 2."},{"circuit-opt-silent",
                                                      "Do not print any info"}};
    return desc;
  }

};
} // namespace quantum
} // namespace xacc

#endif
