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


class PyObservable : public xacc::Observable {
public:
  /* Inherit the constructors */
  using Observable::Observable;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Observable, name);
  }
  const std::string description() const override { return ""; }

  std::vector<std::shared_ptr<CompositeInstruction>>
  observe(std::shared_ptr<CompositeInstruction> c) override {
      PYBIND11_OVERLOAD_PURE(std::vector<std::shared_ptr<CompositeInstruction>>, xacc::Observable, observe, c);
  }

  const std::string toString() override{
      PYBIND11_OVERLOAD_PURE(std::string, xacc::Observable, toString);
  }
  void fromString(const std::string str) override{
      xacc::error("PyObservable::fromString not implemented");
  }

  const int nBits() override {
      PYBIND11_OVERLOAD_PURE(int, xacc::Observable, nBits);
  }

  void fromOptions(const HeterogeneousMap& options) override {
      PYBIND11_OVERLOAD_PURE(void, xacc::Observable, fromOptions, options);
  }

  double postProcess(std::shared_ptr<AcceleratorBuffer> buffer,
                     const std::string &postProcessTask,
                     const HeterogeneousMap &extra_data) override {
    PYBIND11_OVERLOAD_PURE(double, xacc::Observable, postProcess, buffer,
                           postProcessTask, extra_data);
  }
};

void bind_observable(py::module& m);