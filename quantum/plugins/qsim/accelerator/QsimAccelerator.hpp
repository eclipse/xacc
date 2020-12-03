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
#pragma one
#include "xacc.hpp"
#include "AllGateVisitor.hpp"
// Qsim:
#include "circuit.h"
#include "gates_qsim.h"
#include "formux.h"
#include "fuser_basic.h"
#include "run_qsim.h"
#include "simmux.h"
#include "io_file.h"

namespace xacc {
namespace quantum {
class QsimCircuitVisitor : public AllGateVisitor {
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
    constexpr uint64_t cmask = uint64_t{1} << 1 - 1;
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)cy.bits()[0]},
                             cmask, yGate);
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
    constexpr uint64_t cmask = uint64_t{1} << 1 - 1;
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)crz.bits()[0]},
                             cmask, rzGate);
    m_circuit.gates.emplace_back(std::move(rzGate));
  }

  void visit(CH &ch) override {
    auto hGate = qsim::GateHd<fp_type>::Create(m_time++, ch.bits()[1]);
    constexpr uint64_t cmask = uint64_t{1} << 1 - 1;
    qsim::MakeControlledGate(std::vector<unsigned>{(unsigned)ch.bits()[0]},
                             cmask, hGate);
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

  qsim::Circuit<qsim::GateQSim<fp_type>> getQsimCircuit() const {
    return m_circuit;
  }

private:
  qsim::Circuit<qsim::GateQSim<fp_type>> m_circuit;
  size_t m_time;
};

class QsimAccelerator : public Accelerator {
public:
  // Qsim type:
  using Simulator = qsim::Simulator<qsim::For>;
  using StateSpace = Simulator::StateSpace;
  using State = StateSpace::State;
  using Runner =
      qsim::QSimRunner<qsim::IO,
                       qsim::BasicGateFuser<qsim::IO, qsim::GateQSim<float>>,
                       Simulator>;

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
  virtual BitOrder getBitOrder() override { return BitOrder::MSB; }
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
};
} // namespace quantum
} // namespace xacc
