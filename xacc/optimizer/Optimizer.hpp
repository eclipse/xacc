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
#ifndef XACC_OPTIMIZER_HPP_
#define XACC_OPTIMIZER_HPP_

#include <functional>
#include <vector>

#include "AcceleratorBuffer.hpp"
#include "CompositeInstruction.hpp"
#include "Identifiable.hpp"
#include "heterogeneous.hpp"

namespace xacc {

// using OptimizerFunctorNoGradValue = std::function<double(std::vector<double>)>;
using OptimizerFunctorNoGrad =
    std::function<double(const std::vector<double> &)>;
using OptimizerFunctor =
    std::function<double(const std::vector<double> &, std::vector<double> &)>;
using OptResult = std::pair<double, std::vector<double>>;

using OptFunctionPtr = double (*)(const std::vector<double> &,
                                  std::vector<double> &, void *);
class OptFunction {
protected:
  OptimizerFunctor _function;
  int _dim = 0;

public:
  OptFunction() = default;

  // Standard constructor, takes function that takes params as
  // first arg and gradient as second arg
  OptFunction(OptimizerFunctor f, const int d) : _function(f), _dim(d) {}
  OptFunction(OptimizerFunctorNoGrad f, const int d)
      : _function([&](const std::vector<double> &x, std::vector<double> &) {
          return f(x);
        }),
        _dim(d) {}
  // OptFunction(OptimizerFunctorNoGradValue f, const int d)
  //     : _function([&](const std::vector<double> &x, std::vector<double> &) {
  //         return f(x);
  //       }),
  //       _dim(d) {}
  virtual const int dimensions() const { return _dim; }
  virtual double operator()(const std::vector<double> &x,
                            std::vector<double> &dx) {
    return _function(x, dx);
  }
  virtual double operator()(const std::vector<double> &&x) {
    std::vector<double> dx;
    return _function(x, dx);
  }
};

class Optimizer : public xacc::Identifiable {
protected:
  HeterogeneousMap options;

public:
  template <typename T> void appendOption(const std::string key, T &&value) {
    options.insert(key, value);
  }
  virtual void setOptions(const HeterogeneousMap &opts) { options = opts; }

  virtual OptResult optimize(OptFunction &function) = 0;
  OptResult optimizer(OptFunction &&function) { return optimize(function); }
  // No Opt function optimization: used by fully-customized Optimizer
  // implementations which don't need user-supplied opt function.
  virtual OptResult optimize() {
    // Derived sub-classes must provide concrete impl,
    // otherwise, it's illegal to call optimize() w/o any OptFunction.
    throw std::bad_function_call();
  }

  virtual const std::string get_algorithm() const { return ""; }
  virtual const bool isGradientBased() const { return false; }
};
} // namespace xacc

#endif
