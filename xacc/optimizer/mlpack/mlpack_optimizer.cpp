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

  // Based on that optimizer, instantiate and run
  double results;
  if (mlpack_opt_name == "adam") {
    Adam optimizer(0.5, 1, 0.7, 0.999, 1e-8, 500000, 1e-3, false, true);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "spsa") {
    SPSA optimizer(0.1, 0.102, 0.16, 0.3, 100000, 1e-5);
    results = optimizer.Optimize(f, coordinates);
  } else if (mlpack_opt_name == "l-bfgs") {
    L_BFGS lbfgs;
    results = lbfgs.Optimize(f, coordinates);
  }

  return OptResult{results,
                   arma::conv_to<std::vector<double>>::from(coordinates)};
}

} // namespace xacc
