/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_COMPILER_INSTRUCTIONVISITOR_HPP_
#define XACC_COMPILER_INSTRUCTIONVISITOR_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "Utils.hpp"
#include "Identifiable.hpp"

namespace xacc {

/**
 * The BaseInstructionVisitor is a base class for all
 * classes that are Instruction visitors. It basically
 * provides a means for passing instruction visitor
 * handles in a polymorphic manner.
 */
class BaseInstructionVisitor : public Identifiable {
public:
	/**
	 * The destructor
	 */
	virtual ~BaseInstructionVisitor() {}

	virtual const std::string name() const {
		return "base-visitor-name";
	}

	virtual const std::string description() const {
		return "Base Instruction Visitors are identifiable "
				"for developers who want them to be a "
				"framework extension point.";
	}

	virtual const std::string toString() {
		return "";
	}
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
