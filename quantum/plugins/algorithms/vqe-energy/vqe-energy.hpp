#ifndef XACC_ALGORITHM_VQE_ENERGY_HPP_
#define XACC_ALGORITHM_VQE_ENERGY_HPP_

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class VQEEnergy : public Algorithm {
protected:
  std::shared_ptr<Observable> observable;
  std::shared_ptr<CompositeInstruction> kernel;
  std::shared_ptr<Accelerator> accelerator;
  std::vector<double> initial_params;

  HeterogeneousMap parameters;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;

  const std::string name() const override { return "vqe-energy"; }
  const std::string description() const override { return ""; }
};
} // namespace algorithm
} // namespace xacc
#endif