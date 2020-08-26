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
 *   Thien Nguyen
 *******************************************************************************/
#include "triq_placement.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "circuit.hpp"
#include "BackendMachine.hpp"
#include "NoiseModel.hpp"
namespace {
std::string xaccGateToTriqGate(const std::string &in_xaccGateName) {
  if (in_xaccGateName == "Measure") {
    return "MeasZ";
  } else if (in_xaccGateName == "Tdg") {
    return "Tdag";
  } else if (in_xaccGateName == "Sdg") {
    return "Sdag";
  } else if (in_xaccGateName == "Rx") {
    return "RX";
  } else if (in_xaccGateName == "Ry") {
    return "RY";
  } else if (in_xaccGateName == "Rz") {
    return "RZ";
  } else if (in_xaccGateName == "Swap") {
    return "SWAP";
  }
  return in_xaccGateName;
}
} // namespace
namespace xacc {
namespace quantum {

void TriQPlacement::apply(std::shared_ptr<CompositeInstruction> function,
                          const std::shared_ptr<Accelerator> acc,
                          const HeterogeneousMap &options) {
  // Default option:
  auto compileAlgo = CompileAlgorithm::CompileOpt;
  if (options.stringExists("compile-algorithm")) {
    const auto selectedAlgo = options.getString("compile-algorithm");
    if (selectedAlgo == "CompileOpt") {
      compileAlgo = CompileAlgorithm::CompileOpt;
    } else if (selectedAlgo == "CompileDijkstra") {
      compileAlgo = CompileAlgorithm::CompileDijkstra;
    } else if (selectedAlgo == "CompileRevSwaps") {
      compileAlgo = CompileAlgorithm::CompileRevSwaps;
    }
  }

  std::string backendName;
  if (options.stringExists("backend")) {
    backendName = options.getString("backend");
  }
  // Enable offline testing via JSON loading as well
  std::string backendJson;
  if (options.stringExists("backend-json")) {
    backendJson = options.getString("backend-json");
  } else if (acc && acc->getProperties().stringExists("total-json")) {
    // If this is a remote IBM Accelerator, grab the backend JSON automatically.
    backendJson = acc->getProperties().getString("total-json");
  }

  if (backendName.empty() && backendJson.empty()) {
    // Nothing we can do.
    xacc::warning("No backend information was provided. Skipped!");
    return;
  }

  // Step 1: make sure the circuit is in OpenQASM dialect,
  // i.e. using the Staq compiler to translate.

  // Step 2: construct TriQ's Circuit
  Circuit triqCirc;
  const auto nbQubits = function->nLogicalBits();
  for (size_t i = 0; i < nbQubits; i++) {
    triqCirc.qubits.push_back(new ProgQubit(i));
  }
  // Assuming the circuit has been flattened (after Staq translation)
  for (size_t instIdx = 0; instIdx < function->nInstructions(); ++instIdx) {
    auto xaccInst = function->getInstruction(instIdx);
    const std::string gateName = xaccInst->name();
    ::Gate *pG = ::Gate::create_new_gate(xaccGateToTriqGate(gateName));
    pG->id = instIdx;
    // Make sure the expected number of qubits matched for each gate
    assert(xaccInst->bits().size() == pG->nvars);
    for (const auto& qubitIdx : xaccInst->bits()) {
      pG->vars.push_back(triqCirc.qubits[qubitIdx]);
    }
    triqCirc.gates.push_back(pG);
  }
  // Build DAG info
  auto graphView = function->toGraph();
  for (size_t i = 1; i < graphView->order() - 1; ++i) {
    auto node = graphView->getVertexProperties(i);
    const auto nodeName = node.getString("name");
    const auto nodeId = node.get<std::size_t>("id");
    const auto gateId = nodeId - 1;
    assert(gateId < triqCirc.gates.size());
    auto neighborNodeIds = graphView->getNeighborList(nodeId);
    for (const auto &neighborNodeId : neighborNodeIds) {
      if (neighborNodeId < graphView->order() - 1) {
        auto nextNode = graphView->getVertexProperties(neighborNodeId);
        const auto nextGateId = nextNode.get<std::size_t>("id") - 1;
        assert(nextGateId < triqCirc.gates.size());
        assert(gateId != nextGateId);
        // Add DAG connection information
        triqCirc.gates[nextGateId]->pred.emplace_back(triqCirc.gates[gateId]);
        triqCirc.gates[gateId]->succ.emplace_back(triqCirc.gates[nextGateId]);
      }
    }
  }
  // DEBUG:
  triqCirc.print_gates();
  auto backendNoiseModel = xacc::getService<xacc::NoiseModel>("IBM");
  if (!backendName.empty()) {
    backendNoiseModel->initialize({{"backend", backendName}});
  } else {
    backendNoiseModel->initialize({{"backend-json", backendJson}});
  }

  BackendMachine backendModel(*backendNoiseModel);
  const auto resultQasm = runTriQ(triqCirc, backendModel, compileAlgo);
}

std::string TriQPlacement::runTriQ(Circuit &program, Machine &machine,
                                   int algorithmSelector) const {
  // TODO:
  return "";
}
} // namespace quantum
} // namespace xacc
