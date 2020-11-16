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
class Autodiff : public AlgorithmGradientStrategy {
public:
  const std::string name() const override { return "autodiff"; }
  const std::string description() const override { return ""; }
  void fromObservable(std::shared_ptr<Observable> obs);
  std::vector<double>
  derivative(std::shared_ptr<CompositeInstruction> CompositeInstruction,
             const std::vector<double> &x,
             double *optional_out_fn_val = nullptr);

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
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {
    // The list must be empty, i.e. no evaluation.
    assert(results.empty());
    dx = derivative(m_varKernel, m_currentParams);
    m_varKernel.reset();
    m_currentParams.clear();
  }

private:
  MatrixXcdual m_obsMat;
  size_t m_nbQubits;
  std::shared_ptr<CompositeInstruction> m_varKernel;
  std::vector<double> m_currentParams;
};
} // namespace quantum
} // namespace xacc