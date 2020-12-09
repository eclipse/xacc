#include "NoiseModel.hpp"
#include "xacc_plugin.hpp"
#include "json.hpp"

namespace xacc {
class JsonNoiseModel : public NoiseModel {
public:
  // Identifiable interface impls
  const std::string name() const override { return "json"; }
  const std::string description() const override {
    return "Noise model whose noise channel Kraus operators are provided in a "
           "JSON input file.";
  }

  virtual void initialize(const HeterogeneousMap &params) override {}
  virtual std::string toJson() const override { return ""; }
  virtual RoErrors readoutError(size_t qubitIdx) const override { return {}; }
  virtual std::vector<RoErrors> readoutErrors() const override { return {}; }
  virtual std::vector<KrausOp>
  gateError(xacc::quantum::Gate &gate) const override {
    return {};
  }
  virtual double gateErrorProb(xacc::quantum::Gate &gate) const override {
    return 0.0;
  }
  // Query Fidelity information:
  virtual size_t nQubits() const override { return 0; }
  virtual std::vector<double> averageSingleQubitGateFidelity() const override {
    return {};
  }
  virtual std::vector<std::tuple<size_t, size_t, double>>
  averageTwoQubitGateFidelity() const override {
    return {};
  }
};
} // namespace xacc

REGISTER_PLUGIN(xacc::JsonNoiseModel, xacc::NoiseModel)