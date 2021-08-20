/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
#include "staq_visitors.hpp"
#include "Instruction.hpp"

namespace xacc {
namespace internal_staq {
XACCToStaqOpenQasm::XACCToStaqOpenQasm(std::map<std::string, int> bufNamesToSize) {
  ss << "OPENQASM 2.0;\ninclude \"qelib1.inc\";\n";
  for (auto &kv : bufNamesToSize) {
    ss << "qreg " << kv.first << "[" << kv.second << "];\n";
    cregNames.insert({kv.first, kv.first+"_c"});
    ss << "creg " << kv.first+"_c" << "[" << kv.second << "];\n";
  }
}

XACCToStaqOpenQasm::XACCToStaqOpenQasm(
    std::map<std::string, int> bufNamesToSize,
    std::map<std::string, int> cRegNameToSize) {
  ss << "OPENQASM 2.0;\ninclude \"qelib1.inc\";\n";
  for (auto &kv : bufNamesToSize) {
    ss << "qreg " << kv.first << "[" << kv.second << "];\n";
  }
  for (auto &kv : cRegNameToSize) {
    ss << "creg " << kv.first << "[" << kv.second << "];\n";
  }
}

void XACCToStaqOpenQasm::visit(Hadamard &h) {
  ss << "h " << (h.getBufferNames().empty() ? "q" : h.getBufferName(0))
     << h.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(CNOT &cx) {
  ss << "CX " << (cx.getBufferNames().empty() ? "q" : cx.getBufferName(0))
     << "[" << cx.bits()[0] << "], " << (cx.getBufferNames().empty() ? "q" : cx.getBufferName(1)) << "[" << cx.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(Rz &rz) {
  ss << "rz(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(rz.getParameter(0)) << ") " << (rz.getBufferNames().empty() ? "q" : rz.getBufferName(0))
     << rz.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Ry &ry) {
  ss << "ry(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(ry.getParameter(0)) << ") " << (ry.getBufferNames().empty() ? "q" : ry.getBufferName(0))
     << ry.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Rx &rx) {
  ss << "rx(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(rx.getParameter(0)) << ") " << (rx.getBufferNames().empty() ? "q" : rx.getBufferName(0))
     << rx.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(X &x) {
  ss << "x " << (x.getBufferNames().empty() ? "q" : x.getBufferName(0))
     << x.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Y &y) {
  ss << "y " << (y.getBufferNames().empty() ? "q" : y.getBufferName(0))
     << y.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Z &z) {
  ss << "z " << (z.getBufferNames().empty() ? "q" : z.getBufferName(0))
     << z.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(CY &cy) {
  ss << "cy " << (cy.getBufferNames().empty() ? "q" : cy.getBufferName(0))
     << "[" << cy.bits()[0] << "], " << (cy.getBufferNames().empty() ? "q" : cy.getBufferName(1)) << "[" << cy.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(CZ &cz) {
  ss << "cz " << (cz.getBufferNames().empty() ? "q" : cz.getBufferName(0))
     << "[" << cz.bits()[0] << "], " << (cz.getBufferNames().empty() ? "q" : cz.getBufferName(1)) << "[" << cz.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(Swap &s) {
  ss << "swap " << (s.getBufferNames().empty() ? "q" : s.getBufferName(0))
     << "[" << s.bits()[0] << "], " << (s.getBufferNames().empty() ? "q" : s.getBufferName(1)) << "[" << s.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(CRZ &crz) {
  ss << "crz(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(crz.getParameter(0)) << ") " << (crz.getBufferNames().empty() ? "q" : crz.getBufferName(0))
     << "[" << crz.bits()[0] << "], " << (crz.getBufferNames().empty() ? "q" : crz.getBufferName(1)) << "[" << crz.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(CH &ch) {
  ss << "ch " << (ch.getBufferNames().empty() ? "q" : ch.getBufferName(0))
     << "[" << ch.bits()[0] << "], " << (ch.getBufferNames().empty() ? "q" : ch.getBufferName(1)) << "[" << ch.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(S &s) {
  ss << "s " << (s.getBufferNames().empty() ? "q" : s.getBufferName(0))
     << s.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Sdg &sdg) {
  ss << "sdg " << (sdg.getBufferNames().empty() ? "q" : sdg.getBufferName(0))
     << sdg.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(T &t) {
  ss << "t " << (t.getBufferNames().empty() ? "q" : t.getBufferName(0))
     << t.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(Tdg &tdg) {
  ss << "tdg " << (tdg.getBufferNames().empty() ? "q" : tdg.getBufferName(0))
     << tdg.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(CPhase &cphase) {
  ss << "cu1(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(cphase.getParameter(0)) << ") " << (cphase.getBufferNames().empty() ? "q" : cphase.getBufferName(0))
     << "[" << cphase.bits()[0] << "], " << (cphase.getBufferNames().empty() ? "q" : cphase.getBufferName(1)) << "[" << cphase.bits()[1] << "];\n";
}
void XACCToStaqOpenQasm::visit(Measure &m) {
  if (m.hasClassicalRegAssignment()) {
    ss << "measure " << (m.getBufferNames().empty() ? "q" : m.getBufferName(0))
       << m.bits() << " -> " << m.getBufferName(1) << "["
       << m.getClassicalBitIndex() << "]"
       << ";\n";
  } else {
    ss << "measure " << (m.getBufferNames().empty() ? "q" : m.getBufferName(0))
       << m.bits() << " -> " << cregNames[m.getBufferName(0)] << m.bits()
       << ";\n";
  }
}
void XACCToStaqOpenQasm::visit(Identity &i) {}
void XACCToStaqOpenQasm::visit(U &u) {
    ss << "u3(" << std::fixed << std::setprecision(16) << xacc::InstructionParameterToDouble(u.getParameter(0)) << "," << xacc::InstructionParameterToDouble(u.getParameter(1)) << "," <<xacc::InstructionParameterToDouble(u.getParameter(2)) << ") " << (u.getBufferNames().empty() ? "q" : u.getBufferName(0)) << u.bits() << ";\n";
}
void XACCToStaqOpenQasm::visit(IfStmt &ifStmt) {
  // Note: QASM2 'if' must be inline (no braces)
  // Note: this extended syntax: if (creg[k] == 1) is not supported by IBM.
  // (IBM requires comparison to the full register value, not a bit check)
  for (auto i : ifStmt.getInstructions()) {
    // If statement was specified by the qreg name -> find the associated cReg
    if (cregNames.find(ifStmt.getBufferNames()[0]) != cregNames.end()) {
      ss << "if (" << cregNames[ifStmt.getBufferNames()[0]] << "["
         << ifStmt.bits()[0] << "] == 1) ";
    } else {
      // Explicit creg was used:
      ss << "if (" << ifStmt.getBufferNames()[0] << "[" << ifStmt.bits()[0]
         << "] == 1) ";
    }

    i->accept(this);
  }
}
} // namespace internal_staq
} // namespace xacc