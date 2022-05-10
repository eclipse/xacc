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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once
#include "xacc.hpp"
#include "AllGateVisitor.hpp"
#include "GateModifier.hpp"
#include <cassert>
// Workaround VirtualBox bug: https://www.virtualbox.org/ticket/15471
// VirtualBox enables AVX2 flag but not FMA flag in -march=native
// which is a **bug**, there is no such config.
// qsim lib's simmux relies on AVX2 flag for simulator selection,
// hence we just disable AVX support if compiling on VirtualBox.
#ifdef __AVX2__
#ifndef __FMA__
#undef __AVX2__
#endif
#endif

// Qsim:
#include "circuit.h"
#include "gates_qsim.h"
#include "formux.h"
#include "fuser_basic.h"
#include "run_qsim.h"
#include "simmux.h"
#include "io_file.h"
#include "fuser_mqubit.h"

namespace xacc {
namespace quantum {
class QsimCircuitVisitor : public AllGateVisitor, public InstructionVisitor<Circuit> {
public:
  using fp_type = float;

  QsimCircuitVisitor(size_t nbQubits) {
    m_circuit.num_qubits = nbQubits;
    m_circuit.gates.clear();
    m_time = 0;
  }

  void visit(Hadamard &h) override {
    m_circuit.gates.emplace_back(
        qsim::GateHd<fp_type>::Create(m_time++, h.bits()[0]));
  }

  void visit(CNOT &cnot) override {
    m_circuit.gates.emplace_back(qsim::GateCNot<fp_type>::Create(
        m_time++, cnot.bits()[0], cnot.bits()[1]));
  }

  void visit(Rz &rz) override {
    m_circuit.gates.emplace_back(qsim::GateRZ<fp_type>::Create(
        m_time++, rz.bits()[0],
        InstructionParameterToDouble(rz.getParameter(0))));
  }

  void visit(Ry &ry) override {
    m_circuit.gates.emplace_back(qsim::GateRY<fp_type>::Create(
        m_time++, ry.bits()[0],
        InstructionParameterToDouble(ry.getParameter(0))));
  }

  void visit(Rx &rx) override {
    m_circuit.gates.emplace_back(qsim::GateRX<fp_type>::Create(
        m_time++, rx.bits()[0],
        InstructionParameterToDouble(rx.getParameter(0))));
  }

  void visit(X &x) override {
    m_circuit.gates.emplace_back(
        qsim::GateX<fp_type>::Create(m_time++, x.bits()[0]));
  }

  void visit(Y &y) override {
    m_circuit.gates.emplace_back(
        qsim::GateY<fp_type>::Create(m_time++, y.bits()[0]));
  }

  void visit(Z &z) override {
    m_circuit.gates.emplace_back(
        qsim::GateZ<fp_type>::Create(m_time++, z.bits()[0]));
  }

  void visit(CY &cy) override {
    auto yGate = qsim::GateY<fp_type>::Create(m_time++, cy.bits()[1]);
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)cy.bits()[0]},
                              yGate);
    m_circuit.gates.emplace_back(std::move(yGate));
  }

  void visit(CZ &cz) override {
    m_circuit.gates.emplace_back(
        qsim::GateCZ<fp_type>::Create(m_time++, cz.bits()[0], cz.bits()[1]));
  }

  void visit(Swap &s) override {
    m_circuit.gates.emplace_back(
        qsim::GateSwap<fp_type>::Create(m_time++, s.bits()[0], s.bits()[1]));
  }

  void visit(CRZ &crz) override {
    auto rzGate = qsim::GateRZ<fp_type>::Create(
        m_time++, crz.bits()[1],
        InstructionParameterToDouble(crz.getParameter(0)));
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)crz.bits()[0]},
                              rzGate);
    m_circuit.gates.emplace_back(std::move(rzGate));
  }

  void visit(CH &ch) override {
    auto hGate = qsim::GateHd<fp_type>::Create(m_time++, ch.bits()[1]);
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)ch.bits()[0]},
                             hGate);
    m_circuit.gates.emplace_back(std::move(hGate));
  }

  void visit(S &s) override {
    m_circuit.gates.emplace_back(
        qsim::GateS<fp_type>::Create(m_time++, s.bits()[0]));
  }

  void visit(Sdg &sdg) override {
    auto sGate = qsim::GateS<fp_type>::Create(m_time++, sdg.bits()[0]);
    qsim::MatrixDagger(unsigned{1} << sGate.qubits.size(), sGate.matrix);
    m_circuit.gates.emplace_back(std::move(sGate));
  }

  void visit(T &t) override {
    m_circuit.gates.emplace_back(
        qsim::GateT<fp_type>::Create(m_time++, t.bits()[0]));
  }

  void visit(Tdg &tdg) override {
    auto tGate = qsim::GateT<fp_type>::Create(m_time++, tdg.bits()[0]);
    qsim::MatrixDagger(unsigned{1} << tGate.qubits.size(), tGate.matrix);
    m_circuit.gates.emplace_back(std::move(tGate));
  }

  void visit(CPhase &cphase) override {
    m_circuit.gates.emplace_back(qsim::GateCP<fp_type>::Create(
        m_time++, cphase.bits()[0], cphase.bits()[1],
        InstructionParameterToDouble(cphase.getParameter(0))));
  }

  void visit(Identity &i) override {}

  void visit(U &u) override {
    const auto theta = InstructionParameterToDouble(u.getParameter(0));
    const auto phi = InstructionParameterToDouble(u.getParameter(1));
    const auto lambda = InstructionParameterToDouble(u.getParameter(2));

    // rz(lmda)
    m_circuit.gates.emplace_back(
        qsim::GateRZ<fp_type>::Create(m_time++, u.bits()[0], lambda));
    // ry(theta)
    m_circuit.gates.emplace_back(
        qsim::GateRY<fp_type>::Create(m_time++, u.bits()[0], theta));
    // rz(phi)
    m_circuit.gates.emplace_back(
        qsim::GateRZ<fp_type>::Create(m_time++, u.bits()[0], phi));
  }

  void visit(iSwap &in_iSwapGate) override {
    m_circuit.gates.emplace_back(qsim::GateIS<fp_type>::Create(
        m_time++, in_iSwapGate.bits()[0], in_iSwapGate.bits()[1]));
  }

  void visit(fSim &in_fsimGate) override {
    m_circuit.gates.emplace_back(qsim::GateFS<fp_type>::Create(
        m_time++, in_fsimGate.bits()[0], in_fsimGate.bits()[1],
        InstructionParameterToDouble(in_fsimGate.getParameter(0)),
        InstructionParameterToDouble(in_fsimGate.getParameter(1))));
  }

  // TODO:
  void visit(IfStmt &ifStmt) override {}
  void visit(Measure &measure) override {
    m_circuit.gates.emplace_back(
        qsim::gate::Measurement<qsim::GateQSim<fp_type>>::Create(
            m_time++, std::vector<unsigned>{(unsigned)measure.bits()[0]}));
  }

  void visit(Circuit &in_circuit) override {
    // std::cout << "HOWDY: Visit quantum circuit: " << in_circuit.name() << "\n";
    if (in_circuit.name() == "C-U" &&
        dynamic_cast<xacc::quantum::ControlModifier *>(&in_circuit)) {
      auto *asControlledBlock =
          dynamic_cast<xacc::quantum::ControlModifier *>(&in_circuit);
      assert(asControlledBlock);
      // Controlled circuit
      const auto controlQubits = asControlledBlock->getControlQubits();
      auto baseCircuit = asControlledBlock->getBaseInstruction();
      assert(baseCircuit->isComposite());
      auto asComp = xacc::ir::asComposite(baseCircuit);
      assert(!controlQubits.empty());
      const bool should_perform_mcu_sim = [&]() {
        if (asComp->getInstructions().size() == 1) {
          // Only support these for the moment
          if (asComp->getInstruction(0)->name() == "X" ||
              asComp->getInstruction(0)->name() == "Y" ||
              asComp->getInstruction(0)->name() == "Z") {
            return true;
          }
        }

        return false;
      }();

      if (should_perform_mcu_sim) {
        std::vector<unsigned> ctrlIdx;
        const std::string regName = controlQubits[0].first;
        for (const auto &[reg, idx] : controlQubits) {
          if (reg != regName) {
            xacc::error("Multiple qubit registers are not supported!");
          }
          ctrlIdx.emplace_back(idx);
        }
        assert(ctrlIdx.size() == controlQubits.size());

        auto baseGate = asComp->getInstruction(0);
        if (baseGate->name() == "X") {
          auto xGate =
              qsim::GateX<fp_type>::Create(m_time++, baseGate->bits()[0]);
          qsim::MakeControlledGate(ctrlIdx, xGate);
          m_circuit.gates.emplace_back(std::move(xGate));
        } else if (baseGate->name() == "Y") {
          auto yGate =
              qsim::GateY<fp_type>::Create(m_time++, baseGate->bits()[0]);
          qsim::MakeControlledGate(ctrlIdx, yGate);
          m_circuit.gates.emplace_back(std::move(yGate));
        } else if (baseGate->name() == "Z") {
          auto zGate =
              qsim::GateZ<fp_type>::Create(m_time++, baseGate->bits()[0]);
          qsim::MakeControlledGate(ctrlIdx, zGate);
          m_circuit.gates.emplace_back(std::move(zGate));
        } else {
          xacc::error("Internal error: Unhandled multi-controlled gate.");
        }

        // No need to handle this sub-circuit anymore.
        in_circuit.disable();
        m_controlledBlocks.emplace_back(in_circuit);
      }
    }
  }

  qsim::Circuit<qsim::GateQSim<fp_type>> getQsimCircuit() const {
    return m_circuit;
  }

  ~QsimCircuitVisitor() {
    for (auto &block : m_controlledBlocks) {
      // We temporarily disabled these blocks while handling the simulation,
      // now reset the status.
      block.get().enable();
    }
    m_controlledBlocks.clear();
  }

private:
  qsim::Circuit<qsim::GateQSim<fp_type>> m_circuit;
  size_t m_time;
  std::vector<std::reference_wrapper<xacc::quantum::Circuit>> m_controlledBlocks;
};

struct Factory {
  Factory(unsigned num_threads) : num_threads(num_threads) {}

  using Simulator = qsim::Simulator<qsim::For>;
  using StateSpace = Simulator::StateSpace;

  StateSpace CreateStateSpace() const { return StateSpace(num_threads); }

  Simulator CreateSimulator() const { return Simulator(num_threads); }

  unsigned num_threads;
};

class QsimAccelerator : public Accelerator {
public:
  // Qsim type:
  using Simulator = qsim::Simulator<qsim::For>;
  using StateSpace = Simulator::StateSpace;
  using State = StateSpace::State;
  using Fuser = qsim::MultiQubitGateFuser<qsim::IO, qsim::GateQSim<float>>;
  using Runner = qsim::QSimRunner<qsim::IO, Fuser, Factory>;

  // Identifiable interface impls
  virtual const std::string name() const override { return "qsim"; }
  virtual const std::string description() const override {
    return "XACC Simulation Accelerator based on qsim library.";
  }

  // Accelerator interface impls
  virtual void initialize(const HeterogeneousMap &params = {}) override;
  virtual void updateConfiguration(const HeterogeneousMap &config) override {
    initialize(config);
  }
  virtual const std::vector<std::string> configurationKeys() override {
    return {};
  }
  virtual BitOrder getBitOrder() override { return BitOrder::LSB; }
  virtual HeterogeneousMap getProperties() override {
    return {{"shots", m_shots}};
  }
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<CompositeInstruction>
                           compositeInstruction) override;
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::vector<std::shared_ptr<CompositeInstruction>>
                           compositeInstructions) override;
  virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer,
                     std::shared_ptr<Instruction> inst) override;

private:
  double getExpectationValueZ(
      std::shared_ptr<CompositeInstruction> compositeInstruction,
      const StateSpace &stateSpace, const State &state) const;
  Runner::Parameter m_qsimParam;
  int m_shots;
  bool m_vqeMode;
  int m_numThreads;
};
} // namespace quantum
} // namespace xacc
