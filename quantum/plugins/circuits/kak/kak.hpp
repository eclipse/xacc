#pragma once

#include "Circuit.hpp"
#include <Eigen/Dense>
#include <optional>

namespace xacc {
namespace circuits {
// Use Z-Y-Z decomposition of Nielsen and Chuang (Theorem 4.1).
// An arbitrary one qubit gate matrix can be writen as
// U = exp(j*a) Rz(b) Ry(c) Rz(d).
class ZYZ : public xacc::quantum::Circuit 
{
public:
  ZYZ() : Circuit("z-y-z") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override { return { "unitary" }; }
  DEFINE_CLONE(ZYZ);
};

// KAK decomposition via *Magic* Bell basis transformation
// Reference:
// https://arxiv.org/pdf/quant-ph/0211002.pdf
class KAK : public xacc::quantum::Circuit 
{
public:
  KAK() : Circuit("kak") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(KAK);
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
    double x;
    double y;
    double z;
    // Generates gate sequence:
    std::shared_ptr<CompositeInstruction> toGates(size_t in_bit1, size_t in_bit2) const;
    Eigen::MatrixXcd toMat() const;
  };
    
  std::optional<KakDecomposition> kakDecomposition(const InputMatrix& in_matrix) const;
  // Returns a canonicalized interaction plus before and after corrections.
  KakDecomposition canonicalizeInteraction(double x, double y, double z) const;
  using BidiagResult = std::tuple<InputMatrix, std::vector<std::complex<double>>, InputMatrix>;
  // Finds orthogonal matrices L, R such that L x in_matrix x R is diagonal
  BidiagResult bidiagonalizeUnitary(const InputMatrix& in_matrix) const;
  // Joint diagonalize two symmetric real matrices
  // 4x4 factor into kron(A, B) where A and B are 2x2
  std::tuple<std::complex<double>, GateMatrix, GateMatrix> kronFactor(const InputMatrix& in_matrix) const;
  // Decompose an input matrix in the *Magic* Bell basis
  std::pair<GateMatrix, GateMatrix> so4ToMagicSu2s(const InputMatrix& in_matrix) const;
  // Returns an orthogonal matrix that diagonalizes the given matrix
  Eigen::MatrixXd diagonalizeRealSymmetricMatrix(const Eigen::MatrixXd& in_mat) const;
  // Returns an orthogonal matrix P such that 
  // P^-1 x symmetric_matrix x P is diagonal
  // and P^-1 @ diagonal_matrix @ P = diagonal_matrix
  Eigen::MatrixXd diagonalizeRealSymmetricAndSortedDiagonalMatrices(const Eigen::MatrixXd& in_symMat, const Eigen::MatrixXd& in_diagMat) const;
  // Finds orthogonal matrices that diagonalize both in_mat1 and in_mat2
  std::pair<Eigen::Matrix4d, Eigen::Matrix4d> bidiagonalizeRealMatrixPairWithSymmetricProducts(const Eigen::Matrix4d& in_mat1, const Eigen::Matrix4d& in_mat2) const;
};
} // namespace circuits
} // namespace xacc
