#ifndef XACC_GENERATORS_HWE_HPP_
#define XACC_GENERATORS_HWE_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class HWE: public xacc::quantum::Circuit {
public:
  HWE() : Circuit("hwe") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(HWE);
};
}
}
#endif