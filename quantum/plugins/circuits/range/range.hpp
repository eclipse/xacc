#ifndef XACC_GENERATORS_RANGE_HPP_
#define XACC_GENERATORS_RANGE_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class Range : public xacc::quantum::Circuit {
public:
  Range() : Circuit("range") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(Range);
};
} // namespace circuits
} // namespace xacc
#endif