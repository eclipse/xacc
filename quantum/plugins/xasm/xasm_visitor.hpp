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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_XASMVISITOR_HPP_
#define QUANTUM_GATE_XASMVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

namespace xacc {
namespace quantum {

class XasmVisitor : public AllGateVisitor {
protected:
  std::stringstream ss;

public:
  XasmVisitor() {}

  void visit(Hadamard &h) {
    std::string qubit = std::to_string(h.bits()[0]);
    ss << "H(q[" << qubit << "]);\n";
  }

  void visit(Identity &i) {
    std::string qubit = std::to_string(i.bits()[0]);
    ss << "I(q[" << qubit << "]);\n";
  }

  void visit(CZ &cz) {
    std::string q1 = std::to_string(cz.bits()[0]);
    std::string q2 = std::to_string(cz.bits()[1]);
    ss << "Cz(q[" << q1 << "],q[" << q2 << "]);\n";
  }

  void visit(CNOT &cn) {
    std::string q1 = std::to_string(cn.bits()[0]);
    std::string q2 = std::to_string(cn.bits()[1]);
    ss << "CX(q[" << q1 << "],q[" << q2 << "]);\n";
  }

  void visit(X &x) {
    std::string qubit = std::to_string(x.bits()[0]);
    ss << "X(q[" << qubit << "]);\n";
  }
  void visit(Y &y) {
    std::string qubit = std::to_string(y.bits()[0]);
    ss << "Y(q[" << qubit << "]);\n";
  }

  void visit(Z &z) {
    std::string qubit = std::to_string(z.bits()[0]);
    ss << "Z(q[" << qubit << "]);\n";
  }

  void visit(Measure &m) { ss << "Measure(q[" << m.bits()[0] << "]);\n"; }

  void visit(Rx &rx) {
    std::string qubit = std::to_string(rx.bits()[0]);
    ss << "Rx(q[" << qubit << "], " << rx.getParameter(0).toString()
       << ");\n";
  }

  void visit(Ry &ry) {
    std::string qubit = std::to_string(ry.bits()[0]);
    ss << "Ry(q[" << qubit << "], " << ry.getParameter(0).toString()
       << ");\n";
  }

  void visit(Rz &rz) {
    std::string qubit = std::to_string(rz.bits()[0]);
    ss << "Rz(q[" << qubit << "], " << rz.getParameter(0).toString()
       << ");\n";
  }

  void visit(CPhase &cp) {
    std::string q1 = std::to_string(cp.bits()[0]);
    std::string q2 = std::to_string(cp.bits()[1]);
    auto angleStr = cp.getParameter(0).toString();
    ss << "CPhase(q[" << q1 << "], q[" << q2 << "]," << angleStr << ");\n";
  }

  void visit(Swap &s) {
    CNOT cn1(s.bits());
    CNOT cn2(s.bits());
    CNOT cn3(s.bits());
    visit(cn1);
    visit(cn2);
    visit(cn3);
  }

  void visit(U &u) {
    auto t = u.getParameter(0).toString();
    auto p = u.getParameter(1).toString();
    auto l = u.getParameter(2).toString();

    Rz rz(u.bits()[0], t);
    Ry ry(u.bits()[0], p);
    Rz rz2(u.bits()[0], l);
    visit(rz);
    visit(ry);
    visit(rz2);
  }

  std::string getXasmString() { return ss.str(); }
  virtual ~XasmVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
