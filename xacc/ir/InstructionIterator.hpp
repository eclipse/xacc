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
#ifndef XACC_COMPILER_INSTRUCTIONITERATOR_HPP_
#define XACC_COMPILER_INSTRUCTIONITERATOR_HPP_
#include <stack>
#include "CompositeInstruction.hpp"

namespace xacc {

/**
 * The InstructionIterator provides a mechanism for
 * a pre-order traversal of an Instruction tree.
 */
class InstructionIterator {

protected:
  /**
   * The root of the tree, a function
   */
  std::shared_ptr<Instruction> root;

  /**
   * A stack used to implement the tree traversal
   */
  std::stack<std::shared_ptr<Instruction>> instStack;

public:
  /**
   * The constructor, takes the root of the tree
   * as input.
   *
   * @param r
   */
  InstructionIterator(std::shared_ptr<Instruction> r) : root(r) {
    instStack.push(root);
  }

  /**
   * Return true if there are still instructions left to traverse.
   * @return
   */
  bool hasNext() { return !instStack.empty(); }

  /**
   * Return the next Instruction in the tree.
   * @return
   */
  std::shared_ptr<Instruction> next() {
    std::shared_ptr<Instruction> next = instStack.top();
    instStack.pop();
    auto f = std::dynamic_pointer_cast<CompositeInstruction>(next);
    if (f) {
      for (int i = f->nInstructions() - 1; i >= 0; i--) {
        instStack.push(f->getInstruction(i));
      }
    }
    return next;
  }
};
} // namespace xacc
#endif
