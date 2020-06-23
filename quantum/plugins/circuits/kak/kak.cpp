#include "kak.hpp"
#include "IRProvider.hpp"

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
// Returns L and R matrix pair such that:
//  L @ in_mat1 @ R and L @ in_mat2 @ R are diagonal matrices.
std::pair<Eigen::MatrixXd, Eigen::MatrixXd> bidiagonalizeRealMatrixPairWithSymmetricProducts(const Eigen::MatrixXd& in_mat1,  const Eigen::MatrixXd& in_mat2)
{
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(in_mat1, Eigen::ComputeThinU | Eigen::ComputeThinV);
  // SVD
  auto base_left = svd.matrixU();
  auto base_right = svd.matrixV();
  auto base_diag = svd.singularValues();
  
  // Try diagonalizing the second matrix with the same factors as the first.
  // auto semi_corrected = base_left.transpose().dot(in_mat2).dot(base_right.transpose());

}
}

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> Kak::requiredKeys() 
{
  return { "qubits", "unitary" };
}

bool Kak::expand(const HeterogeneousMap &parameters) 
{
  // This is a simple "first-principle" implementation
  // of a Kak decomposition.


  return true;
}

Kak::KakDecomposition Kak::kakDecomposition(const InputMatrix& in_matrix) const
{
  KakDecomposition result;

  return result;
}


Kak::BidiagResult Kak::bidiagonalizeUnitary(const InputMatrix& in_matrix) const
{
  // TODO
  return std::make_tuple(in_matrix, std::vector<std::complex<double>> {}, in_matrix);
}
} // namespace circuits
} // namespace xacc