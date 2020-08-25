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

namespace xacc {
namespace quantum {

void TriQPlacement::apply(std::shared_ptr<CompositeInstruction> function,
                          const std::shared_ptr<Accelerator> acc,
                          const HeterogeneousMap &options) {
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
    Gate *pG = Gate::create_new_gate(gateName);
    pG->id = instIdx;
    const auto nqVars = xaccInst->bits().size();
    assert(nqVars == pG->nvars);

    for (int j = 0; j < pG->nvars; j++) {
      pG->vars.push_back(triqCirc.qubits[xaccInst->bits()[j]]);
    }
    triqCirc.gates.push_back(pG);
  }
  // DEBUG:
  triqCirc.print_gates();
}
} // namespace quantum
} // namespace xacc
