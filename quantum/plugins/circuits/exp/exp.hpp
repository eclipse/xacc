#ifndef XACC_GENERATORS_EXP_HPP_
#define XACC_GENERATORS_EXP_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class Exp : public xacc::quantum::Circuit, public Cloneable<Instruction> {
public:
  Exp() : Circuit("exp_i_theta") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<Exp>();
  }
};
} // namespace circuits
} // namespace xacc
#endif