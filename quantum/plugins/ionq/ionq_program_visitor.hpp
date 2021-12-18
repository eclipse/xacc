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
#ifndef QUANTUM_GATE_ACCELERATORS_IONQPROGRAMVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_IONQPROGRAMVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "ionq_program.hpp"
#include "xacc.hpp"

using namespace xacc::ionq;

namespace xacc {
namespace quantum {
class IonQProgramVisitor : public AllGateVisitor {
protected:
  constexpr static double pi = xacc::constants::pi;

  std::vector<xacc::ionq::CircuitInstruction> instructions;
  //   Experiment experiment;
  //   std::string experimentName;
  //   int nTotalQubits = 0;
  //   std::vector<int> usedMemorySlots;

public:
  //   int maxMemorySlots = 0;
  //   std::map<int, int> qubit2MemorySlot;

  const std::string name() const override { return "ionq-visitor"; }

  const std::string description() const override {
    return "Map XACC IR to IonQ Program.";
  }

  IonQProgramVisitor() {}

  const std::vector<xacc::ionq::CircuitInstruction> getCircuitInstructions() {
    return instructions;
  }

  void visit(Hadamard &h) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("h");
    inst.set_target(h.bits()[0]);

    instructions.push_back(inst);
  }

  void visit(Identity &i) override {
    // std::stringstream js;
    // native += "id q[" + std::to_string(i.bits()[0]) + "];\n";
    // js << "{\"name\":\"id\",\"params\": [],\"qubits\":[" <<
    // i.bits()[0]<<"]},"; operationsJsonStr += js.str();
  }

  void visit(CRZ &crz) override {
    auto lambda = crz.getParameter(0).as<double>();
    U u1_1(crz.bits()[1], 0.0, 0.0, lambda / 2.0);
    CNOT cx1(crz.bits());
    U u1_2(crz.bits()[1], 0.0, 0.0, lambda / -2.0);
    CNOT cx2(crz.bits());
    visit(u1_1);
    visit(cx1);
    visit(u1_2);
    visit(cx2);
  }

  void visit(CH &ch) override {
    Hadamard h1(ch.bits()[1]);
    Sdg sdg(ch.bits()[1]);
    CNOT cn1(ch.bits());
    Hadamard h2(ch.bits()[1]);
    T t1(ch.bits()[1]);
    CNOT cn2(ch.bits());
    T t2(ch.bits()[1]);
    Hadamard h3(ch.bits()[1]);
    S s1(ch.bits()[1]);
    X x(ch.bits()[1]);
    S s2(ch.bits()[0]);
    visit(h1);
    visit(sdg);
    visit(cn1);
    visit(h2);
    visit(t1);
    visit(cn2);
    visit(t2);
    visit(h3);
    visit(s1);
    visit(x);
    visit(s2);
  }

  void visit(S &s) override {
    U u(s.bits()[0], 0.0, 0.0, 3.1415926 / 2.0);
    visit(u);
  }
  void visit(Sdg &sdg) override {
    U u(sdg.bits()[0], 0.0, 0.0, 3.1415926 / -2.0);
    visit(u);
  }
  void visit(T &t) override {
    U u(t.bits()[0], 0.0, 0.0, 3.1415926 / 4.0);
    visit(u);
  }
  void visit(Tdg &tdg) override {
    U u(tdg.bits()[0], 0.0, 0.0, 3.1415926 / -4.0);
    visit(u);
  }

  void visit(CNOT &cn) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("cnot");
    inst.set_target(cn.bits()[1]);
    inst.set_control(cn.bits()[0]);

    instructions.push_back(inst);
  }

  void visit(X &x) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("x");
    inst.set_target(x.bits()[0]);

    instructions.push_back(inst);
  }

  void visit(Y &y) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("y");
    inst.set_target(y.bits()[0]);

    instructions.push_back(inst);
  }

  void visit(Z &z) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("z");
    inst.set_target(z.bits()[0]);

    instructions.push_back(inst);
  }

  void visit(U &u) override {
    auto t = u.getParameter(0).as<double>();
    auto p = u.getParameter(1).as<double>();
    auto l = u.getParameter(2).as<double>();
    auto qubit = u.bits()[0];
    {
      // u3_qasm_def.rz(lam, 0)
      xacc::ionq::CircuitInstruction inst;
      inst.set_gate("rz");
      inst.set_target(qubit);
      inst.set_rotation(l);
      instructions.push_back(inst);
    }
    {
      // u3_qasm_def.sx(0)
      // v = Square root of not gate
      xacc::ionq::CircuitInstruction inst;
      inst.set_gate("v");
      inst.set_target(qubit);
      instructions.push_back(inst);
    }
    {
      // u3_qasm_def.rz(theta+pi, 0)
      xacc::ionq::CircuitInstruction inst;
      inst.set_gate("rz");
      inst.set_target(qubit);
      inst.set_rotation(t + M_PI);
      instructions.push_back(inst);
    }
    {
      // u3_qasm_def.sx(0)
      xacc::ionq::CircuitInstruction inst;
      inst.set_gate("v");
      inst.set_target(qubit);
      instructions.push_back(inst);
    }
    {
      // u3_qasm_def.rz(phi+3*pi, 0)
      xacc::ionq::CircuitInstruction inst;
      inst.set_gate("rz");
      inst.set_target(qubit);
      inst.set_rotation(p + 3.0 * M_PI);
      instructions.push_back(inst);
    }
  }

  void visit(Rx &rx) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("rx");
    inst.set_target(rx.bits()[0]);
    inst.set_rotation(rx.getParameter(0).as<double>());
    instructions.push_back(inst);
  }

  void visit(Ry &ry) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("ry");
    inst.set_target(ry.bits()[0]);
    inst.set_rotation(ry.getParameter(0).as<double>());
    instructions.push_back(inst);
  }

  void visit(Rz &rz) override {
    xacc::ionq::CircuitInstruction inst;
    inst.set_gate("rz");
    inst.set_target(rz.bits()[0]);
    inst.set_rotation(rz.getParameter(0).as<double>());
    instructions.push_back(inst);
  }

  void visit(CPhase &cp) override {
    auto lambda = cp.getParameter(0).as<double>();
    U u1_1(cp.bits()[0], 0.0, 0.0, lambda / 2.0);
    CNOT cx1(cp.bits());
    U u1_2(cp.bits()[1], 0.0, 0.0, -1.0 * lambda / 2.0);
    CNOT cx2(cp.bits());
    U u1_3(cp.bits()[1], 0.0, 0.0, lambda / 2.0);

    visit(u1_1);
    visit(cx1);
    visit(u1_2);
    visit(cx2);
    visit(u1_3);
  }

  void visit(Measure &m) override {}
  /**
   * The destructor
   */
  virtual ~IonQProgramVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
