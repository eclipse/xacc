#pragma once

#include <Eigen/Dense>

#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"

using namespace xacc::quantum;

namespace xacc {

class QubitTapering : public xacc::ObservableTransform {
 public:
  QubitTapering() = default;
  std::shared_ptr<xacc::Observable> transform(
      std::shared_ptr<xacc::Observable> obs) override;

  const std::string name() const override { return "qubit-tapering"; }
  const std::string description() const override {
    return "Reduce number of qubits required by exploiting Z2 symmetries.";
  }

 private:
  Eigen::MatrixXi computeTableaux(PauliOperator &H, const int nQubits);
  std::vector<std::vector<int>> generateCombinations(
      const int nQubits, std::function<void(std::vector<int> &)> &&f =
                             [](std::vector<int> &tmp) { return; });
  int binaryVectorInnerProduct(std::vector<int> &bv1, std::vector<int> &bv2);
    const double computeGroundStateEnergy(PauliOperator &op, const int n);

  Eigen::MatrixXi gauss(Eigen::MatrixXi &A, std::vector<int> &pivotCols);

};
}  // namespace xacc