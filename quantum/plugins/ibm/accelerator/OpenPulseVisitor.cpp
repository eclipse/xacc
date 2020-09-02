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
#include "OpenPulseVisitor.hpp"
#include "xacc.hpp"
namespace {
std::shared_ptr<xacc::CompositeInstruction>
constructUCmdDefComposite(size_t qIdx, const std::string &uGateType) {

  const auto cmdDefName = "pulse::" + uGateType + "_" + std::to_string(qIdx);
  // If we have a pulse cmd-def defined for U3:
  if (xacc::hasContributedService<xacc::Instruction>(cmdDefName)) {
    return xacc::ir::asComposite(
        xacc::getContributedService<xacc::Instruction>(cmdDefName));
  }
  return nullptr;
}
} // namespace
namespace xacc {
namespace quantum {
PulseMappingVisitor::PulseMappingVisitor() {
  auto provider = xacc::getService<IRProvider>("quantum");
  pulseComposite = provider->createComposite("PulseComposite");
}

void PulseMappingVisitor::visit(Hadamard &h) {
  const auto commandDef = constructPulseCommandDef(h);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // H = U2(0, pi) = U3(pi/2, 0, pi)
    const auto asU2 = constructUCmdDefComposite(h.bits()[0], "u2");
    auto hCmdDef =
        (*asU2)({0.0, M_PI});
    pulseComposite->addInstruction(hCmdDef);
  }
}

void PulseMappingVisitor::visit(CNOT &cnot) {
  const auto commandDef = constructPulseCommandDef(cnot);
  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  }
}

void PulseMappingVisitor::visit(Rz &rz) {
  const auto commandDef = constructPulseCommandDef(rz);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // Rz(theta) = U1(theta) = U3(0, 0, theta)
    const auto asU1 = constructUCmdDefComposite(rz.bits()[0], "u1");
    auto rzCmdDef = (*asU1)({rz.getParameter(0).as<double>()});
    pulseComposite->addInstruction(rzCmdDef);
  }
}

void PulseMappingVisitor::visit(Ry &ry) {
  const auto commandDef = constructPulseCommandDef(ry);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // Ry(theta) = U3(theta, 0, 0)
    const auto asU3 = constructUCmdDefComposite(ry.bits()[0], "u3");
    auto ryCmdDef = (*asU3)({ry.getParameter(0).as<double>(), 0.0, 0.0});
    pulseComposite->addInstruction(ryCmdDef);
  }
}

void PulseMappingVisitor::visit(Rx &rx) {
  const auto commandDef = constructPulseCommandDef(rx);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // Rx(theta) = U3(theta, -pi/2, pi/2)
    const auto asU3 = constructUCmdDefComposite(rx.bits()[0], "u3");
    auto rxCmdDef =
        (*asU3)({rx.getParameter(0).as<double>(), -M_PI/ 2.0,
                 M_PI/ 2.0});
    pulseComposite->addInstruction(rxCmdDef);
  }
}

void PulseMappingVisitor::visit(X &x) {
  const auto commandDef = constructPulseCommandDef(x);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // X = U3(pi, 0, pi)
    const auto asU3 = constructUCmdDefComposite(x.bits()[0], "u3");
    auto xCmdDef = (*asU3)({M_PI, 0.0, M_PI});
    pulseComposite->addInstruction(xCmdDef);
  }
}

void PulseMappingVisitor::visit(Y &y) {
  const auto commandDef = constructPulseCommandDef(y);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // Y = U3(pi, pi/2, pi/2)
    const auto asU3 = constructUCmdDefComposite(y.bits()[0], "u3");
    auto yCmdDef = (*asU3)({M_PI, M_PI/ 2.0,
                            M_PI/ 2.0});
    pulseComposite->addInstruction(yCmdDef);
  }
}

void PulseMappingVisitor::visit(Z &z) {
  const auto commandDef = constructPulseCommandDef(z);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // Z = U1(pi) = U3(0, 0, pi)
    const auto asU1 = constructUCmdDefComposite(z.bits()[0], "u1");
    auto zCmdDef = (*asU1)({M_PI});
    pulseComposite->addInstruction(zCmdDef);
  }
}

void PulseMappingVisitor::visit(CY &cy) {
  // CY(a,b) = sdg(b); cx(a,b); s(b);
  auto sdg = std::make_shared<Sdg>(cy.bits()[1]);
  auto cx = std::make_shared<CNOT>(cy.bits());
  auto s = std::make_shared<S>(cy.bits()[1]);
  visit(*sdg);
  visit(*cx);
  visit(*s);
}

void PulseMappingVisitor::visit(CZ &cz) {
  // CZ(a,b) =  H(b); CX(a,b); H(b);
  auto h1 = std::make_shared<Hadamard>(cz.bits()[1]);
  auto cx = std::make_shared<CNOT>(cz.bits());
  auto h2 = std::make_shared<Hadamard>(cz.bits()[0]);
  visit(*h1);
  visit(*cx);
  visit(*h2);
}

void PulseMappingVisitor::visit(Swap &s) {
  // SWAP(a,b) =  CX(a,b); CX(b,a); CX(a,b);
  auto cx1 = std::make_shared<CNOT>(s.bits());
  auto cx2 = std::make_shared<CNOT>(s.bits()[1], s.bits()[0]);
  auto cx3 = std::make_shared<CNOT>(s.bits());
  visit(*cx1);
  visit(*cx2);
  visit(*cx3);
}

void PulseMappingVisitor::visit(CRZ &crz) {
  // CRZ(theta)(a,b) = U1(theta/2)(b); CX(a,b); U3(-theta/2)(b);
  // CX(a,b);
  const double theta = crz.getParameter(0).as<double>();
  {
    const auto asU1 = constructUCmdDefComposite(crz.bits()[1], "u1");
    auto cmdDef = (*asU1)({theta / 2.0});
    pulseComposite->addInstruction(cmdDef);
  }
  {
    auto cx = std::make_shared<CNOT>(crz.bits());
    visit(*cx);
  }
  {
    const auto asU1 = constructUCmdDefComposite(crz.bits()[1], "u1");
    auto cmdDef = (*asU1)({-theta / 2.0});
    pulseComposite->addInstruction(cmdDef);
  }
  {
    auto cx = std::make_shared<CNOT>(crz.bits());
    visit(*cx);
  }
}

void PulseMappingVisitor::visit(CH &ch) {
  // CH(a,b) = S(b); H(b); T(b); CX(a,b); Tdg(b); H(b); Sdg(b);
  {
    auto s = std::make_shared<S>(ch.bits()[1]);
    visit(*s);
  }
  {
    auto h = std::make_shared<Hadamard>(ch.bits()[1]);
    visit(*h);
  }
  {
    auto t = std::make_shared<T>(ch.bits()[1]);
    visit(*t);
  }
  {
    auto cx = std::make_shared<CNOT>(ch.bits());
    visit(*cx);
  }
  {
    auto tdg = std::make_shared<Tdg>(ch.bits()[1]);
    visit(*tdg);
  }
  {
    auto h = std::make_shared<Hadamard>(ch.bits()[1]);
    visit(*h);
  }
  {
    auto sdg = std::make_shared<Sdg>(ch.bits()[1]);
    visit(*sdg);
  }
}

void PulseMappingVisitor::visit(S &s) {
  const auto commandDef = constructPulseCommandDef(s);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // S = U1(pi/2) = U3(0,0,pi/2)
    const auto asU1 = constructUCmdDefComposite(s.bits()[0], "u1");
    auto sCmdDef = (*asU1)({M_PI/ 2.0});
    pulseComposite->addInstruction(sCmdDef);
  }
}

void PulseMappingVisitor::visit(Sdg &sdg) {
  const auto commandDef = constructPulseCommandDef(sdg);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // S-dagger = U1(-pi/2) = U3(0,0,-pi/2)
    const auto asU1 = constructUCmdDefComposite(sdg.bits()[0], "u1");
    auto sdgCmdDef = (*asU1)({-M_PI/ 2.0});
    pulseComposite->addInstruction(sdgCmdDef);
  }
}

void PulseMappingVisitor::visit(T &t) {
  const auto commandDef = constructPulseCommandDef(t);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // T = U1(pi/4) = U3(0,0,pi/4)
    const auto asU1 = constructUCmdDefComposite(t.bits()[0], "u1");
    auto tCmdDef = (*asU1)({M_PI/ 4.0});
    pulseComposite->addInstruction(tCmdDef);
  }
}

void PulseMappingVisitor::visit(Tdg &tdg) {
  const auto commandDef = constructPulseCommandDef(tdg);

  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  } else {
    // T-dagger = U1(-pi/4) = U3(0,0,-pi/4)
    const auto asU1 = constructUCmdDefComposite(tdg.bits()[0], "u1");
    auto tdgCmdDef = (*asU1)({-M_PI/ 4.0});
    pulseComposite->addInstruction(tdgCmdDef);
  }
}

void PulseMappingVisitor::visit(CPhase &cphase) {
  // Ref:
  // gate cu1(lambda) a, b {
  //   u1(lambda / 2) a;
  //   cx a, b;
  //   u1(-lambda / 2) b;
  //   cx a, b;
  //   u1(lambda / 2) b;
  // }
  const double lambda = cphase.getParameter(0).as<double>();
  {
    const auto asU1 = constructUCmdDefComposite(cphase.bits()[0], "u1");
    auto cmdDef = (*asU1)({lambda / 2.0});
    pulseComposite->addInstruction(cmdDef);
  }
  {
    auto cx = std::make_shared<CNOT>(cphase.bits());
    visit(*cx);
  }
  {
    const auto asU1 = constructUCmdDefComposite(cphase.bits()[1], "u1");
    auto cmdDef = (*asU1)({-lambda / 2.0});
    pulseComposite->addInstruction(cmdDef);
  }
  {
    auto cx = std::make_shared<CNOT>(cphase.bits());
    visit(*cx);
  }
  {
    const auto asU1 = constructUCmdDefComposite(cphase.bits()[1], "u1");
    auto cmdDef = (*asU1)({lambda / 2.0});
    pulseComposite->addInstruction(cmdDef);
  }
}

void PulseMappingVisitor::visit(Identity &i) {
  // Ignore for now
}

void PulseMappingVisitor::visit(U &u) {
  const auto asU3 = constructUCmdDefComposite(u.bits()[0], "u3");
  // Just pass the params to the cmddef.
  std::vector<double> params;
  for (const auto &param : u.getParameters()) {
    params.emplace_back(param.as<double>());
  }

  auto uCmdDef = (*asU3)(params);
  pulseComposite->addInstruction(uCmdDef);
}

void PulseMappingVisitor::visit(Measure &measure) {
  const auto commandDef = constructPulseCommandDef(measure);
  if (xacc::hasContributedService<xacc::Instruction>(commandDef)) {
    auto pulseInst = xacc::getContributedService<xacc::Instruction>(commandDef);
    pulseComposite->addInstruction(pulseInst);
  }
}

std::string
PulseMappingVisitor::constructPulseCommandDef(xacc::quantum::Gate &in_gate) {
  const auto getGateCommandDefName =
      [](xacc::quantum::Gate &in_gate) -> std::string {
    std::string gateName = in_gate.name();
    std::transform(gateName.begin(), gateName.end(), gateName.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (gateName == "cnot") {
      return "cx";
    } else {
      return gateName;
    }
  };

  std::string result = "pulse::" + getGateCommandDefName(in_gate);

  for (const auto &qIdx : in_gate.bits()) {
    result += ("_" + std::to_string(qIdx));
  }

  return result;
}
} // namespace quantum
} // namespace xacc