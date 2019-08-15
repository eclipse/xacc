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
#ifndef QUANTUM_GATE_COUNTGATESOFTYPEVISITOR_HPP_
#define QUANTUM_GATE_COUNTGATESOFTYPEVISITOR_HPP_

#include "InstructionIterator.hpp"

namespace xacc {
namespace quantum {

/**
 */
template <typename GateType>
class CountGatesOfTypeVisitor : public xacc::BaseInstructionVisitor,
                                public xacc::InstructionVisitor<GateType> {
protected:
  int count = 0;
  std::shared_ptr<CompositeInstruction> function;

public:
  CountGatesOfTypeVisitor(std::shared_ptr<CompositeInstruction> f) : function(f) {}

  virtual void visit(GateType &gate) { count++; }

  int countGates() {
    xacc::InstructionIterator it(function);
    while (it.hasNext()) {
      // Get the next node in the tree
      auto nextInst = it.next();
      if (nextInst->isEnabled())
        nextInst->accept(this);
    }

    return count;
  }
};
} // namespace quantum
} // namespace xacc
#endif
