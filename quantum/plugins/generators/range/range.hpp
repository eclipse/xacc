#ifndef XACC_GENERATORS_RANGE_HPP_
#define XACC_GENERATORS_RANGE_HPP_

#include "IRGenerator.hpp"

namespace xacc {
class AcceleratorBuffer;
class Function;
} // namespace xacc

namespace xacc {
namespace generators {
class Range : public xacc::IRGenerator {
public:
  std::shared_ptr<xacc::Function> generate(
      std::map<std::string, xacc::InstructionParameter> &parameters) override;

  bool validateOptions() override;

  const std::string name() const override { return "range"; }

  const std::string description() const override { return ""; }
};
} // namespace generators
} // namespace xacc
#endif