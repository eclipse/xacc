#pragma once

#include "NoiseModel.hpp"

namespace xacc {
namespace quantum {
class IbmqNoiseModel : public NoiseModel {
public:
  // Identifiable interface impls
  const std::string name() const override { return "IBM"; }
  const std::string description() const override {
    return "Backend noise model based on IBMQ backend specifications.";
  }
  void initialize(const HeterogeneousMap &params) override {}
  std::string toJson() const override { return ""; }
  RoErrors readoutError(size_t qubitIdx) const override {
    return std::make_pair(0.0, 0.0);
  }
  std::vector<RoErrors> readoutErrors() const override { return {}; }

  std::vector<KrausOp>
  gateError(const xacc::quantum::Gate &gate) const override {
    return {};
  }
};
} // namespace quantum
} // namespace xacc
