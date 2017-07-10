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
#ifndef XACC_COMPILER_INSTRUCTIONVISITOR_HPP_
#define XACC_COMPILER_INSTRUCTIONVISITOR_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "Utils.hpp"

namespace xacc {

/**
 * The BaseInstructionVisitor is a base class for all
 * classes that are Instruction visitors. It basically
 * provides a means for passing instruction visitor
 * handles in a polymorphic manner.
 */
class BaseInstructionVisitor {
public:
	/**
	 * The destructor
	 */
	virtual ~BaseInstructionVisitor() {}
};

/**
 * The InstructionVisitor provides a visit method
 * for the provided template parameter.
 */
template<class T>
class InstructionVisitor {
public:
	/**
	 * This method should be implemented by subclasses
	 * to perform Visitor-specific behavior on the given
	 * instance of the template parameter T.
	 */
	virtual void visit(T&) = 0;

	/**
	 * The destructor
	 */
	virtual ~InstructionVisitor() {}
};

/**
 * BaseInstructionVisitable is an interface that
 * is to be implemented by any and all Instructions that
 * want to be available for visitation. Derivations of this class
 * simply inherit from BaseInstructionVisitable and declare the
 * DEFINE_VISITABLE macro alongside the rest of the classes
 * member methods.
 */
class BaseInstructionVisitable {
public:
	/**
	 * Accept the provided BaseInstructionVisitor as a shared pointer.
	 * @param visitor The visitor to invoke visit() on.
	 */
	virtual void accept(std::shared_ptr<BaseInstructionVisitor> visitor) = 0;

	/**
	 * Accept the provided BaseInstructionVisitor as a raw pointer.
	 * @param visitor The visitor to invoke visit() on.
	 */
	virtual void accept(BaseInstructionVisitor* visitor) = 0;

	/**
	 * The Destructor
	 */
	virtual ~BaseInstructionVisitable() {}

protected:

	/**
	 * This method is invoked by the DEFINE_VISITABLE macro to
	 * invoke the visit method on the provided visitor. This method
	 * takes the visitor as a shared pointer.
	 */
	template<class T>
	static void acceptImpl(T& visited, std::shared_ptr<BaseInstructionVisitor> visitor) {
		auto castedVisitor = std::dynamic_pointer_cast<InstructionVisitor<T>>(visitor);
		if (castedVisitor) {
			castedVisitor->visit(visited);
			return;
		}
	}

	/**
	 * This method is invoked by the DEFINE_VISITABLE macro to
	 * invoke the visit method on the provided visitor. This method
	 * takes the visitor as a raw pointer.
	 */

	template<class T>
	static void acceptImpl(T& visited, BaseInstructionVisitor* visitor) {
		auto castedVisitor = dynamic_cast<InstructionVisitor<T>*>(visitor);
		if (castedVisitor) {
			castedVisitor->visit(visited);
			return;
		}
	}

// Convenience macro for subclasses to invoke
// to get Visitor pattern functionality.
#define DEFINE_VISITABLE() \
            virtual void accept(std::shared_ptr<BaseInstructionVisitor> v) \
                { acceptImpl(*this, v); } \
			virtual void accept(BaseInstructionVisitor* v) \
				{ acceptImpl(*this, v); }

#define EMPTY_DEFINE_VISITABLE() \
		virtual void accept(std::shared_ptr<BaseInstructionVisitor> v) \
		{ } \
		virtual void accept(BaseInstructionVisitor* v) \
		{}
};

}
#endif
