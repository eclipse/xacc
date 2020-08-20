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
  std::string toJson() const override;
  RoErrors readoutError(size_t qubitIdx) const override {
    // If this noise model has not been initialized with a backend, give zero ro
    // errors.
    return m_roErrors.empty() ? std::make_pair(0.0, 0.0) : m_roErrors[qubitIdx];
  }
  std::vector<RoErrors> readoutErrors() const override { return m_roErrors; }

  std::vector<KrausOp>
  gateError(xacc::quantum::Gate &gate) const override; 

private:
  // Gets the name of the equivalent universal gate.
  // e.g. an Rz <=> u1; H <==> u2, etc.
  std::string getUniversalGateEquiv(xacc::quantum::Gate &in_gate) const;
  // Helper methods to generate noise Kraus Ops
  std::vector<double>
  calculateAmplitudeDamping(xacc::quantum::Gate &in_gate) const;
  std::vector<double>
  calculateDepolarizing(xacc::quantum::Gate &in_gate,
                        const std::vector<double> &in_amplitudeDamping) const;
  // Compute the gate fidelity given the amplitude damping noise:
  double
  averageGateFidelity(xacc::quantum::Gate &in_gate,
                      const std::vector<double> &in_amplitudeDamping) const;

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
