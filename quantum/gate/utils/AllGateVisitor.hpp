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
                       public InstructionVisitor<Rphi>,
                       public InstructionVisitor<XX>,
                       public InstructionVisitor<IfStmt>,
                       public InstructionVisitor<XY>,
                       public InstructionVisitor<Reset>,
                       public InstructionVisitor<RZZ> {
public:
  void visit(RZZ &rzz) override {}
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

  void visit(Measure &m) override {}
  void visit(Identity &i) override {}
  void visit(U &u) override {}

  void visit(Rphi &r) override {
    double phi = InstructionParameterToDouble(r.getParameter(0));
    double theta = InstructionParameterToDouble(r.getParameter(1));

    // Trick to rotate around azimuthal angle phi by theta
    Rz rz1(r.bits()[0], phi);
    Rx rx(r.bits()[0], theta);
    Rz rz2(r.bits()[0], -phi);

    visit(rz1);
    visit(rx);
    visit(rz2);
  }

  void visit(XX &xx) override {
    double alpha = InstructionParameterToDouble(xx.getParameter(0));
    std::size_t left = xx.bits()[0];
    std::size_t right = xx.bits()[1];

    // Suppose CRx(α) = |0><0|⊗ I + |1><1|⊗ Rx(α), where Rx(α) is
    // defined in Section 4.2 of Mike & Ike. Then based on the
    // Controlled-X decomposition from [1] and Controlled-sqrt(X)
    // decomposition shown in the Methods section of [2], we can
    // generalize and see that a controlled-Rx can be decomposed in
    // terms of XX like this:
    //
    // CRx(α) = (Rz(-α/2)Ry(-π/2)Rx(α/2)⊗ Rx(α/2)) XX(-α/4) (Ry(π/2)⊗ I)
    //
    // Then, if we rearrange this equation, we can instead decompose XX
    // in terms of CRx:
    //
    // XX(α) = (Rx(2α)Ry(π/2)Rz(-2α)⊗ Rx(2α)) CRx(-4α) (Ry(-π/2)⊗ I))
    //
    // However, CRx is not in XACC IR. So given that Rx is a special
    // unitary and Rx(θ) = Rz(-π/2)Ry(θ)Rz(π/2), we can use Lemma 4.3 of [3]
    // to decompose CRx into CNOTs, Ry, and Rz as follows:
    //
    // CRx(θ) = (I⊗ Rz(-π/2)Ry(θ/2)) CNOT (I⊗ Ry(-θ/2)) CNOT (I⊗ Rz(π/2))
    //
    // Then we substitute this into the earlier definition to find the
    // decomposition we use below:
    //
    // XX(α) = (Rx(2α)Ry(π/2)Rz(-2α)⊗ Rx(2α)Rz(-π/2)Ry(-2*α)) CNOT (I⊗ Ry(2α)) CNOT (Ry(-π/2)⊗ Rz(π/2))
    //
    // [1]: https://doi.org/10.1088/1367-2630/aa5e47
    // [2]: https://www.nature.com/articles/s41586-019-1427-5
    // [3]: https://arxiv.org/abs/quant-ph/9503016
    Ry left1(left, -M_PI/2.0);
    Rz right1(right, M_PI/2.0);
    CNOT cx1(left, right);
    Ry right2(right, 2*alpha);
    CNOT cx2(left, right);
    Rz left2(left, -2.0*alpha);
    Ry left3(left, M_PI/2.0);
    Rx left4(left, 2.0*alpha);
    Ry right3(right, -2.0*alpha);
    Rz right4(right, -M_PI/2.0);
    Rx right5(right, 2.0*alpha);

    visit(left1);
    visit(right1);
    visit(cx1);
    visit(right2);
    visit(cx2);
    visit(left2);
    visit(left3);
    visit(left4);
    visit(right3);
    visit(right4);
    visit(right5);
  }

  void visit(Sdg &sdg) override {}
  void visit(T &t) override {}
  void visit(Tdg &tdg) override {}
  void visit(IfStmt &tdg) override {}
  void visit(Reset &reset) override {}
};
} // namespace quantum
} // namespace xacc
#endif
