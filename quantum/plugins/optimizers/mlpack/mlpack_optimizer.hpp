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
#ifndef XACC_MLPACK_OPTIMIZER_HPP_
#define XACC_MLPACK_OPTIMIZER_HPP_

#include <type_traits>
#include <utility>

#include "Optimizer.hpp"
#include <armadillo>
#include <ensmallen.hpp>

namespace xacc {

class MLPACKFunction {
protected:
  std::vector<double> grad;
  OptFunction &opt_function;

public:
  MLPACKFunction(OptFunction &optF)
      : opt_function(optF), grad(std::vector<double>(optF.dimensions())) {}
  std::size_t NumFunctions() const { return 1; }
  void Shuffle() { /* Nothing to do here */
  }

  // Given parameters x, return the value of f(x).
  double Evaluate(const arma::mat &x) {
    auto x_vec = arma::conv_to<std::vector<double>>::from(x);
    return opt_function(x_vec, grad);
  }

  double Evaluate(const arma::mat &coordinates, const size_t begin,
                  const size_t batchSize) {
    return Evaluate(coordinates);
  }
  double EvaluateWithGradient(const arma::mat &coordinates,
                              arma::mat &gradient) {
    auto val = Evaluate(coordinates);
    Gradient(coordinates, gradient);
    return val;
  }
  double EvaluateWithGradient(const arma::mat &coordinates, std::size_t bb,
                              arma::mat &gradient, std::size_t b) {
    auto val = Evaluate(coordinates);
    Gradient(coordinates, gradient);
    return val;
  }
  // Given parameters x and a matrix g, store f'(x) in the provided matrix g.
  // g should have the same size (rows, columns) as x.
  void Gradient(const arma::mat &x, arma::mat &gradient) {
    arma::mat g(grad);
    gradient = g;
  }
};

class MLPACKOptimizer : public Optimizer {
public:
  MLPACKOptimizer() = default;
  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  const std::string get_algorithm() const override;

  const std::string name() const override { return "mlpack"; }
  const std::string description() const override { return ""; }
};
} // namespace xacc
#endif
