#ifndef GSL_OPTIMIZER_HPP
#define GSL_OPTIMIZER_HPP

#include <xacc.hpp>
#include <xacc_service.hpp>
#include <Optimizer.hpp>

namespace xacc {

class GSLOptimizer : public xacc::Optimizer {
public:
  GSLOptimizer() = default;
  ~GSLOptimizer() = default;

  const std::string name() const override { return "gsl"; }
  const std::string description() const override { return ""; }

  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  virtual const std::string get_algorithm() const override;
};

} // namespace xacc
#endif