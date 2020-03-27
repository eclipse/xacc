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
#include <memory>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

#include "xacc.hpp"

namespace py = pybind11;
using namespace xacc;


class PyOptimizer : public xacc::Optimizer {
public:
  /* Inherit the constructors */
  using Optimizer::Optimizer;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Optimizer, name);
  }
  const std::string description() const override { return ""; }

  OptResult optimize(OptFunction &function) override {
      PYBIND11_OVERLOAD_PURE(OptResult, xacc::Optimizer, optimize, function);
  }
  
  OptResult optimize() override {
      PYBIND11_OVERLOAD(OptResult, xacc::Optimizer, optimize);
  }
  
  void setOptions(const HeterogeneousMap &opts) override {
      PYBIND11_OVERLOAD_PURE(void, xacc::Optimizer, setOptions, opts);
  }

};

void bind_optimizer(py::module& m);