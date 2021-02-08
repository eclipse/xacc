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
#ifndef PYTHON_PYXACCVISITOR_HPP_
#define PYTHON_PYXACCVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

namespace xacc {
namespace quantum {

class PyXACCVisitor : public AllGateVisitor {
protected:
  std::string pyxaccStr;

  std::stringstream ss;
  std::string qreg_name = "q";
  std::string tab_prepend = "";

  void base_gate_impl(Instruction &inst) {
    ss << tab_prepend;
    ss << inst.name() << "(";
    ss << qreg_name << "[" << inst.bits()[0] << "]";
    for (int i = 1; i < inst.bits().size(); i++) {
      ss << ", " << qreg_name << "[" << inst.bits()[i] << "]";
    }
    if (inst.isParameterized()) {
      ss << ", " << inst.getParameter(0).toString();
      for (int i = 1; i < inst.nParameters(); i++) {
        ss << ", " << inst.getParameter(i).toString();
      }
    }
    ss << ")\n";
  }

public:
  PyXACCVisitor() = default;

  PyXACCVisitor(const std::string &_qreg_name, const std::string &_tab_prepend)
      : qreg_name(_qreg_name), tab_prepend(_tab_prepend) {}

  void visit(Hadamard &h) override { base_gate_impl(h); }

  void visit(CZ &cz) override { base_gate_impl(cz); }

  void visit(CNOT &cn) override { base_gate_impl(cn); }

  void visit(X &x) override { base_gate_impl(x); }

  void visit(Y &y) override { base_gate_impl(y); }

  void visit(Z &z) override { base_gate_impl(z); }

  void visit(Measure &m) override {
    ss << tab_prepend;
    ss << m.name() << "(";
    ss << qreg_name << "[" << m.bits()[0] << "]";
    for (int i = 1; i < m.bits().size(); i++) {
      ss << ", " << qreg_name << "[" << m.bits()[i] << "]";
    }
    ss << ")\n";
  }

  void visit(Rx &rx) override { base_gate_impl(rx); }

  void visit(Ry &ry) override { base_gate_impl(ry); }

  void visit(Rz &rz) override { base_gate_impl(rz); }

  void visit(CPhase &cp) override { base_gate_impl(cp); }

  void visit(Swap &s) override { base_gate_impl(s); }

  void visit(U &u) override { base_gate_impl(u); }

  void visit(U1 &u1) override { base_gate_impl(u1); }
  void visit(CY &cy) override { base_gate_impl(cy); }
  void visit(fSim &fsim) override { base_gate_impl(fsim); }
  void visit(iSwap &isw) override { base_gate_impl(isw); }
  void visit(XY &xy) override { base_gate_impl(xy); }
  void visit(CRZ &crz) override { base_gate_impl(crz); }
  void visit(CH &ch) override { base_gate_impl(ch); }
  void visit(S &s) override { base_gate_impl(s); }
  void visit(Sdg &sdg) override { base_gate_impl(sdg); }
  void visit(T &t) override { base_gate_impl(t); }
  void visit(Tdg &tdg) override { base_gate_impl(tdg); }

  std::string getPyXACCString() { return ss.str(); }

  /**
   * The destructor
   */
  virtual ~PyXACCVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
