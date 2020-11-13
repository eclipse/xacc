#include "Observable.hpp"
#include "Eigen/Dense"
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
  Eigen::MatrixXcd m_obsMat;
};
} // namespace quantum
} // namespace xacc