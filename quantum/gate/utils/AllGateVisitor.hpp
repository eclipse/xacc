/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef QUANTUM_GATE_ALLGATEVISITOR_HPP_
#define QUANTUM_GATE_ALLGATEVISITOR_HPP_

#include "InstructionIterator.hpp"

#include "CommonGates.hpp"
#include <Instruction.hpp>

namespace xacc {
namespace quantum {

class AllGateVisitor : public BaseInstructionVisitor,
                       public InstructionVisitor<Hadamard>,
                       public InstructionVisitor<CNOT>,
                       public InstructionVisitor<Rz>,
                       public InstructionVisitor<Rx>,
                       public InstructionVisitor<Ry>,
                       public InstructionVisitor<X>,
                       public InstructionVisitor<Y>,
                       public InstructionVisitor<Z>,
                       public InstructionVisitor<CPhase>,
                       public InstructionVisitor<Swap>,
                       public InstructionVisitor<iSwap>,
                       public InstructionVisitor<fSim>,
                       public InstructionVisitor<Measure>,
                       public InstructionVisitor<Identity>,
                       public InstructionVisitor<CZ>,
                       public InstructionVisitor<CY>,
                       public InstructionVisitor<CRZ>,
                       public InstructionVisitor<CH>,
                       public InstructionVisitor<S>,
                       public InstructionVisitor<Sdg>,
                       public InstructionVisitor<T>,
                       public InstructionVisitor<Tdg>,
                       public InstructionVisitor<U>,
                       public InstructionVisitor<U1>,
                       public InstructionVisitor<IfStmt>,
                       public InstructionVisitor<XY>,
                       public InstructionVisitor<Reset> {
public:
  void visit(Hadamard &h) override {}
  void visit(CNOT &h) override {}
  void visit(Rz &h) override {}
  void visit(Ry &h) override {}
  void visit(Rx &h) override {}
  void visit(U1 &u1) override {
    InstructionParameter p = u1.getParameter(0);
    Rz rz(u1.bits());
    rz.setParameter(0, p);
    visit(rz);
  }
  void visit(X &h) override {}
  void visit(Y &h) override {}
  void visit(Z &h) override {}

  void visit(CY &cy) override {
    Sdg sdg(cy.bits()[1]);
    CNOT cn(cy.bits());
    S s(cy.bits()[1]);
    visit(sdg);
    visit(cn);
    visit(s);
  }

  void visit(CZ &cz) override {
    Hadamard h(cz.bits()[1]);
    CNOT cn(cz.bits());
    Hadamard h2(cz.bits()[1]);
    visit(h);
    visit(cn);
    visit(h2);
  }

  void visit(Swap &s) override {
    CNOT c1(s.bits()), c2(s.bits()[1], s.bits()[0]), c3(s.bits());
    visit(c1);
    visit(c2);
    visit(c3);
  }

  void visit(fSim &fsim) override {}
  void visit(iSwap &isw) override {}
  void visit(XY &xy) override {}
  void visit(CRZ &crz) override {}
  void visit(CH &ch) override {
    S s(ch.bits()[1]);
    Hadamard h(ch.bits()[1]);
    T t(ch.bits()[1]);
    CNOT cn(ch.bits());
    Tdg tdg(ch.bits()[1]);
    Hadamard h2(ch.bits()[1]);
    Sdg sdg(ch.bits()[1]);

    visit(s);
    visit(h);
    visit(t);
    visit(cn);
    visit(tdg);
    visit(h2);
    visit(sdg);
  }
  void visit(S &s) override {}
  void visit(CPhase &cp) override {}

  void visit(Measure &cp) override {}
  void visit(Identity &cp) override {}
  void visit(U &cp) override {}

  void visit(Sdg &sdg) override {}
  void visit(T &t) override {}
  void visit(Tdg &tdg) override {}
  void visit(IfStmt &tdg) override {}
  void visit(Reset &reset) override {}
};
} // namespace quantum
} // namespace xacc
#endif
