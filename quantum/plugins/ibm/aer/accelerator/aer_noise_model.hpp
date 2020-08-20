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
  void initialize(const HeterogeneousMap &params) override;
  std::string toJson() const override { return ""; }
  RoErrors readoutError(size_t qubitIdx) const override {
    // If this noise model has not been initialized with a backend, give zero ro
    // errors.
    return m_roErrors.empty() ? std::make_pair(0.0, 0.0) : m_roErrors[qubitIdx];
  }
  std::vector<RoErrors> readoutErrors() const override { return m_roErrors; }

  std::vector<KrausOp>
  gateError(const xacc::quantum::Gate &gate) const override {
    return {};
  }

private:
  // Parsed parameters needed for noise model construction.
  size_t m_nbQubits;
  std::vector<double> m_qubitT1;
  std::vector<double> m_qubitT2;
  std::unordered_map<std::string, double> m_gateErrors;
  std::unordered_map<std::string, double> m_gateDurations;
  std::vector<std::pair<double, double>> m_roErrors;
};
} // namespace quantum
} // namespace xacc
