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
#pragma one
#include "xacc.hpp"
#include "AllGateVisitor.hpp"
#include <pybind11/embed.h>

namespace xacc {
class NoiseModel;

namespace quantum {
class QlmCircuitVisitor : public AllGateVisitor {
public:
  QlmCircuitVisitor(size_t nbQubit);
  void visit(Hadamard &h) override;
  void visit(CNOT &cnot) override;
  void visit(Rz &rz) override;
  void visit(Ry &ry) override;
  void visit(Rx &rx) override;
  void visit(X &x) override;
  void visit(Y &y) override;
  void visit(Z &z) override;
  void visit(CY &cy) override;
  void visit(CZ &cz) override;
  void visit(Swap &s) override;
  void visit(CRZ &crz) override;
  void visit(CH &ch) override;
  void visit(S &s) override;
  void visit(Sdg &sdg) override;
  void visit(T &t) override;
  void visit(Tdg &tdg) override;
  void visit(CPhase &cphase) override;
  void visit(Identity &i) override {}
  void visit(U &u) override;
  void visit(iSwap &in_iSwapGate) override;
  void visit(fSim &in_fsimGate) override;
  void visit(IfStmt &ifStmt) override {}
  void visit(Measure &measure) override;
  pybind11::object &getProgram() { return m_aqasmProgram; }

private:
  pybind11::object createProgram();
  size_t m_nbQubit;
  pybind11::module m_aqasm;
  pybind11::object m_aqasmProgram;
  pybind11::object m_qreg;
  pybind11::object m_creg;
};

class QlmAccelerator : public Accelerator {
public:
  // Identifiable interface impls
  virtual const std::string name() const override { return "atos-qlm"; }
  virtual const std::string description() const override {
    return "XACC Simulation Accelerator for ATOS QLM.";
  }

  // Accelerator interface impls
  virtual void initialize(const HeterogeneousMap &params = {}) override;
  virtual void updateConfiguration(const HeterogeneousMap &config) override {
    initialize(config);
  }
  virtual const std::vector<std::string> configurationKeys() override {
    return {};
  }
  
  virtual std::vector<std::pair<int, int>> getConnectivity() override;

  virtual BitOrder getBitOrder() override { return BitOrder::MSB; }
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction>
                           compositeInstruction) override;
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           compositeInstructions) override;
  virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer,
                     std::shared_ptr<Instruction> inst) override{};

private:
  pybind11::object constructQlmJob(
      std::shared_ptr<AcceleratorBuffer> buffer,
      std::shared_ptr<CompositeInstruction> compositeInstruction) const;
  pybind11::object constructQlmCirc(
      std::shared_ptr<AcceleratorBuffer> buffer,
      std::shared_ptr<CompositeInstruction> compositeInstruction) const;
  void persistResultToBuffer(std::shared_ptr<AcceleratorBuffer> buffer,
                             pybind11::object &result,
                             pybind11::object &job) const;

private:
  int m_shots;
  pybind11::object m_qlmQpuServer;
  std::shared_ptr<NoiseModel> m_noiseModel;
  bool m_remoteAccess;
  pybind11::object m_qlmaasConnection;
};
} // namespace quantum
} // namespace xacc
