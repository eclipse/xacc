#ifndef XACC_GENERATORS_RANGE_HPP_
#define XACC_GENERATORS_RANGE_HPP_

#include "Circuit.hpp"

namespace xacc {
class AcceleratorBuffer;
class Function;
} // namespace xacc

namespace xacc {
namespace circuits {
class Range : public xacc::quantum::Circuit, public Cloneable<Instruction> {
public:
  Range() : Circuit("range") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  std::shared_ptr<Instruction> clone() override {
    return std::make_shared<Range>();
  }
};
} // namespace circuits
} // namespace xacc
#endif