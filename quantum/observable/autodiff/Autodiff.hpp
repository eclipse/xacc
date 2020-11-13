#include "Observable.hpp"
#include "Eigen/Dense"
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
class Autodiff : public Differentiable {
public:
  const std::string name() const override { return "autodiff"; }
  const std::string description() const override { return ""; }
  void fromObservable(std::shared_ptr<Observable> obs) override;
  Differentiable::Result derivative(
      std::shared_ptr<CompositeInstruction> CompositeInstruction, const std::vector<double> &x) override;

private:
  MatrixXcdual m_obsMat;
  size_t m_nbQubits;
};
} // namespace quantum
} // namespace xacc