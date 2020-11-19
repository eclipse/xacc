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
#include "QlmAccelerator.hpp"
#include "xacc_plugin.hpp"
#include <cassert>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <dlfcn.h>
#include "xacc_config.hpp"
using namespace pybind11::literals;

namespace {
inline bool isMeasureGate(const xacc::InstPtr &in_instr) {
  return (in_instr->name() == "Measure");
}

void exportAqasm(pybind11::object &in_program, const std::string &in_fileName) {
  in_program.attr("export")(in_fileName);
}

pybind11::object qlm_qalloc(pybind11::object &in_program, size_t in_nbQubits) {
  return in_program.attr("qalloc")(in_nbQubits);
}

pybind11::object qlm_calloc(pybind11::object &in_program, size_t in_nbBits) {
  return in_program.attr("calloc")(in_nbBits);
}

pybind11::object to_circ(pybind11::object &in_program) {
  return in_program.attr("to_circ")();
}

pybind11::object to_job(pybind11::object &in_circ) {
  return in_circ.attr("to_job")();
}

pybind11::object exp_val_z_obs(size_t in_nbQubits,
                               const std::vector<size_t> &in_bits) {
  auto qatCore = pybind11::module::import("qat.core");
  auto Observable = qatCore.attr("Observable");
  auto Term = qatCore.attr("Term");
  const std::string obsStr(in_bits.size(), 'Z');
  auto obs = Observable(in_nbQubits);
  obs.attr("add_term")(Term(1.0, obsStr, in_bits));
  // pybind11::print(obs);
  return obs;
}

enum class JobType { Sample, Observable };
int getJobType(JobType in_type) {
  auto processingType =
      pybind11::module::import("qat.comm.shared.ttypes").attr("ProcessingType");
  switch (in_type) {
  case JobType::Sample:
    return processingType.attr("SAMPLE").cast<int>();
  case JobType::Observable:
    return processingType.attr("OBSERVABLE").cast<int>();
  default:
    __builtin_unreachable();
  }
}

pybind11::object getAqasmGate(const std::string &in_xaccGateName) {
  static std::unordered_map<std::string, pybind11::object> gateNameToGate;
  // Lazy construct (making sure the Accelerator has been initialized)
  // List of AQASM native gates:
  // Constant gates: X, Y, Z, H, S, T, CNOT, CCNOT, CSIGN, SWAP, SQRTSWAP, ISWAP
  // Parametrized gates: RX, RY, RZ, PH (phase shift)
  if (gateNameToGate.empty()) {
    auto aqasm = pybind11::module::import("qat.lang.AQASM");
    gateNameToGate.emplace("X", aqasm.attr("X"));
    gateNameToGate.emplace("Y", aqasm.attr("Y"));
    gateNameToGate.emplace("Z", aqasm.attr("Z"));
    gateNameToGate.emplace("Rx", aqasm.attr("RX"));
    gateNameToGate.emplace("Ry", aqasm.attr("RY"));
    gateNameToGate.emplace("Rz", aqasm.attr("RZ"));
    gateNameToGate.emplace("H", aqasm.attr("H"));
    gateNameToGate.emplace("T", aqasm.attr("T"));
    gateNameToGate.emplace("S", aqasm.attr("S"));
    gateNameToGate.emplace("PH", aqasm.attr("PH"));
    // Two-qubit gates:
    gateNameToGate.emplace("CNOT", aqasm.attr("CNOT"));
    gateNameToGate.emplace("Swap", aqasm.attr("SWAP"));
    gateNameToGate.emplace("CZ", aqasm.attr("CSIGN"));
    gateNameToGate.emplace("iSwap", aqasm.attr("ISWAP"));
    // Fall-back
    gateNameToGate.emplace("CustomGate", aqasm.attr("CustomGate"));
  }

  const auto gateObjIter = gateNameToGate.find(in_xaccGateName);
  if (gateObjIter != gateNameToGate.end()) {
    return gateObjIter->second;
  } else {
    return gateNameToGate["CustomGate"];
  }
}

pybind11::array_t<std::complex<double>> fSimGateMat(double in_theta,
                                                    double in_phi) {
  pybind11::array_t<std::complex<double>> gateMat({4, 4});
  auto r = gateMat.mutable_unchecked<2>();
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      r(i, j) = 0.0;
    }
  }

  r(0, 0) = 1.0;
  r(1, 1) = std::cos(in_theta);
  r(1, 2) = std::complex<double>(0, -std::sin(in_theta));
  r(2, 1) = std::complex<double>(0, -std::sin(in_theta));
  r(2, 2) = std::cos(in_theta);
  r(3, 3) = std::exp(std::complex<double>(0, -in_phi));
  return gateMat;
}

pybind11::array_t<std::complex<double>>
u3GateMat(double in_theta, double in_phi, double in_lambda) {
  pybind11::array_t<std::complex<double>> gateMat({2, 2});
  auto r = gateMat.mutable_unchecked<2>();
  r(0, 0) = std::cos(in_theta / 2.0);
  r(0, 1) =
      -std::exp(std::complex<double>(0, in_lambda)) * std::sin(in_theta / 2.0);
  r(1, 0) =
      std::exp(std::complex<double>(0, in_phi)) * std::sin(in_theta / 2.0);
  r(1, 1) = std::exp(std::complex<double>(0, in_phi + in_lambda)) *
            std::cos(in_theta / 2.0);
  return gateMat;
}
} // namespace

namespace xacc {
namespace quantum {
QlmCircuitVisitor::QlmCircuitVisitor(size_t nbQubit) : m_nbQubit(nbQubit) {
  m_aqasm = pybind11::module::import("qat.lang.AQASM");
  m_aqasmProgram = createProgram();
  m_qreg = qlm_qalloc(m_aqasmProgram, m_nbQubit);
  m_creg = qlm_calloc(m_aqasmProgram, m_nbQubit);
}

pybind11::object QlmCircuitVisitor::createProgram() {
  return m_aqasm.attr("Program")();
}

void QlmCircuitVisitor::visit(Hadamard &h) {
  auto h_gate = getAqasmGate(h.name());
  m_aqasmProgram.attr("apply")(h_gate, m_qreg[pybind11::int_(h.bits()[0])]);
}

void QlmCircuitVisitor::visit(CNOT &cnot) {
  auto cx_gate = getAqasmGate(cnot.name());
  m_aqasmProgram.attr("apply")(cx_gate, m_qreg[pybind11::int_(cnot.bits()[0])],
                               m_qreg[pybind11::int_(cnot.bits()[1])]);
}

void QlmCircuitVisitor::visit(Rz &rz) {
  auto rz_gate = getAqasmGate(rz.name());
  const double theta = InstructionParameterToDouble(rz.getParameter(0));
  m_aqasmProgram.attr("apply")(rz_gate(theta),
                               m_qreg[pybind11::int_(rz.bits()[0])]);
}

void QlmCircuitVisitor::visit(Ry &ry) {
  auto ry_gate = getAqasmGate(ry.name());
  const double theta = InstructionParameterToDouble(ry.getParameter(0));
  m_aqasmProgram.attr("apply")(ry_gate(theta),
                               m_qreg[pybind11::int_(ry.bits()[0])]);
}

void QlmCircuitVisitor::visit(Rx &rx) {
  auto rx_gate = getAqasmGate(rx.name());
  const double theta = InstructionParameterToDouble(rx.getParameter(0));
  m_aqasmProgram.attr("apply")(rx_gate(theta),
                               m_qreg[pybind11::int_(rx.bits()[0])]);
}

void QlmCircuitVisitor::visit(X &x) {
  auto x_gate = getAqasmGate(x.name());
  m_aqasmProgram.attr("apply")(x_gate, m_qreg[pybind11::int_(x.bits()[0])]);
}

void QlmCircuitVisitor::visit(Y &y) {
  auto y_gate = getAqasmGate(y.name());
  m_aqasmProgram.attr("apply")(y_gate, m_qreg[pybind11::int_(y.bits()[0])]);
}

void QlmCircuitVisitor::visit(Z &z) {
  auto z_gate = getAqasmGate(z.name());
  m_aqasmProgram.attr("apply")(z_gate, m_qreg[pybind11::int_(z.bits()[0])]);
}

void QlmCircuitVisitor::visit(S &s) {
  auto s_gate = getAqasmGate(s.name());
  m_aqasmProgram.attr("apply")(s_gate, m_qreg[pybind11::int_(s.bits()[0])]);
}

void QlmCircuitVisitor::visit(Sdg &sdg) {
  auto s_gate = getAqasmGate("S");
  auto sdg_gate = s_gate.attr("dag")();
  m_aqasmProgram.attr("apply")(sdg_gate, m_qreg[pybind11::int_(sdg.bits()[0])]);
}

void QlmCircuitVisitor::visit(T &t) {
  auto t_gate = getAqasmGate(t.name());
  m_aqasmProgram.attr("apply")(t_gate, m_qreg[pybind11::int_(t.bits()[0])]);
}

void QlmCircuitVisitor::visit(Tdg &tdg) {
  auto t_gate = getAqasmGate("T");
  auto tdg_gate = t_gate.attr("dag")();
  m_aqasmProgram.attr("apply")(tdg_gate, m_qreg[pybind11::int_(tdg.bits()[0])]);
}

void QlmCircuitVisitor::visit(CY &cy) {
  auto y_gate = getAqasmGate("Y");
  auto cy_gate = y_gate.attr("ctrl")();
  m_aqasmProgram.attr("apply")(cy_gate, m_qreg[pybind11::int_(cy.bits()[0])],
                               m_qreg[pybind11::int_(cy.bits()[1])]);
}

void QlmCircuitVisitor::visit(CZ &cz) {
  auto cz_gate = getAqasmGate(cz.name());
  m_aqasmProgram.attr("apply")(cz_gate, m_qreg[pybind11::int_(cz.bits()[0])],
                               m_qreg[pybind11::int_(cz.bits()[1])]);
}

void QlmCircuitVisitor::visit(Swap &s) {
  auto swap_gate = getAqasmGate(s.name());
  m_aqasmProgram.attr("apply")(swap_gate, m_qreg[pybind11::int_(s.bits()[0])],
                               m_qreg[pybind11::int_(s.bits()[1])]);
}

void QlmCircuitVisitor::visit(CRZ &crz) {
  auto rz_gate = getAqasmGate("Rz");
  const double theta = InstructionParameterToDouble(crz.getParameter(0));
  auto crz_gate = rz_gate(theta).attr("ctrl")();
  m_aqasmProgram.attr("apply")(crz_gate, m_qreg[pybind11::int_(crz.bits()[0])],
                               m_qreg[pybind11::int_(crz.bits()[1])]);
}

void QlmCircuitVisitor::visit(CH &ch) {
  auto h_gate = getAqasmGate("H");
  auto ch_gate = h_gate.attr("ctrl")();
  m_aqasmProgram.attr("apply")(ch_gate, m_qreg[pybind11::int_(ch.bits()[0])],
                               m_qreg[pybind11::int_(ch.bits()[1])]);
}

void QlmCircuitVisitor::visit(CPhase &cphase) {
  auto ph_gate = getAqasmGate("PH");
  const double theta = InstructionParameterToDouble(cphase.getParameter(0));
  auto cp_gate = ph_gate(theta).attr("ctrl")();
  m_aqasmProgram.attr("apply")(cp_gate,
                               m_qreg[pybind11::int_(cphase.bits()[0])],
                               m_qreg[pybind11::int_(cphase.bits()[1])]);
}

void QlmCircuitVisitor::visit(U &u) {
  auto c_gate = getAqasmGate("CustomGate");
  const auto theta = InstructionParameterToDouble(u.getParameter(0));
  const auto phi = InstructionParameterToDouble(u.getParameter(1));
  const auto lambda = InstructionParameterToDouble(u.getParameter(2));
  auto u3_gate = c_gate(u3GateMat(theta, phi, lambda));
  m_aqasmProgram.attr("apply")(u3_gate, m_qreg[pybind11::int_(u.bits()[0])]);
}

void QlmCircuitVisitor::visit(iSwap &in_iSwapGate) {
  auto iswap_gate = getAqasmGate(in_iSwapGate.name());
  m_aqasmProgram.attr("apply")(iswap_gate,
                               m_qreg[pybind11::int_(in_iSwapGate.bits()[0])],
                               m_qreg[pybind11::int_(in_iSwapGate.bits()[1])]);
}

void QlmCircuitVisitor::visit(fSim &in_fsimGate) {
  const auto theta = InstructionParameterToDouble(in_fsimGate.getParameter(0));
  const auto phi = InstructionParameterToDouble(in_fsimGate.getParameter(1));
  auto c_gate = getAqasmGate("CustomGate");
  auto fsim_gate = c_gate(fSimGateMat(theta, phi));
  m_aqasmProgram.attr("apply")(fsim_gate,
                               m_qreg[pybind11::int_(in_fsimGate.bits()[0])],
                               m_qreg[pybind11::int_(in_fsimGate.bits()[1])]);
}

void QlmCircuitVisitor::visit(Measure &measure) {}

void QlmAccelerator::initialize(const HeterogeneousMap &params) {
  static bool PythonInit = false;
  if (!PythonInit) {
    if (!XACC_IS_APPLE) {
      // If not MacOs, preload Python lib to the address space.
      // Note: we don't need to dlclose, just need to load the lib to prevent
      // linking issue on Linux.
      auto libPythonPreload =
          dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    }
    pybind11::initialize_interpreter();
    PythonInit = true;
  }
  m_shots = -1;
  if (params.keyExists<int>("shots")) {
    m_shots = params.get<int>("shots");
    if (m_shots < 1) {
      xacc::error("Invalid 'shots' parameter.");
    }
  }

  // TODO: choose linalg, feymann, mps simulators
  auto qpuMod = pybind11::module::import("qat.linalg.qpu");
  m_qlmQpuServer = qpuMod.attr("get_qpu_server")();
}

pybind11::object QlmAccelerator::constructQlmJob(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::shared_ptr<CompositeInstruction> compositeInstruction) const {
  QlmCircuitVisitor visitor(buffer->size());
  // Walk the IR tree, and visit each node
  InstructionIterator it(compositeInstruction);
  std::vector<size_t> measureBitIdxs;
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(&visitor);
    }
    if (isMeasureGate(nextInst)) {
      measureBitIdxs.emplace_back(nextInst->bits()[0]);
    }
  }
  // Debug:
  // exportAqasm(visitor.getProgram(), "test.aqasm");
  // Shots:
  auto circ = to_circ(visitor.getProgram());
  auto job = to_job(circ);
  if (m_shots > 0 || measureBitIdxs.empty()) {
    job.attr("nbshots") = m_shots;
    job.attr("type") = getJobType(JobType::Sample);
  } else {
    // Exp-val calc.
    job.attr("observable") = exp_val_z_obs(buffer->size(), measureBitIdxs);
    job.attr("type") = getJobType(JobType::Observable);
  }
  return job;
}

void QlmAccelerator::persistResultToBuffer(
    std::shared_ptr<AcceleratorBuffer> buffer, pybind11::object &result) const {
  if (result.attr("value").is_none()) {
    auto iter = pybind11::iter(result);
    while (iter != pybind11::iterator::sentinel()) {
      auto sampleData = *iter;
      auto bitStr = pybind11::str(sampleData.attr("state")).cast<std::string>();
      bitStr = bitStr.substr(1, bitStr.size() - 2);
      auto bitStrProb = sampleData.attr("probability").cast<double>();
      int count = std::round(bitStrProb * m_shots);
      buffer->appendMeasurement(bitStr, count);
      ++iter;
    }
  } else {
    auto expVal = result.attr("value").cast<double>();
    buffer->addExtraInfo("exp-val-z", expVal);
  }
}

void QlmAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> compositeInstruction) {
  auto qlmJob = constructQlmJob(buffer, compositeInstruction);
  auto result = m_qlmQpuServer.attr("submit")(qlmJob);
  persistResultToBuffer(buffer, result);
}

void QlmAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  std::vector<std::shared_ptr<AcceleratorBuffer>> childBuffers;
  std::vector<pybind11::object> batch;
  for (auto &f : compositeInstructions) {
    childBuffers.emplace_back(
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size()));
    batch.emplace_back(constructQlmJob(buffer, f));
  }

  // Submit the whole batch:
  auto batchResult = m_qlmQpuServer.attr("submit")(batch);
  // pybind11::print(result);
  auto iter = pybind11::iter(batchResult);
  int childBufferIndex = 0;
  while (iter != pybind11::iterator::sentinel()) {
    auto result = (*iter).cast<pybind11::object>();
    persistResultToBuffer(childBuffers[childBufferIndex++], result);
    ++iter;
  }
  assert(childBufferIndex == childBuffers.size());
  for (auto &childBuffer : childBuffers) {
    buffer->appendChild(childBuffer->name(), childBuffer);
  }
}
} // namespace quantum
} // namespace xacc

REGISTER_ACCELERATOR(xacc::quantum::QlmAccelerator)
