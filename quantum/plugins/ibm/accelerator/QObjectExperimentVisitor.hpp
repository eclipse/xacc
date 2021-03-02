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
#ifndef QUANTUM_GATE_ACCELERATORS_QOBJECTVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QOBJECTVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"
#include "QObject.hpp"
#include "xacc.hpp"

using namespace xacc::ibm;

namespace xacc {
namespace quantum {
class QObjectExperimentVisitor : public AllGateVisitor {
protected:
  constexpr static double pi = xacc::constants::pi;

  std::vector<xacc::ibm::Instruction> instructions;
  Experiment experiment;
  std::string experimentName;
  int nTotalQubits = 0;
  std::vector<int> usedMemorySlots;
  std::optional<int64_t> conditionalRegId;
  std::map<std::pair<std::string, std::size_t>, std::size_t> cReg_to_meas_idx;

public:
  int maxMemorySlots = 0;
  std::map<int, int> qubit2MemorySlot;

  const std::string name() const override { return "qobject-visitor"; }

  const std::string description() const override {
    return "Map XACC IR to QObject.";
  }

  // IBM gate set: either { u1, u2, u3, cx} or {rz, sx, x, cx}
  // XACC Composite is decomposed into this gate set when generating the QObject.
  enum class GateSet { U_CX, RZ_SX_CX };
  GateSet gateSet;
  // Do we want to include Identity gate in the generated QObj?
  // By default, we skip all Id gates.
  bool skipIdGate;
  QObjectExperimentVisitor(const std::string expName, const int nQubits,
                           GateSet nativeGateSet = GateSet::U_CX,
                           bool ignoreIdGate = true)
      : experimentName(expName), nTotalQubits(nQubits), gateSet(nativeGateSet),
        skipIdGate(ignoreIdGate) {
    int counter = 0;
    for (int b = 0; b < nQubits; b++) {
      qubit2MemorySlot.insert({b, counter});
      counter++;
    }
    xacc::ibm::RegisterAllocator::getInstance()->reset();
  }

  const std::string toString() override {
    experiment = getExperiment();
    nlohmann::json json;
    nlohmann::to_json(json, experiment);
    return json.dump();
  }

  Experiment getExperiment() {
    //   xacc::info("Getting Experiment " + experimentName + ", " +
    //   std::to_string(experiment.get_instructions().size()));
    if (experiment.get_instructions().empty()) {

      //   maxMemorySlots++;

      // construct config and header, add
      ExperimentHeader header;
      header.set_name(experimentName);
      header.set_qreg_sizes(
          {{QregSizeElement("q"), QregSizeElement(nTotalQubits)}});
      header.set_n_qubits(nTotalQubits);
      std::vector<std::vector<QregSizeElement>> qlabels;
      for (int i = 0; i < nTotalQubits; i++) {
        qlabels.push_back({"q", i});
      }
      header.set_qubit_labels(qlabels);
      header.set_memory_slots(maxMemorySlots);

      std::vector<std::vector<ClbitLabel>> clabels;
      for (int i = 0; i < maxMemorySlots; i++) {
        clabels.push_back({"c0", i});
      }
      header.set_clbit_labels(clabels);
      header.set_creg_sizes({{"c0", maxMemorySlots}});

      ExperimentConfig config;
      config.set_memory_slots(maxMemorySlots);
      config.set_n_qubits(nTotalQubits);

      //   Experiment experiment;
      experiment.set_config(config);
      experiment.set_header(header);
      //   xacc::info("Adding insts " + std::to_string(instructions.to))
      
      // Note: the experiment was constructed in terms of { u1, u2, u3, cx} gate set.
      // U + CX gate set
      if (gateSet == GateSet::U_CX) {
        // Use the instructions as is
        experiment.set_instructions(instructions);
      } else if (gateSet == GateSet::RZ_SX_CX) {
        // Convert the u1, u2, u3 to {rz, sx} gates,
        // keeping all others (cx, measure, etc.)
        std::vector<xacc::ibm::Instruction> new_instructions;
        for (const auto &inst : instructions) {
          if (inst.get_name() == "u1") {
            // u1 is rz
            auto newInst = inst;
            newInst.get_mutable_name() = "rz";
            new_instructions.emplace_back(newInst);
          } else if (inst.get_name() == "u2") {
            // Copy the instruction and only update the name + params.
            // This is to make sure information about Bfunc (conditional) etc.
            // is copied to the decomposed gate sequence.
            
            // Note: this decomposition is adapted from Qiskit Terra,
            // see qiskit/circuit/library/standard_gates/equivalence_library.py
            const auto u2_params = inst.get_params();
            assert(u2_params.size() == 2);
            const double phi = u2_params[0];
            const double lam = u2_params[1];
            {
              // u2_to_u1sx.append(U1Gate(lam - pi/2), [0])
              auto newInst = inst;
              newInst.get_mutable_name() = "rz";
              newInst.set_params({lam - M_PI / 2.0});
              new_instructions.emplace_back(newInst);
            }
            {
              // u2_to_u1sx.sx(0)
              auto newInst = inst;
              newInst.get_mutable_name() = "sx";
              newInst.set_params({});
              new_instructions.emplace_back(newInst);
            }
            {
              // u2_to_u1sx.append(U1Gate(phi + pi/2), [0])
              auto newInst = inst;
              newInst.get_mutable_name() = "rz";
              newInst.set_params({phi + M_PI / 2.0});
              new_instructions.emplace_back(newInst);
            }
          } else if (inst.get_name() == "u3") {
            const auto u3_params = inst.get_params();
            assert(u3_params.size() == 3);
            const double theta = u3_params[0];
            const double phi = u3_params[1];
            const double lam = u3_params[2];
            {
              // u3_qasm_def.rz(lam, 0)
              auto newInst = inst;
              newInst.get_mutable_name() = "rz";
              newInst.set_params({lam});
              new_instructions.emplace_back(newInst);
            }
            {
              // u3_qasm_def.sx(0)
              auto newInst = inst;
              newInst.get_mutable_name() = "sx";
              newInst.set_params({});
              new_instructions.emplace_back(newInst);
            }
            {
              // u3_qasm_def.rz(theta+pi, 0)
              auto newInst = inst;
              newInst.get_mutable_name() = "rz";
              newInst.set_params({theta + M_PI});
              new_instructions.emplace_back(newInst);
            }
            {
              // u3_qasm_def.sx(0)
              auto newInst = inst;
              newInst.get_mutable_name() = "sx";
              newInst.set_params({});
              new_instructions.emplace_back(newInst);
            }
            {
              // u3_qasm_def.rz(phi+3*pi, 0)
              auto newInst = inst;
              newInst.get_mutable_name() = "rz";
              newInst.set_params({phi + 3.0 * M_PI});
              new_instructions.emplace_back(newInst);
            }
          } else {
            // Other instructions, e.g. cx, just append.
            new_instructions.emplace_back(inst);
          }
        }

        experiment.set_instructions(new_instructions);
      }
    }
    //   xacc::info("Returning Experiment " + experimentName + ", " +
    //   std::to_string(experiment.get_instructions().size()));

    return experiment;
  }

  void setConditional(xacc::ibm::Instruction &io_inst) {
    if (conditionalRegId.has_value()) {
      io_inst.set_condition_reg_id(conditionalRegId.value());
    }
  }

  void visit(Hadamard &h) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(h.bits()[0]);
    inst.get_mutable_name() = "u2";
    inst.set_params({0.0, pi});
    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Identity &i) override {
    // Only add "id" instruction if requested.
    if (!skipIdGate) {
      xacc::ibm::Instruction inst;
      inst.get_mutable_qubits().push_back(i.bits()[0]);
      inst.get_mutable_name() = "id";
      setConditional(inst);
      instructions.push_back(inst);
    }
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

  /**
   * Visit CNOT gates
   */
  void visit(CNOT &cn) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(cn.bits()[0]);
    inst.get_mutable_qubits().push_back(cn.bits()[1]);
    inst.get_mutable_name() = "cx";
    setConditional(inst);
    instructions.push_back(inst);
  }

  /**
   * Visit X gates
   */
  void visit(X &x) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(x.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({pi, 0.0, pi});

    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Y &y) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(y.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({pi, pi / 2.0, pi});

    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Z &z) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(z.bits()[0]);
    inst.get_mutable_name() = "u1";
    inst.set_params({pi});

    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(U &u) override {

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(u.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({u.getParameter(0).as<double>(),
                     u.getParameter(1).as<double>(),
                     u.getParameter(2).as<double>()});
    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Measure &m) override {
    if (std::find(usedMemorySlots.begin(), usedMemorySlots.end(),
                  m.getParameter(0).as<int>()) != usedMemorySlots.end()) {
      xacc::error(
          "IBM: Invalid classical bit index for measurement, already used.");
    }

    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(m.bits()[0]);
    inst.get_mutable_name() = "measure";
    inst.set_memory({maxMemorySlots});
    instructions.push_back(inst);
    if (m.hasClassicalRegAssignment()) {
      auto classicalBit = m.getParameter(0).as<int>();
      auto classicalReg = m.getBufferNames()[1];
      cReg_to_meas_idx[std::make_pair(classicalReg, classicalBit)] =
          inst.get_memory()[0];
    }
    // if (classicalBit > maxMemorySlots) {
    maxMemorySlots++; // = qubit2MemorySlot[m.bits()[0]];
    // }
  }

  void visit(Reset &reset) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(reset.bits()[0]);
    inst.get_mutable_name() = "reset";
    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Rx &rx) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(rx.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params(
        {rx.getParameter(0).as<double>(), -1.0 * pi / 2.0, pi / 2.0});
    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Ry &ry) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(ry.bits()[0]);
    inst.get_mutable_name() = "u3";
    inst.set_params({ry.getParameter(0).as<double>(), 0.0, 0.0});
    setConditional(inst);
    instructions.push_back(inst);
  }

  void visit(Rz &rz) override {
    xacc::ibm::Instruction inst;
    inst.get_mutable_qubits().push_back(rz.bits()[0]);
    inst.get_mutable_name() = "u1";
    inst.set_params({rz.getParameter(0).as<double>()});
    setConditional(inst);
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

  void visit(IfStmt &ifStmt) override {
    const auto cregId = ifStmt.bits()[0];
    const auto cregName = ifStmt.getParameters()[0].as<std::string>();
    const auto absRegId = [&]() {
      auto iter = cReg_to_meas_idx.find(std::make_pair(cregName, cregId));
      if (iter != cReg_to_meas_idx.end()) {
        return iter->second;
      } else {
        return cregId;
      }
    }();
    auto bfuncInst = xacc::ibm::Instruction::createConditionalInst(absRegId);
    const auto regId = bfuncInst.get_bFunc()->registerId;
    instructions.push_back(bfuncInst);
    // All the instructions that are scoped inside this block must have a
    // `condition` field points to this register Id.
    conditionalRegId = regId;
    for (auto &i : ifStmt.getInstructions()) {
      i->accept(this);
    }
    conditionalRegId.reset();
  }

  /**
   * The destructor
   */
  virtual ~QObjectExperimentVisitor() {}
};

} // namespace quantum
} // namespace xacc

#endif
