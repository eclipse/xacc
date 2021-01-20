#include "Observable.hpp"
#include "Eigen/Dense"
#include <autodiff/forward.hpp>
#include <autodiff/forward/eigen.hpp>
#include "AlgorithmGradientStrategy.hpp"

#if defined(__clang__)
namespace std {
template <>
complex<autodiff::dual> operator*(const complex<autodiff::dual> &__z,
                                  const complex<autodiff::dual> &__w) {
  autodiff::dual __a = __z.real();
  autodiff::dual __b = __z.imag();
  autodiff::dual __c = __w.real();
  autodiff::dual __d = __w.imag();
  autodiff::dual __ac = __a * __c;
  autodiff::dual __bd = __b * __d;
  autodiff::dual __ad = __a * __d;
  autodiff::dual __bc = __b * __c;
  autodiff::dual __x = __ac - __bd;
  autodiff::dual __y = __ad + __bc;
  return complex<autodiff::dual>(__x, __y);
}
} // namespace std
#endif

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
class Autodiff : public AlgorithmGradientStrategy {
public:
  const std::string name() const override { return "autodiff"; }
  const std::string description() const override { return ""; }
  void fromObservable(std::shared_ptr<Observable> obs);
  std::vector<double>
  derivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
             const std::vector<double> &x,
             double *optional_out_fn_val = nullptr);
  
  // Static helper to evaluate the derivatives and optionally the function value.
  static std::vector<double>
  computeDerivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
                    const MatrixXcdual &obsMat, const std::vector<double> &x,
                    size_t nbQubits, double *optional_out_fn_val = nullptr);

  // AlgorithmGradientStrategy implementation:
  virtual bool isNumerical() const override { return true; }
  virtual void setFunctionValue(const double expValue) {}
  // Pass parameters to initialize specific gradient implementation
  virtual bool initialize(const HeterogeneousMap parameters) override;
  // Generate circuits to enable gradient computation
  virtual std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit,
                        const std::vector<double> &x) override {
    // Cache the kernel and current params.
    m_varKernel = circuit;
    m_currentParams = x;
    // Returns an empty vector -> no circuits will be appended.
    return {};
  }
  virtual void
  compute(std::vector<double> &dx,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override;

private:
  MatrixXcdual m_obsMat;
  size_t m_nbQubits;
  std::shared_ptr<CompositeInstruction> m_varKernel;
  std::vector<double> m_currentParams;
  // Support for QCOR kernel evaluator
  std::function<std::shared_ptr<CompositeInstruction>(std::vector<double>)> kernel_evaluator;
  // Step size if using kernel_evaluator (black-box Composite)
  double m_stepSize = 1e-5;
};
} // namespace quantum
} // namespace xacc