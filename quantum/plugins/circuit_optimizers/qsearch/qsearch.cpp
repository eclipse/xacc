#include "qsearch.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_plugin.hpp"
#include <numeric>
#include "InstructionIterator.hpp"

namespace xacc {
namespace quantum {
QsearchOptimizer::QsearchOptimizer() {}

void QsearchOptimizer::apply(std::shared_ptr<CompositeInstruction> program,
                             const std::shared_ptr<Accelerator> accelerator,
                             const HeterogeneousMap &options) {

  if (!xacc::hasService<Instruction>("qsearch")) {
    xacc::error("Cannot use the qsearch circuit optimizer, qsearch circuit "
                "synthesis is not installed.");
  }

  if (!circ_synth) {
    circ_synth = std::dynamic_pointer_cast<xacc::CompositeInstruction>(
        xacc::getService<xacc::Instruction>("qsearch"));
  }

  auto visitor =
      std::make_shared<KernelToUnitaryVisitor>(program->nPhysicalBits());
  xacc::InstructionIterator iter(program);
  while (iter.hasNext()) {
    auto inst = iter.next();
    if (!inst->isComposite() && inst->isEnabled()) {
      inst->accept(visitor);
    }
  }
  auto unitary = visitor->getMat();

  HeterogeneousMap m;
  m.insert("unitary", unitary);
  m.merge(options);
  circ_synth->clear();
  circ_synth->expand(m);

  if (circ_synth->nInstructions() < program->nInstructions()) {
    std::stringstream ss;
    ss << "qsearch optimized circuit from " << program->nInstructions() << " to " << circ_synth->nInstructions() << " gates.";
    // Optimizer found a more efficient circuit, program now has "
    //    << circ_synth->nInstructions() << " instructions instead of "
    //    << program->nInstructions();
    xacc::info(ss.str());

    program->clear();
    program->addInstructions(circ_synth->getInstructions());
  }
}

using namespace Eigen;

using namespace xacc::quantum;

MatrixXcd X_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd Y_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd Z_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd Id_Mat{MatrixXcd::Identity(2, 2)};
MatrixXcd H_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd T_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd Tdg_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd S_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd Sdg_Mat{MatrixXcd::Zero(2, 2)};
MatrixXcd CX_Mat{MatrixXcd::Zero(4, 4)};
MatrixXcd CY_Mat{MatrixXcd::Zero(4, 4)};
MatrixXcd CZ_Mat{MatrixXcd::Zero(4, 4)};
MatrixXcd CH_Mat{MatrixXcd::Zero(4, 4)};
MatrixXcd Swap_Mat{MatrixXcd::Zero(4, 4)};

MatrixXcd kroneckerProduct(MatrixXcd &lhs, MatrixXcd &rhs) {
  MatrixXcd result(lhs.rows() * rhs.rows(), lhs.cols() * rhs.cols());
  for (int i = 0; i < lhs.rows(); ++i) {
    for (int j = 0; j < lhs.cols(); ++j) {
      result.block(i * rhs.rows(), j * rhs.cols(), rhs.rows(), rhs.cols()) =
          lhs(i, j) * rhs;
    }
  }
  return result;
}

KernelToUnitaryVisitor::KernelToUnitaryVisitor(size_t in_nbQubits)
    : m_nbQubit(in_nbQubits) {
  static bool static_gate_init = false;
  m_circuitMat = MatrixXcd::Identity(1ULL << in_nbQubits, 1ULL << in_nbQubits);
  if (!static_gate_init) {
    X_Mat << 0.0 + 0.0_i, 1.0 + 0.0_i, 1.0 + 0.0_i, 0.0 + 0.0_i;
    Y_Mat << 0.0 + 0.0_i, -I, I, 0.0 + 0.0_i;
    Z_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, -1.0 + 0.0_i;
    H_Mat << 1.0 / std::sqrt(2.0) + 0.0_i, 1.0 / std::sqrt(2.0) + 0.0_i,
        1.0 / std::sqrt(2.0) + 0.0_i, -1.0 / std::sqrt(2.0) + 0.0_i;
    T_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, std::exp(I * M_PI / 4.0);
    Tdg_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, std::exp(-I * M_PI / 4.0);
    S_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, I;
    Sdg_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, -I;
    CX_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        0.0 + 0.0_i, 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 1.0 + 0.0_i,
        0.0 + 0.0_i;
    CY_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        0.0 + 0.0_i, -I, 0.0 + 0.0_i, 0.0 + 0.0_i, I, 0.0 + 0.0_i;
    CZ_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        -1.0 + 0.0_i;
    CH_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 / std::sqrt(2.0) + 0.0_i, 1.0 / std::sqrt(2.0) + 0.0_i, 0.0 + 0.0_i,
        0.0 + 0.0_i, 1.0 / std::sqrt(2.0) + 0.0_i,
        -1.0 / std::sqrt(2.0) + 0.0_i;
    Swap_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        0.0 + 0.0_i, 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 1.0 + 0.0_i,
        0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
        1.0 + 0.0_i;
    static_gate_init = true;
  }
}
void KernelToUnitaryVisitor::visit(Hadamard &h) {
  m_circuitMat = singleQubitGateExpand(H_Mat, h.bits()[0]) * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(CNOT &cnot) {
  MatrixXcd fullMat =
      twoQubitGateExpand(CX_Mat, cnot.bits()[0], cnot.bits()[1]);
  m_circuitMat = fullMat * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(Rz &rz) {
  double theta = InstructionParameterToDouble(rz.getParameter(0));
  MatrixXcd fullMat =
      singleParametricQubitGateExpand(theta, Rot::Z, rz.bits()[0]);
  m_circuitMat = fullMat * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(Ry &ry) {
  double theta = InstructionParameterToDouble(ry.getParameter(0));
  MatrixXcd fullMat =
      singleParametricQubitGateExpand(theta, Rot::Y, ry.bits()[0]);
  m_circuitMat = fullMat * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(Rx &rx) {
  double theta = InstructionParameterToDouble(rx.getParameter(0));
  MatrixXcd fullMat =
      singleParametricQubitGateExpand(theta, Rot::X, rx.bits()[0]);
  m_circuitMat = fullMat * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(X &x) {
  m_circuitMat = singleQubitGateExpand(X_Mat, x.bits()[0]) * m_circuitMat;
}

void KernelToUnitaryVisitor::visit(Y &y) {
  m_circuitMat = singleQubitGateExpand(Y_Mat, y.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(Z &z) {
  m_circuitMat = singleQubitGateExpand(Z_Mat, z.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(CY &cy) {
  MatrixXcd fullMat = twoQubitGateExpand(CY_Mat, cy.bits()[0], cy.bits()[1]);
  m_circuitMat = fullMat * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(CZ &cz) {
  MatrixXcd fullMat = twoQubitGateExpand(CZ_Mat, cz.bits()[0], cz.bits()[1]);
  m_circuitMat = fullMat * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(Swap &s) {
  MatrixXcd fullMat = twoQubitGateExpand(Swap_Mat, s.bits()[0], s.bits()[1]);
  m_circuitMat = fullMat * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(CRZ &crz) {}
void KernelToUnitaryVisitor::visit(CH &ch) {
  MatrixXcd fullMat = twoQubitGateExpand(CH_Mat, ch.bits()[0], ch.bits()[1]);
  m_circuitMat = fullMat * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(S &s) {
  m_circuitMat = singleQubitGateExpand(S_Mat, s.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(Sdg &sdg) {
  m_circuitMat = singleQubitGateExpand(Sdg_Mat, sdg.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(T &t) {
  m_circuitMat = singleQubitGateExpand(T_Mat, t.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(Tdg &tdg) {
  m_circuitMat = singleQubitGateExpand(Tdg_Mat, tdg.bits()[0]) * m_circuitMat;
}
void KernelToUnitaryVisitor::visit(CPhase &cphase) {
  xacc::error("Need to implement CPhase gate to matrix.");
}

void KernelToUnitaryVisitor::visit(Measure &measure) {}
void KernelToUnitaryVisitor::visit(Identity &i) {}
void KernelToUnitaryVisitor::visit(U &u) {
  double in_theta = u.getParameter(0).as<double>();
  double in_phi = u.getParameter(1).as<double>();
  double in_lambda = u.getParameter(2).as<double>();

  MatrixXcd u_mat(2, 2);
  u_mat << std::cos(in_theta / 2.0),
      -std::exp(std::complex<double>(0, in_lambda)) * std::sin(in_theta / 2.0),
      std::exp(std::complex<double>(0, in_phi)) * std::sin(in_theta / 2.0),
      std::exp(std::complex<double>(0, in_phi + in_lambda)) *
          std::cos(in_theta / 2.0);
  m_circuitMat = singleQubitGateExpand(u_mat, u.bits()[0]) * m_circuitMat;

  // xacc::error("We don't support U3 gate to matrix.");
}

void KernelToUnitaryVisitor::visit(IfStmt &ifStmt) {}
// Identifiable Impl
MatrixXcd KernelToUnitaryVisitor::getMat() const { return m_circuitMat; }
MatrixXcd KernelToUnitaryVisitor::singleQubitGateExpand(MatrixXcd &in_gateMat,
                                                        size_t in_loc) const {
  if (m_nbQubit == 1) {
    return in_gateMat;
  }
  if (in_loc == 0) {
    auto dim = 1ULL << (m_nbQubit - 1);
    MatrixXcd rhs = MatrixXcd::Identity(dim, dim);
    return kroneckerProduct(in_gateMat, rhs);
  } else if (in_loc == m_nbQubit - 1) {
    auto dim = 1ULL << (m_nbQubit - 1);
    MatrixXcd lhs = MatrixXcd::Identity(dim, dim);
    return kroneckerProduct(lhs, in_gateMat);
  } else {
    auto leftDim = 1ULL << in_loc;
    auto rightDim = 1ULL << (m_nbQubit - 1 - in_loc);
    MatrixXcd lhs = MatrixXcd::Identity(leftDim, leftDim);
    MatrixXcd rhs = MatrixXcd::Identity(rightDim, rightDim);
    MatrixXcd lhsKron = kroneckerProduct(lhs, in_gateMat);
    return kroneckerProduct(lhsKron, rhs);
  }
}

MatrixXcd KernelToUnitaryVisitor::singleParametricQubitGateExpand(
    double in_var, Rot in_rotType, size_t in_bitLoc) const {
  static const std::complex<double> I_dual = I;
  MatrixXcd gateMat = [&]() {
    switch (in_rotType) {
    case Rot::X: {
      MatrixXcd rxMat =
          static_cast<std::complex<double>>(cos(in_var / 2)) *
              MatrixXcd::Identity(2, 2) -
          I_dual * static_cast<std::complex<double>>(sin(in_var / 2)) * X_Mat;
      return rxMat;
    }
    case Rot::Y: {
      MatrixXcd ryMat =
          static_cast<std::complex<double>>(cos(in_var / 2)) *
              MatrixXcd::Identity(2, 2) -
          I_dual * static_cast<std::complex<double>>(sin(in_var / 2)) * Y_Mat;
      return ryMat;
    }
    case Rot::Z: {
      MatrixXcd rzMat =
          static_cast<std::complex<double>>(cos(in_var / 2)) *
              MatrixXcd::Identity(2, 2) -
          I_dual * static_cast<std::complex<double>>(sin(in_var / 2)) * Z_Mat;
      return rzMat;
    }
    default:
      __builtin_unreachable();
    }
  }();
  return singleQubitGateExpand(gateMat, in_bitLoc);
}

MatrixXcd KernelToUnitaryVisitor::twoQubitGateExpand(MatrixXcd &in_gateMat,
                                                     size_t in_bit1,
                                                     size_t in_bit2) const {
  const auto matSize = 1ULL << m_nbQubit;
  MatrixXcd resultMat = MatrixXcd::Identity(matSize, matSize);
  // Qubit index list (0->(dim-1))
  std::vector<size_t> index_list(m_nbQubit);
  std::iota(index_list.begin(), index_list.end(), 0);
  const std::vector<size_t> idx{m_nbQubit - in_bit2 - 1,
                                m_nbQubit - in_bit1 - 1};
  for (size_t k = 0; k < matSize; ++k) {
    std::vector<std::complex<double>> oldColumn(matSize);
    for (size_t i = 0; i < matSize; ++i) {
      oldColumn[i] = resultMat(i, k);
    }

    for (size_t i = 0; i < matSize; ++i) {
      size_t local_i = 0;
      for (size_t l = 0; l < idx.size(); ++l) {
        local_i |= ((i >> idx[l]) & 1) << l;
      }

      std::complex<double> res = 0.0_i;
      for (size_t j = 0; j < (1ULL << idx.size()); ++j) {
        size_t locIdx = i;
        for (size_t l = 0; l < idx.size(); ++l) {
          if (((j >> l) & 1) != ((i >> idx[l]) & 1)) {
            locIdx ^= (1ULL << idx[l]);
          }
        }

        res += oldColumn[locIdx] * in_gateMat(local_i, j);
      }

      resultMat(i, k) = res;
    }
  }

  return resultMat;
}

} // namespace quantum
} // namespace xacc

REGISTER_PLUGIN(xacc::quantum::QsearchOptimizer, xacc::IRTransformation)