/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_ALLGATEVISITORTESTER_HPP_
#define QUANTUM_ALLGATEVISITORTESTER_HPP_

#include "AllGateVisitor.hpp"

namespace xacc {
namespace quantum {

class TestAllGateVisitor : public AllGateVisitor {

public:

  bool foundH = false;
  bool foundI = false;
  bool foundCz = false;
  bool foundCX = false;
  bool foundX = false;
  bool foundY = false;
  bool foundZ = false;
  bool foundM = false;
  bool foundRx = false;
  bool foundRy = false;
  bool foundRz = false;
  bool foundCp = false;
  bool foundSw = false;
  bool foundU = false;

  void visit(Hadamard &h) { foundH=true; }

  void visit(Identity &i) {foundI = true;}

  void visit(CZ &cz) { foundCz = true;}

  void visit(CNOT &cn) { foundCX = true; }

  void visit(X &x) { foundX = true; }

  void visit(Y &y) { foundY = true; }

  void visit(Z &z) { foundZ = true; }

  void visit(Measure &m) { foundM = true;  }

  void visit(ConditionalFunction &c) {
    // nothing
  }

  void visit(Rx &rx) { foundRx = true; }

  void visit(Ry &ry) {foundRy = true; }

  void visit(Rz &rz) { foundRz = true; }

  void visit(CPhase &cp) {foundCp = true; }

  void visit(Swap &s) { foundSw = true; }
 
  void visit(U &u) {foundU = true;}
  
  void visit(GateFunction &f) {
    // nothing
  }

  virtual ~TestAllGateVisitor() {}
};


class Unknown : public GateInstruction {
public:
  Unknown() : GateInstruction("UNKNOWN") {}
  Unknown(std::vector<int> qbits) : GateInstruction("H", qbits) {}
  Unknown(int qbit) : Unknown(std::vector<int>{qbit}) {}

  void customVisitAction(BaseInstructionVisitor& iv) override {
    xacc::info("Called custom visitor action!");
    iv.getNativeAssembly() += "hello";
    wasVisited = true;
  }
    const int nRequiredBits() const override {return 1;}

  bool wasVisited = false;
  
  DEFINE_CLONE(Unknown)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc

#endif