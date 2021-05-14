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
#include "ControlledGateApplicator.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace circuits {

bool ControlledU::expand(const xacc::HeterogeneousMap &runtimeOptions) {
  // Single control or multiple-controls (as vector of int or qubits)
  if (!runtimeOptions.keyExists<int>("control-idx") &&
      !runtimeOptions.keyExists<std::vector<int>>("control-idx") &&
      !runtimeOptions.keyExists<std::pair<std::string, size_t>>(
          "control-idx") &&
      !runtimeOptions.keyExists<std::vector<std::pair<std::string, size_t>>>(
          "control-idx")) {
    xacc::error("'control-idx' is required.");
    return false;
  }

  if (!runtimeOptions.pointerLikeExists<CompositeInstruction>("U")) {
    xacc::error("'U' composite is required.");
    return false;
  }

  // Original U
  auto uComposite = std::shared_ptr<CompositeInstruction>(
      runtimeOptions.getPointerLike<CompositeInstruction>("U"),
      xacc::empty_delete<CompositeInstruction>());

  const std::vector<std::pair<std::string, size_t>> ctrlIdxs =
      [&runtimeOptions,
       uComposite]() -> std::vector<std::pair<std::string, size_t>> {
    // Try to find the control buffer name:
    const std::string buffer_name = [&]() -> std::string {
      if (runtimeOptions.stringExists("control-buffer")) {
        return runtimeOptions.getString("control-buffer");
      }

      // Use the buffer name of the input Composite:
      if (!uComposite->getBufferNames().empty()) {
        return uComposite->getBufferName(0);
      }
      // No information, just use a generic name
      return "q";
    }();

    if (runtimeOptions.keyExists<int>("control-idx")) {
      return {std::make_pair(
          buffer_name,
          static_cast<size_t>(runtimeOptions.get<int>("control-idx")))};
    } else if (runtimeOptions.keyExists<std::vector<int>>("control-idx")) {
      std::vector<std::pair<std::string, size_t>> result;
      for (const auto &qId :
           runtimeOptions.get<std::vector<int>>("control-idx")) {
        result.emplace_back(buffer_name, static_cast<size_t>(qId));
      }
      return result;
    }

    // Qubit input (as string + int pairs)
    if (runtimeOptions.keyExists<std::pair<std::string, size_t>>(
            "control-idx")) {
      auto qubit =
          runtimeOptions.get<std::pair<std::string, size_t>>("control-idx");
      return {qubit};
    }

    if (runtimeOptions.keyExists<std::vector<std::pair<std::string, size_t>>>(
            "control-idx")) {
      auto qubits =
          runtimeOptions.get<std::vector<std::pair<std::string, size_t>>>(
              "control-idx");
      return qubits;
    }
    return {};
  }();

  if (ctrlIdxs.empty()) {
    xacc::error("Failed to retrieve control bits.");
  }
  // Check duplicate
  const std::set<std::pair<std::string, size_t>> s(ctrlIdxs.begin(),
                                                   ctrlIdxs.end());
  if (s.size() != ctrlIdxs.size()) {
    xacc::error("Control bits must be unique.");
  }
  auto ctrlU = uComposite;
  // Recursive application of control bits:
  for (const auto &ctrlIdx : ctrlIdxs) {
    ctrlU = applyControl(ctrlU, ctrlIdx);
  }

  for (int instId = 0; instId < ctrlU->nInstructions(); ++instId) {
    auto inst = ctrlU->getInstruction(instId)->clone();
    addInstruction(inst);
  }
  return true;
}

std::shared_ptr<xacc::CompositeInstruction> ControlledU::applyControl(
    const std::shared_ptr<xacc::CompositeInstruction> &in_program,
    const std::pair<std::string, size_t> &in_ctrlIdx) {
  std::string qcor_compute_section_key = "__qcor__compute__segment__";
  m_gateProvider = xacc::getService<xacc::IRProvider>("quantum");
  m_composite = m_gateProvider->createComposite(
      "CTRL_" + in_program->name() + "_" + in_ctrlIdx.first +
      std::to_string(in_ctrlIdx.second));
  // Set the current control qubit before visiting...
  m_ctrlIdx = in_ctrlIdx;
  InstructionIterator it(in_program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      auto metadata = nextInst->getMetadata();
      if (metadata.keyExists<bool>(qcor_compute_section_key) &&
          metadata.get<bool>(qcor_compute_section_key)) {
        // Just add the instructino
        m_composite->addInstruction(nextInst);
      } else {
        // Otherwise add the ctrl version of the gate
        nextInst->accept(this);
      }
    }
  }

  return m_composite;
}

void ControlledU::visit(Hadamard &h) {
  if (h.bits()[0] == m_ctrlIdx.second &&
      h.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(h.getBufferName(0), h.bits()[0]);
    // CH
    m_composite->addInstruction(
        m_gateProvider->createInstruction("CH", {m_ctrlIdx, targetIdx}));
  }
}

void ControlledU::visit(X &x) {
  if (x.bits()[0] == m_ctrlIdx.second &&
      x.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(x.getBufferName(0), x.bits()[0]);
    // CNOT
    m_composite->addInstruction(
        m_gateProvider->createInstruction("CX", {m_ctrlIdx, targetIdx}));
  }
}

void ControlledU::visit(Y &y) {
  if (y.bits()[0] == m_ctrlIdx.second &&
      y.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(y.getBufferName(0), y.bits()[0]);
    // CY
    m_composite->addInstruction(
        m_gateProvider->createInstruction("CY", {m_ctrlIdx, targetIdx}));
  }
}

void ControlledU::visit(Z &z) {
  if (z.bits()[0] == m_ctrlIdx.second &&
      z.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(z.getBufferName(0), z.bits()[0]);
    // CZ
    m_composite->addInstruction(
        m_gateProvider->createInstruction("CZ", {m_ctrlIdx, targetIdx}));
  }
}

void ControlledU::visit(CNOT &cnot) {
  // CCNOT gate:
  // We now have two control bits
  const auto ctrlIdx1 = std::make_pair(cnot.getBufferName(0), cnot.bits()[0]);
  const auto ctrlIdx2 = m_ctrlIdx;
  // Target qubit
  const auto targetIdx = std::make_pair(cnot.getBufferName(1), cnot.bits()[1]);

  // gate ccx a,b,c (see qelib1.inc)
  // Maps qubit indices:
  const auto a = m_ctrlIdx;
  const auto b = ctrlIdx1;
  const auto c = targetIdx;
  //     h c;
  m_composite->addInstruction(m_gateProvider->createInstruction("H", {c}));

  //     cx b,c; tdg c;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {b, c}));
  m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", {c}));

  //     cx a,c; t c;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {a, c}));
  m_composite->addInstruction(m_gateProvider->createInstruction("T", {c}));

  //     cx b,c; tdg c;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {b, c}));
  m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", {c}));

  //     cx a,c; t b; t c; h c;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {a, c}));
  m_composite->addInstruction(m_gateProvider->createInstruction("T", {b}));
  m_composite->addInstruction(m_gateProvider->createInstruction("T", {c}));
  m_composite->addInstruction(m_gateProvider->createInstruction("H", {c}));

  //     cx a,b; t a; tdg b;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {a, b}));
  m_composite->addInstruction(m_gateProvider->createInstruction("T", {a}));
  m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", {b}));

  //     cx a,b;
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {a, b}));
}

void ControlledU::visit(Rx &rx) {
  if (rx.bits()[0] == m_ctrlIdx.second &&
      rx.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(rx.getBufferName(0), rx.bits()[0]);
    ;
    const auto angle = InstructionParameterToDouble(rx.getParameter(0));
    // Rx(angle) = u3(angle,-pi/2,pi/2)
    auto rxAsU3 = U(targetIdx.second, angle, -M_PI_2, M_PI_2);
    rxAsU3.setBufferNames({targetIdx.first});
    visit(rxAsU3);
  }
}

void ControlledU::visit(Ry &ry) {
  if (ry.bits()[0] == m_ctrlIdx.second &&
      ry.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(ry.getBufferName(0), ry.bits()[0]);
    const auto angle = InstructionParameterToDouble(ry.getParameter(0));
    //  Ry(theta)  ==  u3(theta,0,0)
    auto ryAsU3 = U(targetIdx.second, angle, 0.0, 0.0);
    ryAsU3.setBufferNames({targetIdx.first});
    visit(ryAsU3);
  }
}

void ControlledU::visit(Rz &rz) {
  if (rz.bits()[0] == m_ctrlIdx.second &&
      rz.getBufferName(0) == m_ctrlIdx.first) {
    xacc::error("Control bit must be different from target qubit(s).");
  } else {
    const auto targetIdx = std::make_pair(rz.getBufferName(0), rz.bits()[0]);
    const auto angle = InstructionParameterToDouble(rz.getParameter(0));
    // CRz
    m_composite->addInstruction(m_gateProvider->createInstruction(
        "CRZ", {m_ctrlIdx, targetIdx}, {angle}));
  }
}

void ControlledU::visit(S &s) {
  // Ctrl-S = CPhase(pi/2)
  const auto targetIdx = std::make_pair(s.getBufferName(0), s.bits()[0]);
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {m_ctrlIdx, targetIdx}, {M_PI_2}));
}

void ControlledU::visit(Sdg &sdg) {
  // Ctrl-Sdg = CPhase(-pi/2)
  const auto targetIdx = std::make_pair(sdg.getBufferName(0), sdg.bits()[0]);
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {m_ctrlIdx, targetIdx}, {-M_PI_2}));
}

void ControlledU::visit(T &t) {
  // Ctrl-T = CPhase(pi/4)
  const auto targetIdx = std::make_pair(t.getBufferName(0), t.bits()[0]);
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {m_ctrlIdx, targetIdx}, {M_PI_4}));
}

void ControlledU::visit(Tdg &tdg) {
  // Ctrl-Tdg = CPhase(-pi/4)
  const auto targetIdx = std::make_pair(tdg.getBufferName(0), tdg.bits()[0]);
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {m_ctrlIdx, targetIdx}, {-M_PI_4}));
}

void ControlledU::visit(Swap &s) {
  // Fredkin gate: controlled-swap
  // Decompose Swap gate into CNOT gates;
  // then re-visit those CNOT gates (becoming CCNOT).
  // CX(0, 1) CX (1, 0) CX(0, 1) is a compute-action pattern,
  // we only need to convert the middle CX to CCX:
  const auto qubit1 = std::make_pair(s.getBufferName(0), s.bits()[0]);
  const auto qubit2 = std::make_pair(s.getBufferName(1), s.bits()[1]);
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {qubit1, qubit2}));

  // Visit the middle CNOT ==> CCNOT
  CNOT c2(s.bits()[1], s.bits()[0]);
  c2.setBufferNames({s.getBufferName(1), s.getBufferName(0)});
  visit(c2);

  // Add the uncompute CNOT
  m_composite->addInstruction(m_gateProvider->createInstruction("CX", {qubit1, qubit2}));
}

void ControlledU::visit(U &u) {
  // controlled-U
  // gate cu3(theta, phi, lambda) c, t {
  //   u1((lambda - phi) / 2) t;
  //   cx c, t;
  //   u3(-theta / 2, 0, -(phi + lambda) / 2) t;
  //   cx c, t;
  //   u3(theta / 2, phi, 0) t;
  // }
  const auto targetIdx = std::make_pair(u.getBufferName(0), u.bits()[0]);
  const double theta = InstructionParameterToDouble(u.getParameter(0));
  const double phi = InstructionParameterToDouble(u.getParameter(1));
  const double lambda = InstructionParameterToDouble(u.getParameter(2));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "Rz", {targetIdx}, {(lambda - phi) / 2}));
  m_composite->addInstruction(
      m_gateProvider->createInstruction("CX", {m_ctrlIdx, targetIdx}));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "U", {targetIdx}, {-theta / 2, 0.0, -(phi + lambda) / 2}));
  m_composite->addInstruction(
      m_gateProvider->createInstruction("CX", {m_ctrlIdx, targetIdx}));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "U", {targetIdx}, {theta / 2, phi, 0.0}));
}

void ControlledU::visit(U1 &u1) {
  // cU1 == CPhase
  const auto targetIdx = std::make_pair(u1.getBufferName(0), u1.bits()[0]);
  auto angle = u1.getParameter(0);
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {m_ctrlIdx, targetIdx}, {angle}));
}

void ControlledU::visit(CY &cy) {
  // controlled-Y = Sdg(target) - CX - S(target)
  CNOT c1(cy.bits());
  c1.setBufferNames({cy.getBufferName(0), cy.getBufferName(1)});

  Sdg sdg(cy.bits()[1]);
  sdg.setBufferNames({cy.getBufferName(1)});

  S s(cy.bits()[1]);
  s.setBufferNames({cy.getBufferName(1)});

  // Revisit these gates: CNOT->CCNOT; S -> CPhase
  visit(sdg);
  visit(c1);
  visit(s);
}

void ControlledU::visit(CZ &cz) {
  // CZ = H(target) - CX - H(target)
  CNOT c1(cz.bits());
  c1.setBufferNames({cz.getBufferName(0), cz.getBufferName(1)});

  Hadamard h1(cz.bits()[1]);
  h1.setBufferNames({cz.getBufferName(1)});

  Hadamard h2(cz.bits()[1]);
  h2.setBufferNames({cz.getBufferName(1)});

  // Revisit these gates: CNOT->CCNOT; H -> CH
  visit(h1);
  visit(c1);
  visit(h2);
}

void ControlledU::visit(CRZ &crz) {
  const auto theta = InstructionParameterToDouble(crz.getParameter(0));
  // Decompose
  Rz rz1(crz.bits()[1], theta / 2);
  rz1.setBufferNames({crz.getBufferName(1)});

  CNOT c1(crz.bits());
  c1.setBufferNames({crz.getBufferName(0), crz.getBufferName(1)});

  Rz rz2(crz.bits()[1], -theta / 2);
  rz2.setBufferNames({crz.getBufferName(1)});

  CNOT c2(crz.bits());
  c2.setBufferNames({crz.getBufferName(0), crz.getBufferName(1)});

  // Revisit:
  visit(rz1);
  visit(c1);
  visit(rz2);
  visit(c2);
}

void ControlledU::visit(CH &ch) {
  // controlled-H = Ry(pi/4, target) - CX - Ry(-pi/4, target)
  CNOT c1(ch.bits());
  c1.setBufferNames({ch.getBufferName(0), ch.getBufferName(1)});

  Ry ry1(ch.bits()[1], M_PI_4);
  ry1.setBufferNames({ch.getBufferName(1)});

  Ry ry2(ch.bits()[1], -M_PI_4);
  ry2.setBufferNames({ch.getBufferName(1)});

  visit(ry1);
  visit(c1);
  visit(ry2);
}

void ControlledU::visit(CPhase &cphase) {
  // This is effectively CRz but due to the global phase difference
  // in the matrix definitions of U1 and Rz,
  // CU1 (i.e. CPhase) and CRz are different gates with a relative phase
  // difference. Ref: ccu1(t, ctrl1, ctrl2, target) = cu1(t/2, ctrl1, ctrl2)
  // cx(ctrl2, target)
  // cu1(-t/2, ctrl1, target)
  // cx(ctrl2, target)
  // cu1(t/2, ctrl1, target)

  const auto ctrlIdx1 =
      std::make_pair(cphase.getBufferName(0), cphase.bits()[0]);
  const auto ctrlIdx2 = m_ctrlIdx;
  // Target qubit
  const auto targetIdx =
      std::make_pair(cphase.getBufferName(1), cphase.bits()[1]);
  // Angle
  const auto angle = InstructionParameterToDouble(cphase.getParameter(0));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {ctrlIdx1, ctrlIdx2}, {angle / 2}));
  m_composite->addInstruction(
      m_gateProvider->createInstruction("CX", {ctrlIdx2, targetIdx}));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {ctrlIdx1, targetIdx}, {-angle / 2}));
  m_composite->addInstruction(
      m_gateProvider->createInstruction("CX", {ctrlIdx2, targetIdx}));
  m_composite->addInstruction(m_gateProvider->createInstruction(
      "CPhase", {ctrlIdx1, targetIdx}, {angle / 2}));
}
} // namespace circuits
} // namespace xacc