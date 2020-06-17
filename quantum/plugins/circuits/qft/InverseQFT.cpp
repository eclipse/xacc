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
#include "InverseQFT.hpp"
#include <memory>
#include "xacc_service.hpp"

namespace xacc {

namespace circuits {

const std::vector<std::string> InverseQFT::requiredKeys() {
  return {"nq", "start", "end"};
}
bool InverseQFT::expand(const xacc::HeterogeneousMap &runtimeOptions) {

  // Generate the QFT algorithm and get the
  // individual instructions
  auto qft = std::dynamic_pointer_cast<CompositeInstruction>(
      xacc::getService<Instruction>("qft"));
  if (!qft->expand(runtimeOptions)) {
    return false;
  }

  auto instructions =
      std::dynamic_pointer_cast<CompositeInstruction>(qft)->getInstructions();

  // Reverse those instructions
  std::reverse(instructions.begin(), instructions.end());

  // Add them to the return function
  for (auto i : instructions) {
    // If it is a parametric CPhase gate, 
    // we need to inverse the angle.
    if (i->name() == "CPhase") {
      i->setParameter(0, -i->getParameter(0).as<double>());
    }
    addInstruction(i);
  }

  return true;
}

} // namespace circuits
} // namespace xacc
