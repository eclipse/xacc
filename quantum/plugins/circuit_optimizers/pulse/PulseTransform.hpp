#pragma once

#include "IRTransformation.hpp"

namespace xacc {
namespace quantum {
class PulseTransform : public IRTransformation 
{
public:
  PulseTransform() {}
  void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap& options = {}) override; 
  
  const IRTransformationType type() const override { return IRTransformationType::Optimization; } 

  const std::string name() const override { return "quantum-control"; }
  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc