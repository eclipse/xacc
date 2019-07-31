#include "nlopt_optimizer.hpp"
#include "nlopt.hpp"

#include "Utils.hpp"
#include <iostream>

namespace xacc {
OptResult NLOptimizer::optimize(OptFunction &function) {

  auto dim = function.dimensions();
  nlopt::algorithm algo = nlopt::algorithm::LN_COBYLA;
  double tol = 1e-8;
  int maxeval = 1000;

  if (options.count("nlopt-optimizer")) {
    auto optimizerAlgo = options["nlopt-optimizer"].as<std::string>();
    if (optimizerAlgo == "cobyla") {
      algo = nlopt::algorithm::LN_COBYLA;
    } else if (optimizerAlgo == "nelder-mead") {
      algo = nlopt::algorithm::LN_NELDERMEAD;
    } else {
      xacc::XACCLogger::instance()->error("Invalid optimizer at this time: " +
                                          optimizerAlgo);
    }
  }

  if (options.count("nlopt-ftol")) {
    tol = options["nlopt-ftol"].as<double>();
  }

  if (options.count("nlopt-maxeval")) {
    maxeval = options["nlopt-maxeval"].as<int>();
  }

  std::vector<double> x(dim);
  if (options.count("initial-parameters")) {
      x = options["initial-parameters"].as<std::vector<double>>();
  }
  nlopt::opt _opt(algo, dim);
  std::function<double(const std::vector<double> &, std::vector<double> &,
                       void *)>
      f = [&](const std::vector<double> &x, std::vector<double> &grad,
              void *f_data) -> double { return function(x); };

  auto fptr = LambdaToVFunc::ptr(f);

  _opt.set_min_objective(fptr, NULL);
  _opt.set_lower_bounds(std::vector<double>(dim, -3.1415926));
  _opt.set_upper_bounds(std::vector<double>(dim, 3.1415926));
  _opt.set_maxeval(maxeval);
  _opt.set_ftol_rel(tol);

  double optF;
  try {
    auto result = _opt.optimize(x, optF);
  } catch (std::exception &e) {
    xacc::XACCLogger::instance()->error("NLOpt failed: " +
                                        std::string(e.what()));
  }
  return OptResult{optF, x};
}

} // namespace xacc
