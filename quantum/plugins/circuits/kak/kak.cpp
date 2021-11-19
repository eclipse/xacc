#include "kak.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <Eigen/Eigenvalues>
#include <unsupported/Eigen/KroneckerProduct>
#include <unsupported/Eigen/MatrixFunctions>
#include "PauliOperator.hpp"

namespace {
constexpr std::complex<double> I{0.0, 1.0};
int getTempId() {
  static int tempIdCounter = 0;
  tempIdCounter++;
  return tempIdCounter;
}

// Define some special matrices
const Eigen::MatrixXcd &KAK_MAGIC() {
  static Eigen::MatrixXcd KAK_MAGIC(4, 4);
  static bool init = false;
  if (!init) {
    KAK_MAGIC << 1, 0, 0, I, 0, I, 1, 0, 0, I, -1, 0, 1, 0, 0, -I;
    KAK_MAGIC = KAK_MAGIC * std::sqrt(0.5);
    init = true;
  }

  return KAK_MAGIC;
}

const Eigen::MatrixXcd &KAK_MAGIC_DAG() {
  static Eigen::MatrixXcd KAK_MAGIC_DAG = KAK_MAGIC().adjoint();
  return KAK_MAGIC_DAG;
}

const Eigen::MatrixXcd &KAK_GAMMA() {
  static Eigen::MatrixXcd KAK_GAMMA(4, 4);
  static bool init = false;
  if (!init) {
    KAK_GAMMA << 1, 1, 1, 1, 1, 1, -1, -1, -1, 1, -1, 1, 1, -1, -1, 1;
    KAK_GAMMA = 0.25 * KAK_GAMMA;
    init = true;
  }

  return KAK_GAMMA;
}

// Splits i = 0...length into approximate equivalence classes
// determine by the predicate
std::vector<std::pair<int, int>>
contiguousGroups(int in_length, std::function<bool(int, int)> in_predicate) {
  int start = 0;
  std::vector<std::pair<int, int>> result;
  while (start < in_length) {
    auto past = start + 1;
    while ((past < in_length) && in_predicate(start, past)) {
      past++;
    }
    result.emplace_back(start, past);
    start = past;
  }
  return result;
}

Eigen::MatrixXd blockDiag(const Eigen::MatrixXd &in_first,
                          const Eigen::MatrixXd &in_second) {
  Eigen::MatrixXd bdm = Eigen::MatrixXd::Zero(
      in_first.rows() + in_second.rows(), in_first.cols() + in_second.cols());
  bdm.block(0, 0, in_first.rows(), in_first.cols()) = in_first;
  bdm.block(in_first.rows(), in_first.cols(), in_second.rows(),
            in_second.cols()) = in_second;
  return bdm;
}

inline bool isSquare(const Eigen::MatrixXcd &in_mat) {
  return in_mat.rows() == in_mat.cols();
}

// If the matrix is finite: no NaN elements
template <typename Derived>
inline bool isFinite(const Eigen::MatrixBase<Derived> &x) {
  return ((x - x).array() == (x - x).array()).all();
}

// Default tolerace for validation
constexpr double TOLERANCE = 1e-6;

bool isDiagonal(const Eigen::MatrixXcd &in_mat, double in_tol = TOLERANCE) {
  if (!isFinite(in_mat)) {
    return false;
  }

  for (int i = 0; i < in_mat.rows(); ++i) {
    for (int j = 0; j < in_mat.cols(); ++j) {
      if (i != j) {
        if (std::abs(in_mat(i, j)) > in_tol) {
          return false;
        }
      }
    }
  }

  return true;
}

bool allClose(const Eigen::MatrixXcd &in_mat1, const Eigen::MatrixXcd &in_mat2,
              double in_tol = TOLERANCE) {
  if (!isFinite(in_mat1) || !isFinite(in_mat2)) {
    return false;
  }

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

bool isHermitian(const Eigen::MatrixXcd &in_mat) {
  if (!isSquare(in_mat) || !isFinite(in_mat)) {
    return false;
  }
  return allClose(in_mat, in_mat.adjoint());
}

bool isUnitary(const Eigen::MatrixXcd &in_mat) {
  if (!isSquare(in_mat) || !isFinite(in_mat)) {
    return false;
  }

  Eigen::MatrixXcd Id =
      Eigen::MatrixXcd::Identity(in_mat.rows(), in_mat.cols());

  return allClose(in_mat * in_mat.adjoint(), Id);
}

bool isOrthogonal(const Eigen::MatrixXcd &in_mat, double in_tol = TOLERANCE) {
  if (!isSquare(in_mat) || !isFinite(in_mat)) {
    return false;
  }

  // Is real
  for (int i = 0; i < in_mat.rows(); ++i) {
    for (int j = 0; j < in_mat.cols(); ++j) {
      if (std::abs(in_mat(i, j).imag()) > in_tol) {
        return false;
      }
    }
  }
  // its transpose is its inverse
  return allClose(in_mat.inverse(), in_mat.transpose(), in_tol);
}
// Is Orthogonal and determinant == 1
bool isSpecialOrthogonal(const Eigen::MatrixXcd &in_mat, double in_tol = TOLERANCE) {
  return isOrthogonal(in_mat, in_tol) &&
         (std::abs(std::abs(in_mat.determinant()) - 1.0) < in_tol);
}

bool isCanonicalized(double x, double y, double z) {
  // 0 ≤ abs(z) ≤ y ≤ x ≤ pi/4
  // if x = pi/4, z >= 0
  const double TOL = TOLERANCE;
  if (std::abs(z) >= 0 && y >= std::abs(z) && x >= y && x <= M_PI_4 + TOL) {
    if (std::abs(x - M_PI_4) < TOL) {
      return (z >= 0);
    }
    return true;
  }
  return false;
}
// Compute exp(i(x XX + y YY + z ZZ)) matrix
Eigen::Matrix4cd interactionMatrixExp(double x, double y, double z) {
  Eigen::MatrixXcd X{Eigen::MatrixXcd::Zero(2, 2)};
  Eigen::MatrixXcd Y{Eigen::MatrixXcd::Zero(2, 2)};
  Eigen::MatrixXcd Z{Eigen::MatrixXcd::Zero(2, 2)};
  X << 0, 1, 1, 0;
  Y << 0, -I, I, 0;
  Z << 1, 0, 0, -1;
  auto XX = Eigen::kroneckerProduct(X, X);
  auto YY = Eigen::kroneckerProduct(Y, Y);
  auto ZZ = Eigen::kroneckerProduct(Z, Z);
  Eigen::MatrixXcd herm = x * XX + y * YY + z * ZZ;
  herm = I * herm;
  Eigen::MatrixXcd unitary = herm.exp();
  return unitary;
}

// Simplify the Z-Y-Z decomposition:
// i.e. combining rotations and removing trivial rotation
std::shared_ptr<xacc::CompositeInstruction>
simplifySingleQubitSeq(double zAngleBefore, double yAngle, double zAngleAfter,
                       size_t bitIdx) {
  auto zExpBefore = zAngleBefore / M_PI - 0.5;
  auto middleExp = yAngle / M_PI;
  std::string middlePauli = "Rx";
  auto zExpAfter = zAngleAfter / M_PI + 0.5;

  // Helper functions:
  const auto isNearZeroMod = [](double a, double period) -> bool {
    const auto halfPeriod = period / 2;
    const double TOL = 1e-8;
    return std::abs(fmod(a + halfPeriod, period) - halfPeriod) < TOL;
  };

  const auto toQuarterTurns = [](double in_exp) -> int {
    return static_cast<int>(round(2 * in_exp)) % 4;
  };

  const auto isCliffordRotation = [&](double in_exp) -> bool {
    return isNearZeroMod(in_exp, 0.5);
  };

  const auto isQuarterTurn = [&](double in_exp) -> bool {
    return (isCliffordRotation(in_exp) && toQuarterTurns(in_exp) % 2 == 1);
  };

  const auto isHalfTurn = [&](double in_exp) -> bool {
    return (isCliffordRotation(in_exp) && toQuarterTurns(in_exp) == 2);
  };

  const auto isNoTurn = [&](double in_exp) -> bool {
    return (isCliffordRotation(in_exp) && toQuarterTurns(in_exp) == 0);
  };

  // Clean up angles
  if (isCliffordRotation(zExpBefore)) {
    if ((isQuarterTurn(zExpBefore) || isQuarterTurn(zExpAfter)) !=
        (isHalfTurn(middleExp) && isNoTurn(zExpBefore - zExpAfter))) {
      zExpBefore += 0.5;
      zExpAfter -= 0.5;
      middlePauli = "Ry";
    }
    if (isHalfTurn(zExpBefore) || isHalfTurn(zExpAfter)) {
      zExpBefore -= 1;
      zExpAfter += 1;
      middleExp = -middleExp;
    }
  }
  if (isNoTurn(middleExp)) {
    zExpBefore += zExpAfter;
    zExpAfter = 0;
  } else if (isHalfTurn(middleExp)) {
    zExpAfter -= zExpBefore;
    zExpBefore = 0;
  }

  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto composite = gateRegistry->createComposite("__TEMP__COMPOSITE__" +
                                                 std::to_string(getTempId()));

  if (!isNoTurn(zExpBefore)) {
    composite->addInstruction(
        gateRegistry->createInstruction("Rz", {bitIdx}, {zExpBefore * M_PI}));
  }
  if (!isNoTurn(middleExp)) {
    composite->addInstruction(gateRegistry->createInstruction(
        middlePauli, {bitIdx}, {middleExp * M_PI}));
  }
  if (!isNoTurn(zExpAfter)) {
    composite->addInstruction(
        gateRegistry->createInstruction("Rz", {bitIdx}, {zExpAfter * M_PI}));
  }

  return composite;
}

std::shared_ptr<xacc::CompositeInstruction>
singleQubitGateGen(const Eigen::Matrix2cd &in_mat, size_t in_bitIdx) {
  using GateMatrix = Eigen::Matrix2cd;
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");

  // Use Z-Y decomposition of Nielsen and Chuang (Theorem 4.1).
  // An arbitrary one qubit gate matrix can be written as
  // U = [ exp(j*(a-b/2-d/2))*cos(c/2), -exp(j*(a-b/2+d/2))*sin(c/2)
  //       exp(j*(a+b/2-d/2))*sin(c/2), exp(j*(a+b/2+d/2))*cos(c/2)]
  // where a,b,c,d are real numbers.
  const auto singleQubitGateDecompose = [](const Eigen::Matrix2cd &matrix)
      -> std::tuple<double, double, double, double> {
    if (allClose(matrix, GateMatrix::Identity())) {
      return std::make_tuple(0.0, 0.0, 0.0, 0.0);
    }
    const auto checkParams = [&matrix](double a, double bHalf, double cHalf,
                                       double dHalf) {
      GateMatrix U;
      U << std::exp(I * (a - bHalf - dHalf)) * std::cos(cHalf),
          -std::exp(I * (a - bHalf + dHalf)) * std::sin(cHalf),
          std::exp(I * (a + bHalf - dHalf)) * std::sin(cHalf),
          std::exp(I * (a + bHalf + dHalf)) * std::cos(cHalf);

      return allClose(U, matrix);
    };

    double a, bHalf, cHalf, dHalf;
    const double TOLERANCE = 1e-9;
    if (std::abs(matrix(0, 1)) < TOLERANCE) {
      auto two_a = fmod(std::arg(matrix(0, 0) * matrix(1, 1)), 2 * M_PI);
      a = (std::abs(two_a) < TOLERANCE ||
           std::abs(two_a) > 2 * M_PI - TOLERANCE)
              ? 0
              : two_a / 2.0;
      auto dHalf = 0.0;
      auto b = std::arg(matrix(1, 1)) - std::arg(matrix(0, 0));
      std::vector<double> possibleBhalf{fmod(b / 2.0, 2 * M_PI),
                                        fmod(b / 2.0 + M_PI, 2.0 * M_PI)};
      std::vector<double> possibleChalf{0.0, M_PI};
      bool found = false;
      for (int i = 0; i < possibleBhalf.size(); ++i) {
        for (int j = 0; j < possibleChalf.size(); ++j) {
          bHalf = possibleBhalf[i];
          cHalf = possibleChalf[j];
          if (checkParams(a, bHalf, cHalf, dHalf)) {
            found = true;
            break;
          }
        }
        if (found) {
          break;
        }
      }
      assert(found);
      return std::make_tuple(a, bHalf, cHalf, dHalf);
    } else if (std::abs(matrix(0, 0)) < TOLERANCE) {
      auto two_a = fmod(std::arg(-matrix(0, 1) * matrix(1, 0)), 2 * M_PI);
      a = (std::abs(two_a) < TOLERANCE ||
           std::abs(two_a) > 2 * M_PI - TOLERANCE)
              ? 0
              : two_a / 2.0;
      dHalf = 0;
      auto b = std::arg(matrix(1, 0)) - std::arg(matrix(0, 1)) + M_PI;
      std::vector<double> possibleBhalf{fmod(b / 2., 2 * M_PI),
                                        fmod(b / 2. + M_PI, 2 * M_PI)};
      std::vector<double> possibleChalf{M_PI / 2., 3. / 2. * M_PI};
      bool found = false;
      for (int i = 0; i < possibleBhalf.size(); ++i) {
        for (int j = 0; j < possibleChalf.size(); ++j) {
          bHalf = possibleBhalf[i];
          cHalf = possibleChalf[j];
          if (checkParams(a, bHalf, cHalf, dHalf)) {
            found = true;
            break;
          }
        }
        if (found) {
          break;
        }
      }
      assert(found);
      return std::make_tuple(a, bHalf, cHalf, dHalf);
    } else {
      auto two_a = fmod(std::arg(matrix(0, 0) * matrix(1, 1)), 2 * M_PI);
      a = (std::abs(two_a) < TOLERANCE ||
           std::abs(two_a) > 2 * M_PI - TOLERANCE)
              ? 0
              : two_a / 2.0;
      auto two_d = 2. * std::arg(matrix(0, 1)) - 2. * std::arg(matrix(0, 0));
      std::vector<double> possibleDhalf{
          fmod(two_d / 4., 2 * M_PI), fmod(two_d / 4. + M_PI / 2., 2 * M_PI),
          fmod(two_d / 4. + M_PI, 2 * M_PI),
          fmod(two_d / 4. + 3. / 2. * M_PI, 2 * M_PI)};
      auto two_b = 2. * std::arg(matrix(1, 0)) - 2. * std::arg(matrix(0, 0));
      std::vector<double> possibleBhalf{
          fmod(two_b / 4., 2 * M_PI), fmod(two_b / 4. + M_PI / 2., 2 * M_PI),
          fmod(two_b / 4. + M_PI, 2 * M_PI),
          fmod(two_b / 4. + 3. / 2. * M_PI, 2 * M_PI)};
      auto tmp = std::acos(std::abs(matrix(1, 1)));
      std::vector<double> possibleChalf{
          fmod(tmp, 2 * M_PI), fmod(tmp + M_PI, 2 * M_PI),
          fmod(-1. * tmp, 2 * M_PI), fmod(-1. * tmp + M_PI, 2 * M_PI)};
      bool found = false;
      for (int i = 0; i < possibleBhalf.size(); ++i) {
        for (int j = 0; j < possibleChalf.size(); ++j) {
          for (int k = 0; k < possibleDhalf.size(); ++k) {
            bHalf = possibleBhalf[i];
            cHalf = possibleChalf[j];
            dHalf = possibleDhalf[k];
            if (checkParams(a, bHalf, cHalf, dHalf)) {
              found = true;
              break;
            }
          }
          if (found) {
            break;
          }
        }
        if (found) {
          break;
        }
      }
      assert(found);
      return std::make_tuple(a, bHalf, cHalf, dHalf);
    }
  };
  // Use Z-Y decomposition of Nielsen and Chuang (Theorem 4.1).
  // An arbitrary one qubit gate matrix can be writen as
  // U = [ exp(j*(a-b/2-d/2))*cos(c/2), -exp(j*(a-b/2+d/2))*sin(c/2)
  //       exp(j*(a+b/2-d/2))*sin(c/2), exp(j*(a+b/2+d/2))*cos(c/2)]
  // where a,b,c,d are real numbers.
  // Then U = exp(j*a) Rz(b) Ry(c) Rz(d).
  auto [a, bHalf, cHalf, dHalf] = singleQubitGateDecompose(in_mat);
  // Validate U = exp(j*a) Rz(b) Ry(c) Rz(d).
  const auto validate = [](const GateMatrix &in_mat, double a, double b,
                           double c, double d) {
    GateMatrix Rz_b, Ry_c, Rz_d;
    Rz_b << std::exp(-I * b / 2.0), 0, 0, std::exp(I * b / 2.0);
    Rz_d << std::exp(-I * d / 2.0), 0, 0, std::exp(I * d / 2.0);
    Ry_c << std::cos(c / 2), -std::sin(c / 2), std::sin(c / 2), std::cos(c / 2);
    auto mat = std::exp(I * a) * Rz_b * Ry_c * Rz_d;
    return allClose(in_mat, mat);
  };
  // Validate the *raw* decomposition
  assert(validate(in_mat, a, 2 * bHalf, 2 * cHalf, 2 * dHalf));

  // Simplify/optimize the sequence:
  auto composite =
      simplifySingleQubitSeq(2 * dHalf, 2 * cHalf, 2 * bHalf, in_bitIdx);

  // Validate the *simplified* sequence
  const auto validateSimplifiedSequence =
      [](const std::shared_ptr<xacc::CompositeInstruction> &in_composite,
         const GateMatrix &in_mat) {
        const auto Rx = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2.0), -I * std::sin(angle / 2.0),
              -I * std::sin(angle / 2.0), std::cos(angle / 2.0);
          return result;
        };
        const auto Ry = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2), -std::sin(angle / 2),
              std::sin(angle / 2), std::cos(angle / 2);
          return result;
        };
        const auto Rz = [](double angle) {
          GateMatrix result;
          result << std::exp(-I * angle / 2.0), 0, 0, std::exp(I * angle / 2.0);
          return result;
        };

        GateMatrix totalU = GateMatrix::Identity();
        for (size_t i = 0; i < in_composite->nInstructions(); ++i) {
          auto inst = in_composite->getInstruction(i);
          assert(inst->name() == "Rx" || inst->name() == "Ry" ||
                 inst->name() == "Rz");
          const auto angle = inst->getParameter(0).as<double>();
          if (inst->name() == "Rx") {
            totalU = Rx(angle) * totalU;
          }
          if (inst->name() == "Ry") {
            totalU = Ry(angle) * totalU;
          }
          if (inst->name() == "Rz") {
            totalU = Rz(angle) * totalU;
          }
        }

        // Normalize the upto global phase:
        // Find index of the largest element:
        size_t colIdx = 0;
        size_t rowIdx = 0;
        double maxVal = std::abs(totalU(0, 0));
        for (size_t i = 0; i < totalU.rows(); ++i) {
          for (size_t j = 0; j < totalU.cols(); ++j) {
            if (std::abs(totalU(i, j)) > maxVal) {
              maxVal = std::abs(totalU(i, j));
              colIdx = j;
              rowIdx = i;
            }
          }
        }

        const std::complex<double> globalFactor =
            in_mat(rowIdx, colIdx) / totalU(rowIdx, colIdx);
        totalU = globalFactor * totalU;
        return allClose(in_mat, totalU, TOLERANCE);
      };

  assert(validateSimplifiedSequence(composite, in_mat));
  return composite;
}
} // namespace

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> KAK::requiredKeys() { return {"unitary"}; }

bool KAK::expand(const HeterogeneousMap &parameters) {
  Eigen::Matrix4cd unitary;
  if (parameters.keyExists<Eigen::Matrix4cd>("unitary")) {
    unitary = parameters.get<Eigen::Matrix4cd>("unitary");
  } else if (parameters.keyExists<Eigen::MatrixXcd>("unitary")) {
    Eigen::MatrixXcd general_matrix =
        parameters.get<Eigen::MatrixXcd>("unitary");
    if (general_matrix.rows() != 4 && general_matrix.cols() != 4) {
      xacc::error("kak error - this matrix is not 4x4.");
    }

    unitary = Eigen::Map<Eigen::Matrix4cd>(general_matrix.data(), 4, 4);
  } else if (parameters.keyExists<std::vector<std::complex<double>>>(
                 "unitary")) {
    auto matAsVec =
        parameters.get<std::vector<std::complex<double>>>("unitary");
    // Correct size: 4 x 4
    if (matAsVec.size() == 16) {
      for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
          // Expect row-by-row layout
          unitary(row, col) = matAsVec[4 * row + col];
        }
      }
    }
  }

  if (!isUnitary(unitary)) {
    xacc::error("Input matrix is not a 4x4 unitary matrix");
    return false;
  }

  // Vector of qubits:
  // Default is {0, 1}
  // This can be specified if needed.
  std::vector<size_t> bits{0, 1};
  if (parameters.keyExists<std::vector<int>>("qubits")) {
    auto qubitVec = parameters.get<std::vector<int>>("qubits");
    if (qubitVec.size() != 2) {
      xacc::error("Expected 2 qubits.");
      return false;
    }
    bits[0] = qubitVec[0];
    bits[1] = qubitVec[1];
  }

  auto result = kakDecomposition(unitary);
  if (!result.has_value()) {
    return false;
  }

  auto composite = result->toGates(bits[0], bits[1]);
  addInstructions(composite->getInstructions());
  return true;
}

std::optional<KAK::KakDecomposition>
KAK::kakDecomposition(const InputMatrix &in_matrix) const {
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
  for (size_t i = 0; i < 4; ++i) {
    angles(i) = std::arg(diag[i]);
  }
  auto factors = KAK_GAMMA() * angles;
  KakDecomposition result;
  {
    result.g = std::exp(I * factors(0));
    result.a0 = a0;
    result.a1 = a1;
    result.b0 = b0;
    result.b1 = b1;
    result.x = factors(1).real();
    assert(std::abs(factors(1).imag()) < 1e-9);
    result.y = factors(2).real();
    assert(std::abs(factors(2).imag()) < 1e-9);
    result.z = factors(3).real();
    assert(std::abs(factors(3).imag()) < 1e-9);
  }

  const bool validateMatrix = allClose(result.toMat(), in_matrix);
  // Failed to validate
  if (!validateMatrix) {
    return std::nullopt;
  }

  auto canonicalizedInteraction =
      canonicalizeInteraction(result.x, result.y, result.z);

  // Combine the single-qubit blocks:
  result.b1 = canonicalizedInteraction.b1 * result.b1;
  result.b0 = canonicalizedInteraction.b0 * result.b0;
  result.a1 = result.a1 * canonicalizedInteraction.a1;
  result.a0 = result.a0 * canonicalizedInteraction.a0;
  result.g = result.g * canonicalizedInteraction.g;
  result.x = canonicalizedInteraction.x;
  result.y = canonicalizedInteraction.y;
  result.z = canonicalizedInteraction.z;

  assert(isCanonicalized(result.x, result.y, result.z));
  assert(allClose(result.toMat(), in_matrix));

  return result;
}

Eigen::MatrixXcd KAK::KakDecomposition::toMat() const {
  auto before = Eigen::kroneckerProduct(b1, b0);
  auto after = Eigen::kroneckerProduct(a1, a0);
  Eigen::MatrixXcd unitary = interactionMatrixExp(x, y, z);
  auto total = g * after * unitary * before;
  return total;
}

std::shared_ptr<CompositeInstruction>
KAK::KakDecomposition::toGates(size_t in_bit1, size_t in_bit2) const {
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  const auto generateInteractionComposite = [&](size_t bit1, size_t bit2,
                                                double x, double y, double z) {
    const double TOL = 1e-8;
    // Full decomposition is required
    if (std::abs(z) >= TOL) {
      const double xAngle = M_PI * (x * -2 / M_PI + 0.5);
      const double yAngle = M_PI * (y * -2 / M_PI + 0.5);
      const double zAngle = M_PI * (z * -2 / M_PI + 0.5);
      auto composite = gateRegistry->createComposite(
          "__TEMP__INTERACTION_COMPOSITE__" + std::to_string(getTempId()));

      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit2, bit1}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rz", {bit1}, {zAngle}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit1}, {M_PI_2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit2}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit1, bit2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit2}));
      composite->addInstruction(
          gateRegistry->createInstruction("Ry", {bit1}, {yAngle}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {xAngle}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit1, bit2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {-M_PI_2}));

      const auto validateGateSequence = [&](const Eigen::Matrix4cd &in_target) {
        const auto H = []() {
          GateMatrix result;
          result << 1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 1.0 / std::sqrt(2),
              -1.0 / std::sqrt(2);
          return result;
        };
        const auto Rx = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2.0), -I * std::sin(angle / 2.0),
              -I * std::sin(angle / 2.0), std::cos(angle / 2.0);
          return result;
        };
        const auto Ry = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2), -std::sin(angle / 2),
              std::sin(angle / 2), std::cos(angle / 2);
          return result;
        };
        const auto Rz = [](double angle) {
          GateMatrix result;
          result << std::exp(-I * angle / 2.0), 0, 0, std::exp(I * angle / 2.0);
          return result;
        };
        const auto CZ = []() {
          Eigen::Matrix4cd cz;
          cz << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1;
          return cz;
        };

        Eigen::Matrix2cd IdMat = Eigen::Matrix2cd::Identity();
        Eigen::Matrix4cd totalU = Eigen::Matrix4cd::Identity();
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(-M_PI_2));
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(xAngle));
        totalU *= Eigen::kroneckerProduct(Ry(yAngle), IdMat);
        totalU *= Eigen::kroneckerProduct(IdMat, H());
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(IdMat, H());
        totalU *= Eigen::kroneckerProduct(Rx(M_PI_2), IdMat);
        totalU *= Eigen::kroneckerProduct(Rz(zAngle), IdMat);
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        // Find index of the largest element:
        size_t colIdx = 0;
        size_t rowIdx = 0;
        double maxVal = std::abs(totalU(0, 0));
        for (size_t i = 0; i < totalU.rows(); ++i) {
          for (size_t j = 0; j < totalU.cols(); ++j) {
            if (std::abs(totalU(i, j)) > maxVal) {
              maxVal = std::abs(totalU(i, j));
              colIdx = j;
              rowIdx = i;
            }
          }
        }

        const std::complex<double> globalFactor =
            in_target(rowIdx, colIdx) / totalU(rowIdx, colIdx);
        totalU = globalFactor * totalU;
        return allClose(totalU, in_target);
      };

      assert(validateGateSequence(interactionMatrixExp(x, y, z)));
      return composite;
    }
    // ZZ interaction is near zero: only XX and YY
    else if (y >= TOL) {
      const double xAngle = -2 * x;
      const double yAngle = -2 * y;
      auto composite = gateRegistry->createComposite(
          "__TEMP__INTERACTION_COMPOSITE__" + std::to_string(getTempId()));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {M_PI_2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit2, bit1}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("Ry", {bit1}, {yAngle}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {xAngle}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit1, bit2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {-M_PI_2}));

      const auto validateGateSequence = [&](const Eigen::Matrix4cd &in_target) {
        const auto H = []() {
          GateMatrix result;
          result << 1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 1.0 / std::sqrt(2),
              -1.0 / std::sqrt(2);
          return result;
        };
        const auto Rx = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2.0), -I * std::sin(angle / 2.0),
              -I * std::sin(angle / 2.0), std::cos(angle / 2.0);
          return result;
        };
        const auto Ry = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2), -std::sin(angle / 2),
              std::sin(angle / 2), std::cos(angle / 2);
          return result;
        };
        const auto Rz = [](double angle) {
          GateMatrix result;
          result << std::exp(-I * angle / 2.0), 0, 0, std::exp(I * angle / 2.0);
          return result;
        };
        const auto CZ = []() {
          Eigen::Matrix4cd cz;
          cz << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1;
          return cz;
        };

        Eigen::Matrix2cd IdMat = Eigen::Matrix2cd::Identity();
        Eigen::Matrix4cd totalU = Eigen::Matrix4cd::Identity();
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(-M_PI_2));
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(xAngle));
        totalU *= Eigen::kroneckerProduct(Ry(yAngle), IdMat);
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(M_PI_2));

        // Find index of the largest element:
        size_t colIdx = 0;
        size_t rowIdx = 0;
        double maxVal = std::abs(totalU(0, 0));
        for (size_t i = 0; i < totalU.rows(); ++i) {
          for (size_t j = 0; j < totalU.cols(); ++j) {
            if (std::abs(totalU(i, j)) > maxVal) {
              maxVal = std::abs(totalU(i, j));
              colIdx = j;
              rowIdx = i;
            }
          }
        }

        const std::complex<double> globalFactor =
            in_target(rowIdx, colIdx) / totalU(rowIdx, colIdx);
        totalU = globalFactor * totalU;
        return allClose(totalU, in_target);
      };

      assert(validateGateSequence(interactionMatrixExp(x, y, z)));
      return composite;
    }
    // only XX is significant
    else {
      const double xAngle = -2 * x;
      auto composite = gateRegistry->createComposite(
          "__TEMP__INTERACTION_COMPOSITE__" + std::to_string(getTempId()));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit2, bit1}));
      composite->addInstruction(
          gateRegistry->createInstruction("Rx", {bit2}, {xAngle}));
      composite->addInstruction(
          gateRegistry->createInstruction("CZ", {bit1, bit2}));
      composite->addInstruction(gateRegistry->createInstruction("H", {bit1}));

      const auto validateGateSequence = [&](const Eigen::Matrix4cd &in_target) {
        const auto H = []() {
          GateMatrix result;
          result << 1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 1.0 / std::sqrt(2),
              -1.0 / std::sqrt(2);
          return result;
        };
        const auto Rx = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2.0), -I * std::sin(angle / 2.0),
              -I * std::sin(angle / 2.0), std::cos(angle / 2.0);
          return result;
        };
        const auto Ry = [](double angle) {
          GateMatrix result;
          result << std::cos(angle / 2), -std::sin(angle / 2),
              std::sin(angle / 2), std::cos(angle / 2);
          return result;
        };
        const auto Rz = [](double angle) {
          GateMatrix result;
          result << std::exp(-I * angle / 2.0), 0, 0, std::exp(I * angle / 2.0);
          return result;
        };
        const auto CZ = []() {
          Eigen::Matrix4cd cz;
          cz << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1;
          return cz;
        };

        Eigen::Matrix2cd IdMat = Eigen::Matrix2cd::Identity();
        Eigen::Matrix4cd totalU = Eigen::Matrix4cd::Identity();

        totalU *= Eigen::kroneckerProduct(H(), IdMat);
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(IdMat, Rx(xAngle));
        totalU *= CZ();
        totalU *= Eigen::kroneckerProduct(H(), IdMat);

        // Find index of the largest element:
        size_t colIdx = 0;
        size_t rowIdx = 0;
        double maxVal = std::abs(totalU(0, 0));
        for (size_t i = 0; i < totalU.rows(); ++i) {
          for (size_t j = 0; j < totalU.cols(); ++j) {
            if (std::abs(totalU(i, j)) > maxVal) {
              maxVal = std::abs(totalU(i, j));
              colIdx = j;
              rowIdx = i;
            }
          }
        }

        const std::complex<double> globalFactor =
            in_target(rowIdx, colIdx) / totalU(rowIdx, colIdx);
        totalU = globalFactor * totalU;
        return allClose(totalU, in_target);
      };

      assert(validateGateSequence(interactionMatrixExp(x, y, z)));
      return composite;
    }
  };

  auto a0Comp = singleQubitGateGen(a0, in_bit2);
  auto a1Comp = singleQubitGateGen(a1, in_bit1);
  auto b0Comp = singleQubitGateGen(b0, in_bit2);
  auto b1Comp = singleQubitGateGen(b1, in_bit1);
  auto interactionComp =
      generateInteractionComposite(in_bit2, in_bit1, x, y, z);
  auto totalComposite = gateRegistry->createComposite(
      "__TEMP__KAK_COMPOSITE__" + std::to_string(getTempId()));
  // U = g x (Gate A1 Gate A0) x exp(i(xXX + yYY + zZZ))x(Gate b1 Gate b0)
  // Before:
  totalComposite->addInstructions(b0Comp->getInstructions());
  totalComposite->addInstructions(b1Comp->getInstructions());
  // Interaction:
  totalComposite->addInstructions(interactionComp->getInstructions());
  // After:
  totalComposite->addInstructions(a0Comp->getInstructions());
  totalComposite->addInstructions(a1Comp->getInstructions());
  // Ignore global phase
  return totalComposite;
}

KAK::BidiagResult
KAK::bidiagonalizeUnitary(const InputMatrix &in_matrix) const {
  Eigen::Matrix4d realMat;
  Eigen::Matrix4d imagMat;
  for (int row = 0; row < in_matrix.rows(); ++row) {
    for (int col = 0; col < in_matrix.cols(); ++col) {
      realMat(row, col) = in_matrix(row, col).real();
      imagMat(row, col) = in_matrix(row, col).imag();
    }
  }
  // Assert A X B.T and A.T X B are hermitian
  assert(isHermitian(realMat * imagMat.transpose()));
  assert(isHermitian(realMat.transpose() * imagMat));

  auto [left, right] =
      bidiagonalizeRealMatrixPairWithSymmetricProducts(realMat, imagMat);

  // Convert to special orthogonal w/o breaking diagonalization.
  if (left.determinant() < 0) {
    for (int i = 0; i < left.cols(); ++i) {
      left(0, i) = -left(0, i);
    }
  }
  if (right.determinant() < 0) {
    for (int i = 0; i < right.rows(); ++i) {
      right(i, 0) = -right(i, 0);
    }
  }

  auto diag = left * in_matrix * right;
  // Validate:
  assert(isDiagonal(diag));

  std::vector<std::complex<double>> diagVec;
  for (int i = 0; i < diag.rows(); ++i) {
    diagVec.emplace_back(diag(i, i));
  }

  return std::make_tuple(left, diagVec, right);
}

std::tuple<std::complex<double>, KAK::GateMatrix, KAK::GateMatrix>
KAK::kronFactor(const InputMatrix &in_matrix) const {
  KAK::GateMatrix f1 = KAK::GateMatrix::Zero();
  KAK::GateMatrix f2 = KAK::GateMatrix::Zero();

  // Get row and column of the max element
  size_t a = 0;
  size_t b = 0;
  double maxVal = std::abs(in_matrix(a, b));
  for (int row = 0; row < in_matrix.rows(); ++row) {
    for (int col = 0; col < in_matrix.cols(); ++col) {
      if (std::abs(in_matrix(row, col)) > maxVal) {
        a = row;
        b = col;
        maxVal = std::abs(in_matrix(a, b));
      }
    }
  }

  // Extract sub-factors touching the reference cell.
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      f1((a >> 1) ^ i, (b >> 1) ^ j) = in_matrix(a ^ (i << 1), b ^ (j << 1));
      f2((a & 1) ^ i, (b & 1) ^ j) = in_matrix(a ^ i, b ^ j);
    }
  }

  // Rescale factors to have unit determinants.
  f1 /= (std::sqrt(f1.determinant()));
  f2 /= (std::sqrt(f2.determinant()));

  // Determine global phase.
  std::complex<double> g =
      in_matrix(a, b) / (f1(a >> 1, b >> 1) * f2(a & 1, b & 1));
  if (g.real() < 0.0) {
    f1 *= -1;
    g = -g;
  }

  // Validate:
  Eigen::Matrix4cd testMat = g * Eigen::kroneckerProduct(f1, f2);
  assert(allClose(testMat, in_matrix));

  return std::make_tuple(g, f1, f2);
}

std::pair<KAK::GateMatrix, KAK::GateMatrix>
KAK::so4ToMagicSu2s(const InputMatrix &in_matrix) const {
  assert(isSpecialOrthogonal(in_matrix));
  auto matInMagicBasis = KAK_MAGIC() * in_matrix * KAK_MAGIC_DAG();
  auto [g, f1, f2] = kronFactor(matInMagicBasis);
  return std::make_pair(f1, f2);
}

Eigen::MatrixXd
KAK::diagonalizeRealSymmetricMatrix(const Eigen::MatrixXd &in_mat) const {
  assert(isHermitian(in_mat));
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(in_mat);
  Eigen::MatrixXd p = solver.eigenvectors();
  // Orthogonal basis (Hermitian/symmetric matrix)
  assert(isOrthogonal(p));
  // An orthogonal matrix P such that PT x matrix x P is diagonal.
  assert(isDiagonal(p.transpose() * in_mat * p));
  return p;
}

Eigen::MatrixXd KAK::diagonalizeRealSymmetricAndSortedDiagonalMatrices(
    const Eigen::MatrixXd &in_symMat, const Eigen::MatrixXd &in_diagMat) const {
  assert(isDiagonal(in_diagMat));
  assert(isHermitian(in_symMat));
  const auto similarSingular = [&in_diagMat](int i, int j) {
    return std::abs(in_diagMat(i, i) - in_diagMat(j, j)) < 1e-5;
  };

  const auto ranges = contiguousGroups(in_diagMat.rows(), similarSingular);
  Eigen::MatrixXd p = Eigen::MatrixXd::Zero(in_symMat.rows(), in_symMat.cols());

  for (const auto &[start, end] : ranges) {
    const int blockSize = end - start;

    Eigen::MatrixXd block = Eigen::MatrixXd(blockSize, blockSize);
    for (int i = 0; i < blockSize; ++i) {
      for (int j = 0; j < blockSize; ++j) {
        block(i, j) = in_symMat(i + start, j + start);
      }
    }
    auto blockDiag = diagonalizeRealSymmetricMatrix(block);

    for (int i = 0; i < blockSize; ++i) {
      for (int j = 0; j < blockSize; ++j) {
        p(i + start, j + start) = blockDiag(i, j);
      }
    }
  }

  // P.T x symmetric_matrix x P is diagonal
  assert(isDiagonal(p.transpose() * in_symMat * p));
  // and P.T x diagonal_matrix x P = diagonal_matrix
  assert(allClose(p.transpose() * in_diagMat * p, in_diagMat));

  return p;
}

std::pair<Eigen::Matrix4d, Eigen::Matrix4d>
KAK::bidiagonalizeRealMatrixPairWithSymmetricProducts(
    const Eigen::Matrix4d &in_mat1, const Eigen::Matrix4d &in_mat2) const {
  const auto svd = [](const Eigen::MatrixXd &in_mat)
      -> std::tuple<Eigen::MatrixXd, Eigen::VectorXd, Eigen::MatrixXd> {
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(in_mat, Eigen::ComputeThinU |
                                                      Eigen::ComputeThinV);
    return std::make_tuple(svd.matrixU(), svd.singularValues(),
                           svd.matrixV().adjoint());
  };
  // Use SVD to bi-diagonalize the first matrix.
  auto [baseLeft, baseDiagVec, baseRight] = svd(in_mat1);

  Eigen::MatrixXd baseDiag =
      Eigen::MatrixXd::Zero(baseDiagVec.size(), baseDiagVec.size());
  for (int i = 0; i < baseDiagVec.size(); ++i) {
    baseDiag(i, i) = baseDiagVec(i);
  }

  // Determine where we switch between diagonalization-fixup strategies.
  const auto dim = baseDiag.rows();
  auto rank = dim;
  while (rank > 0 && std::abs(baseDiag(rank - 1, rank - 1) < 1e-5)) {
    rank--;
  }
  Eigen::MatrixXd baseDiagTrim = Eigen::MatrixXd::Zero(rank, rank);
  for (int i = 0; i < rank; ++i) {
    for (int j = 0; j < rank; ++j) {
      baseDiagTrim(i, j) = baseDiag(i, j);
    }
  }

  // Try diagonalizing the second matrix with the same factors as the first.
  auto semiCorrected = baseLeft.transpose() * in_mat2 * baseRight.transpose();

  Eigen::MatrixXd overlap = Eigen::MatrixXd::Zero(rank, rank);
  for (int i = 0; i < rank; ++i) {
    for (int j = 0; j < rank; ++j) {
      overlap(i, j) = semiCorrected(i, j);
    }
  }

  auto overlapAdjust =
      diagonalizeRealSymmetricAndSortedDiagonalMatrices(overlap, baseDiagTrim);

  const auto extraSize = dim - rank;
  Eigen::MatrixXd extra(extraSize, extraSize);
  for (int i = 0; i < extraSize; ++i) {
    for (int j = 0; j < extraSize; ++j) {
      extra(i, j) = semiCorrected(i + rank, j + rank);
    }
  }

  static const auto emptySvdResult =
      std::make_tuple(Eigen::MatrixXd::Zero(0, 0), Eigen::VectorXd::Zero(0),
                      Eigen::MatrixXd::Zero(0, 0));
  auto [extraLeftAdjust, extraDiag, extraRightAdjust] =
      (dim > rank) ? svd(extra) : emptySvdResult;

  auto leftAdjust = blockDiag(overlapAdjust, extraLeftAdjust);
  auto rightAdjust = blockDiag(overlapAdjust.transpose(), extraRightAdjust);
  auto left = leftAdjust.transpose() * baseLeft.transpose();
  auto right = baseRight.transpose() * rightAdjust.transpose();
  // L x mat1 x R and L x mat2 x R are diagonal matrices.
  assert(isDiagonal(left * in_mat1 * right));
  assert(isDiagonal(left * in_mat2 * right));
  return std::make_pair(left, right);
}

KAK::KakDecomposition KAK::canonicalizeInteraction(double x, double y,
                                                   double z) const {
  // Accumulated global phase.
  std::complex<double> phase = 1.0;
  // Per-qubit left factors.
  std::vector<GateMatrix> left{GateMatrix::Identity(), GateMatrix::Identity()};
  // Per-qubit right factors.
  std::vector<GateMatrix> right{GateMatrix::Identity(), GateMatrix::Identity()};
  // Remaining XX/YY/ZZ interaction vector.
  std::vector<double> v{x, y, z};

  std::vector<GateMatrix> flippers{(GateMatrix() << 0, I, I, 0).finished(),
                                   (GateMatrix() << 0, 1, -1, 0).finished(),
                                   (GateMatrix() << I, 0, 0, -I).finished()};

  std::vector<GateMatrix> swappers{
      (GateMatrix() << I * M_SQRT1_2, M_SQRT1_2, -M_SQRT1_2, -I * M_SQRT1_2)
          .finished(),
      (GateMatrix() << I * M_SQRT1_2, I * M_SQRT1_2, I * M_SQRT1_2,
       -I * M_SQRT1_2)
          .finished(),
      (GateMatrix() << 0, I * M_SQRT1_2 + M_SQRT1_2, I * M_SQRT1_2 - M_SQRT1_2,
       0)
          .finished()};

  const auto shift = [&](int k, int step) {
    v[k] += step * M_PI_2;
    phase *= std::pow(I, step);
    const auto expFact = ((step % 4) + 4) % 4;
    const GateMatrix mat = flippers[k].array().pow(expFact);
    right[0] = mat * right[0];
    right[1] = mat * right[1];
  };

  const auto negate = [&](int k1, int k2) {
    v[k1] *= -1;
    v[k2] *= -1;
    phase *= -1;
    const auto &s = flippers[3 - k1 - k2];
    left[1] = left[1] * s;
    right[1] = s * right[1];
  };

  const auto swap = [&](int k1, int k2) {
    std::iter_swap(v.begin() + k1, v.begin() + k2);
    const auto &s = swappers[3 - k1 - k2];
    left[0] = left[0] * s;
    left[1] = left[1] * s;
    right[0] = s * right[0];
    right[1] = s * right[1];
  };

  const auto canonicalShift = [&](int k) {
    while (v[k] <= -M_PI_4) {
      shift(k, +1);
    }
    while (v[k] > M_PI_4) {
      shift(k, -1);
    }
  };

  const auto sort = [&]() {
    if (std::abs(v[0]) < std::abs(v[1])) {
      swap(0, 1);
    }
    if (std::abs(v[1]) < std::abs(v[2])) {
      swap(1, 2);
    }
    if (std::abs(v[0]) < std::abs(v[1])) {
      swap(0, 1);
    }
  };

  canonicalShift(0);
  canonicalShift(1);
  canonicalShift(2);
  sort();

  if (v[0] < 0) {
    negate(0, 2);
  }
  if (v[1] < 0) {
    negate(1, 2);
  }
  canonicalShift(2);

  if ((v[0] > M_PI_4 - 1e-9) && (v[2] < 0)) {
    shift(0, -1);
    negate(0, 2);
  }

  assert(isCanonicalized(v[0], v[1], v[2]));

  KakDecomposition result;
  {
    result.g = phase;
    result.a0 = left[1];
    result.a1 = left[0];
    result.b0 = right[1];
    result.b1 = right[0];
    result.x = v[0];
    result.y = v[1];
    result.z = v[2];
  }

  assert(allClose(result.toMat(), interactionMatrixExp(x, y, z)));
  return result;
}
bool ZYZ::expand(const xacc::HeterogeneousMap &runtimeOptions) {
  Eigen::Matrix2cd unitary;
  if (runtimeOptions.keyExists<Eigen::Matrix2cd>("unitary")) {
    unitary = runtimeOptions.get<Eigen::Matrix2cd>("unitary");
  } else if (runtimeOptions.keyExists<Eigen::MatrixXcd>("unitary")) {
    Eigen::MatrixXcd general_matrix =
        runtimeOptions.get<Eigen::MatrixXcd>("unitary");
    if (general_matrix.rows() != 2 && general_matrix.cols() != 2) {
      xacc::error("kak error - this matrix is not 2x2.");
    }

    unitary = Eigen::Map<Eigen::Matrix2cd>(general_matrix.data(), 2, 2);
  } else if (runtimeOptions.keyExists<std::vector<std::complex<double>>>(
                 "unitary")) {
    auto matAsVec =
        runtimeOptions.get<std::vector<std::complex<double>>>("unitary");
    // Correct size: 2 x 2
    if (matAsVec.size() == 4) {
      for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
          // Expect row-by-row layout
          unitary(row, col) = matAsVec[2 * row + col];
        }
      }
    }
  } else {
    xacc::error("unitary matrix is required.");
    return false;
  }

  assert(isUnitary(unitary));
  auto decomposed = singleQubitGateGen(unitary, 0);
  addInstructions(decomposed->getInstructions());
  return true;
}
} // namespace circuits
} // namespace xacc