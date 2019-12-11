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
 *   Sarah Powers - based off initial implementation in QCS by Alexander J. McCaskey
 *******************************************************************************/
#include "MapToPhysical.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "EmbeddingAlgorithm.hpp"

namespace xacc {
namespace quantum {

  void MapToPhysical::apply(std::shared_ptr<CompositeInstruction> function,
                            const std::shared_ptr<Accelerator> acc,
                            const HeterogeneousMap &options) {

    auto embeddingAlgorithm = xacc::getService<EmbeddingAlgorithm>("cmr");

    /* BUILD HW GRAPH */
    // get hardware edges
    auto hwConnectivity = acc->getConnectivity();

    std::map<int, int> physical2Logical, logical2Physical;
    int counter = 0;
    std::set<int> nUniqueBits;
    for (auto &edge : hwConnectivity) {
      nUniqueBits.insert(edge.first);
      nUniqueBits.insert(edge.second);
    }

    // create mapping
    for (auto &i : nUniqueBits) {
      physical2Logical.insert({i, counter});
      logical2Physical.insert({counter, i});
      counter++;
    }
    
    // build up xacc::Graph for the hardware
    int nBits = nUniqueBits.size();

    auto hwGraph = xacc::getService<Graph>("boost-ugraph");
    for (int i =0; i < nBits; i++) {
      HeterogeneousMap m{std::make_pair("bias", 1.0)};
      hwGraph->addVertex(m);
    }
  
    for (auto &edge : hwConnectivity) {
      hwGraph->addEdge(edge.first, edge.second);
    }


    /* BUILD PB GRAPH from IR */

    // passed in a CompositeInstruction
    auto logicalGraph = function->toGraph();
    InstructionIterator it(function);
    std::set<int> nUniquePbBits;

    std::vector<std::pair<int,int>> pbEdges;
    while (it.hasNext()) {
      // Get next node in the tree
      auto nextInst = it.next();
      if (nextInst->isEnabled() && nextInst->bits().size() == 2) {
        pbEdges.push_back({nextInst->bits()[0], nextInst->bits()[1]});
        nUniquePbBits.insert(nextInst->bits()[0]);
        nUniquePbBits.insert(nextInst->bits()[1]);
      }
    }

    auto nPbBits = nUniquePbBits.size();
    // create xacc::Graph where everything gets stored
    auto pbGraph = xacc::getService<Graph>("boost-ugraph");
    
    for (int i = 0; i < nPbBits; i++) {
      HeterogeneousMap m{std::make_pair("bias", 1.0)};
      pbGraph->addVertex(m);
    }

    for (auto &inst : pbEdges) {
      if (!pbGraph->edgeExists(inst.first, inst.second)) {
        pbGraph->addEdge(inst.first, inst.second, 1.0);
      }
    }

    // Compute the minor graph embedding
    auto embedding =  embeddingAlgorithm->embed(pbGraph, hwGraph);
    
    std::vector<std::size_t> physicalMap;
    // check the output
    for (auto &kv : embedding) {
      if (kv.second.size() > 1) {
        xacc::error("Invalid logical to physical qubit mapping.");
      }
      physicalMap.push_back(kv.second[0]);
    }
    // embeddinge values get stored over initial (clobbering)
    function->mapBits(physicalMap);

    return;
  }

}
}

