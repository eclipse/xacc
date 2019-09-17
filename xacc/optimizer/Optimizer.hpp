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

#include "heterogeneous.hpp"
#include "Identifiable.hpp"
#include "CompositeInstruction.hpp"
#include "AcceleratorBuffer.hpp"

namespace xacc {

using OptResult = std::pair<double, std::vector<double>>;

class OptFunction {
protected:
  std::function<double(const std::vector<double> &, std::vector<double> &)>
      _function;
  int _dim = 0;

public:
  // Nullary Constructor, make dummy function
  OptFunction()
      : _function([](const std::vector<double> &x, std::vector<double> &dx) {
          return 0.0;
        }) {}

  // Standard constructor, takes function that takes params as
  // first arg and gradient as second arg
  OptFunction(
      std::function<double(const std::vector<double> &, std::vector<double> &)>
          f,
      const int d)
      : _function(f), _dim(d) {}
  virtual const int dimensions() const { return _dim; }
  virtual double operator()(const std::vector<double> &x,
                            std::vector<double> &dx) {
    return _function(x, dx);
  }
};

class Optimizer : public xacc::Identifiable {
protected:
  HeterogeneousMap options;

public:
  template <typename T> void appendOption(const std::string key, T &value) {
    options.insert(key, value);
  }
  void setOptions(const HeterogeneousMap &opts) { options = opts; }

  virtual OptResult optimize(OptFunction &function) = 0;
};
} // namespace xacc
#endif
