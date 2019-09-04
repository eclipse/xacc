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
#include "QFT.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace xacc {

namespace circuits {

const std::vector<std::string> QFT::requiredKeys() {
  return {"nq", "start", "end"};
}
bool QFT::expand(const xacc::HeterogeneousMap &runtimeOptions) {

  int start = 0, end = -1, nqubits;
  if (!runtimeOptions.keyExists<int>("nq")) {
    if (!runtimeOptions.keyExists<int>("start") &&
        !runtimeOptions.keyExists<int>("end")) {
      return false;
    } else {
      if (runtimeOptions.keyExists<int>("start")) {
        start = runtimeOptions.get<int>("start");
      }

      if (runtimeOptions.keyExists<int>("end")) {
        end = runtimeOptions.get<int>("end");
      }
    }
  } else {
    end = runtimeOptions.get<int>("nq");
  }

  if (end == -1) {
    std::stringstream ss;
    runtimeOptions.print<int, std::string>(ss);
    xacc::error("Invalid QFT runtimeOptions:\n" + ss.str());
  }

  auto gateRegistry = xacc::getService<IRProvider>("quantum");

  auto bitReversal = [&](std::vector<std::size_t> qubits)
      -> std::vector<std::shared_ptr<Instruction>> {
    std::vector<std::shared_ptr<Instruction>> swaps;
    auto endStart = qubits.size() - 1;
    for (auto i = 0; i < std::floor(qubits.size() / 2.0); ++i) {
      swaps.push_back(gateRegistry->createInstruction(
          "Swap", std::vector<std::size_t>{qubits[i], qubits[endStart]}));
      endStart--;
    }

    return swaps;
  };

  std::function<std::vector<std::shared_ptr<Instruction>>(
      std::vector<std::size_t> &)>
      coreqft;
  coreqft = [&](std::vector<std::size_t> &qubits)
      -> std::vector<std::shared_ptr<Instruction>> {
    // Get the first qubit
    auto q = qubits[0];

    // If we have only one left, then
    // just return a hadamard, if not,
    // then we need to build up some cphase gates
    if (qubits.size() == 1) {
      auto hadamard =
          gateRegistry->createInstruction("H", std::vector<std::size_t>{q});
      return std::vector<std::shared_ptr<Instruction>>{hadamard};
    } else {

      // Get the 1 the N qubits
      std::vector<std::size_t> qs(qubits.begin() + 1, qubits.end());

      // Compute the number of qubits
      auto n = 1 + qs.size();

      // Build up a list of cphase gates
      std::vector<std::shared_ptr<Instruction>> cphaseGates;
      int idx = 0;
      for (int i = n - 1; i > 0; --i) {
        auto q_idx = qs[idx];
        auto angle = 3.1415926 / std::pow(2, n - i);
        InstructionParameter p(angle);
        auto cp = gateRegistry->createInstruction(
            "CPhase", std::vector<std::size_t>{q, q_idx});
        cp->setParameter(0, p);
        cphaseGates.push_back(cp);
        idx++;
      }

      // Recursively build these up...
      auto insts = coreqft(qs);

      // Reverse the cphase gates
      std::reverse(cphaseGates.begin(), cphaseGates.end());

      // Add them to the return list
      for (auto cp : cphaseGates) {
        insts.push_back(cp);
      }

      // add a final hadamard...
      insts.push_back(
          gateRegistry->createInstruction("H", std::vector<std::size_t>{q}));

      // and return
      return insts;
    }
  };

  std::vector<std::size_t> qubits;
  for (int i = start; i < end; i++) {
    qubits.push_back(i);
  }

  auto qftInstructions = coreqft(qubits);

  auto swaps = bitReversal(qubits);
  for (auto s : swaps) {
    qftInstructions.push_back(s);
  }

  for (auto i : qftInstructions) {
    addInstruction(i);
  }

  return true;
}

} // namespace circuits
} // namespace xacc
