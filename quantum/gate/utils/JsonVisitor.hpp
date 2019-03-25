/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef QUANTUM_GATE_JSONVISITOR_HPP_
#define QUANTUM_GATE_JSONVISITOR_HPP_

#include <memory>
#include "AllGateVisitor.hpp"

namespace xacc {

namespace quantum {

template<typename Writer, typename Buffer>
class JsonVisitor : public AllGateVisitor {

protected:
  std::shared_ptr<Buffer> buffer;
  std::shared_ptr<Writer> writer;
  std::shared_ptr<Function> function;
  std::shared_ptr<InstructionIterator> topLevelInstructionIterator;

  std::vector<std::shared_ptr<Function>> functions;

public:
  JsonVisitor(std::shared_ptr<xacc::Function> f);
  JsonVisitor(std::vector<std::shared_ptr<xacc::Function>> fs);
  std::string write();
  void visit(Identity &i) { baseGateInst(dynamic_cast<GateInstruction &>(i)); }

  void visit(Hadamard &h) { baseGateInst(dynamic_cast<GateInstruction &>(h)); }

  void visit(CNOT &cn) { baseGateInst(dynamic_cast<GateInstruction &>(cn)); }

  void visit(CZ &cz) { baseGateInst(dynamic_cast<GateInstruction &>(cz)); }

  void visit(Swap &s) { baseGateInst(dynamic_cast<GateInstruction &>(s)); }

  void visit(Rz &rz);

  void visit(Rx &rx);

  void visit(Ry &ry);

  void visit(CPhase &cp);
  void visit(ConditionalFunction &cn);
  void visit(Measure &cn);

  void visit(X &cn) { baseGateInst(dynamic_cast<GateInstruction &>(cn)); }

  void visit(Y &y) { baseGateInst(dynamic_cast<GateInstruction &>(y)); }

  void visit(Z &z) { baseGateInst(dynamic_cast<GateInstruction &>(z)); }

  void visit(U &u);

  void visit(GateFunction &function) {}

protected:
  void baseGateInst(GateInstruction &inst, bool endObject = true);
};
} // namespace quantum
} // namespace xacc
#endif
