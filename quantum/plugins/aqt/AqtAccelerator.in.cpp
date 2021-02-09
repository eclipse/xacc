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
  // auto zi_mixin = pybind11::module::import("qtrl.instruments.zurich.ZurichMixin");
  auto dacManager = pybind11::module::import("qtrl.managers.ZurichDAC_manager")
                        .attr("ZurichDACManager")(adcFileName, varManager);
  auto adcManager = pybind11::module::import("qtrl.managers.ZurichADC_manager")
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
  m_circuitCollectionCtor = pybind11::module::import("qtrl.qpu.circuit").attr("CircuitCollection");

  // Warm up code: the QPU interface seems not add the readout
  // resonator to the list of ZI channels, need to do a dummy sequence
  // here to get them initialized....
  {
    auto add_readout =  pybind11::module::import("qtrl.sequence_utils.readout").attr("add_readout");
    m_config.attr("add_readout") = add_readout;
    auto qseq = pybind11::module::import("qtrl.sequencer");
    // Create a two-element sequence
    auto x90_seq = qseq.attr("Sequence")(2);
    auto x90_pulse = m_config.attr("pulses")["Q0/X90"];
    x90_seq.attr("append")(x90_pulse);
    const std::vector<std::string> s_refs{"Start", "Start"};
    m_config.attr("add_readout")(m_config, x90_seq, s_refs, s_refs);
    m_config.attr("write_sequence")(x90_seq);
  }

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
    const std::vector<std::shared_ptr<CompositeInstruction>>
        &compositeInstructions) {
  auto compiler = xacc::getCompiler("staq");
  // Create a qubit list from 0->(n-1)
  std::vector<int> qubits(buffer->size());
  std::iota(qubits.begin(), qubits.end(), 0);
  std::vector<pybind11::object> circuit_collection;

  for (const auto &compositeInstruction : compositeInstructions) {
    auto circuit_src = compiler->translate(compositeInstruction);
    auto qtrlSeq = openQasm2QtrlSeq(m_openQASM2qtrl, circuit_src);
    std::cout << "HOWDY:";
    for (const auto &ll : qtrlSeq) {
      std::cout << ll << " ";
    }
    std::cout << "\n";

    auto circuit = m_circuitCtor(qubits, qtrlSeq);
    circuit_collection.emplace_back(circuit);
  }

  return m_circuitCollectionCtor(qubits, circuit_collection);
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  auto circuit = createQtrlCircuit(buffer, { compositeInstruction });
  m_qpu.attr("run")(circuit, m_shots);
  // Result data that is appended to the circuit objs post-processing.
  auto results = circuit.attr("results");
  pybind11::print(results);
  // Results is a list (for each Circuit in the CircuitCollection)
  // of maps (dicts) from bitstring to count.
  auto result = results[0];
  auto bitStringIter = pybind11::iter(result);
  while (bitStringIter != pybind11::iterator::sentinel()) {
    const std::string bitString = (*bitStringIter).cast<std::string>();
    const int count = result[bitString.c_str()].cast<int>();
    buffer->appendMeasurement(bitString, count);
    ++bitStringIter;
  }
}

void AqtAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  std::vector<std::shared_ptr<AcceleratorBuffer>> childBuffers;
  for (auto &f : compositeInstructions) {
    childBuffers.emplace_back(
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size()));
  }

  auto circuit = createQtrlCircuit(buffer, compositeInstructions);
  m_qpu.attr("run")(circuit, m_shots);
  // Result data that is appended to the circuit objs post-processing.
  auto results = circuit.attr("results");
  auto iter = pybind11::iter(results);
  int childBufferIndex = 0;
  while (iter != pybind11::iterator::sentinel()) {
    auto childBuffer = childBuffers[childBufferIndex];
    auto result = (*iter).cast<pybind11::object>();
    auto bitStringIter = pybind11::iter(result);
    while (bitStringIter != pybind11::iterator::sentinel()) {
      const std::string bitString = (*bitStringIter).cast<std::string>();
      const int count = result[bitString.c_str()].cast<int>();
      childBuffer->appendMeasurement(bitString, count);
      ++bitStringIter;
    }

    ++iter;
    ++childBufferIndex;
  }
  assert(childBufferIndex == childBuffers.size());
  for (auto &childBuffer : childBuffers) {
    buffer->appendChild(childBuffer->name(), childBuffer);
  }
}
} // namespace quantum
} // namespace xacc

REGISTER_ACCELERATOR(xacc::quantum::AqtAccelerator)
