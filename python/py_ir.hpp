/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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

#include "IRTransformation.hpp"
#include "xacc.hpp"

namespace py = pybind11;

using namespace xacc;

class PyIRTransformation : public xacc::IRTransformation {
public:
  /* Inherit the constructors */
  using IRTransformation::IRTransformation;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::IRTransformation, name);
  }
  const std::string description() const override { return ""; }

  void apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options = {}) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::IRTransformation, apply, program, accelerator, options);
  }
  const IRTransformationType type() const override {
    PYBIND11_OVERLOAD_PURE(IRTransformationType, xacc::IRTransformation, type);
  }
};

void bind_ir(py::module& m);
