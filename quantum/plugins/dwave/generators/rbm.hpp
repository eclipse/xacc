#ifndef XACC_DWAVE_GENERATORS_RBM_HPP_
#define XACC_DWAVE_GENERATORS_RBM_HPP_

#include "AnnealingProgram.hpp"

namespace xacc {
namespace dwave {
class RBM : public xacc::quantum::AnnealingProgram {
public:
  RBM() : AnnealingProgram("rbm") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
 std::shared_ptr<Instruction> clone() override {                              \
    return std::make_shared<RBM>();                                          \
  }
};
} // namespace dwave
} // namespace xacc
#endif