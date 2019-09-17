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
#include "CircuitOptimizer.hpp"
#include "Graph.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "CommonGates.hpp"
#include "Circuit.hpp"
#include "GateIR.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {

std::shared_ptr<IR> CircuitOptimizer::transform(std::shared_ptr<IR> ir) {

  if (!xacc::optionExists("circuit-opt-silent")) {
    xacc::info("Executing XACC Circuit Optimizer.");
  }

  std::shared_ptr<IRProvider> irProvider = xacc::getService<IRProvider>("quantum");
  auto gateir = std::dynamic_pointer_cast<GateIR>(ir);
  if (!gateir)
    xacc::error(
        "Invalid IR instance passed to Circuit Optimizer, must be gate.");

  auto isRotation = [](const std::string inst) {
    return inst == "Rz" || inst == "Ry" || inst == "Rx";
  };

  auto ipToDouble = [](xacc::InstructionParameter p) {
    if (p.which() == 0) {
      return (double)p.as<int>();
    } else if (p.which() == 1) {
      return p.as<double>();
    } else {
      std::stringstream s;
      s << p.toString();
      xacc::error("CircuitOptimizer: invalid gate parameter " +
                  std::to_string(p.which()) + ", " + s.str());
    }
    return 0.0;
  };

  int nTries = 2;

  for (auto &k : gateir->getComposites()) {
    for (int j = 0; j < nTries; ++j) {
      auto gateFunction = std::dynamic_pointer_cast<Circuit>(k);

      // Remove any zero rotations (Rz(theta<1e-12))
      for (int i = 0; i < gateFunction->nInstructions(); i++) {
        auto inst = gateFunction->getInstruction(i);
        if (isRotation(inst->name())) {
          auto param = inst->getParameter(0);
          if (!param.isVariable()) {
              double val = ipToDouble(param);
              if (std::fabs(val) < 1e-12) {
                 inst->disable();
            }
          }
        }
      }
      // Remove all CNOT(p,q) CNOT(p,q) pairs
      while (true) {
        bool modified = false;
        gateFunction = std::dynamic_pointer_cast<Circuit>(
            gateFunction->enabledView());
        auto graphView = gateFunction->toGraph();
        for (int i = 1; i < graphView->order() - 2; i++) {
          auto node = graphView->getVertexProperties(i);
          if (node.getString("name") == "CNOT" &&
              gateFunction->getInstruction(node.get<int>("id") - 1)->isEnabled()) {
            auto nAsVec = graphView->getNeighborList(node.get<int>("id"));
            // std::vector<int> nAsVec(neighbors.begin(), neighbors.end());

            if (nAsVec[0] == nAsVec[1]) {
              gateFunction->getInstruction(node.get<int>("id") - 1)->disable();
              gateFunction->getInstruction(nAsVec[0] - 1)->disable();
              modified = true;
              break;
            }
          }
        }
        if (!modified)
          break;
      }
      // Remove all H(p)H(p) pairs
      while (true) {
        bool modified = false;
        gateFunction = std::dynamic_pointer_cast<Circuit>(
            gateFunction->enabledView());
        auto graphView = gateFunction->toGraph();

        for (int i = 1; i < graphView->order() - 2; ++i) {
          auto node = graphView->getVertexProperties(i);

          auto nAsVec = graphView->getNeighborList(node.get<int>("id"));
        //   std::vector<int> nAsVec(adj.begin(), adj.end());
          if (nAsVec.size() == 1) {
            auto nextNode = graphView->getVertexProperties(nAsVec[0]);
            if (node.getString("name") == "H" && nextNode.getString("name") == "H") {
              gateFunction->getInstruction(node.get<int>("id") - 1)->disable();
              gateFunction->getInstruction(nAsVec[0] - 1)->disable();
              modified = true;
              break;
            }
          }
        }

        if (!modified)
          break;
      }
      // Merge adjacent rotation gates Rz()Rz() or Rx()Rx() or Ry()Ry()
      while (true) {
        bool modified = false;
        gateFunction = std::dynamic_pointer_cast<Circuit>(gateFunction->enabledView());
        auto graphView = gateFunction->toGraph();
        // start/end nodes get added: start at 1, stop at n-2 b/c last will be n-2 neighbor
        for (int i = 1; i < graphView->order() - 2; i++) {
          auto node = graphView->getVertexProperties(i);
          auto nAsVec = graphView->getNeighborList(node.get<int>("id"));
          // if it has more than 1 neighbor, don't consider
          if (nAsVec.size() == 1) {
            auto nextNode = graphView->getVertexProperties(nAsVec[0]);
            if (isRotation(node.getString("name")) && isRotation(nextNode.getString("name"))
                  && node.getString("name") == nextNode.getString("name")) {
              auto val1 = ipToDouble(gateFunction->getInstruction(node.get<int>("id") - 1)->getParameter(0));
              auto val2 = ipToDouble(gateFunction->getInstruction(nextNode.get<int>("id") - 1)->getParameter(0));

              if (std::fabs(val1+val2) < 1e-12) {
                gateFunction->getInstruction(node.get<int>("id") - 1)->disable();
                gateFunction->getInstruction(nextNode.get<int>("id") - 1)->disable();
              }
              else {
		        InstructionParameter tmp(val1+val2);
                gateFunction->getInstruction(node.get<int>("id") - 1)->setParameter(0, tmp);
                gateFunction->getInstruction(nextNode.get<int>("id") - 1)->disable();
              }
              modified = true;
              break;

            }
          }
        }

        if (!modified)
          break;
      }
    }
  }
  return ir;
}

} // namespace quantum
} // namespace xacc
