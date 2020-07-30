/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#pragma once
#include <vector>
#include <memory>

namespace xacc {
class CompositeInstruction;
namespace quantum {
// Describes a common pattern whereby there is
// a common base ansatz/state-preparation circuit (no measurement)
// and a set of *observe* sub-circuits appended to that base circuit.
class ObservedAnsatz {
public:
  [[nodiscard]] static ObservedAnsatz fromObservedComposites(
      const std::vector<std::shared_ptr<CompositeInstruction>> &in_composites);
  std::shared_ptr<CompositeInstruction> getBase() const { return m_baseAnsatz; }
  std::vector<std::shared_ptr<CompositeInstruction>>
  getObservedSubCircuits() const {
    return m_obsCircuits;
  }
  // Helper for users to verify that the decomposition indeed valid
  bool validate(const std::vector<std::shared_ptr<CompositeInstruction>>
                    &in_composites) const;

private:
  std::shared_ptr<CompositeInstruction> m_baseAnsatz;
  std::vector<std::shared_ptr<CompositeInstruction>> m_obsCircuits;
};
} // namespace quantum
} // namespace xacc