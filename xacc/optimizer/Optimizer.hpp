#ifndef XACC_OPTIMIZER_HPP_
#define XACC_OPTIMIZER_HPP_

#include <functional>
#include <vector>

#include "InstructionParameter.hpp"
#include "Identifiable.hpp"

namespace xacc {

using OptResult = std::pair<double, std::vector<double>>;
using OptimizerOptions = std::map<std::string, xacc::InstructionParameter>;

class OptFunction {
protected:
  std::function<double(const std::vector<double> &)> _function;
  int _dim = 0;

public:
  OptFunction(std::function<double(const std::vector<double> &)> f, const int d)
      : _function(f), _dim(d) {}
  virtual const int dimensions() const { return _dim; }
  virtual double operator()(const std::vector<double> &params) {
    return _function(params);
  }
};

class Optimizer : public xacc::Identifiable {
protected:
  std::map<std::string, xacc::InstructionParameter> options;

public:
  virtual OptResult optimize(OptFunction &function) = 0;
  void
  setOptions(const std::map<std::string, xacc::InstructionParameter> &opts) {
    options = opts;
  }
};
} // namespace xacc
#endif
