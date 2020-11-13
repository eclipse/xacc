
#include "Autodiff.hpp"
#include "xacc_plugin.hpp"
#include <autodiff/forward.hpp>
#include <autodiff/forward/eigen.hpp>

// Extend autodiff to support complex type.
namespace autodiff::forward::traits {
template <typename T> struct isArithmetic<std::complex<T>> : std::true_type {};
} // namespace autodiff::forward::traits

// Custom Eigen Matrix and Vector type for autodiff Dual type.
using cxdual = autodiff::forward::Dual<std::complex<double>, std::complex<double>>;
typedef Eigen::Matrix<cxdual, -1, 1, 0> VectorXcdual;
typedef Eigen::Matrix<cxdual, -1, -1, 0> MatrixXcdual;

namespace xacc {
namespace quantum {
void Autodiff::fromObservable(std::shared_ptr<Observable> obs) {
  const auto nbQubits = obs->nBits();
  const auto dim = 1ULL << nbQubits;
  m_obsMat = Eigen::MatrixXcd::Zero(dim, dim);
  for (auto &triplet : obs->to_sparse_matrix()) {
    m_obsMat(triplet.row(), triplet.row()) = triplet.coeff();
  }
}

Differentiable::Result Autodiff::derivative(
    std::shared_ptr<CompositeInstruction> CompositeInstruction, const std::vector<double> &x) {
  
  
  
  // TODO
  return std::make_pair(0.0, std::vector<double>{});
}
} // namespace quantum
} // namespace xacc

REGISTER_PLUGIN(xacc::quantum::Autodiff, xacc::Differentiable)