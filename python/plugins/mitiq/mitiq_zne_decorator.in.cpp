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
#include "mitiq_zne_decorator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include <fstream>
#include <pylifecycle.h>
#include <set>

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include <dlfcn.h>

using namespace cppmicroservices;
namespace py = pybind11;
using namespace py::literals;

namespace xacc {
namespace python {
MitiqZNE::~MitiqZNE() {
  if (libpython_handle) {
    int i = dlclose(libpython_handle);
    if (i != 0) {
      std::cout << "error closing python lib in mitiq: " << i << "\n";
      std::cout << dlerror() << "\n";
    }
  }
}
void MitiqZNE::initialize(const HeterogeneousMap &params) {
  if (!guard && !Py_IsInitialized()) {
    guard = std::make_unique<py::scoped_interpreter>();
    libpython_handle = dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
  }

  if (Py_IsInitialized()) {
    xacc = py::module::import("xacc");
    mitiq = py::module::import("mitiq");
    qiskit = py::module::import("qiskit");
  }
}
void MitiqZNE::execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction> function) {

  // Map to OpenQasm
  auto compiler = xacc::getCompiler("staq");
  compiler->setExtraOptions({{"no-optimize", true}});
  auto circuit_src = compiler->translate(function);

  // Pass the necessary variables through the globals() map
  auto globals = py::globals();
  globals["global_qpu"] = decoratedAccelerator;
  globals["global_circuit_src"] = circuit_src;
  globals["global_staq_compiler"] = compiler;
  globals["global_buffer"] = buffer;
  globals["global_xacc_allocator"] = xacc.attr("qalloc");
  globals["global_mitiq_executor"] = mitiq.attr("execute_with_zne");
  globals["global_qk_from_qasm"] =
      qiskit.attr("QuantumCircuit").attr("from_qasm_str");

  auto py_src = R"#(
def noisy_exec(circ):
    buffer = globals()['global_buffer']
    program = globals()['global_staq_compiler'].compile(circ.qasm()).getComposites()[0]
    tmp = globals()['global_xacc_allocator'](buffer.size())
    globals()['global_qpu'].execute(tmp, program)
    tmp.addExtraInfo('exp-val-z', tmp.getExpectationValueZ())
    buffer.appendChild('mitiq-'+buffer.name(), tmp)
    return tmp.getExpectationValueZ()
circuit = globals()['global_qk_from_qasm'](globals()['global_circuit_src'])
fixed_exp = globals()['global_mitiq_executor'](circuit, noisy_exec)
)#";

  // Execute and get the fixed expectation value.
  auto locals = py::dict();
  py::exec(py_src, py::globals(), locals);
  buffer->addExtraInfo("exp-val-z", locals["fixed_exp"].cast<double>());
}

void MitiqZNE::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  for (auto &f : functions) {
    auto tmpBuffer =
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
    execute(tmpBuffer, f);
    buffer->appendChild(f->name(), tmpBuffer);
  }
}

} // namespace python
} // namespace xacc

namespace {

/**
 */
class US_ABI_LOCAL MitiqZNEActivator : public BundleActivator {

public:
  MitiqZNEActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto xt = std::make_shared<xacc::python::MitiqZNE>();
    context.RegisterService<xacc::AcceleratorDecorator>(xt);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(MitiqZNEActivator)
