#ifndef XACC_GENERATORS_EXP_HPP_
#define XACC_GENERATORS_EXP_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class Exp : public xacc::quantum::Circuit {
public:
  Exp() : Circuit("exp_i_theta") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(Exp);
};
} // namespace circuits
} // namespace xacc
#endif