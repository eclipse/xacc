/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef XACC_COMPILER_INSTRUCTIONITERATOR_HPP_
#define XACC_COMPILER_INSTRUCTIONITERATOR_HPP_
#include <stack>
#include "Function.hpp"

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
	InstructionIterator(std::shared_ptr<Instruction> r) : root(r){
		instStack.push(root);
	}

	/**
	 * Return true if there are still instructions left to traverse.
	 * @return
	 */
	bool hasNext() {
		return !instStack.empty();
	}

	/**
	 * Return the next Instruction in the tree.
	 * @return
	 */
	std::shared_ptr<Instruction> next() {
		std::shared_ptr<Instruction> next = instStack.top();
		instStack.pop();
		auto f = std::dynamic_pointer_cast<Function>(next);
		if (f) {
			for (int i = f->nInstructions() - 1; i >= 0; i--) {
				instStack.push(f->getInstruction(i));
			}
		}
		return next;
	}
};
}
#endif
