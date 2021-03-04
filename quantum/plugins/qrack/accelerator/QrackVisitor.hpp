/*******************************************************************************
 * Copyright (c) 2019-2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *******************************************************************************/

#pragma once
#include "Identifiable.hpp"
#include "AllGateVisitor.hpp"
#include "AcceleratorBuffer.hpp"
#include "OptionsProvider.hpp"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_TARGET_OPENCL_VERSION 200
#endif

#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#endif

#include "qrack/qfactory.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {
class QrackVisitor : public AllGateVisitor, public OptionsProvider, public xacc::Cloneable<QrackVisitor> {
public:
  void initialize(std::shared_ptr<AcceleratorBuffer> buffer, int shots, bool use_opencl, bool use_qunit, bool use_opencl_multi, bool use_stabilizer, int device_id, bool doNormalize, double zero_threshold);
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

  virtual std::shared_ptr<QrackVisitor> clone() { return std::make_shared<QrackVisitor>(); }

private:
  std::shared_ptr<AcceleratorBuffer> m_buffer; 
  Qrack::QInterfacePtr m_qReg;
  std::vector<bitLenInt> m_measureBits;
  int m_shots;
  // If true, it will perform a trajectory simulation and return the bit string of measurement results.
  bool m_shotsMode;
  std::string m_bitString;

  double calcExpectationValueZ() const;
  std::map<bitCapInt, int> measure_shots();
};
}}
