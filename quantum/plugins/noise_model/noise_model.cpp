#include "NoiseModel.hpp"
#include "xacc_plugin.hpp"
#include "json.hpp"
#include "xacc.hpp"

namespace xacc {
class JsonNoiseModel : public NoiseModel {
public:
  // Identifiable interface impls
  const std::string name() const override { return "json"; }
  const std::string description() const override {
    return "Noise model whose noise channel Kraus operators are provided in a "
           "JSON input file.";
  }

  virtual void initialize(const HeterogeneousMap &params) override {
    if (params.stringExists("noise-model")) {
      std::string noise_model_str = params.getString("noise-model");
      // Check if this is a file name
      std::ifstream test(noise_model_str);
      if (test) {
        std::string str((std::istreambuf_iterator<char>(test)),
                        std::istreambuf_iterator<char>());
        m_noiseModel = nlohmann::json::parse(str);
      } else {
        m_noiseModel = nlohmann::json::parse(params.getString("noise-model"));
      }
    }

    // Debug:
    std::cout << "HOWDY: \n" << m_noiseModel.dump() << "\n";
  }
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

private:
  nlohmann::json m_noiseModel;
};
} // namespace xacc

REGISTER_PLUGIN(xacc::JsonNoiseModel, xacc::NoiseModel)