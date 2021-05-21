#pragma once
#include "xacc.hpp"
#include "IRTransformation.hpp"

namespace xacc {
namespace quantum {
class NearestNeighborTransform : public IRTransformation {
public:
  NearestNeighborTransform() {}
  void apply(std::shared_ptr<CompositeInstruction> in_program,
             const std::shared_ptr<Accelerator> in_accelerator,
             const HeterogeneousMap &in_options = {}) override;

  const IRTransformationType type() const override {
    return IRTransformationType::Placement;
  }
  const std::string name() const override { return "nnizer"; }
  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc