#ifndef QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_
#define QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_

#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"
#include "GateFunction.hpp"

namespace xacc {
namespace quantum {

class CircuitOptimizer : public IRTransformation, public OptionsProvider {

public:
  CircuitOptimizer() {}

  virtual std::shared_ptr<IR> transform(std::shared_ptr<IR> ir);

  virtual const std::string name() const { return "circuit-optimizer"; }

  virtual const std::string description() const { return ""; }

  virtual std::shared_ptr<options_description> getOptions() {
    auto desc =
        std::make_shared<options_description>("CircuitOptimizer Options");
    desc->add_options()("circuit-opt-n-tries", value<std::string>(),
                        "Provide the number of passes to use in optimizing "
                        "this circuit. Default = 2.")("circuit-opt-silent",
                                                      "Do not print any info");
    return desc;
  }

  virtual bool handleOptions(variables_map &map) { return false; }
};
} // namespace quantum
} // namespace xacc

#endif
