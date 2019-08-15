#ifndef XACC_ALGORITHM_RDM_HPP_
#define XACC_ALGORITHM_RDM_HPP_

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class RDM : public Algorithm {
protected:
  std::shared_ptr<Function> ansatz;
  std::shared_ptr<Accelerator> accelerator;

  AlgorithmParameters parameters;

public:
  bool initialize(const AlgorithmParameters &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "rdm"; }
  const std::string description() const override { return ""; }
};
} // namespace algorithm
} // namespace xacc
#endif