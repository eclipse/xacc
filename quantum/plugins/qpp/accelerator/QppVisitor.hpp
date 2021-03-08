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
#include "Identifiable.hpp"
#include "AllGateVisitor.hpp"
#include "AcceleratorBuffer.hpp"
#include "OptionsProvider.hpp"
#include "qpp.h"

using namespace xacc;
using KetVectorType = qpp::ket;

namespace xacc {
namespace quantum {
class QppVisitor : public AllGateVisitor, public OptionsProvider, public xacc::Cloneable<QppVisitor> {
public:
  void initialize(std::shared_ptr<AcceleratorBuffer> buffer, bool shotsMode = false);
  void finalize();

  void visit(Hadamard& h) override;
  void visit(CNOT& cnot) override;
  void visit(Rz& rz) override;
  void visit(Ry& ry) override;
  void visit(Rx& rx) override;
  void visit(X& x) override;
  void visit(Y& y) override;
  void visit(Z& z) override;
  void visit(CY& cy) override;
  void visit(CZ& cz) override;
  void visit(Swap& s) override;
  void visit(CRZ& crz) override;
  void visit(CH& ch) override;
  void visit(S& s) override;
  void visit(Sdg& sdg) override;
  void visit(T& t) override;
  void visit(Tdg& tdg) override;
  void visit(CPhase& cphase) override;
  void visit(Measure& measure) override;
  void visit(Identity& i) override;
  void visit(U& u) override;
  void visit(IfStmt& ifStmt) override;
  void visit(iSwap& in_iSwapGate) override;
  void visit(fSim& in_fsimGate) override;
  void visit(Reset& in_resetGate) override;
  virtual std::shared_ptr<QppVisitor> clone() override { return std::make_shared<QppVisitor>(); }
  const KetVectorType& getStateVec() const { return m_stateVec; }
  static double calcExpectationValueZ(const KetVectorType& in_stateVec, const std::vector<qpp::idx>& in_bits);
  double getExpectationValueZ(std::shared_ptr<CompositeInstruction> in_composite);

  // Gate-by-gate API (FTQC)
  void applyGate(Gate& in_gate);
  bool measure(size_t in_bit);
  bool isInitialized() const { return m_initialized; }
  // Allocate more qubits (zero state)
  void allocateQubits(size_t in_nbQubits);
private:
  qpp::idx xaccIdxToQppIdx(size_t in_idx) const;
private:
  std::shared_ptr<AcceleratorBuffer> m_buffer;  
  std::vector<qpp::idx> m_dims;
  KetVectorType m_stateVec;
  std::vector<qpp::idx> m_measureBits;
  // If true, it will perform a trajectory simulation and return the bit string of measurement results.
  // Otherwise, it will only compute the expectation value.
  bool m_shotsMode;
  std::string m_bitString;
  bool m_initialized = false;
};
}}
