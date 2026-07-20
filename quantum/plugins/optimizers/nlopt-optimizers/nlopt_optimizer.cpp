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

#include "xacc_plugin.hpp"

namespace xacc {

double c_wrapper(const std::vector<double> &x, std::vector<double> &grad,
                 void *extra) {
  auto e = reinterpret_cast<ExtraNLOptData *>(extra);
  return e->f(x, grad);
}

const std::string NLOptimizer::get_algorithm() const {
  std::string optimizerAlgo = "cobyla";
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("nlopt-optimizer")) {
    optimizerAlgo = options.getString("nlopt-optimizer");
  }
  return optimizerAlgo;
}

const bool NLOptimizer::isGradientBased() const {

  std::string optimizerAlgo = "cobyla";
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("nlopt-optimizer")) {
    optimizerAlgo = options.getString("nlopt-optimizer");
  }

  if (optimizerAlgo == "l-bfgs") {
    return true;
  } else {
    return false;
  }
}

OptResult NLOptimizer::optimize(OptFunction &function) {

  auto dim = function.dimensions();
  nlopt::algorithm algo = nlopt::algorithm::LN_COBYLA;
  double tol = 1e-6, absFTol = 1e-6, relFTol = 1e-6, absXTol = 1e-6, relXTol = 1e-6;
  int maxeval = 1000;

  bool maximize = false;
  if (options.keyExists<bool>("maximize")) {
      xacc::info("Turning on maximize!");
    maximize = options.get<bool>("maximize");
  }

  std::string optimizerAlgo;
  if (options.stringExists("algorithm")) {
    optimizerAlgo = options.getString("algorithm");
  }
  if (options.stringExists("nlopt-optimizer")) {
    optimizerAlgo = options.getString("nlopt-optimizer");
  }

  if (options.stringExists("nlopt-optimizer") || options.stringExists("algorithm")) {
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

  if (options.keyExists<double>("ftol")) {
    tol = options.get<double>("ftol");
    xacc::info("[NLOpt] function tolerance set to " + std::to_string(tol));
  }
  if (options.keyExists<double>("nlopt-ftol")) {
    tol = options.get<double>("nlopt-ftol");
    xacc::info("[NLOpt] function tolerance set to " + std::to_string(tol));
  }

  if (options.keyExists<double>("absolute-ftol")) {
    absFTol = options.get<double>("absolute-ftol");
    xacc::info("[NLOpt] absolute function tolerance set to " + std::to_string(absFTol));
  } else {
    absFTol = tol;
  }

  if (options.keyExists<double>("relative-ftol")) {
    relFTol = options.get<double>("relative-ftol");
    xacc::info("[NLOpt] relative function tolerance set to " + std::to_string(relFTol));
  } else {
    relFTol = tol;
  }

  if (options.keyExists<double>("absolute-xtol")) {
    absXTol = options.get<double>("absolute-xtol");
    xacc::info("[NLOpt] absolute parameter tolerance set to " + std::to_string(absXTol));
  } else {
    absXTol = tol;
  }

  if (options.keyExists<double>("relative-xtol")) {
    relXTol = options.get<double>("relative-xtol");
    xacc::info("[NLOpt] relative parameter tolerance set to " + std::to_string(relXTol));
  } else {
    relXTol = tol;
  }

  if (options.keyExists<int>("maxeval")) {
    maxeval = options.get<int>("maxeval");
    xacc::info("[NLOpt] max function evaluations set to " +
               std::to_string(maxeval));
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
  if (maximize) {
    _opt.set_max_objective(c_wrapper, d);
  } else {
    _opt.set_min_objective(c_wrapper, d);
  }
  // Default lower bounds
  std::vector<double> lowerBounds(dim, -3.1415926);
  if (options.keyExists<std::vector<double>>("lower-bounds")) {
    lowerBounds = options.get<std::vector<double>>("lower-bounds");
  }
  if (options.keyExists<std::vector<double>>("nlopt-lower-bounds")) {
    lowerBounds = options.get<std::vector<double>>("nlopt-lower-bounds");
  }

  // Default upper bounds
  std::vector<double> upperBounds(dim, 3.1415926);
  if (options.keyExists<std::vector<double>>("upper-bounds")) {
    upperBounds = options.get<std::vector<double>>("upper-bounds");
  }
  if (options.keyExists<std::vector<double>>("nlopt-upper-bounds")) {
    upperBounds = options.get<std::vector<double>>("nlopt-upper-bounds");
  }

  // if fails to find mininum should not throw error
  bool throwError = true;
  if (options.keyExists<bool>("throw-error")) {
    throwError = options.get<bool>("throw-error");
  }

  _opt.set_lower_bounds(lowerBounds);
  _opt.set_upper_bounds(upperBounds);
  _opt.set_maxeval(maxeval);
  _opt.set_ftol_rel(relFTol);
  _opt.set_ftol_abs(absFTol);
  _opt.set_xtol_rel(relXTol);
  _opt.set_xtol_abs(absXTol);

  if (options.keyExists<double>("stopval")) {
    const double stopVal = options.get<double>("stopval");
    xacc::info("[NLOpt] function stopval set to " + std::to_string(stopVal));
    _opt.set_stopval(stopVal);
  }
  if (options.keyExists<double>("nlopt-stopval")) {
    const double stopVal = options.get<double>("nlopt-stopval");
    xacc::info("[NLOpt] function stopval set to " + std::to_string(stopVal));
    _opt.set_stopval(stopVal);
  }

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

    if (throwError) {
      xacc::error("NLOpt failed with error code = " + std::to_string(r) + ", " +
                std::string(e.what()));
    } else {
      xacc::warning("NLOpt failed with error code = " + std::to_string(r) + ", " +
                std::string(e.what()));
    }

  }
  return OptResult{optF, x};
}

} // namespace xacc


REGISTER_OPTIMIZER(xacc::NLOptimizer)