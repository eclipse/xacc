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
#include "DefaultParameterSetter.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {

std::list<std::shared_ptr<DWQMI>> DefaultParameterSetter::setParameters(
    std::shared_ptr<Graph> problemGraph,
    std::shared_ptr<Graph> hardwareGraph, Embedding embedding) {

  std::list<std::shared_ptr<DWQMI>> instList;
  auto nHardwareVerts = hardwareGraph->order();
  auto nProblemVerts = problemGraph->order();

  auto countEdgesBetweenSubTrees = [&](std::vector<int> Ti,
                                       std::vector<int> Tj) -> int {
    int nEdges = 0;
    for (auto i : Ti) {
      for (auto j : Tj) {
        if (hardwareGraph->edgeExists(i, j)) {
          nEdges++;
        }
      }
    }
    return nEdges;
  };

  auto subTreeContains = [](std::vector<int> tree, int i) -> bool {
    return std::find(tree.begin(), tree.end(), i) != tree.end();
  };

  // Setup the hardware bias values
  for (auto &embKv : embedding) {
    auto probVert = embKv.first;
    auto hardwareMapping = embKv.second;
    auto newBias = problemGraph->getVertexProperties(probVert).get<double>("bias") /
                   hardwareMapping.size();
    for (auto h : hardwareMapping) {
      auto embeddedInst = std::make_shared<DWQMI>(h, h, newBias);
      instList.push_back(embeddedInst);
    }
  }

  double chain_strength = 1.0;
  if (xacc::optionExists("chain-strength")) {
      chain_strength = std::stod(xacc::getOption("chain-strength"));
  }

  for (int i = 0; i < nHardwareVerts; i++) {
    for (int j = 0; j < nHardwareVerts; j++) {
      if (hardwareGraph->edgeExists(i, j) && i < j && i != j) {
        for (int pi = 0; pi < nProblemVerts; pi++) {
          for (int pj = 0; pj < nProblemVerts; pj++) {

            auto Ti = embedding[pi];
            auto Tj = embedding[pj];

            if (subTreeContains(Ti, i) && subTreeContains(Tj, j)) {
              double newWeight = 0.0;
              if (pi != pj) {
                // If problem edge does not exist,
                // Graph.getEdgeWeight retusn 0.0;
                newWeight = problemGraph->getEdgeWeight(pi, pj) /
                            countEdgesBetweenSubTrees(Ti, Tj);
              } else {
                // ferro-magnetic coupling parameter that ensures that physical
                // qubits representing one logical qubit remain highly
                // correlated.
                for (auto neighbor : problemGraph->getNeighborList(pi)) {
                  newWeight +=
                      std::fabs(problemGraph->getEdgeWeight(pi, neighbor));
                }
                newWeight += problemGraph->getVertexProperties(pi).get<double>("bias");
                newWeight *= -1.0 * chain_strength;
              }

              if (std::fabs(newWeight) > 1e-4) {
                auto embeddedInst = std::make_shared<DWQMI>(i, j, newWeight);
                instList.push_back(embeddedInst);
              }
            }
          }
        }
      }
    }
  }

  return instList;
}
} // namespace quantum
} // namespace xacc
