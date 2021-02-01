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
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {

class QuilVisitor : public AllGateVisitor {
protected:

  std::string quilStr;
  constexpr static double pi = 3.1415926;

  std::string classicalAddresses;

  std::map<int, int> qubitToClassicalBitIndex;

  std::vector<int> measuredQubits;

  bool includeMeasures = true;

  int numAddresses = 0;

  bool retain_xacc_gates = false;
public:
  QuilVisitor() {}
  QuilVisitor(bool measures) : includeMeasures(measures) {}
  QuilVisitor(bool measures, bool retain) : includeMeasures(measures), retain_xacc_gates(retain) {}

  void visit(Hadamard &h) {
    std::string qubit = std::to_string(h.bits()[0]);
    if (retain_xacc_gates) {
      quilStr += "H " + qubit + "\n";
    } else {
      Rz rz1(h.bits()[0], xacc::constants::pi/2.);
      Rx rx1(h.bits()[0], xacc::constants::pi/2.);
      Rz rz2(h.bits()[0], xacc::constants::pi/2.);
      
      visit(rz1);
      visit(rx1);
      visit(rz2);
    }
  }

  void visit(Identity &i) {
    std::string qubit = std::to_string(i.bits()[0]);
    quilStr += "I " + qubit + "\n";
  }

  void visit(CZ &cz) {
    std::string q1 = std::to_string(cz.bits()[0]);
    std::string q2 = std::to_string(cz.bits()[1]);
    quilStr += "CZ " + q1 + " " +
               q2 + "\n";
  }

  void visit(CNOT &cn) {
    std::string q1 = std::to_string(cn.bits()[0]);
    std::string q2 = std::to_string(cn.bits()[1]);
    if (retain_xacc_gates) {
      quilStr += "CNOT " + q1 + " " + q2 + "\n";
    } else {
      Rz rz1(cn.bits()[1], -xacc::constants::pi/2.);
      Rx rx1(cn.bits()[1], xacc::constants::pi/2.);
      CZ cz(cn.bits()[1], cn.bits()[0]);
      Rz rz2(cn.bits()[0], xacc::constants::pi);
      Rx rx2(cn.bits()[1], -xacc::constants::pi/2.);
      Rz rz3(cn.bits()[1], xacc::constants::pi/2.);
      
      visit(rz1);
      visit(rx1);
      visit(cz);
      visit(rz2);
      visit(rx2);
      visit(rz3);
    }
  }

  void visit(X &x) {
      std::string qubit = std::to_string(x.bits()[0]);
      auto angleStr = std::to_string(pi);
      quilStr += "RX(pi) " + qubit + "\n"; }

  void visit(Y &y) {
      std::string qubit = std::to_string(y.bits()[0]);
      quilStr += "RZ(pi) " + qubit + "\nRX(pi) " + qubit + "\n"; }

  void visit(Z &z) {
      std::string qubit = std::to_string(z.bits()[0]);
      quilStr += "RZ(pi) " + qubit + "\n"; }
  int countMeasures = 0;
  void visit(Measure &m) {
    if (includeMeasures) {
      int classicalBitIdx = m.getClassicalBitIndex();
      quilStr += "MEASURE " + std::to_string(m.bits()[0]) + " ro[" +
                 std::to_string(countMeasures) + "]\n";
     countMeasures++;
      classicalAddresses += std::to_string(classicalBitIdx) + ", ";
      numAddresses++;
      qubitToClassicalBitIndex.insert(
          std::make_pair(m.bits()[0], classicalBitIdx));
      measuredQubits.push_back(m.bits()[0]);
    } else {
      measuredQubits.push_back(m.bits()[0]);
    }
  }

  void visit(Rx &rx) {
    std::string qubit = std::to_string(rx.bits()[0]);
    auto angleStr = rx.getParameter(0).toString();

    auto angleDouble = rx.getParameter(0).as<double>();
    
    if (retain_xacc_gates) {
      quilStr += "RX("+angleStr+") " + qubit + "\n";
    } else {
      if (std::fabs(std::fmod(angleDouble, xacc::constants::pi)) < 1e-6 || std::fabs(std::fmod(angleDouble,xacc::constants::pi/2.)) < 1e-6) {
        quilStr += "RX("+angleStr+") " + qubit + "\n";
      } else {
        // decompose into rigetti gate set
        quilStr += "RZ(pi/2) " + qubit + "\nRX(pi/2) " + qubit +"\nRZ(" + angleStr + ") " + qubit + "\nRX(-pi/2) " + qubit + "\nRZ(-pi/2) "+ qubit + "\n";
      }
    }
  }

  void visit(Ry &ry) {
    auto angleStr = ry.getParameter(0).toString();
    std::string qubit = std::to_string(ry.bits()[0]);
      
    if (retain_xacc_gates) {
        quilStr += "RY("+angleStr+") " + qubit + "\n";
    } else {
      Rx rx1(ry.bits()[0], xacc::constants::pi/2.);
      Rz rz1(ry.bits()[0], ry.getParameter(0).as<double>());
      Rx rx2(ry.bits()[0], -xacc::constants::pi/2.);
      
      visit(rx1);
      visit(rz1);
      visit(rx2);
    }
      
  }

  void visit(Rz &rz) {
    std::string qubit = std::to_string(rz.bits()[0]);
    auto angleStr = rz.getParameter(0).toString();
    quilStr += "RZ(" + angleStr + ") " + qubit + "\n";
  }

  void visit(CPhase &cp) {
    std::string q1 = std::to_string(cp.bits()[0]);
    std::string q2 = std::to_string(cp.bits()[1]);
    auto angleStr = cp.getParameter(0).toString();
    quilStr += "CPHASE(" + angleStr + ") " + q1 +
               " " + q2 + "\n";
  }

  void visit(Swap &s) {
    CNOT cn1 (s.bits());
    CNOT cn2 (s.bits());
    CNOT cn3 (s.bits());
    visit(cn1);
    visit(cn2);
    visit(cn3);
    // std::string q1 = std::to_string(s.bits()[0]);
    // std::string q2 = std::to_string(s.bits()[1]);
    // quilStr += "SWAP " + q1 + " " +
    //            q2 + "\n";
  }

 void visit(U& u) {
    auto t = u.getParameter(0).toString();
    auto p = u.getParameter(1).toString();
    auto l = u.getParameter(2).toString();

    Rz rz(u.bits()[0], t);
    Ry ry(u.bits()[0], p);
    Rz rz2 (u.bits()[0], l);
    visit(rz);
    visit(ry);
    visit(rz2);

    // std::stringstream s;
    // s << "   Rz(" << t << ", " << u.bits()[0] << ")\n";
    // s << "   Ry(" << p << ", " << u.bits()[0] << ")\n";
    // s << "   Rz(" << l << ", " << u.bits()[0] << ")\n";

    // quilStr += s.str();
  }

  void visit(XY& xy) override {
    auto angleStr = xy.getParameter(0).toString();
    std::string ctrl = std::to_string(xy.bits()[0]);
    std::string tgt = std::to_string(xy.bits()[1]);
    quilStr += "XY("+angleStr+") " +ctrl + " " + tgt + "\n";
  }

  /**
   * Return the quil string
   */
  std::string getQuilString() { return quilStr; }

  /**
   * Return the classical measurement indices
   * as a json int array represented as a string.
   */
  std::string getClassicalAddresses() {
    auto retStr = classicalAddresses.substr(0, classicalAddresses.size() - 2);
    return "[" + retStr + "]";
  }

  int getNumberOfAddresses() { return numAddresses; }

  std::vector<int> getMeasuredQubits() { return measuredQubits; }

  /**
   * The destructor
   */
  virtual ~QuilVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
