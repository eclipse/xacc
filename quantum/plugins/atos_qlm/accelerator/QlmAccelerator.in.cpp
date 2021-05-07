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
#include <pybind11/functional.h>
#include <dlfcn.h>
#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"
#include <random>

using namespace pybind11::literals;

namespace {
// Default variables:
// By default, this accelerator targets ORNL's QLM installation.
// It can be used w/ other QLM by providing the host address explicitly.
const std::string QLM_HOST_NAME = "quantumbull.ornl.gov";

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

// Gate name and arity
std::pair<std::string, size_t>
getXasmGate(const std::string &in_aqasmGateName) {
  static std::unordered_map<std::string, std::pair<std::string, size_t>>
      aqasmToXasm{{"H", std::make_pair("H", 1)},
                  {"X", std::make_pair("X", 1)},
                  {"Y", std::make_pair("Y", 1)},
                  {"Z", std::make_pair("Z", 1)},
                  {"CNOT", std::make_pair("CNOT", 2)},
                  {"CSIGN", std::make_pair("CZ", 2)},
                  {"ISWAP", std::make_pair("iSwap", 2)},
                  {"SWAP", std::make_pair("Swap", 2)}};

  const auto iter = aqasmToXasm.find(in_aqasmGateName);
  if (iter != aqasmToXasm.end()) {
    return iter->second;
  }
  return std::make_pair("", 0);
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

pybind11::array_t<std::complex<double>>
matToNumpy(const std::vector<std::vector<std::complex<double>>> &in_mat) {
  pybind11::array_t<std::complex<double>> gateMat(
      {in_mat.size(), in_mat.size()});
  auto r = gateMat.mutable_unchecked<2>();
  for (int i = 0; i < in_mat.size(); ++i) {
    for (int j = 0; j < in_mat.size(); ++j) {
      r(i, j) = in_mat[i][j];
    }
  }
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
    try {
      pybind11::initialize_interpreter();
    } catch (...) {
    }
    PythonInit = true;
  }

  // Are we on the QLM machine?
  // check if we can access the QAT python package.
  // Otherwise, check the qlmaas package.
  m_remoteAccess = []() {
    try {
      pybind11::module::import("qat.qpus.LinAlg");
      pybind11::module::import("qat.qpus.Feynman");
      pybind11::module::import("qat.qpus.MPS");
      // Local access
      return false;
    } catch (std::exception &e) {
      // std::cout << "Unable to import QAT. Try remote access (QLMaaS).\n";
      try {
        pybind11::module::import("qat.qlmaas");
        std::cout << "Remote access via qlmaas.\n";
        return true;
      } catch (std::exception &e) {
        xacc::error("QLMaaS package is not available. Please install qlmaas "
                    "via pip to use QLM accelerator.");
      }
    }
    return true;
  }();

  // Establish the connection
  if (m_remoteAccess) {
    // By default, a user may have a config file in $HOME/.qlmaas/config.ini
    // e.g. they've used the Python API to connect to the QLM via qlmaas.
    const std::string qlmaasConfig(std::string(::getenv("HOME")) +
                                   "/.qlmaas/config.ini");

    // XACC config file:
    const std::string xaccQlmaasConfig(std::string(::getenv("HOME")) +
                                       "/.qlm_config");
    if (xacc::fileExists(qlmaasConfig)) {
      xacc::info("Using QLMaaS config file: " + qlmaasConfig);
      // Note: this may require users to type the password
      // or they have username and password saved in environment variables or
      // QLM password file.
      m_qlmaasConnection =
          pybind11::module::import("qat.qlmaas").attr("QLMaaSConnection")();
      std::cout << "Successfully establish a connection to the QLM machine.\n";
    } else {
      std::string username, password, hostname;
      if (xacc::fileExists(xaccQlmaasConfig)) {
        std::ifstream stream(xaccQlmaasConfig);
        std::string contents((std::istreambuf_iterator<char>(stream)),
                             std::istreambuf_iterator<char>());
        std::tie(username, password,
                 hostname) = [](const std::string &config_file_content) {
          std::string hostName = QLM_HOST_NAME;
          std::string userName;
          std::string password;
          std::vector<std::string> lines;
          lines = xacc::split(config_file_content, '\n');
          for (auto l : lines) {
            if (l.find("host") != std::string::npos) {
              std::vector<std::string> split = xacc::split(l, ':');
              auto key = split[1];
              xacc::trim(key);
              hostName = key;
            } else if (l.find("username") != std::string::npos) {
              std::vector<std::string> split;
              split = xacc::split(l, ':');
              auto _userName = split[1];
              xacc::trim(_userName);
              userName = _userName;
            } else if (l.find("password") != std::string::npos) {
              std::vector<std::string> split;
              split = xacc::split(l, ':');
              auto _password = split[1];
              xacc::trim(_password);
              password = _password;
            }
          }

          return std::make_tuple(userName, password, hostName);
        }(contents);
      } else {
        // Try parse from HetMap
        hostname = QLM_HOST_NAME;
        if (params.stringExists("hostname")) {
          hostname = params.getString("hostname");
        }
        if (params.stringExists("username")) {
          username = params.getString("username");
        }
        if (params.stringExists("password")) {
          password = params.getString("password");
        }
      }

      if (hostname.empty() || username.empty() || password.empty()) {
        xacc::error(
            "'username' or 'password' were not provided. Please set them in "
            "$HOME/.qlm_config file or provided in the configurations.");
      }

      m_qlmaasConnection = [&]() -> pybind11::object {
        auto locals = pybind11::dict();
        locals["hostname"] = hostname;
        locals["username"] = username;
        locals["password"] = password;
        auto py_src =
            R"#(
from qat.qlmaas import QLMaaSConnection
import os
os.environ["QLM_USER"] = locals()['username']
os.environ["QLM_PASSWD"] = locals()['password']
qlmaas_connection = QLMaaSConnection(hostname=locals()['hostname'], check_host=False)
  )#";
        pybind11::exec(py_src, pybind11::globals(), locals);
        std::cout
            << "Successfully establish a connection to the QLM machine.\n";
        return locals["qlmaas_connection"];
      }();
    }
  }
  m_shots = -1;
  if (params.keyExists<int>("shots")) {
    m_shots = params.get<int>("shots");
    if (m_shots < 1) {
      xacc::error("Invalid 'shots' parameter.");
    }
  }

  // Handle noisy simulation:
  m_noiseModel.reset();
  // Noise construction can only be done locally (on the QLM) atm.
  // The ability to construct noise model is not available on the client side.
  if (!m_remoteAccess) {
    // First, check direct noise model input:
    if (params.pointerLikeExists<xacc::NoiseModel>("noise-model")) {
      m_noiseModel = xacc::as_shared_ptr(
          params.getPointerLike<xacc::NoiseModel>("noise-model"));
      // QAT gateset generator:
      // Note: this predef only contains *static* gates,
      // i.e. not include Rx, Ry, Rz
      auto predef_generator =
          pybind11::module::import("qat.core.circuit_builder.matrix_util")
              .attr("get_predef_generator");
      auto QuantumChannelKraus =
          pybind11::module::import("qat.quops.quantum_channels")
              .attr("QuantumChannelKraus");
      const auto nbQubits = m_noiseModel->nQubits();
      auto aqasmGates = predef_generator().attr("keys")();
      auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
      pybind11::dict all_gates_noise;
      // Hanle parametric (Rx, Ry, Rz) gates:
      for (const auto &gateName : {"Rx", "Ry", "Rz"}) {
        pybind11::dict gates_noise;
        for (size_t qId = 0; qId < nbQubits; ++qId) {
          auto inst = gateRegistry->createInstruction(gateName, qId);
          auto gate = std::dynamic_pointer_cast<xacc::quantum::Gate>(inst);
          const auto errorChannels = m_noiseModel->getNoiseChannels(*gate);
          if (!errorChannels.empty()) {
            // std::cout << "Gate " << gate->toString() << "\n";
            gates_noise[pybind11::int_(qId)] = pybind11::cpp_function(
                [errorChannels, QuantumChannelKraus](double theta,
                                                     pybind11::kwargs kwarg) {
                  // std::cout << "Getting noise channel info\n";
                  std::vector<pybind11::array_t<std::complex<double>>>
                      kraus_mats;
                  for (const auto &channel : errorChannels) {
                    for (const auto &op : channel.mats) {
                      kraus_mats.emplace_back(matToNumpy(op));
                    }
                  }

                  auto krausChannel = QuantumChannelKraus(kraus_mats);
                  // pybind11::print(krausChannel);
                  return krausChannel;
                });
          }
        }

        if (gates_noise.size() > 0) {
          std::string aqasmGateName = gateName;
          std::transform(aqasmGateName.begin(), aqasmGateName.end(),
                         aqasmGateName.begin(), ::toupper);
          all_gates_noise[aqasmGateName.c_str()] = gates_noise;
        }
      }

      // Static prefef gate:
      for (const auto &aqasmGateName : aqasmGates) {
        const auto gateName = aqasmGateName.cast<std::string>();
        const auto [xaccEquiv, gateArity] = getXasmGate(gateName);
        if (!xaccEquiv.empty()) {
          pybind11::dict gates_noise;
          // One-qubit gate noise
          if (gateArity == 1) {
            for (size_t qId = 0; qId < nbQubits; ++qId) {
              auto inst = gateRegistry->createInstruction(xaccEquiv, qId);
              auto gate = std::dynamic_pointer_cast<xacc::quantum::Gate>(inst);
              const auto errorChannels = m_noiseModel->getNoiseChannels(*gate);
              if (!errorChannels.empty()) {
                // std::cout << "Gate " << gate->toString() << "\n";
                gates_noise[pybind11::int_(qId)] =
                    pybind11::cpp_function([errorChannels, QuantumChannelKraus](
                                               pybind11::kwargs kwarg) {
                      // std::cout << "Getting noise channel info\n";
                      std::vector<pybind11::array_t<std::complex<double>>>
                          kraus_mats;
                      for (const auto &channel : errorChannels) {
                        for (const auto &op : channel.mats) {
                          kraus_mats.emplace_back(matToNumpy(op));
                        }
                      }

                      auto krausChannel = QuantumChannelKraus(kraus_mats);
                      // pybind11::print(krausChannel);
                      return krausChannel;
                    });
              }
            }
          }

          // Two-qubit gate noise:
          if (gateArity == 2) {
            for (size_t qId1 = 0; qId1 < nbQubits; ++qId1) {
              for (size_t qId2 = 0; qId2 < nbQubits; ++qId2) {
                if (qId1 != qId2) {
                  auto inst =
                      gateRegistry->createInstruction(xaccEquiv, {qId1, qId2});
                  auto gate =
                      std::dynamic_pointer_cast<xacc::quantum::Gate>(inst);
                  const auto errorChannels =
                      m_noiseModel->getNoiseChannels(*gate);
                  if (!errorChannels.empty()) {
                    // std::cout << "Has noise: " << gate->toString() << "\n";
                    // Bit order:
                    std::vector<pybind11::array_t<std::complex<double>>>
                        kraus_mats;
                    if (errorChannels[0].bit_order == KrausMatBitOrder::LSB) {
                      for (const auto &channel : errorChannels) {
                        for (const auto &op : channel.mats) {
                          kraus_mats.emplace_back(matToNumpy(op));
                        }
                      }
                    } else {
                      // Flip Kron product order
                      const auto flipKronOrder =
                          [](const std::vector<
                              std::vector<std::complex<double>>> &in_mat) {
                            assert(in_mat.size() == 4);
                            auto result = in_mat;
                            const std::vector<size_t> order{0, 2, 1, 3};
                            for (size_t i = 0; i < 4; ++i) {
                              for (size_t j = 0; j < 4; ++j) {
                                result[order[i]][order[j]] = in_mat[i][j];
                              }
                            }
                            return result;
                          };

                      for (const auto &channel : errorChannels) {
                        for (const auto &op : channel.mats) {
                          kraus_mats.emplace_back(
                              matToNumpy(flipKronOrder(op)));
                        }
                      }
                    }
                    if (kraus_mats.size() > 0) {
                      // std::cout << "Kraus mats:\n";
                      // pybind11::print(kraus_mats);
                      auto krausChannel = QuantumChannelKraus(kraus_mats);
                      all_gates_noise[gateName.c_str()] =
                          pybind11::cpp_function(
                              [krausChannel](pybind11::kwargs kwarg) {
                                // pybind11::print(krausChannel);
                                return krausChannel;
                              });
                    }
                  }
                }
              }
            }
          }
          if (gates_noise.size() > 0) {
            all_gates_noise[gateName.c_str()] = gates_noise;
          }
        }
      }

      // pybind11::print(all_gates_noise);
      auto qlmHardwareMod = pybind11::module::import("qat.hardware.default");
      auto hardwareModel = qlmHardwareMod.attr("HardwareModel");
      auto gatesSpecification =
          qlmHardwareMod.attr("DefaultGatesSpecification");
      auto gates_spec = gatesSpecification();
      auto hw_model = hardwareModel(gates_spec, all_gates_noise);
      // Noisy simulator:
      auto noisyQProc = pybind11::module::import("qat.qpus").attr("NoisyQProc");
      // Note: we use deterministic-vectorized to be able to compute the
      // exp-val from OBS-job.
      m_qlmQpuServer = noisyQProc(hw_model, "deterministic-vectorized");
    } else if (params.stringExists("backend")) {
      m_noiseModel = xacc::getService<NoiseModel>("IBM");
      m_noiseModel->initialize(params);
      auto qlmHardwareMod = pybind11::module::import("qat.hardware.default");
      auto hardwareModel = qlmHardwareMod.attr("HardwareModel");
      auto gatesSpecification =
          qlmHardwareMod.attr("DefaultGatesSpecification");
      // List of gate to initialize the QLM noise model:
      // Note: other gates (dagger and control) are expresses in terms of
      // these gates.
      const std::vector<std::string> GATE_SET{"X",  "Y", "Z", "Rx", "Ry",
                                              "Rz", "H", "S", "T",  "U"};
      const std::unordered_map<std::string, double> QLM_GATE_ERRORS = [&]() {
        auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
        std::unordered_map<std::string, double> result;
        for (const auto &gateName : GATE_SET) {
          auto gate = gateRegistry->createInstruction(gateName, 0);
          auto aqasmGateName = gateName;
          std::for_each(aqasmGateName.begin(), aqasmGateName.end(),
                        [](char &c) { c = ::toupper(c); });
          if (aqasmGateName == "U") {
            aqasmGateName = "CustomGate";
          }
          result[aqasmGateName] = m_noiseModel->gateErrorProb(
              *std::dynamic_pointer_cast<xacc::quantum::Gate>(gate));
        }
        // Add 2-qubit noise:
        CNOT cx_gate(0, 1);
        result["CNOT"] = m_noiseModel->gateErrorProb(cx_gate);
        return result;
      }();

      pybind11::dict gates_noise;
      for (const auto &iter : QLM_GATE_ERRORS) {
        auto aqasmGate = iter.first;
        auto errorRate = iter.second;
        // std::cout << aqasmGate << ": " << errorRate << "\n";
        if (aqasmGate == "RX" || aqasmGate == "RY" || aqasmGate == "RZ") {
          gates_noise[aqasmGate.c_str()] = pybind11::cpp_function(
              [errorRate, aqasmGate](double theta, pybind11::kwargs kwarg) {
                auto make_depolarizing_channel =
                    pybind11::module::import("qat.quops.quantum_channels")
                        .attr("make_depolarizing_channel");
                return make_depolarizing_channel(errorRate, 1);
              });
        } else {
          gates_noise[aqasmGate.c_str()] = pybind11::cpp_function(
              [errorRate, aqasmGate](pybind11::kwargs kwarg) {
                auto make_depolarizing_channel =
                    pybind11::module::import("qat.quops.quantum_channels")
                        .attr("make_depolarizing_channel");
                const int arity = (aqasmGate == "CNOT") ? 2 : 1;
                return make_depolarizing_channel(errorRate, arity);
              });
        }
      }

      auto gates_spec = gatesSpecification();
      auto hw_model = hardwareModel(gates_spec, gates_noise);
      // Noisy simulator:
      auto noisyQProc = pybind11::module::import("qat.qpus").attr("NoisyQProc");
      m_qlmQpuServer = noisyQProc(hw_model);
      // For noisy sim, we must run QLM in shots mode:
      constexpr int DEFAULT_NUM_SHOTS = 1024;
      if (m_shots < 0) {
        m_shots = DEFAULT_NUM_SHOTS;
      }
    }
  }

  if (!m_noiseModel) {
    // No noise:
    // There are 4 types of simulator:
    // (1) LinAlg
    // (2) MPS
    // (3) Feynman
    // (4) BDD (Quantum Multi-valued Decision Diagrams)
    // Default is LinAlg, which is the most versatile (consistent perf. in
    // most cases) User can switch between them using the "sim-type" option:
    using SimFactory = std::function<pybind11::object(
        const HeterogeneousMap &, bool, pybind11::object &)>;
    const SimFactory createMpsSim = [](const HeterogeneousMap &configs,
                                       bool isRemote,
                                       pybind11::object &qlmaasConnection) {
      auto simClass = [&]() -> pybind11::object {
        if (!isRemote) {
          return pybind11::module::import("qat.qpus").attr("MPS");
        } else {
          return qlmaasConnection.attr("get_qpu")("qat.qpus:MPS");
        }
      }();

      // Default MPS settings:
      // lnnize=True, no_merge=False, threshold=None, n_trunc=None
      // Supported users-options (that we exposed to XACC):
      // mps-threshold: specify threshold below which Schmidt coefficients
      // are truncated. max-bond: specify maximum number of non-zero Schmidt
      // coefficients.
      std::optional<double> threshold;
      if (configs.keyExists<double>("mps-threshold")) {
        threshold = configs.get<double>("mps-threshold");
      }

      std::optional<double> n_trunc;
      if (configs.keyExists<int>("max-bond")) {
        n_trunc = configs.get<int>("max-bond");
      }

      return simClass("lnnize"_a = true, "no_merge"_a = false,
                      "threshold"_a = threshold, "n_trunc"_a = n_trunc);
    };

    const SimFactory createFeynmanSim = [](const HeterogeneousMap &configs,
                                           bool isRemote,
                                           pybind11::object &qlmaasConnection) {
      // Support changing number of threads:
      int nbThreads = 1;
      if (configs.keyExists<int>("threads")) {
        nbThreads = configs.get<int>("threads");
      }
      auto simClass = [&]() -> pybind11::object {
        if (!isRemote) {
          return pybind11::module::import("qat.qpus").attr("Feynman");
        } else {
          return qlmaasConnection.attr("get_qpu")("qat.qpus:Feynman");
        }
      }();

      return simClass("threads"_a = nbThreads);
    };

    const SimFactory createBddSim = [](const HeterogeneousMap &configs,
                                       bool isRemote,
                                       pybind11::object &qlmaasConnection) {
      // Support changing number of threads:
      int nbThreads = 1;
      if (configs.keyExists<int>("threads")) {
        nbThreads = configs.get<int>("threads");
      }
      auto simClass = [&]() -> pybind11::object {
        if (!isRemote) {
          return pybind11::module::import("qat.qpus").attr("Bdd");
        } else {
          return qlmaasConnection.attr("get_qpu")("qat.qpus:Bdd");
        }
      }();

      return simClass("threads"_a = nbThreads);
    };

    static const std::unordered_map<std::string, SimFactory> SIM_REGISTRY{
        // LinAlg doesn't have any extra runtime params
        {"LinAlg",
         [](const HeterogeneousMap &configs, bool isRemote,
            pybind11::object &qlmaasConnection) {
           if (!isRemote) {
             return pybind11::module::import("qat.qpus").attr("LinAlg")();
           } else {
             auto qpuClass =
                 qlmaasConnection.attr("get_qpu")("qat.qpus:LinAlg");
             return qpuClass();
           }
         }},
        {"MPS", createMpsSim},
        {"Feynman", createFeynmanSim},
        {"Bdd", createBddSim},
    };

    // Default:
    std::string simType = "LinAlg";
    if (params.stringExists("sim-type")) {
      simType = params.getString("sim-type");
    }

    const auto iter = SIM_REGISTRY.find(simType);
    if (iter == SIM_REGISTRY.end()) {
      xacc::error("The requested sim-type of '" + simType + "' is invalid.");
    }

    m_qlmQpuServer = (iter->second)(params, m_remoteAccess, m_qlmaasConnection);

    // Important notes: Feynman and BDD don't support Observable mode,
    // hence, must use shots.
    if (simType == "Feynman" || simType == "Bdd") {
      // No shots was specified.
      constexpr int DEFAULT_NUM_SHOTS = 1024;
      if (m_shots < 0) {
        m_shots = DEFAULT_NUM_SHOTS;
      }
    }
  }
}

std::vector<std::pair<int, int>> QlmAccelerator::getConnectivity() {
  if (m_noiseModel) {
    // There is a hardware model, i.e. IBM backend.
    // Note: the noise model has noise data for both directions of the pair,
    // hence, we use set to return non-directional coupling map.
    std::set<std::pair<int, int>> couplingMap;
    auto twoQubitPairs = m_noiseModel->averageTwoQubitGateFidelity();
    for (const auto &[q1, q2, err] : twoQubitPairs) {
      if (q1 < q2) {
        couplingMap.emplace(q1, q2);
      } else {
        couplingMap.emplace(q2, q1);
      }
    }
    std::vector<std::pair<int, int>> result(couplingMap.begin(),
                                            couplingMap.end());
    return result;
  }
  return {};
}

pybind11::object QlmAccelerator::constructQlmCirc(
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
  return circ;
}

pybind11::object QlmAccelerator::constructQlmJob(
    std::shared_ptr<AcceleratorBuffer> buffer,
    std::shared_ptr<CompositeInstruction> compositeInstruction) const {
  if (m_noiseModel) {
    // If having a noise model, use Staq to translate the Composite
    // to OpenQASM so that two-qubit gates are translated to CX -> can map to
    // noise data.
    auto compiler = xacc::getCompiler("staq");
    auto circuit_src = compiler->translate(compositeInstruction);
    auto recompile = compiler->compile(circuit_src)->getComposites()[0];
    // std::cout << "HOWDY: \n" << recompile->toString() << "\n";
    compositeInstruction->clear();
    compositeInstruction->addInstructions(recompile->getInstructions());
  }

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
    // If there is no measure gate, default is to measure all.
    // Otherwise, specify the qubits to be measured.
    if (!measureBitIdxs.empty()) {
      job.attr("qubits") = measureBitIdxs;
    }
  } else {
    // Exp-val calc.
    job.attr("observable") = exp_val_z_obs(buffer->size(), measureBitIdxs);
    job.attr("type") = getJobType(JobType::Observable);
  }
  return job;
}

void QlmAccelerator::persistResultToBuffer(
    std::shared_ptr<AcceleratorBuffer> buffer, pybind11::object &result,
    pybind11::object &job) const {
  static auto randomProbFunc =
      std::bind(std::uniform_real_distribution<double>(0, 1),
                std::mt19937(std::chrono::high_resolution_clock::now()
                                 .time_since_epoch()
                                 .count()));
  // Apply readout error sampling on the result:
  // Note: we handle this separately here since it seems like the QLM API
  // doesn't have a way to specify per-qubit error rate.
  const auto applyReadoutError = [&](const std::string &bitString, int count,
                                     const NoiseModel &in_noiseModel) {
    std::vector<int> measureQubits;
    try {
      measureQubits = job.attr("qubits").cast<std::vector<int>>();
    } catch (...) {
      measureQubits.clear();
      for (int i = 0; i < bitString.size(); ++i) {
        measureQubits.emplace_back(i);
      }
    }
    assert(measureQubits.size() == bitString.size());

    for (int i = 0; i < count; ++i) {
      std::string newBitString;
      for (int j = 0; j < measureQubits.size(); ++j) {
        auto qubitIdx = measureQubits[j];
        const bool bit = (bitString[j] == '1');
        const auto roErrorProb = randomProbFunc();
        const auto [meas0Prep1, meas1Prep0] =
            in_noiseModel.readoutError(qubitIdx);
        const double flipProb = bit ? meas0Prep1 : meas1Prep0;
        const bool measBit = (roErrorProb < flipProb) ? !bit : bit;
        newBitString.push_back(measBit ? '1' : '0');
      }
      assert(newBitString.size() == bitString.size());
      buffer->appendMeasurement(newBitString);
    }
  };

  if (result.attr("value").is_none()) {
    auto iter = pybind11::iter(result);
    while (iter != pybind11::iterator::sentinel()) {
      auto sampleData = *iter;
      auto bitStr = pybind11::str(sampleData.attr("state")).cast<std::string>();
      bitStr = bitStr.substr(1, bitStr.size() - 2);
      auto bitStrProb = sampleData.attr("probability").cast<double>();
      int count = std::round(bitStrProb * m_shots);
      if (m_noiseModel) {
        applyReadoutError(bitStr, count, *m_noiseModel);
      } else {
        buffer->appendMeasurement(bitStr, count);
      }
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
  // No shots, retrieve the density matrix
  if (m_noiseModel && m_shots < 0) {
    auto compute_density_matrix =
        pybind11::module::import("qat.noisy").attr("compute_density_matrix");
    auto qlmJob = constructQlmJob(buffer, compositeInstruction);
    auto result = [&]() {
      if (!m_remoteAccess) {
        return m_qlmQpuServer.attr("submit")(qlmJob);
      } else {
        auto asynchronous_result = m_qlmQpuServer.attr("submit")(qlmJob);
        // wait
        auto sync_result = asynchronous_result.attr("join")();
        return sync_result;
      }
    }();
    persistResultToBuffer(buffer, result, qlmJob);
    auto circ = constructQlmCirc(buffer, compositeInstruction);
    pybind11::array_t<std::complex<double>> rho =
        compute_density_matrix(circ, m_qlmQpuServer);
    // std::cout << "Density matrix:\n";
    // pybind11::print(rho);
    auto rho_mat = rho.unchecked<2>();
    // Flatten density matrix
    std::vector<std::pair<double, double>> dm_mat;
    for (pybind11::ssize_t i = 0; i < rho_mat.shape(0); ++i) {
      for (pybind11::ssize_t j = 0; j < rho_mat.shape(1); ++j) {
        dm_mat.emplace_back(
            std::make_pair(rho_mat(i, j).real(), rho_mat(i, j).imag()));
      }
    }
    buffer->addExtraInfo("density_matrix", dm_mat);
  } else {
    auto qlmJob = constructQlmJob(buffer, compositeInstruction);
    // pybind11::print(qlmJob);
    auto result = [&]() {
      if (!m_remoteAccess) {
        return m_qlmQpuServer.attr("submit")(qlmJob);
      } else {
        auto asynchronous_result = m_qlmQpuServer.attr("submit")(qlmJob);
        // wait
        auto sync_result = asynchronous_result.attr("join")();
        return sync_result;
      }
    }();
    // pybind11::print(result);
    persistResultToBuffer(buffer, result, qlmJob);
  }
}

void QlmAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  std::vector<std::shared_ptr<AcceleratorBuffer>> childBuffers;
  std::vector<pybind11::object> jobs;
  for (auto &f : compositeInstructions) {
    childBuffers.emplace_back(
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size()));
    jobs.emplace_back(constructQlmJob(buffer, f));
  }

  auto batch = pybind11::module::import("qat.core").attr("Batch")(jobs);
  // Submit the whole batch:
  auto batchResult = [&]() {
    if (!m_remoteAccess) {
      return m_qlmQpuServer.attr("submit")(batch);
    } else {
      auto asynchronous_result = m_qlmQpuServer.attr("submit")(batch);
      // wait
      auto sync_result = asynchronous_result.attr("join")();
      return sync_result;
    }
  }();

  // pybind11::print(batchResult);
  auto iter = pybind11::iter(batchResult);
  int childBufferIndex = 0;
  while (iter != pybind11::iterator::sentinel()) {
    auto result = (*iter).cast<pybind11::object>();
    persistResultToBuffer(childBuffers[childBufferIndex], result,
                          jobs[childBufferIndex]);
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

REGISTER_ACCELERATOR(xacc::quantum::QlmAccelerator)
