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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "AqtAccelerator.hpp"
#include "xacc_plugin.hpp"
#include <cassert>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <dlfcn.h>
#include "xacc_config.hpp"
#include "xacc_service.hpp"

using namespace pybind11::literals;

namespace {
std::vector<std::string> openQasm2QtrlSeq(pybind11::object &transpiler,
                                          const std::string &openQasmSrc) {
  auto ll_seq = transpiler.attr("transpile_qasm")(openQasmSrc);
  pybind11::print(ll_seq);
  return {};
}
} // namespace

namespace xacc {
namespace quantum {
void AqtAccelerator::initialize(const HeterogeneousMap &params) {
  static bool PythonInit = false;
  if (!PythonInit) {
    if (!XACC_IS_APPLE) {
      // If not MacOs, preload Python lib to the address space.
      // Note: we don't need to dlclose, just need to load the lib to prevent
      // linking issue on Linux.
      auto libPythonPreload =
          dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    }
    try {
      pybind11::initialize_interpreter();
    } catch (...) {
    }
    PythonInit = true;
    // Requirement: openqasm_transpiler module is available
    m_openQASM2qtrl = pybind11::module::import("openqasm_transpiler");
  }
}

std::vector<std::pair<int, int>> AqtAccelerator::getConnectivity() {
  return {};
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  auto compiler = xacc::getCompiler("staq");
  auto circuit_src = compiler->translate(compositeInstruction);
  std::cout << "HOWDY: \n" << circuit_src << "\n";
  auto qtrlSeq = openQasm2QtrlSeq(m_openQASM2qtrl, circuit_src);
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {}
} // namespace quantum
} // namespace xacc

REGISTER_ACCELERATOR(xacc::quantum::AqtAccelerator)
