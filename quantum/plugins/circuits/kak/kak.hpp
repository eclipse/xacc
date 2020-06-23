#pragma once

#include "Circuit.hpp"
#include <Eigen/Dense>

namespace xacc {
namespace circuits {
class Kak : public xacc::quantum::Circuit 
{
public:
  Kak() : Circuit("kak") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(Kak);
private:
  // Single qubit gate matrix
  using GateMatrix = Eigen::Matrix<std::complex<double>, 2, 2>;
  using InputMatrix = Eigen::Matrix<std::complex<double>, 4, 4>;
  struct KakDecomposition
  {
    // Kak description of an arbitrary two-qubit operation.
    // U = g x (Gate A1 Gate A0) x exp(i(xXX + yYY + zZZ))x(Gate b1 Gate b0)
    // i.e. 
    // (1) A global phase factor
    // (2) Two single-qubit operations (before): Gate b0, b1 
    // (3) The Exp() circuit specified by 3 coefficients (x, y, z)
    // (4) Two single-qubit operations (after): Gate a0, a1 
    std::complex<double> g;
    GateMatrix b0;
    GateMatrix b1;
    GateMatrix a0;
    GateMatrix a1;
    std::complex<double> x;
    std::complex<double> y;
    std::complex<double> z;
  };
    
  KakDecomposition kakDecomposition(const InputMatrix& in_matrix) const;
  using BidiagResult = std::tuple<InputMatrix, std::vector<std::complex<double>>, InputMatrix>;
  // Finds orthogonal matrices L, R such that L @ in_matrix @ R is diagonal
  BidiagResult bidiagonalizeUnitary(const InputMatrix& in_matrix) const;
};
} // namespace circuits
} // namespace xacc
