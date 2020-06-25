#include "kak.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include <Eigen/Eigenvalues>
#include <unsupported/Eigen/KroneckerProduct>
#include <unsupported/Eigen/MatrixFunctions>
#include "PauliOperator.hpp"
#include <armadillo>

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

// Splits i = 0...length into approximate equivalence classes
// determine by the predicate
std::vector<std::pair<int, int>> contiguousGroups(int in_length, std::function<bool(int,int)> in_predicate)
{
  int start = 0;
  std::vector<std::pair<int, int>> result;
  while(start < in_length)
  {
    auto past = start + 1;
    while ((past < in_length) && in_predicate(start, past))
    {
      past++; 
    }
    result.emplace_back(start, past);
    start = past;
  }
  return result;
}

Eigen::MatrixXd blockDiag(const Eigen::MatrixXd& in_first, const Eigen::MatrixXd& in_second)
{
  Eigen::MatrixXd bdm = Eigen::MatrixXd::Zero(in_first.rows() + in_second.rows(), in_first.cols() + in_second.cols());
  bdm.block(0, 0, in_first.rows(), in_first.cols()) = in_first;
  bdm.block(in_first.rows(), in_first.cols(), in_second.rows(), in_second.cols()) = in_second;
  return bdm;
}

bool isSquare(const Eigen::MatrixXcd& in_mat)
{
  return in_mat.rows() == in_mat.cols();
}

bool isDiagonal(const Eigen::MatrixXcd& in_mat, double in_tol = 1e-12)
{
  for (int i = 0; i < in_mat.rows(); ++i)
  {
    for (int j = 0; j < in_mat.cols(); ++j)
    {
      if (i != j)
      {
        if (std::abs(in_mat(i,j)) > in_tol)
        {
          return false;
        }
      }
    }
  }

  return true;
}

bool allClose(const Eigen::MatrixXcd& in_mat1, const Eigen::MatrixXcd& in_mat2, double in_tol = 1e-12)
{
  if (in_mat1.rows() == in_mat2.rows() && in_mat1.cols() == in_mat2.cols())
  {
    for (int i = 0; i < in_mat1.rows(); ++i)
    {
      for (int j = 0; j < in_mat1.cols(); ++j)
      {
        if (std::abs(in_mat1(i,j) - in_mat2(i, j)) > in_tol)
        {
          return false;
        }
      }
    }

    return true;
  }
  return false;
}

bool isHermitian(const Eigen::MatrixXcd& in_mat)
{
  if (!isSquare(in_mat))
  {
    return false;
  }
  return allClose(in_mat, in_mat.adjoint());
}

bool isUnitary(const Eigen::MatrixXcd& in_mat)
{
  if (!isSquare(in_mat))
  {
    return false;
  }

  Eigen::MatrixXcd Id = Eigen::MatrixXcd::Identity(in_mat.rows(), in_mat.cols());

  return allClose(in_mat * in_mat.adjoint(), Id);
}

bool isOrthogonal(const Eigen::MatrixXcd& in_mat, double in_tol = 1e-12)
{
  if (!isSquare(in_mat))
  {
    return false;
  }

  // Is real 
  for (int i = 0; i < in_mat.rows(); ++i)
  {
    for (int j = 0; j < in_mat.cols(); ++j)
    {
      if (std::abs(in_mat(i,j).imag()) > in_tol)
      {
        return false;
      }
    }
  }
  // its transpose is its inverse
  return allClose(in_mat.inverse(), in_mat.transpose(), in_tol);
}
// Is Orthogonal and determinant == 1
bool isSpecialOrthogonal(const Eigen::MatrixXcd& in_mat, double in_tol = 1e-12)
{
  return isOrthogonal(in_mat, in_tol) && (std::abs(std::abs(in_mat.determinant()) - 1.0) < in_tol);
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

bool KAK::expand(const HeterogeneousMap& parameters) 
{
  Eigen::Matrix4cd unitary;
  if (parameters.keyExists<Eigen::Matrix4cd>("unitary"))
  {
    unitary = parameters.get<Eigen::Matrix4cd>("unitary");
  }
  else if (parameters.keyExists<std::vector<std::complex<double>>>("unitary"))
  {
    auto matAsVec = parameters.get<std::vector<std::complex<double>>>("unitary");
    // Correct size: 4 x 4
    if (matAsVec.size() == 16)
    {
      for (int row = 0; row < 4; ++row)
      {
        for (int col = 0; col < 4; ++col)
        {
          // Expect row-by-row layout
          unitary(row, col) = matAsVec[4*row + col];
        }
      }
    }
  }
  
  if (!isUnitary(unitary))
  {
    xacc::error("Input matrix is not a unitary matrix");
    return false;
  }
  
  auto result = kakDecomposition(unitary);
  return true;
}

KAK::KakDecomposition KAK::kakDecomposition(const InputMatrix& in_matrix) const
{
  assert(isUnitary(in_matrix));
  Eigen::MatrixXcd mInMagicBasis = KAK_MAGIC_DAG() * in_matrix * KAK_MAGIC();
  auto [left, diag, right] = bidiagonalizeUnitary(mInMagicBasis);
  // Recover pieces.
  auto [a1, a0] = so4ToMagicSu2s(left.transpose());                        
  auto [b1, b0] = so4ToMagicSu2s(right.transpose());
  assert(isUnitary(a0));
  assert(isUnitary(a1));
  assert(isUnitary(b0));
  assert(isUnitary(b1));

  Eigen::Vector4cd angles;
  for (size_t i = 0; i < 4; ++i)
  {
    angles(i) = std::arg(diag[i]);
  }
  auto factors = KAK_GAMMA() * angles;
  
  // Get factors:
  const auto w = factors(0);
  const auto x = factors(1);
  const auto y = factors(2);
  const auto z = factors(3);
  const auto g = std::exp(I * w);

  xacc::quantum::PauliOperator xx(x, "X0X1");
  xacc::quantum::PauliOperator yy(y, "Y0Y1");
  xacc::quantum::PauliOperator zz(z, "Z0Z1");
  // auto herm = xx + yy; 
  // std::cout << "Herm: " << herm.toString() << "\n";
  
  auto before = Eigen::kroneckerProduct(b1, b0);
  auto after = Eigen::kroneckerProduct(a1, a0);
  Eigen::MatrixXcd X { Eigen::MatrixXcd::Zero(2, 2)};      
  Eigen::MatrixXcd Y { Eigen::MatrixXcd::Zero(2, 2)};       
  Eigen::MatrixXcd Z { Eigen::MatrixXcd::Zero(2, 2)}; 
  X << 0, 1, 1, 0;
  Y << 0, -I, I, 0;
  Z << 1, 0, 0, -1;
  auto XX = Eigen::kroneckerProduct(X, X);
  auto YY = Eigen::kroneckerProduct(Y, Y);
  auto ZZ = Eigen::kroneckerProduct(Z, Z);
  Eigen::MatrixXcd herm = x*XX + y*YY + z*ZZ;
  herm = I*herm;
  Eigen::MatrixXcd unitary = herm.exp();
  auto total = g * after * unitary * before;
  std::cout << "Total U:\n" << total << "\n";
  assert(allClose(total, in_matrix));

  KakDecomposition result;

  return result;
}


KAK::BidiagResult KAK::bidiagonalizeUnitary(const InputMatrix& in_matrix) const
{
  Eigen::Matrix4d realMat;
  Eigen::Matrix4d imagMat;
  for (int row = 0; row < in_matrix.rows(); ++row)
  {
    for (int col = 0; col < in_matrix.cols(); ++col)
    {
      realMat(row, col) = in_matrix(row, col).real();
      imagMat(row, col) = in_matrix(row, col).imag();
    }
  }
  // Assert A X B.T and A.T X B are hermitian
  assert(isHermitian(realMat * imagMat.transpose()));
  assert(isHermitian(realMat.transpose() * imagMat));

  auto [left, right] = bidiagonalizeRealMatrixPairWithSymmetricProducts(realMat, imagMat);

  // Convert to special orthogonal w/o breaking diagonalization.
  if (left.determinant() < 0)
  {
    for (int i = 0; i < left.cols(); ++i)
    {
      left(0, i) = -left(0, i);
    }
  }
  if (right.determinant() < 0)
  {
    for (int i = 0; i < right.rows(); ++i)
    {
      right(i, 0) = -right(i, 0);
    }
  }

  auto diag = left * in_matrix * right;
  // Validate:
  assert(isDiagonal(diag));
  
  std::vector<std::complex<double>> diagVec;
  for (int i = 0; i < diag.rows(); ++i)
  {
    diagVec.emplace_back(diag(i, i));
  }

  return std::make_tuple(left, diagVec, right);
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
  std::cout << "Input: \n" << in_matrix << "\n";
  std::cout << "Validate: \n" << testMat << "\n";

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
  assert(isSpecialOrthogonal(in_matrix));
  auto matInMagicBasis = KAK_MAGIC() * in_matrix * KAK_MAGIC_DAG();
  auto [g, f1, f2] = kronFactor(matInMagicBasis);
  return std::make_pair(f1, f2);
}

Eigen::MatrixXd KAK::diagonalizeRealSymmetricMatrix(const Eigen::MatrixXd& in_mat) const
{ 
  assert(isHermitian(in_mat));
  // IMPORTANT: Eigen doesn't support eigenvalues/eigenvectors calculation *PROPERLY*
  // i.e. w.r.t. to standard BLAS (such as LAPACK)
  // Hence, we *MUST* use Armadillo.
  arma::mat inputMat(in_mat.data(), in_mat.rows(), in_mat.cols());
  arma::vec eigval;
  arma::mat eigvec;
  eig_sym(eigval, eigvec, inputMat);
  // Copy result to Eigen
  Eigen::MatrixXd p = Eigen::Map<Eigen::MatrixXd>(eigvec.memptr(), eigvec.n_rows, eigvec.n_cols);
  // Orthogonal basis (Hermitian/symmetric matrix)  
  assert(isOrthogonal(p));
  // An orthogonal matrix P such that PT x matrix x P is diagonal.
  assert(isDiagonal(p.transpose() * in_mat * p));
  return p;
}

Eigen::MatrixXd KAK::diagonalizeRealSymmetricAndSortedDiagonalMatrices(const Eigen::MatrixXd& in_symMat, const Eigen::MatrixXd& in_diagMat) const
{
  assert(isDiagonal(in_diagMat));
  assert(isHermitian(in_symMat));
  std::cout << "[diagonalizeRealSymmetricAndSortedDiagonalMatrices] in_symMat = \n" << in_symMat << "\n";
  std::cout << "[diagonalizeRealSymmetricAndSortedDiagonalMatrices] in_diagMat = \n" << in_diagMat << "\n";

  const auto similarSingular = [&in_diagMat](int i, int j) {
    return std::abs(in_diagMat(i,i) - in_diagMat(j,j)) < 1e-5;
  };

  const auto ranges = contiguousGroups(in_diagMat.rows(), similarSingular);
  Eigen::MatrixXd p = Eigen::MatrixXd::Zero(in_symMat.rows(), in_symMat.cols());

  for (const auto& [start, end]: ranges)
  {
    const int blockSize = end - start;
    
    Eigen::MatrixXd block = Eigen::MatrixXd(blockSize, blockSize);
    for (int i = 0; i < blockSize; ++i)
    {
      for (int j = 0; j < blockSize; ++j)
      {
        block(i,j) = in_symMat(i + start, j + start);
      }
    }
    std::cout << "Begin = " << start << "; End = " << end << "\n";
    std::cout << "Block = \n" << block << "\n"; 
    auto blockDiag = diagonalizeRealSymmetricMatrix(block);
    std::cout << "blockDiag = \n" << blockDiag << "\n"; 

    for (int i = 0; i < blockSize; ++i)
    {
      for (int j = 0; j < blockSize; ++j)
      {
        p(i + start, j + start) = blockDiag(i,j);
      }
    }
  }

  std::cout << "[diagonalizeRealSymmetricAndSortedDiagonalMatrices] p = \n" << p << "\n";
  // P.T x symmetric_matrix x P is diagonal
  assert(isDiagonal(p.transpose() * in_symMat * p));
  // and P.T x diagonal_matrix x P = diagonal_matrix
  assert(allClose(p.transpose() * in_diagMat * p, in_diagMat));

  return p;
}

std::pair<Eigen::Matrix4d, Eigen::Matrix4d> KAK::bidiagonalizeRealMatrixPairWithSymmetricProducts(const Eigen::Matrix4d& in_mat1, const Eigen::Matrix4d& in_mat2) const
{
  std::cout << "bidiagonalizeRealMatrixPairWithSymmetricProducts\n";
  std::cout << "in_mat1: \n" << in_mat1 << "\n";
  std::cout << "in_mat2: \n" << in_mat2 << "\n";

  const auto svd = [](const Eigen::MatrixXd& in_mat) -> std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::MatrixXd> {
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(in_mat, Eigen::ComputeThinU | Eigen::ComputeThinV);
    return std::make_tuple(svd.matrixU(), svd.singularValues(), svd.matrixV().adjoint());
  };
  // Use SVD to bi-diagonalize the first matrix.
  auto [base_left, base_diag_vec, base_right] = svd(in_mat1);
  
  std::cout << "base_left: \n" << base_left << "\n"; 
  std::cout << "base_diag_vec: \n" << base_diag_vec << "\n"; 
  std::cout << "base_right: \n" << base_right << "\n"; 
  
  
  Eigen::MatrixXd base_diag = Eigen::MatrixXd::Zero(base_diag_vec.size(), base_diag_vec.size());
  for (int i = 0; i < base_diag_vec.size(); ++i)
  {
    base_diag(i, i) = base_diag_vec(i);
  }

  std::cout << "in_mat1: \n" << in_mat1 << "\n";
  std::cout << "in_mat2: \n" << in_mat2 << "\n";

  // Determine where we switch between diagonalization-fixup strategies.
  const auto dim = base_diag.rows();
  auto rank = dim;
  while (rank > 0 && std::abs(base_diag(rank - 1, rank - 1) < 1e-5))
  {
    rank--;
  } 
  std::cout << "Rank = " << rank << "; Dim = " << dim << "\n";
  Eigen::MatrixXd base_diag_trim = Eigen::MatrixXd::Zero(rank, rank);
  for (int i = 0; i < rank; ++i)
  {
    for (int j = 0; j < rank; ++j)
    {
      base_diag_trim(i, j) = base_diag(i, j);
    }
  }

  std::cout << "base_diag_trim: \n" << base_diag_trim << "\n";


  // Try diagonalizing the second matrix with the same factors as the first.
  auto semi_corrected = base_left.transpose() * in_mat2 * base_right.transpose();
  std::cout << "semi_corrected: \n" << semi_corrected << "\n";
  // Fix up the part of the second matrix's diagonalization that's matched
  // against non-zero diagonal entries in the first matrix's diagonalization
  // by performing simultaneous diagonalization.
  Eigen::MatrixXd overlap = Eigen::MatrixXd::Zero(rank, rank);
  for (int i = 0; i < rank; ++i)
  {
    for (int j = 0; j < rank; ++j)
    {
      overlap(i, j) = semi_corrected(i, j);
    }
  }
  std::cout << "overlap: \n" << overlap << "\n";

  auto overlap_adjust = diagonalizeRealSymmetricAndSortedDiagonalMatrices(overlap, base_diag_trim);
  // Fix up the part of the second matrix's diagonalization that's matched
  // against zeros in the first matrix's diagonalization by performing an SVD.
  const auto extraSize = dim - rank;
  Eigen::MatrixXd extra(extraSize, extraSize);
  for (int i = 0; i < extraSize; ++i)
  {
    for (int j = 0; j < extraSize; ++j)
    {
      extra(i, j) = semi_corrected(i + rank, j + rank);
    }
  } 
  
  static const auto emptySvdResult = std::make_tuple(Eigen::MatrixXd::Zero(0,0), Eigen::VectorXd::Zero(0), Eigen::MatrixXd::Zero(0,0));
  auto [extra_left_adjust, extra_diag, extra_right_adjust] = (dim > rank) ? svd(extra): emptySvdResult;
  // Merge the fixup factors into the initial diagonalization.
  auto left_adjust = blockDiag(overlap_adjust, extra_left_adjust);
  auto right_adjust = blockDiag(overlap_adjust.transpose(), extra_right_adjust);
  auto left = left_adjust.transpose() * base_left.transpose();
  auto right = base_right.transpose() * right_adjust.transpose(); 
  // L x mat1 x R and L x mat2 x R are diagonal matrices.
  assert(isDiagonal(left * in_mat1 * right));
  assert(isDiagonal(left * in_mat2 * right));
  return std::make_pair(left, right);
}
} // namespace circuits
} // namespace xacc