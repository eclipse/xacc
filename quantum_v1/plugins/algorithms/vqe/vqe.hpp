#ifndef XACC_ALGORITHM_VQE_HPP_
#define XACC_ALGORITHM_VQE_HPP_

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class VQE : public Algorithm {
protected:
  std::shared_ptr<Observable> observable;
  std::shared_ptr<Optimizer> optimizer;
  std::shared_ptr<Function> kernel;
  std::shared_ptr<Accelerator> accelerator;
  std::vector<double> initial_params;

  AlgorithmParameters parameters;

public:
  bool initialize(const AlgorithmParameters &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  //   void execute(xacc::Observable &observable, Optimizer &optimizer)
  //   override;
  const std::string name() const override { return "vqe"; }
  const std::string description() const override { return ""; }
};
} // namespace algorithm
} // namespace xacc
#endif