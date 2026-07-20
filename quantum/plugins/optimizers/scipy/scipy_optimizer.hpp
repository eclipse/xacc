#ifndef SCIPYOPTIMIZER_HPP
#define SCIPYOPTIMIZER_HPP

#include <xacc.hpp>
#include <xacc_service.hpp>
#include <Optimizer.hpp>
#include <pybind11/embed.h>

namespace xacc {

class ScipyOptimizer : public xacc::Optimizer {
public:
  ScipyOptimizer() = default;
  ~ScipyOptimizer() = default;

  const std::string name() const override { return "scipy"; }
  const std::string description() const override { return ""; }

  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  virtual const std::string get_algorithm() const override;
};

} // namespace xacc
#endif // SCIPYOPTIMIZER_HPP
