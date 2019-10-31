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
#include "mlpack_optimizer.hpp"
#include "Utils.hpp"
#include <iostream>
#include "xacc.hpp"

using namespace ens;

namespace xacc {

OptResult MLPACKOptimizer::optimize(OptFunction &function) {

  // Set initial parameters
  auto dim = function.dimensions();
  std::vector<double> c;
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    c = options.get<std::vector<double>>("initial-parameters");
  } else {
    c = std::vector<double>(dim);
  }
  arma::mat coordinates(c);

  // Create the MLPack function adapter
  MLPACKFunction f(function);

  // Figure out what the desired optimizer is
  std::string mlpack_opt_name = "adam";
  if (options.stringExists("mlpack-optimizer")) {
    mlpack_opt_name = options.getString("mlpack-optimizer");
  }
  // step size
  double stepSize = 0.5;
  if (options.keyExists<double>("mlpack-step-size")) {
    stepSize = options.get<double>("mlpack-step-size");
  }

  // eps
  double eps = 1e-8;
  if (options.keyExists<double>("mlpack-eps")) {
    eps = options.get<double>("mlpack-eps");
  }
  // max iter
  int maxiter = 500000;
  if (options.keyExists<int>("mlpack-max-iter")) {
    maxiter = options.get<int>("mlpack-max-iter");
  }
  // tolerance
  double tol = 1e-4;
  if (options.keyExists<double>("mlpack-tolerance")) {
    tol = options.get<double>("mlpack-tolerance");
  }
  double momentum = 0.05;
  if (options.keyExists<double>("mlpack-momentum")) {
    momentum = options.get<double>("mlpack-momentum");
  }

  // Based on that optimizer, instantiate and run
  double results;
  if (mlpack_opt_name == "adam") {

    // beta1
    double beta1 = 0.7;
    if (options.keyExists<double>("mlpack-beta1")) {
      beta1 = options.get<double>("mlpack-beta1");
    }
    // beta2
    double beta2 = 0.999;
    if (options.keyExists<double>("mlpack-beta2")) {
      beta2 = options.get<double>("mlpack-beta2");
    }

    Adam optimizer(stepSize, 1, beta1, beta2, eps, maxiter, tol, false, false);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "spsa") {
    SPSA optimizer(0.1, 0.102, 0.16, 0.3, 100000, 1e-5);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "l-bfgs") {
    L_BFGS lbfgs;
    results = lbfgs.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "adagrad") {
    AdaGrad optimizer(stepSize, 1, eps, maxiter, tol, false, false);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "adadelta") {
    // rho
    double rho = 0.05;
    if (options.keyExists<double>("mlpack-rho")) {
      rho = options.get<double>("mlpack-rho");
    }
    AdaDelta optimizer(stepSize, 1, rho, eps, maxiter, tol, false, false);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "cmaes") {
#ifdef HAS_LAPACK
    int lambda = 0;
    double upper = 10., lower = -10.;
    if (options.keyExists<int>("mlpack-cmaes-lambda")) {
      lambda = options.get<int>("mlpack-cmaes-lambda");
    }
    if (options.keyExists<double>("mlpack-cmaes-upper-bound")) {
      upper = options.get<double>("mlpack-cmaes-upper-bound");
    }
    if (options.keyExists<double>("mlpack-cmaes-lower-bound")) {
      lower = options.get<double>("mlpack-cmaes-lower-bound");
    }
    CMAES<> optimizer(10, upper, lower, 1, maxiter, tol);
    results = optimizer.Optimize(f, coordinates);
#else
    xacc::error("Cannot run mlpack cmaes algorithm, lapack not found.");
#endif
  } else if (mlpack_opt_name == "gd") {
    GradientDescent optimizer(stepSize, maxiter, tol);
    results = optimizer.Optimize(f, coordinates);

  } else if (mlpack_opt_name == "momentum-sgd") {

    MomentumUpdate momentumUpdate(momentum);
    MomentumSGD optimizer(stepSize, 1, maxiter, tol, false, momentumUpdate,
                          NoDecay(), false, false);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "momentum-nestorov") {
    NesterovMomentumUpdate nesterovMomentumUpdate(momentum);
    NesterovMomentumSGD optimizer(stepSize, 1, maxiter, tol, false,
                                  nesterovMomentumUpdate, NoDecay(), false,
                                  false);
    results = optimizer.Optimize(f, coordinates);

  } else if (mlpack_opt_name == "sgd") {
    VanillaUpdate vanillaUpdate;
    StandardSGD optimizer(stepSize, 1, maxiter, tol, false, vanillaUpdate,
                          NoDecay(), false, false);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "rms-prop") {
    double alpha = 0.99;
    if (options.keyExists<double>("mlpack-alpha")) {
      alpha = options.get<double>("mlpack-alpha");
    }
    RMSProp optimizer(stepSize, 1, alpha, eps, maxiter, tol, false, false,
                      false);
    results = optimizer.Optimize(f, coordinates);
  } else {
    xacc::error("invalid mlpack optimizer: " + mlpack_opt_name);
  }

  return OptResult{results,
                   arma::conv_to<std::vector<double>>::from(coordinates)};
}

} // namespace xacc
