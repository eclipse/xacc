/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

namespace xacc {
namespace quantum {

/**
 * The QuilVisitor is an InstructionVisitor that visits
 * quantum gate instructions and creates an equivalent
 * Quil string that can be executed by the Rigetti
 * superconducting quantum computer.
 *
 */
class QuilVisitor : public AllGateVisitor {
protected:
  /**
   * Reference to the Quil string
   * this visitor is trying to construct
   */
  std::string quilStr;
  constexpr static double pi = 3.1415926;
  /**
   * Reference to the classical memory address indices
   * where measurements are recorded.
   */
  std::string classicalAddresses;

  std::map<int, int> qubitToClassicalBitIndex;

  std::vector<int> measuredQubits;

  bool includeMeasures = true;

  int numAddresses = 0;

public:
  QuilVisitor() {}
  QuilVisitor(bool measures) : includeMeasures(measures) {}

  /**
   * Visit hadamard gates
   */
  void visit(Hadamard &h) {
    std::string qubit = std::to_string(h.bits()[0]);
    quilStr += "RZ(pi/2) " + qubit + "\nRX(pi/2) " + qubit + "\nRZ(pi/2) " + qubit + "\n";
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

  /**
   * Visit CNOT gates
   */
  void visit(CNOT &cn) {
    std::string q1 = std::to_string(cn.bits()[0]);
    std::string q2 = std::to_string(cn.bits()[1]);
    quilStr += "RZ(-pi/2) " + q2 + "\nRX(pi/2) " + q2 + "\nCZ " + q2 + " " + q1 + "\nRX(-pi/2) " + q2 + "\nRZ(pi/2) " + q2 + "\nRZ(pi) "+ q1 + "\n";
  }
  /**
   * Visit X gates
   */
  void visit(X &x) {
      std::string qubit = std::to_string(x.bits()[0]);
      auto angleStr = std::to_string(pi);
      quilStr += "RX(pi) " + qubit + "\n"; }

  /**
   *
   */
  void visit(Y &y) {
      std::string qubit = std::to_string(y.bits()[0]);
      quilStr += "RZ(pi) " + qubit + "\nRX(pi) " + qubit + "\n"; }

  /**
   * Visit Z gates
   */
  void visit(Z &z) {
      std::string qubit = std::to_string(z.bits()[0]);
      quilStr += "RZ(pi) " + qubit + "\n"; }

  /**
   * Visit Measurement gates
   */
  void visit(Measure &m) {
    if (includeMeasures) {
      int classicalBitIdx = m.getClassicalBitIndex();
      quilStr += "MEASURE " + std::to_string(m.bits()[0]) + " ro[" +
                 std::to_string(classicalBitIdx) + "]\n";
      classicalAddresses += std::to_string(classicalBitIdx) + ", ";
      numAddresses++;
      qubitToClassicalBitIndex.insert(
          std::make_pair(m.bits()[0], classicalBitIdx));
      measuredQubits.push_back(m.bits()[0]);
    } else {
      measuredQubits.push_back(m.bits()[0]);
    }
  }

  /**
   * Visit Conditional functions
   */
//   void visit(ConditionalFunction &c) {
//     auto visitor = std::make_shared<QuilVisitor>();
//     auto classicalBitIdx = qubitToClassicalBitIndex[c.getConditionalQubit()];
//     quilStr += "JUMP-UNLESS @" + c.name() + " [" +
//                std::to_string(classicalBitIdx) + "]\n";
//     for (auto inst : c.getInstructions()) {
//       inst->accept(visitor);
//     }
//     quilStr += visitor->getQuilString();
//     quilStr += "LABEL @" + c.name() + "\n";
//   }

  void visit(Rx &rx) {
    std::string qubit = std::to_string(rx.bits()[0]);
    auto angleStr = rx.getParameter(0).toString();
    quilStr += "RX(" + angleStr + ") " + qubit + "\n";
  }

  void visit(Ry &ry) {
    auto angleStr = ry.getParameter(0).toString();
    std::string qubit = std::to_string(ry.bits()[0]);
    quilStr += "RX(pi/2) " + qubit + "\nRZ(" + angleStr + ") " + qubit + "\nRX(-pi/2) " + qubit + "\n";
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
    std::string q1 = std::to_string(s.bits()[0]);
    std::string q2 = std::to_string(s.bits()[1]);
    quilStr += "SWAP " + q1 + " " +
               q2 + "\n";
  }

 void visit(U& u) {
    auto t = u.getParameter(0).toString();
    auto p = u.getParameter(1).toString();
    auto l = u.getParameter(2).toString();

    std::stringstream s;
    s << "   Rz(" << t << ", " << u.bits()[0] << ")\n";
    s << "   Ry(" << p << ", " << u.bits()[0] << ")\n";
    s << "   Rz(" << l << ", " << u.bits()[0] << ")\n";

    quilStr += s.str();
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
