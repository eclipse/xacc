#ifndef XACC_GENERATORS_HWE_HPP_
#define XACC_GENERATORS_HWE_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {
class HWE: public xacc::quantum::Circuit, public Cloneable<Instruction> {
public:
  HWE() : Circuit("hwe") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<HWE>();
  }
};
}
}
#endif