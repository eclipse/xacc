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
#include "py_compiler.hpp"
#include "py_heterogeneous_map.hpp"

void bind_compiler(py::module &m) {

  // Expose the Compiler
  py::class_<xacc::Compiler, std::shared_ptr<xacc::Compiler>, PyCompiler>(
      m, "Compiler",
      "The XACC Compiler takes as input quantum kernel source code, "
      "and compiles it to the XACC intermediate representation.")
      .def(py::init<>(), "")
      .def("name", &xacc::Compiler::name, "Return the name of this Compiler.")
      .def("compile",
           (std::shared_ptr<xacc::IR>(xacc::Compiler::*)(
               const std::string &, std::shared_ptr<xacc::Accelerator>)) &
               xacc::Compiler::compile,
           "Compile the "
           "given source code against the given Accelerator.")
      .def("compile",
           (std::shared_ptr<xacc::IR>(xacc::Compiler::*)(const std::string &)) &
               xacc::Compiler::compile,
           "Compile the "
           "given source code.")
      .def("compile",
           (std::shared_ptr<xacc::IR>(xacc::Compiler::*)()) &
               xacc::Compiler::compile,
           "Compile the "
           "given source code.")
      .def("translate",
           (const std::string (xacc::Compiler::*)(
               std::shared_ptr<CompositeInstruction>)) &
               xacc::Compiler::translate,
           "Translate the given IR Function instance to source code in this "
           "Compiler's language.")
      .def(
          "translate",
          [](xacc::Compiler &c, std::shared_ptr<CompositeInstruction> prog,
             const PyHeterogeneousMap &opts) {
            xacc::HeterogeneousMap m;
            for (auto &item : opts) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return c.translate(prog, m);
          },
          "")
      .def(
          "setExtraOptions",
          [](xacc::Compiler &c, const PyHeterogeneousMap &opts) {
            HeterogeneousMap m;
            for (auto &item : opts) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            c.setExtraOptions(m);
          },
          "");
}