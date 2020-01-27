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
#include <pybind11/pybind11.h>

#include "Algorithm.hpp"
#include "xacc.hpp"

namespace py = pybind11;
using namespace xacc;


class PyAlgorithm : public xacc::Algorithm {
public:
  /* Inherit the constructors */
  using Algorithm::Algorithm;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Algorithm, name);
  }
  const std::string description() const override { return ""; }

  bool initialize(const HeterogeneousMap &params) override {
    PYBIND11_OVERLOAD_PURE(bool, xacc::Algorithm, initialize, params);
  }

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Algorithm, execute, buffer);
  }
  const std::vector<std::string> requiredParameters() const override {
      return {};
  }

  virtual std::shared_ptr<Algorithm> clone() override {
      PYBIND11_OVERLOAD_PURE(std::shared_ptr<Algorithm>, xacc::Algorithm, clone);
  }
  bool shouldClone() override {return false;}

};
void bind_algorithm(py::module& m);
