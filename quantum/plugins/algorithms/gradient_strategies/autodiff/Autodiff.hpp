#include "Observable.hpp"
#include "Eigen/Dense"
#include <autodiff/forward.hpp>
#include <autodiff/forward/eigen.hpp>
#include "AlgorithmGradientStrategy.hpp"

// Extend autodiff to support complex type.
namespace autodiff::forward::traits {
template <typename T> struct isArithmetic<std::complex<T>> : std::true_type {};
} // namespace autodiff::forward::traits

// Custom Eigen Matrix and Vector type for autodiff Dual type.
using cxdual = std::complex<autodiff::dual>;
typedef Eigen::Matrix<cxdual, -1, 1, 0> VectorXcdual;
typedef Eigen::Matrix<cxdual, -1, -1, 0> MatrixXcdual;

namespace xacc {
namespace quantum {
// An interface to support Autodiff of a *variational* circuit w.r.t. an
// observable operator.
// i.e. compute the gradient/derivative of the expectation value w.r.t. each
// variational parameter in the CompositeInstruction.
class Differentiable {
public:
  virtual void fromObservable(std::shared_ptr<Observable> obs) = 0;
  // If optional_out_fn_val is provided, function value will also be evaluated.
  virtual std::vector<double>
  derivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
             const std::vector<double> &x,
             double *optional_out_fn_val = nullptr) = 0;
};

class Autodiff : public Differentiable, public AlgorithmGradientStrategy {
public:
  const std::string name() const override { return "autodiff"; }
  const std::string description() const override { return ""; }
  void fromObservable(std::shared_ptr<Observable> obs) override;
  std::vector<double>
  derivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
             const std::vector<double> &x,
             double *optional_out_fn_val = nullptr) override;

  // AlgorithmGradientStrategy implementation:
  virtual bool isNumerical() const override { return true; }
  virtual bool isAnalytical() const override { return true; }
  // Pass parameters to initialize specific gradient implementation
  virtual bool initialize(const HeterogeneousMap parameters) override;
  // Generate circuits to enable gradient computation
  virtual std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit,
                        const std::vector<double> &x) override {
    return {};
  }
  virtual void
  compute(std::vector<double> &dx,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {}
  virtual std::vector<double>
  gradient(std::shared_ptr<CompositeInstruction> circuit,
           const std::vector<double> &x) override {
    return derivative(circuit, x);
  }

private:
  MatrixXcdual m_obsMat;
  size_t m_nbQubits;
};
} // namespace quantum
} // namespace xacc