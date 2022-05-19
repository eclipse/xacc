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
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "headers.hpp"
#include "circuit.hpp"
#include "mapper.hpp"
#include "machine.hpp"
#include "pattern.hpp"
#include "backtrack.hpp"
#include "targetter.hpp"
#include "optimize_1q.hpp"
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

  // Approximation factor to determine the stopping condition
  // of the Z3 solver.
  // The smaller this value is, the longer it takes to optimize.
  // Must be > 1.0  
  double approxFactor = 1.001;
  if (options.keyExists<double>("approx-factor")) {
    approxFactor = options.get<double>("approx-factor");
    if (approxFactor <= 1.0) {
      xacc::error("Invalid 'approx-factor'. Must be > 1.0");
    }
  }
  // Enable offline testing via JSON loading as well
  std::string backendJson;
  if (options.stringExists("backend-json")) {
    backendJson = options.getString("backend-json");
  } else if (acc && acc->getProperties().stringExists("total-json")) {
    // If this is a remote IBM Accelerator, grab the backend JSON automatically.
    backendJson = acc->getProperties().getString("total-json");
  }

  xacc::NoiseModel *providedNoiseModel = nullptr;
  if (options.pointerLikeExists<xacc::NoiseModel>("backend-noise-model")) {
    providedNoiseModel =
        options.getPointerLike<xacc::NoiseModel>("backend-noise-model");
  }

  if (backendName.empty() && backendJson.empty() && !providedNoiseModel) {
    // Nothing we can do.
    xacc::warning("No backend information was provided. Skipped!");
    return;
  }
  // Step 1: make sure the circuit is in OpenQASM dialect,
  // i.e. using the Staq compiler to translate.
  auto staq = xacc::getCompiler("staq");
  const auto openQASMSrc = staq->translate(function);
  // std::cout << "Before placement: \n" << openQASMSrc << "\n";
  auto ir = staq->compile(openQASMSrc);
  function->clear();
  function->addInstructions(ir->getComposites()[0]->getInstructions());

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
    const auto nodeId = i;
    const auto gateId = nodeId - 1;
    assert(gateId < triqCirc.gates.size());
    auto neighborNodeIds = graphView->getNeighborList(nodeId);
    for (const auto &neighborNodeId : neighborNodeIds) {
      if (neighborNodeId < graphView->order() - 1) {
        const auto nextGateId = neighborNodeId - 1;
        assert(nextGateId < triqCirc.gates.size());
        assert(gateId != nextGateId);
        // Add DAG connection information
        triqCirc.gates[nextGateId]->pred.emplace_back(triqCirc.gates[gateId]);
        triqCirc.gates[gateId]->succ.emplace_back(triqCirc.gates[nextGateId]);
      }
    }
  }
  // DEBUG:
  // triqCirc.print_gates();
  auto backendNoiseModel = providedNoiseModel
                               ? xacc::as_shared_ptr(providedNoiseModel)
                               : xacc::getService<xacc::NoiseModel>("IBM");
  if (!backendName.empty()) {
    backendNoiseModel->initialize({{"backend", backendName}});
  } else {
    backendNoiseModel->initialize({{"backend-json", backendJson}});
  }

  BackendMachine backendModel(*backendNoiseModel);
  // Step 3: Run TriQ (placement + optimize) for this backend
  // TriQ outputs a lot to std::cout, hence we need to bypass its logs. 
  std::cout << std::flush;
  auto origBuf = std::cout.rdbuf();
  if (!xacc::verbose) {
    std::cout.rdbuf(NULL);
  }
  const auto resultQasm = runTriQ(triqCirc, backendModel, compileAlgo, approxFactor);
  std::cout.rdbuf(origBuf);
  // DEBUG:
  // std::cout << "After placement: \n" << resultQasm << "\n";
  // Step 4: Reconstruct the Composite
  auto newIr = staq->compile(resultQasm);
  function->clear();
  function->addInstructions(newIr->getComposites()[0]->getInstructions());
}

std::string TriQPlacement::runTriQ(Circuit &program, Machine &machine,
                                   int algorithmSelector, double approxFactor) const {
  ::Mapper pMapper(&machine, &program);
  pMapper.set_config(MapSum, VarUnique);
  pMapper.config.approx_factor = approxFactor;
  pMapper.map_with_z3();
  pMapper.print_stats();
  const ::CompileAlgorithm selectedAlgo = static_cast<::CompileAlgorithm>(algorithmSelector);
  auto torder = program.topological_ordering();
  program.enforce_topological_ordering(torder);
  auto bsol = [&](){
    int cx_count = 0;
    for (auto &gi : program.gates) {
      if (gi->nvars == 2) {
        cx_count++;
      }
    }
    if (cx_count < 20) {
      ::Backtrack B(&program, &machine);
      B.init(&pMapper.qubit_map, torder);
      B.solve(B.root, 1);
      if (selectedAlgo == CompileOpt) {
        return B.get_solution(1);
      } else {
        return B.get_solution(0);
      }
    } else {
      BacktrackFiniteLookahead B(&program, &machine, *torder, pMapper.qubit_map, 10);
      return B.solve();
    }
  }();
  
  program.add_scheduling_dependency(torder, bsol);
  auto torder_new = program.topological_ordering();
  XaccTargetter Tgen(&machine, &program, &pMapper.qubit_map, torder_new, bsol);
  auto C_trans = Tgen.map_and_insert_swap_operations();
  // We don't need to run single-qubit merge optimization here,
  // we have a separate pass for this and the TriQ implementation
  // is not very stable (crash)
  // OptimizeSingleQubitOps sq_opt(C_trans);
  // auto C_1q_opt = sq_opt.test_optimize();
  char fnTemplate[] = "/tmp/CircuitXXXXXX";
  if (mkstemp(fnTemplate) == -1) {
    xacc::error("Failed to create a temporary file.");
  }
  const std::string outFilename(fnTemplate);
  Tgen.print_code(C_trans, outFilename);
  
  if (C_trans) {
    delete C_trans;
  }
  delete torder;
  // Load the output QASM:
  std::ifstream outFile(outFilename);
  std::string optSrc((std::istreambuf_iterator<char>(outFile)), std::istreambuf_iterator<char>());
  // Delete the temporary file
  remove(outFilename.c_str());  
  return optSrc;
}
} // namespace quantum
} // namespace xacc
