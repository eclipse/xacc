#include "kak.hpp"
#include "IRProvider.hpp"
#include <unsupported/Eigen/KroneckerProduct>

namespace {
constexpr std::complex<double> I { 0.0, 1.0 };

// Define some special matrices
const Eigen::MatrixXcd& KAK_MAGIC() 
{
  static Eigen::MatrixXcd KAK_MAGIC(4, 4);
  static bool init = false;
  if (!init)
  {
    KAK_MAGIC <<  1, 0, 0, I,
                  0, I, 1, 0,
                  0, I, -1, 0,
                  1, 0, 0, -I;
    KAK_MAGIC = KAK_MAGIC * std::sqrt(0.5); 
    init = true;
  }

  return KAK_MAGIC;
}

const Eigen::MatrixXcd& KAK_MAGIC_DAG() 
{
  static Eigen::MatrixXcd KAK_MAGIC_DAG = KAK_MAGIC().adjoint();
  return KAK_MAGIC_DAG;
}
           
const Eigen::MatrixXcd& KAK_GAMMA()
{
  static Eigen::MatrixXcd KAK_GAMMA(4, 4);
  static bool init = false;
  if (!init)
  {
    KAK_GAMMA << 1, 1, 1, 1,
                1, 1, -1, -1,
                -1, 1, -1, 1,
                1, -1, -1, 1;
    KAK_GAMMA = 0.25 * KAK_GAMMA;
    init = true;
  }

  return KAK_GAMMA;
} 
}

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> KAK::requiredKeys() 
{
  return { "qubits", "unitary" };
}

bool KAK::expand(const HeterogeneousMap &parameters) 
{
  // This is a simple "first-principle" implementation
  // of a Kak decomposition.
  // !! TEMP CODE !!!
  Eigen::MatrixXcd m = Eigen::MatrixXcd::Random(4, 4);
  Eigen::MatrixXcd mInMagicBasis = KAK_MAGIC_DAG() * m * KAK_MAGIC();
  auto result = bidiagonalizeUnitary(mInMagicBasis);
  Eigen::MatrixXcd f1 = Eigen::MatrixXcd::Random(2, 2);
  Eigen::MatrixXcd f2 = Eigen::MatrixXcd::Random(2, 2);
  kronFactor(Eigen::kroneckerProduct(f1, f2));
  // !! TEMP CODE !!!

  auto [left, diag, right] = bidiagonalizeUnitary(mInMagicBasis);
  // Recover pieces.
  auto [a1, a0] = so4ToMagicSu2s(left.transpose());                        
  auto [b1, b0] = so4ToMagicSu2s(right.transpose());
                               
  return true;
}

KAK::KakDecomposition KAK::kakDecomposition(const InputMatrix& in_matrix) const
{
  KakDecomposition result;

  return result;
}


KAK::BidiagResult KAK::bidiagonalizeUnitary(const InputMatrix& in_matrix) const
{
  // Using SVD: A = U x S x V_dag
  // => L x A x R = Diag 
  // => L = U^-1
  // => R = V_dag^-1
  Eigen::JacobiSVD<Eigen::MatrixXcd> svd(in_matrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
  const auto U = svd.matrixU();
  const auto V = svd.matrixV().adjoint();
  const auto singularValues = svd.singularValues();
  auto L = U.inverse();
  auto R = V.inverse();
  assert(singularValues.size() == 4);
  auto diag = L * in_matrix * R;
  std::vector<std::complex<double>> d;
  for (int i = 0; i < singularValues.size(); ++i)
  {
    d.emplace_back(singularValues(i));
  }

  // Validate:
  auto testMat = L * in_matrix * R;
  for (int row = 0; row < testMat.rows(); ++row)
  {
    for (int col = 0; col < testMat.cols(); ++col)
    {
      if (row == col)
      {
        assert(std::abs(testMat(row, col) - d[row]) < 1e-6);
      }
      else
      {
        assert(std::abs(testMat(row, col)) < 1e-6);
      }
    }
  }

  return std::make_tuple(L, d, R);
}

std::tuple<std::complex<double>, KAK::GateMatrix, KAK::GateMatrix> KAK::kronFactor(const InputMatrix& in_matrix) const
{
  KAK::GateMatrix f1 = KAK::GateMatrix::Zero();
  KAK::GateMatrix f2 = KAK::GateMatrix::Zero();
  
  // Get row and column of the max element
  size_t a = 0;
  size_t b = 0;
  double maxVal = std::abs(in_matrix(a, b));
  for (int row = 0; row < in_matrix.rows(); ++row)
  {
    for (int col = 0; col < in_matrix.cols(); ++col)
    {
      if (std::abs(in_matrix(row, col)) > maxVal)
      {
        a = row;
        b = col;
        maxVal = std::abs(in_matrix(a, b));
      }
    }
  }
  
  // Extract sub-factors touching the reference cell.
  for (int i = 0; i < 2; ++i)
  {
    for (int j = 0; j < 2; ++j)
    {
      f1((a >> 1) ^ i, (b >> 1) ^ j) = in_matrix(a ^ (i << 1), b ^ (j << 1));
      f2((a & 1) ^ i, (b & 1) ^ j) = in_matrix(a ^ i, b ^ j);
    }
  }

  // Rescale factors to have unit determinants.
  f1 /= (std::sqrt(f1.determinant()));
  f2 /= (std::sqrt(f2.determinant()));

  //Determine global phase.
  std::complex<double> g = in_matrix(a, b) / (f1(a >> 1, b >> 1) * f2(a & 1, b & 1));
  if (g.real() < 0.0)
  {
    f1 *= -1;
    g = -g;
  }

  // Validate:
  auto testMat = g * Eigen::kroneckerProduct(f1, f2);
  for (int row = 0; row < in_matrix.rows(); ++row)
  {
    for (int col = 0; col < in_matrix.cols(); ++col)
    {
      assert(std::abs(testMat(row, col) - in_matrix(row, col)) < 1e-6);
    }
  }

  return std::make_tuple(g, f1, f2);
}

std::pair<KAK::GateMatrix, KAK::GateMatrix> KAK::so4ToMagicSu2s(const InputMatrix& in_matrix) const
{
  auto matInMagicBasis = KAK_MAGIC() * in_matrix * KAK_MAGIC_DAG();
  auto [g, f1, f2] = kronFactor(matInMagicBasis);
  return std::make_pair(f1, f2);
}
} // namespace circuits
} // namespace xacc