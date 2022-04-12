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
#pragma once
#include "xacc.hpp"
#include "AllGateVisitor.hpp"
#include "GateModifier.hpp"
using namespace xacc::quantum;

namespace xacc {
namespace circuits {

class ControlledU : public AllGateVisitor, public quantum::Circuit, public quantum::ControlModifier {
public:
  ControlledU() : Circuit("C-U") { m_expanded = false; }
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  // Input: The composite "U" and the control Idx.
  // Control Idx must *not* be one of the qubits that U is acting on.
  const std::vector<std::string> requiredKeys() override {
    return {"U", "control-idx"};
  }

  virtual std::shared_ptr<Instruction> getBaseInstruction() const override {
    if (!m_expanded) {
      xacc::error("Controlled modifier block not yet expanded.");
    }
    return m_originalU;
  }
  virtual std::vector<std::pair<std::string, size_t>>
  getControlQubits() const override {
    if (!m_expanded) {
      xacc::error("Controlled modifier block not yet expanded.");
    }
    return m_ctrlIdxs;
  }

  DEFINE_CLONE(ControlledU);

  // AllGateVisitor implementation
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
  void visit(U &u) override;
  void visit(U1 &u1) override;
  // Nothing to do
  void visit(Identity &i) override {}
  // These are unsupported.
  void visit(Measure &measure) override { xacc::error("Unsupported!"); }
  void visit(IfStmt &ifStmt) override { xacc::error("Unsupported!"); }
  void visit(fSim &fsim) override { xacc::error("Unsupported!"); }
  void visit(iSwap &isw) override { xacc::error("Unsupported!"); }

private:
  // Apply *single* control on the input composite.
  // Returns the new composite.
  std::shared_ptr<xacc::CompositeInstruction>
  applyControl(const std::shared_ptr<xacc::CompositeInstruction> &in_program,
               const std::pair<std::string, size_t> &in_ctrlIdx);

private:
  std::shared_ptr<xacc::CompositeInstruction> m_composite;
  std::shared_ptr<xacc::IRProvider> m_gateProvider;
  // The current control qubit (buffer name & index)
  std::pair<std::string, size_t> m_ctrlIdx;
  bool m_expanded;
  std::vector<std::pair<std::string, size_t>> m_ctrlIdxs;
  std::shared_ptr<Instruction> m_originalU;
};
} // namespace circuits
} // namespace xacc