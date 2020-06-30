/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "nlopt_optimizer.hpp"
#include "nlopt.hpp"

#include "xacc.hpp"
#include <iostream>
using namespace std::placeholders;

namespace xacc {

double c_wrapper(const std::vector<double> &x, std::vector<double> &grad,
                 void *extra) {
  auto e = reinterpret_cast<ExtraNLOptData *>(extra);
  return e->f(x, grad);
}

const std::string NLOptimizer::get_algorithm() const {
  std::string nlopt_opt_name = "cobyla";
  if (options.stringExists("nlopt-optimizer")) {
    nlopt_opt_name = options.getString("nlopt-optimizer");
  }
  return nlopt_opt_name;
}

const bool NLOptimizer::isGradientBased() const {

  std::string nlopt_opt_name = "cobyla";
  if (options.stringExists("nlopt-optimizer")) {
    nlopt_opt_name = options.getString("nlopt-optimizer");
  }

  if (nlopt_opt_name == "l-bfgs") {
    return true;
  } else {
    return false;
  }
}

OptResult NLOptimizer::optimize(OptFunction &function) {

  auto dim = function.dimensions();
  nlopt::algorithm algo = nlopt::algorithm::LN_COBYLA;
  double tol = 1e-6;
  int maxeval = 1000;

  if (options.stringExists("nlopt-optimizer")) {
    auto optimizerAlgo = options.getString("nlopt-optimizer");
    if (optimizerAlgo == "cobyla") {
      algo = nlopt::algorithm::LN_COBYLA;
    } else if (optimizerAlgo == "nelder-mead") {
      algo = nlopt::algorithm::LN_NELDERMEAD;
    } else if (optimizerAlgo == "l-bfgs") {
      algo = nlopt::algorithm::LD_LBFGS;
    } else {
      xacc::XACCLogger::instance()->error("Invalid optimizer at this time: " +
                                          optimizerAlgo);
    }
  }

  if (options.keyExists<double>("nlopt-ftol")) {
    tol = options.get<double>("nlopt-ftol");
    xacc::info("[NLOpt] function tolerance set to " + std::to_string(tol));
  }

  if (options.keyExists<int>("nlopt-maxeval")) {
    maxeval = options.get<int>("nlopt-maxeval");
    xacc::info("[NLOpt] max function evaluations set to " +
               std::to_string(maxeval));
  }

  std::vector<double> x(dim);
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
  }

  ExtraNLOptData data;
  data.f = function;
  auto d = reinterpret_cast<void *>(&data);

  nlopt::opt _opt(algo, dim);
  _opt.set_min_objective(c_wrapper, d);
  _opt.set_lower_bounds(std::vector<double>(dim, -3.1415926));
  _opt.set_upper_bounds(std::vector<double>(dim, 3.1415926));
  _opt.set_maxeval(maxeval);
  _opt.set_ftol_rel(tol);

  if (dim != x.size()) {
    xacc::error("Invalid optimization configuration: function dim == " +
                std::to_string(dim) +
                ", param_size == " + std::to_string(x.size()));
  }
  double optF;
  nlopt::result r;
  try {
    r = _opt.optimize(x, optF);
  } catch (std::exception &e) {
    xacc::error("NLOpt failed with error code = " + std::to_string(r) + ", " +
                std::string(e.what()));
  }

  return OptResult{optF, x};
}

} // namespace xacc
