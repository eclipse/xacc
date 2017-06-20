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
#ifndef XACC_COMPILER_INSTRUCTION_HPP_
#define XACC_COMPILER_INSTRUCTION_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "Utils.hpp"

namespace xacc {

/**
 */
class BaseInstructionVisitor {
public:
	virtual ~BaseInstructionVisitor() {}
};

template<class T>
class InstructionVisitor {
public:
	virtual void visit(T&) = 0;
	virtual ~InstructionVisitor() {}
};

class BaseInstructionVisitable {
public:
	virtual void accept(std::shared_ptr<BaseInstructionVisitor> visitor) = 0;
	virtual void accept(BaseInstructionVisitor* visitor) = 0;

	virtual ~BaseInstructionVisitable() {}
protected:
	template<class T>
	static void acceptImpl(T& visited, std::shared_ptr<BaseInstructionVisitor> visitor) {
		auto castedVisitor = std::dynamic_pointer_cast<InstructionVisitor<T>>(visitor);
		if (castedVisitor) {
			castedVisitor->visit(visited);
			return;
		}
	}

	template<class T>
	static void acceptImpl(T& visited, BaseInstructionVisitor* visitor) {
		auto castedVisitor = dynamic_cast<InstructionVisitor<T>*>(visitor);
		if (castedVisitor) {
			castedVisitor->visit(visited);
			return;
		}
	}

#define DEFINE_VISITABLE() \
            virtual void accept(std::shared_ptr<BaseInstructionVisitor> v) \
                { acceptImpl(*this, v); } \
			virtual void accept(BaseInstructionVisitor* v) \
				{ acceptImpl(*this, v); }

};

/**
 *
 */
class Instruction : public BaseInstructionVisitable {

public:

	/**
	 *
	 * @return
	 */
	virtual const std::string getName() = 0;

	/**
	 *
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName) = 0;

	/**
	 *
	 * @return
	 */
	virtual const std::vector<int> bits() = 0;

	virtual bool isComposite() { return false; }

	virtual bool isEnabled() { return true; }

	virtual void disable() {}
	virtual void enable() {}

	/**
	 *
	 * @param visitor
	 */
//	virtual void accept(std::shared_ptr<InstructionVisitor> visitor) = 0;

	/**
	 * The destructor
	 */
	virtual ~Instruction() {
	}
};

}
#endif
