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
#include "scikit_quant_optimizer.hpp"
#include <pybind11/functional.h>

#include "xacc.hpp"
#include <iostream>
#include <dlfcn.h>

#include "xacc_plugin.hpp"

#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace xacc {

ScikitQuantOptimizer::~ScikitQuantOptimizer() {
  if (libpython_handle) {
    int i = dlclose(libpython_handle);
    if (i != 0) {
      std::cout << "error closing python lib in mitiq: " << i << "\n";
      std::cout << dlerror() << "\n";
    }
  }
}
void ScikitQuantOptimizer::initialize() {
  if (!guard && !Py_IsInitialized()) {
    guard = std::make_unique<py::scoped_interpreter>();
    libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    initialized = true;
  }
}

const std::string ScikitQuantOptimizer::get_algorithm() const {
  std::string optimizerAlgo = "cobyla";

  return optimizerAlgo;
}

const bool ScikitQuantOptimizer::isGradientBased() const { return false; }

OptResult ScikitQuantOptimizer::optimize(OptFunction &function) {

  if (!initialized) {
    initialize();
  }
  auto dim = function.dimensions();
  std::vector<double> x(dim);
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
  }

  auto locals = py::dict();
  locals["x"] = x;

  std::vector<std::vector<double>> bounds;
  for (int i = 0; i < dim; i++) {
    bounds.push_back({-xacc::constants::pi, xacc::constants::pi});
  }

  if (options.keyExists<std::vector<std::vector<double>>>("bounds")) {
    bounds.clear();
    auto new_bounds = options.get<std::vector<std::vector<double>>>("bounds");
    if (dim != new_bounds.size()) {
      xacc::error("SKQuant Error - Invalid number of bounds dim != bounds.size()");
    }
    bounds = new_bounds;
  }
  locals["in_bounds"] = bounds;

  int budget = 100;
  if (options.keyExists<int>("budget")) {
    budget = options.get<int>("budget");
  }
  locals["budget"] = budget;

  std::string method = "imfil";
  if (options.stringExists("method")) {
    method = options.getString("method");
  }
  locals["method"] = method;

  if (options.keyExists<int>("maxmp")) {
    locals["maxmp"] = options.get<int>("maxmp");
  }

  if (options.keyExists<int>("minfcall")) {
    locals["minfcall"] = options.get<int>("minfcall");
  }
  
  if (options.keyExists<int>("maxfail")) {
    locals["maxfail"] = options.get<int>("maxfail");
  }

  if (options.keyExists<int>("verbose")) {
    locals["verbose"] = options.get<int>("verbose");
  }

  if (dim == 1) {
    std::function<double(const double)> wrapper([&](const double x) {
      std::vector<double> dx;
      return function({x}, dx);
    });
    locals["obj_func"] = wrapper;
  } else {
    std::function<double(const std::vector<double>)> wrapper(
        [&](const std::vector<double> x) {
          std::vector<double> dx;
          return function(x, dx);
        });
    locals["obj_func"] = wrapper;
  }

  auto py_src =
      R"#(import numpy as np
from skquant.opt import minimize
x0 = np.array(locals()['x'])
bounds = np.array(locals()['in_bounds'], dtype=float)
budget = locals()['budget']

opts = {}
if 'maxmp' in locals():
    opts['maxmp'] = locals()['maxmp']
if 'minfcall' in locals():
    opts['minfcall'] = locals()['maxmp']
if 'maxfail' in locals():
    opts['maxfail'] = locals()['maxmp']
if 'verbose' in locals():
    opts['verbose'] = locals()['verbose']
result, _ = minimize(locals()['obj_func'], x0, bounds, budget, method=locals()['method'], options=opts)
opt_val = result.optval
opt_params = result.optpar
  )#";

  try {
    py::exec(py_src, py::globals(), locals);
  } catch (std::exception &e) {
    std::stringstream ss;
    ss << "XACC ScikitQuant Exec Error:\n";
    ss << e.what();
    xacc::error(ss.str());
  }
  auto opt_val = locals["opt_val"].cast<double>();
  if (dim == 1) {
    auto opt_params = locals["opt_params"].cast<double>();
    return OptResult{opt_val, {opt_params}};
  } else {
    auto opt_params = locals["opt_params"].cast<std::vector<double>>();
    return OptResult{opt_val, opt_params};
  }
}

} // namespace xacc

REGISTER_OPTIMIZER(xacc::ScikitQuantOptimizer)