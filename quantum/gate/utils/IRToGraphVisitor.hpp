/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_IRTOGRAPHVISITOR_HPP_
#define QUANTUM_IRTOGRAPHVISITOR_HPP_

#include "AllGateVisitor.hpp"
#include "Graph.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {

using CircuitNode = HeterogeneousMap;

class IRToGraphVisitor : public AllGateVisitor {

protected:
  std::shared_ptr<Graph> graph;

  std::map<int, CircuitNode> qubitToLastNode;

  std::size_t id = 0;

  void addSingleQubitGate(Gate &inst);
  void addTwoQubitGate(Gate &inst);

public:
  IRToGraphVisitor(const int nQubits);
  std::shared_ptr<Graph> getGraph();

  void visit(Hadamard &h) override { addSingleQubitGate(h); }

  void visit(Identity &i) override { addSingleQubitGate(i);}

  void visit(CZ &cz) override { addTwoQubitGate(cz); }

  void visit(CNOT &cn) override { addTwoQubitGate(cn); }

  void visit(X &x) override { addSingleQubitGate(x); }

  void visit(Y &y) override { addSingleQubitGate(y); }

  void visit(Z &z) override { addSingleQubitGate(z); }

  void visit(Measure &m) override { addSingleQubitGate(m); }

  void visit(Rx &rx) override { addSingleQubitGate(rx); }

  void visit(Ry &ry) override { addSingleQubitGate(ry); }

  void visit(Rz &rz) override { addSingleQubitGate(rz); }

  void visit(CPhase &cp) override { addTwoQubitGate(cp); }

  void visit(Swap &s) override { addTwoQubitGate(s); }

  void visit(U &u) override { addSingleQubitGate(u); }

  void visit(S &s) override { addSingleQubitGate(s);}
  void visit(Sdg &sdg) override {addSingleQubitGate(sdg);}
  void visit(T &t) override {addSingleQubitGate(t);}
  void visit(Tdg &tdg) override {addSingleQubitGate(tdg);}
  void visit(Rphi &rphi) override { addSingleQubitGate(rphi); }
  void visit(U1 &u1) override { addSingleQubitGate(u1); }

  void visit(CRZ &crz) override { addTwoQubitGate(crz); }
  void visit(CH &ch) override { addTwoQubitGate(ch); }
  void visit(CY &cy) override { addTwoQubitGate(cy); }
  void visit(fSim &fsim) override { addTwoQubitGate(fsim); }
  void visit(iSwap &iswap) override { addTwoQubitGate(iswap); }
  void visit(RZZ &rzz) override { addTwoQubitGate(rzz); }
  void visit(XX &xx) override { addTwoQubitGate(xx); }
  void visit(XY &xy) override { addTwoQubitGate(xy); }

  // Base gate visitor, i.e. none of the concrete gates can match.
  // We need to assert here because we cannot generate a graph when there are unknown gates.
  // e.g. there is an 1-1 mapping between node Id (in the graph) and instruction counter (in the circuit),
  // hence, we must fail the graph generation when we encounter an unknown gate.
  void visit(Gate &gate) { xacc::error("Unknown gate '" + gate.name() + "' encountered while processing input circuit. Please provide the IRToGraph implementation for this gate.");}

  virtual ~IRToGraphVisitor() {}
};
} // namespace quantum
} // namespace xacc

#endif
