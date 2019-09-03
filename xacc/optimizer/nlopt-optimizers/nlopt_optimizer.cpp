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

//   std::stringstream ss;
//   options.print<int,std::size_t, std::string,double>(ss);
//   std::cout << "OPTS:\n" << ss.str() << "\n";
  if (options.stringExists("nlopt-optimizer")) {
    auto optimizerAlgo = options.getString("nlopt-optimizer");
    if (optimizerAlgo == "cobyla") {
      algo = nlopt::algorithm::LN_COBYLA;
    } else if (optimizerAlgo == "nelder-mead") {
      algo = nlopt::algorithm::LN_NELDERMEAD;
    } else {
      xacc::XACCLogger::instance()->error("Invalid optimizer at this time: " +
                                          optimizerAlgo);
    }
  }

  if (options.keyExists<double>("nlopt-ftol")) {
    tol = options.get<double>("nlopt-ftol");
  }

  if (options.keyExists<int>("nlopt-maxeval")) {
    maxeval = options.get<int>("nlopt-maxeval");
  }

  std::vector<double> x(dim);
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
        x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
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
      std::cout << "HELLO: " << dim << ", " << x.size() << "\n";
    xacc::XACCLogger::instance()->error("NLOpt failed: " +
                                        std::string(e.what()));

  }
  return OptResult{optF, x};
}

} // namespace xacc
