#include "NoiseModel.hpp"
#include "xacc_plugin.hpp"
#include "json.hpp"
#include "xacc.hpp"
#include <Eigen/Dense>

namespace {
constexpr double NUM_TOL = 1e-9;
bool allClose(const Eigen::MatrixXcd &in_mat1, const Eigen::MatrixXcd &in_mat2,
              double in_tol = NUM_TOL) {
  if (in_mat1.rows() == in_mat2.rows() && in_mat1.cols() == in_mat2.cols()) {
    for (int i = 0; i < in_mat1.rows(); ++i) {
      for (int j = 0; j < in_mat1.cols(); ++j) {
        if (std::abs(in_mat1(i, j) - in_mat2(i, j)) > in_tol) {
          return false;
        }
      }
    }

    return true;
  }
  return false;
}

bool isIdentity(const Eigen::MatrixXcd &mat, double threshold = NUM_TOL) {
  Eigen::MatrixXcd idMat = Eigen::MatrixXcd::Identity(mat.rows(), mat.cols());
  return allClose(mat, idMat, threshold);
}

bool validateKrausCPTP(const std::vector<Eigen::MatrixXcd> &mats,
                       double threshold = NUM_TOL) {
  if (mats.empty()) {
    return true;
  }
  Eigen::MatrixXcd cptp =
      Eigen::MatrixXcd::Zero(mats[0].rows(), mats[0].cols());
  for (const auto &mat : mats) {
    cptp = cptp + mat.adjoint() * mat;
  }
  return isIdentity(cptp, threshold);
}
} // namespace

namespace xacc {
class JsonNoiseModel : public NoiseModel {
public:
  enum class BitOrder { MSB, LSB };

  struct NoiseKrausOp {
    std::vector<std::string> bit_locs;
    Eigen::MatrixXcd matrix;
  };

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
    const auto bit_order = m_noiseModel["bit_order"].get<std::string>();
    if (bit_order == "MSB") {
      m_bitOrder = BitOrder::MSB;
    } else if (bit_order == "LSB") {
      m_bitOrder = BitOrder::LSB;
    } else {
      xacc::error("Unknown value: " + bit_order);
    }

    auto gate_noise = m_noiseModel["gate_noise"];
    for (auto iter = gate_noise.begin(); iter != gate_noise.end(); ++iter) {
      auto noise_info = *iter;
      const auto gate_name = noise_info["gate_name"].get<std::string>();
      const auto register_location =
          noise_info["register_location"].get<std::vector<std::string>>();
      const auto gateKey = createGateLookupKey(gate_name, register_location);
      std::cout << "Process: " << gateKey << "\n";
      auto noise_kraus_ops = noise_info["noise_kraus_ops"];
      std::vector<NoiseKrausOp> noise_ops;
      std::vector<Eigen::MatrixXcd>  kraus_mats;
      for (auto kraus_iter = noise_kraus_ops.begin();
           kraus_iter != noise_kraus_ops.end(); ++kraus_iter) {
        NoiseKrausOp newOp;
        auto kraus_op = *kraus_iter;
        if (kraus_op.find("total_count") != kraus_op.end()) {
          newOp.bit_locs =
              kraus_op["total_count"].get<std::vector<std::string>>();
        } else {
          newOp.bit_locs = register_location;
        }

        const auto op_mat =
            kraus_op["matrix"]
                .get<std::vector<std::vector<std::pair<double, double>>>>();
        const auto nbRows = op_mat.size();
        if (nbRows != (1ULL << newOp.bit_locs.size())) {
          xacc::error("Kraus operator matrix dimension doesn't match the "
                      "number of qubits.");
        }

        Eigen::MatrixXcd mat = Eigen::MatrixXcd::Zero(nbRows, nbRows);
        for (int row = 0; row < nbRows; ++row) {
          const auto &rowVec = op_mat[row];
          if (rowVec.size() != nbRows) {
            xacc::error("Kraus operator matrix must be square.");
          }

          for (int col = 0; col < rowVec.size(); ++col) {
            auto elemPair = rowVec[col];
            mat(row, col) =
                std::complex<double>{elemPair.first, elemPair.second};
          }
        }
        newOp.matrix = mat;
        noise_ops.emplace_back(newOp);
        kraus_mats.emplace_back(mat);
        std::cout << "Parsed Kraus op:\n" << mat << "\n";
      }

      if (!validateKrausCPTP(kraus_mats)) {
        xacc::error("The list of Kraus operators for gate " + gateKey +
                    " don't satisfy the CPTP condition.");
      }
      m_gateNoise.emplace(gateKey, noise_ops);
    }
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
  std::string
  createGateLookupKey(const std::string &in_gateName,
                      const std::vector<std::string> &in_qubitLocs) {
    std::string result = in_gateName + "_" + in_qubitLocs[0];
    for (int i = 1; i < in_qubitLocs.size(); ++i) {
      result = result + "_" + in_qubitLocs[i];
    }
    return result;
  }

private:
  nlohmann::json m_noiseModel;
  BitOrder m_bitOrder;
  std::unordered_map<std::string, std::vector<NoiseKrausOp>> m_gateNoise;
};
} // namespace xacc

REGISTER_PLUGIN(xacc::JsonNoiseModel, xacc::NoiseModel)