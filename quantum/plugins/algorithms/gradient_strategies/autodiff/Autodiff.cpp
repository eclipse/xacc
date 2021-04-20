
#include "Autodiff.hpp"
#include "xacc_plugin.hpp"
#include "AllGateVisitor.hpp"
#include "xacc.hpp"
#include <numeric>

namespace {
using namespace xacc;
using namespace xacc::quantum;
using namespace autodiff;
MatrixXcdual X_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual Y_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual Z_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual Id_Mat{MatrixXcdual::Identity(2, 2)};
MatrixXcdual H_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual T_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual Tdg_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual S_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual Sdg_Mat{MatrixXcdual::Zero(2, 2)};
MatrixXcdual CX_Mat{MatrixXcdual::Zero(4, 4)};
MatrixXcdual CY_Mat{MatrixXcdual::Zero(4, 4)};
MatrixXcdual CZ_Mat{MatrixXcdual::Zero(4, 4)};
MatrixXcdual CH_Mat{MatrixXcdual::Zero(4, 4)};
MatrixXcdual Swap_Mat{MatrixXcdual::Zero(4, 4)};

constexpr std::complex<double> I{0.0, 1.0};
// Note: cannot use std::complex_literal 
// because https://gcc.gnu.org/onlinedocs/gcc/Complex.html#Complex
inline constexpr std::complex<double> operator"" _i(long double x) noexcept {
  return {0., static_cast<double>(x)};
}

MatrixXcdual kroneckerProduct(MatrixXcdual &lhs, MatrixXcdual &rhs) {
  MatrixXcdual result(lhs.rows() * rhs.rows(), lhs.cols() * rhs.cols());
  for (int i = 0; i < lhs.rows(); ++i) {
    for (int j = 0; j < lhs.cols(); ++j) {
      result.block(i * rhs.rows(), j * rhs.cols(), rhs.rows(), rhs.cols()) =
          lhs(i, j) * rhs;
    }
  }
  return result;
}

enum class Rot { X, Y, Z };

class AutodiffCircuitVisitor : public AllGateVisitor {
public:
  AutodiffCircuitVisitor(
      size_t in_nbQubits,
      const std::unordered_map<std::string, autodiff::dual> &in_varMap)
      : m_nbQubit(in_nbQubits), m_varMap(in_varMap) {
    static bool static_gate_init = false;
    m_circuitMat =
        MatrixXcdual::Identity(1ULL << in_nbQubits, 1ULL << in_nbQubits);
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
          0.0 + 0.0_i, 1.0 / std::sqrt(2.0) + 0.0_i, -1.0 / std::sqrt(2.0) + 0.0_i;
      Swap_Mat << 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
          0.0 + 0.0_i, 1.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 1.0 + 0.0_i,
          0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i, 0.0 + 0.0_i,
          1.0 + 0.0_i;
      static_gate_init = true;
    }
  }
  // Gate visitor Impl
  void visit(Hadamard &h) override {
    m_circuitMat = singleQubitGateExpand(H_Mat, h.bits()[0]) * m_circuitMat;
  }

  void visit(CNOT &cnot) override {
    MatrixXcdual fullMat =
        twoQubitGateExpand(CX_Mat, cnot.bits()[0], cnot.bits()[1]);
    m_circuitMat = fullMat * m_circuitMat;
  }

  void visit(Rz &rz) override {
    const bool isVariationalGate = rz.getParameter(0).isVariable();
    // IMPORTANT: we can only handle direct variable usage here,
    // i.e. we don't allow expressions of variables here.
    if (isVariationalGate) {
      InstructionParameter p = rz.getParameter(0);
      const auto [coeff, varName] = parseVarString(p.toString());
      const auto iter = m_varMap.find(varName);
      if (iter != m_varMap.end()) {
        MatrixXcdual fullMat = singleParametricQubitGateExpand(
            coeff * iter->second, Rot::Z, rz.bits()[0]);
        m_circuitMat = fullMat * m_circuitMat;
      } else {
        xacc::error("Unknown variable named '" + varName + "' encountered.");
      }
    } else {
      // Non-parametrized gate, i.e. the angle is fixed, not a variable.
      autodiff::dual theta = InstructionParameterToDouble(rz.getParameter(0));
      MatrixXcdual fullMat =
          singleParametricQubitGateExpand(theta, Rot::Z, rz.bits()[0]);
      m_circuitMat = fullMat * m_circuitMat;
    }
  }

  void visit(Ry &ry) override {
    const bool isVariationalGate = ry.getParameter(0).isVariable();
    // IMPORTANT: we can only handle direct variable usage here,
    // i.e. we don't allow expressions of variables here.
    if (isVariationalGate) {
      InstructionParameter p = ry.getParameter(0);
      const auto [coeff, varName] = parseVarString(p.toString());
      const auto iter = m_varMap.find(varName);
      if (iter != m_varMap.end()) {
        MatrixXcdual fullMat = singleParametricQubitGateExpand(
            coeff * iter->second, Rot::Y, ry.bits()[0]);
        m_circuitMat = fullMat * m_circuitMat;
      } else {
        xacc::error("Unknown variable named '" + varName + "' encountered.");
      }
    } else {
      // Non-parametrized gate, i.e. the angle is fixed, not a variable.
      autodiff::dual theta = InstructionParameterToDouble(ry.getParameter(0));
      MatrixXcdual fullMat =
          singleParametricQubitGateExpand(theta, Rot::Y, ry.bits()[0]);
      m_circuitMat = fullMat * m_circuitMat;
    }
  }

  void visit(Rx &rx) override {
    const bool isVariationalGate = rx.getParameter(0).isVariable();
    // IMPORTANT: we can only handle direct variable usage here,
    // i.e. we don't allow expressions of variables here.
    if (isVariationalGate) {
      InstructionParameter p = rx.getParameter(0);
      const auto [coeff, varName] = parseVarString(p.toString());
      const auto iter = m_varMap.find(varName);
      if (iter != m_varMap.end()) {
        MatrixXcdual fullMat = singleParametricQubitGateExpand(
            coeff * iter->second, Rot::X, rx.bits()[0]);
        m_circuitMat = fullMat * m_circuitMat;
      } else {
        xacc::error("Unknown variable named '" + varName + "' encountered.");
      }
    } else {
      // Non-parametrized gate, i.e. the angle is fixed, not a variable.
      autodiff::dual theta = InstructionParameterToDouble(rx.getParameter(0));
      MatrixXcdual fullMat =
          singleParametricQubitGateExpand(theta, Rot::X, rx.bits()[0]);
      m_circuitMat = fullMat * m_circuitMat;
    }
  }

  void visit(X &x) override {
    m_circuitMat = singleQubitGateExpand(X_Mat, x.bits()[0]) * m_circuitMat;
  }

  void visit(Y &y) override {
    m_circuitMat = singleQubitGateExpand(Y_Mat, y.bits()[0]) * m_circuitMat;
  }
  void visit(Z &z) override {
    m_circuitMat = singleQubitGateExpand(Z_Mat, z.bits()[0]) * m_circuitMat;
  }
  void visit(CY &cy) override {
    MatrixXcdual fullMat =
        twoQubitGateExpand(CY_Mat, cy.bits()[0], cy.bits()[1]);
    m_circuitMat = fullMat * m_circuitMat;
  }
  void visit(CZ &cz) override {
    MatrixXcdual fullMat =
        twoQubitGateExpand(CZ_Mat, cz.bits()[0], cz.bits()[1]);
    m_circuitMat = fullMat * m_circuitMat;
  }
  void visit(Swap &s) override {
    MatrixXcdual fullMat =
        twoQubitGateExpand(Swap_Mat, s.bits()[0], s.bits()[1]);
    m_circuitMat = fullMat * m_circuitMat;
  }
  void visit(CRZ &crz) override {}
  void visit(CH &ch) override {
    MatrixXcdual fullMat =
        twoQubitGateExpand(CH_Mat, ch.bits()[0], ch.bits()[1]);
    m_circuitMat = fullMat * m_circuitMat;
  }
  void visit(S &s) override {
    m_circuitMat = singleQubitGateExpand(S_Mat, s.bits()[0]) * m_circuitMat;
  }
  void visit(Sdg &sdg) override {
    m_circuitMat = singleQubitGateExpand(Sdg_Mat, sdg.bits()[0]) * m_circuitMat;
  }
  void visit(T &t) override {
    m_circuitMat = singleQubitGateExpand(T_Mat, t.bits()[0]) * m_circuitMat;
  }
  void visit(Tdg &tdg) override {
    m_circuitMat = singleQubitGateExpand(Tdg_Mat, tdg.bits()[0]) * m_circuitMat;
  }
  void visit(CPhase &cphase) override {}
  void visit(Measure &measure) override {}
  void visit(Identity &i) override {}
  void visit(U &u) override {
    xacc::error("We don't support Autodiff U3 gate.");
  }
  void visit(IfStmt &ifStmt) override {}
  // Identifiable Impl
  const std::string name() const override { return ""; }
  const std::string description() const override { return ""; }
  MatrixXcdual getMat() const { return m_circuitMat; }
  MatrixXcdual singleQubitGateExpand(MatrixXcdual &in_gateMat,
                                     size_t in_loc) const {
    if (m_nbQubit == 1) {
      return in_gateMat;
    }
    if (in_loc == 0) {
      auto dim = 1ULL << (m_nbQubit - 1);
      MatrixXcdual rhs = MatrixXcdual::Identity(dim, dim);
      return kroneckerProduct(in_gateMat, rhs);
    } else if (in_loc == m_nbQubit - 1) {
      auto dim = 1ULL << (m_nbQubit - 1);
      MatrixXcdual lhs = MatrixXcdual::Identity(dim, dim);
      return kroneckerProduct(lhs, in_gateMat);
    } else {
      auto leftDim = 1ULL << in_loc;
      auto rightDim = 1ULL << (m_nbQubit - 1 - in_loc);
      MatrixXcdual lhs = MatrixXcdual::Identity(leftDim, leftDim);
      MatrixXcdual rhs = MatrixXcdual::Identity(rightDim, rightDim);
      MatrixXcdual lhsKron = kroneckerProduct(lhs, in_gateMat);
      return kroneckerProduct(lhsKron, rhs);
    }
  }

  MatrixXcdual singleParametricQubitGateExpand(autodiff::dual in_var,
                                               Rot in_rotType,
                                               size_t in_bitLoc) const {
    static const cxdual I_dual = I;
    MatrixXcdual gateMat = [&]() {
      switch (in_rotType) {
      case Rot::X: {
        MatrixXcdual rxMat =
            static_cast<cxdual>(cos(in_var / 2)) *
                MatrixXcdual::Identity(2, 2) -
            I_dual * static_cast<cxdual>(sin(in_var / 2)) * X_Mat;
        return rxMat;
      }
      case Rot::Y: {
        MatrixXcdual ryMat =
            static_cast<cxdual>(cos(in_var / 2)) *
                MatrixXcdual::Identity(2, 2) -
            I_dual * static_cast<cxdual>(sin(in_var / 2)) * Y_Mat;
        return ryMat;
      }
      case Rot::Z: {
        MatrixXcdual rzMat =
            static_cast<cxdual>(cos(in_var / 2)) *
                MatrixXcdual::Identity(2, 2) -
            I_dual * static_cast<cxdual>(sin(in_var / 2)) * Z_Mat;
        return rzMat;
      }
      default:
        __builtin_unreachable();
      }
    }();
    return singleQubitGateExpand(gateMat, in_bitLoc);
  }

  MatrixXcdual twoQubitGateExpand(MatrixXcdual &in_gateMat, size_t in_bit1,
                                  size_t in_bit2) const {
    const auto matSize = 1ULL << m_nbQubit;
    MatrixXcdual resultMat = MatrixXcdual::Identity(matSize, matSize);
    // Qubit index list (0->(dim-1))
    std::vector<size_t> index_list(m_nbQubit);
    std::iota(index_list.begin(), index_list.end(), 0);
    const std::vector<size_t> idx{m_nbQubit - in_bit2 - 1,
                                  m_nbQubit - in_bit1 - 1};
    for (size_t k = 0; k < matSize; ++k) {
      std::vector<cxdual> oldColumn(matSize);
      for (size_t i = 0; i < matSize; ++i) {
        oldColumn[i] = resultMat(i, k);
      }

      for (size_t i = 0; i < matSize; ++i) {
        size_t local_i = 0;
        for (size_t l = 0; l < idx.size(); ++l) {
          local_i |= ((i >> idx[l]) & 1) << l;
        }

        cxdual res = 0.0_i;
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

  std::pair<double, std::string>
  parseVarString(const std::string &in_varStr) const {
    // Handle special case: const double * var name
    // This is generated by our exp_i_theta circuit.
    const auto mulPos = in_varStr.find("*");
    if (mulPos != std::string::npos) {
      const double coeff = std::stod(in_varStr.substr(0, mulPos));
      auto tmp = in_varStr.substr(mulPos+1);
      xacc::trim(tmp);
      return std::make_pair(coeff, tmp);
    }
    return std::make_pair(1.0, in_varStr);
  }

private:
  MatrixXcdual m_circuitMat;
  size_t m_nbQubit;
  std::unordered_map<std::string, autodiff::dual> m_varMap;
};
} // namespace

namespace xacc {
namespace quantum {
void Autodiff::fromObservable(std::shared_ptr<Observable> obs) {
  m_nbQubits = obs->nBits();
  const auto dim = 1ULL << m_nbQubits;
  m_obsMat = MatrixXcdual::Zero(dim, dim);
  for (auto &triplet : obs->to_sparse_matrix()) {
    m_obsMat(triplet.row(), triplet.col()) = triplet.coeff();
  }
}

bool Autodiff::initialize(const HeterogeneousMap parameters) {
  if (!parameters.pointerLikeExists<Observable>("observable")) {
    std::cout << "'observable' is required.\n";
    return false;
  }
  auto obs = parameters.getPointerLike<Observable>("observable");
  fromObservable(xacc::as_shared_ptr(obs));
  if (parameters.keyExists<std::function<std::shared_ptr<CompositeInstruction>(
          std::vector<double>)>>("kernel-evaluator")) {
    kernel_evaluator =
        parameters.get<std::function<std::shared_ptr<CompositeInstruction>(
            std::vector<double>)>>("kernel-evaluator");
    if (parameters.keyExists<double>("step")) {
      m_stepSize = parameters.get<double>("step");
    }
  }
  return true;
}

std::vector<double> Autodiff::computeDerivative(
    std::shared_ptr<CompositeInstruction> CompositeInstruction,
    const MatrixXcdual &obsMat, const std::vector<double> &x, size_t nbQubits,
    double *optional_out_fn_val) {
  // std::cout << "Observable: \n" << obsMat << "\n";
  // std::cout << "Circuit: \n" << CompositeInstruction->toString() << "\n";
  // std::cout << "Number of arguments = " << CompositeInstruction->nVariables()
  //           << "\n";

  auto f = [&](std::vector<autodiff::dual> &vars) -> autodiff::dual {
    std::unordered_map<std::string, autodiff::dual> varMap;
    if (CompositeInstruction->getVariables().size() != vars.size()) {
      xacc::error("The number of Composite parameters doesn't match the input "
                  "vector size.");
    }

    for (size_t i = 0; i < vars.size(); ++i) {
      varMap.emplace(CompositeInstruction->getVariables()[i], vars[i]);
    }

    AutodiffCircuitVisitor visitor(nbQubits, varMap);
    InstructionIterator iter(CompositeInstruction);

    while (iter.hasNext()) {
      auto inst = iter.next();
      if (!inst->isComposite() && inst->isEnabled()) {
        inst->accept(&visitor);
      }
    }
    MatrixXcdual circMat = visitor.getMat();
    // std::cout << "Circ Mat:\n" << circMat << "\n";
    VectorXcdual finalState = circMat.col(0);
    // std::cout << "Final state:\n" << finalState << "\n";

    VectorXcdual ket = obsMat * finalState;
    VectorXcdual bra = VectorXcdual::Zero(finalState.size());
    for (int i = 0; i < finalState.size(); ++i) {
      bra[i] = conj(finalState[i]);
    }
    // std::cout << "Mat:\n" << circMat << "\n";
    // std::cout << "Final state:\n" << finalState << "\n";
    cxdual exp_val = bra.cwiseProduct(ket).sum();
    return exp_val.real();
  };

  std::vector<autodiff::dual> argVars;
  for (const auto &varVal : x) {
    argVars.emplace_back(varVal);
  }

  // If requested, evaluate the func:
  if (optional_out_fn_val) {
    *optional_out_fn_val = f(argVars).val;
  }

  // std::cout << "Exp-val: " << fVal << "\n";
  std::vector<double> gradients;
  for (auto &var : argVars) {
    auto dudx = autodiff::derivative(f, autodiff::wrt(var),
                                     autodiff::forward::at(argVars));
    // std::cout << "dudx: " << dudx << "\n";
    gradients.emplace_back(dudx);
  }

  return gradients;
}

std::vector<double>
Autodiff::derivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
                     const std::vector<double> &x,
                     double *optional_out_fn_val) {
  return computeDerivative(CompositeInstruction, m_obsMat, x, m_nbQubits, optional_out_fn_val);
}

void Autodiff::compute(
    std::vector<double> &dx,
    std::vector<std::shared_ptr<AcceleratorBuffer>> results) {
  // The list must be empty, i.e. no remote evaluation.
  assert(results.empty());
  if (kernel_evaluator) {
    // Black-box kernel evaluator functor:
    // Performs finite differences gradient
    std::vector<double> valuesPlus(m_currentParams.size());
    std::vector<double> valuesMinus(m_currentParams.size());
#ifdef WITH_OPENMP_
#pragma omp parallel for collapse(2)
#endif // WITH_OPENMP_
    for (int i = 0; i < m_currentParams.size(); ++i) {
      for (int j = 0; j < 2; ++j) {
        if (j == 0) {
          // Plus
          auto newParameters = m_currentParams;
          newParameters[i] = newParameters[i] + m_stepSize;
          auto kernel = kernel_evaluator(newParameters);
          assert(kernel->getVariables().size() == 0);
          double funcVal = 0.0;
          computeDerivative(kernel, m_obsMat, {} /* no params */, m_nbQubits,
                            &funcVal);
          valuesPlus[i] = funcVal;
        } else {
          // Minus
          auto newParameters = m_currentParams;
          newParameters[i] = newParameters[i] - m_stepSize;
          auto kernel = kernel_evaluator(newParameters);
          assert(kernel->getVariables().size() == 0);
          double funcVal = 0.0;
          computeDerivative(kernel, m_obsMat, {} /* no params */, m_nbQubits,
                            &funcVal);
          valuesMinus[i] = funcVal;
        }
      }
    }

    assert(valuesMinus.size() == m_currentParams.size() &&
           valuesPlus.size() == m_currentParams.size());
    dx.clear();
    for (int i = 0; i < m_currentParams.size(); ++i) {
      dx.emplace_back((valuesPlus[i] - valuesMinus[i]) / (2.0 * m_stepSize));
    }
  } else {
    // Variational composite, perform analytical autodiff.
    dx = derivative(m_varKernel, m_currentParams);
  }

  m_varKernel.reset();
  m_currentParams.clear();
}
} // namespace quantum
} // namespace xacc

REGISTER_PLUGIN(xacc::quantum::Autodiff, xacc::AlgorithmGradientStrategy)