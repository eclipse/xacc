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
  return ll_seq.cast<std::vector<std::string>>();
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
  }
  PythonInit = true;
  // Requirement: openqasm_transpiler module is available
  m_openQASM2qtrl = pybind11::module::import("qtrl.qpu.openqasm_transpiler");

  // Configuration files (YAML):
  // Currently, explicitly requires all YAML files to be loaded.
  // i.e. no defaults yet.
  std::string varFileName;
  std::string pulseFileName;
  std::string adcFileName;
  std::string dacFileName;
  if (params.stringExists("var-yaml")) {
    varFileName = params.getString("var-yaml");
  }
  if (params.stringExists("pulse-yaml")) {
    pulseFileName = params.getString("pulse-yaml");
  }
  if (params.stringExists("adc-yaml")) {
    adcFileName = params.getString("adc-yaml");
  }
  if (params.stringExists("dac-yaml")) {
    dacFileName = params.getString("dac-yaml");
  }

  if (varFileName.empty() || pulseFileName.empty() || adcFileName.empty() ||
      dacFileName.empty()) {
    xacc::error("Missing configuration files. Please provide Variables, "
                "Pulses, ADC, and DAC configuration files.");
  }

  const std::vector<std::string> allFiles = {varFileName, pulseFileName,
                                             adcFileName, dacFileName};
  for (const auto &fileToCheck : allFiles) {
    // Check if this is a file name
    std::ifstream test(fileToCheck);
    if (!test) {
      xacc::error("Cannot open file: " + fileToCheck);
    }
  }

  // !!! IMPORTANT !!!: set `simulation` to true to bypass device discovery,
  // i.e. using the **fake** ZI instrument.
  // Disable this when running on real system.
  auto qtrlSettings = pybind11::module::import("qtrl.settings").attr("Settings");
  qtrlSettings.attr("simulation") = true;
  
  auto qtrlManagers = pybind11::module::import("qtrl.managers");
  auto varManager = qtrlManagers.attr("VariableManager")(varFileName);
  auto pulseManager =
      qtrlManagers.attr("PulseManager")(pulseFileName, varManager);
  auto adcManager = pybind11::module::import("qtrl.managers.ZurichDAC_manager")
                        .attr("ZurichDACManager")(adcFileName, varManager);
  auto dacManager = pybind11::module::import("qtrl.managers.ZurichADC_manager")
                        .attr("ZurichADCManager")(dacFileName, varManager);
  auto kwargs =
      pybind11::dict("variables"_a = varManager, "pulses"_a = pulseManager,
                     "ADC"_a = adcManager, "DAC"_a = dacManager);
  m_config = qtrlManagers.attr("MetaManager")(kwargs);
  pybind11::print(m_config);
  auto qpu = pybind11::module::import("qtrl.qpu.qpu");
  // Create a QTRL QPU
  m_qpu = qpu.attr("QPU")(m_config);
  // Import Circuit and CircuitCollection modules
  m_circuitCtor = pybind11::module::import("qtrl.qpu.circuit").attr("Circuit");
  m_circuitColectionCtor = pybind11::module::import("qtrl.qpu.circuit").attr("CircuitCollection");

  m_shots = 1024;
  if (params.keyExists<int>("shots")) {
    m_shots = params.get<int>("shots");
  }
}

std::vector<std::pair<int, int>> AqtAccelerator::getConnectivity() {
  return {};
}

pybind11::object AqtAccelerator::createQtrlCircuit(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  auto compiler = xacc::getCompiler("staq");
  auto circuit_src = compiler->translate(compositeInstruction);
  auto qtrlSeq = openQasm2QtrlSeq(m_openQASM2qtrl, circuit_src);
  // std::cout << "HOWDY:";
  // for (const auto &ll : qtrlSeq) {
  //   std::cout << ll << " ";
  // }
  // Create a qubit list from 0->(n-1)
  std::vector<int> qubits(buffer->size());
  std::iota(qubits.begin(), qubits.end(), 0);
  auto circuit = m_circuitCtor(qubits, qtrlSeq);
  std::vector<pybind11::object> circuit_collection { circuit };
  return m_circuitColectionCtor(qubits, circuit);
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  auto circuit = createQtrlCircuit(buffer, compositeInstruction);
  m_qpu.attr("run")(circuit, m_shots);
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {}
} // namespace quantum
} // namespace xacc

REGISTER_ACCELERATOR(xacc::quantum::AqtAccelerator)
