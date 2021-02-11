#pragma once

#include "IRVerifier.hpp"

namespace xacc {
namespace quantum {
class qcec : public IRVerifier {
  std::pair<bool, std::string>
  verify(std::shared_ptr<CompositeInstruction> original,
         std::shared_ptr<CompositeInstruction> transformed,
         const HeterogeneousMap &options = {}) override;

  const std::string name() const override { return "qcec"; }
  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc
