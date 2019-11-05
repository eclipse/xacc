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

class PyCompiler : public xacc::Compiler {
public:
  /* Inherit the constructors */
  using Compiler::Compiler;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Compiler, name);
  }
  const std::string description() const override { return ""; }

  std::shared_ptr<IR> compile(const std::string &src,
                              std::shared_ptr<Accelerator> acc) override {
    PYBIND11_OVERLOAD_PURE(std::shared_ptr<IR>, xacc::Compiler, compile, src,
                           acc);
  }

  std::shared_ptr<IR> compile(const std::string &src) override {
    return compile(src, nullptr);
  }

  const std::string
  translate(std::shared_ptr<CompositeInstruction> function) override {
    return "";
  }
};

void bind_compiler(py::module& m);