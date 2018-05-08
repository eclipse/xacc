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
#include <gtest/gtest.h>
#include "Hadamard.hpp"
#include "CNOT.hpp"

using namespace xacc;

using namespace xacc::quantum;

class TestVisitor:
		public BaseInstructionVisitor, // ALWAYS REQUIRED
		public InstructionVisitor<CNOT>,
		public InstructionVisitor<Hadamard> {
public:

	virtual void visit(CNOT& cnot) {
		std::cout << "HELLO WORLD CNOT\n";
	}

    virtual void visit(Hadamard& h) {
		std::cout << "HELLO WORLD HADAMARD\n";
	}

};

TEST(GateVisitorTester,checkVisit) {
	auto v = std::make_shared<TestVisitor>();
	std::shared_ptr<GateInstruction> c = std::make_shared<CNOT>(0,1);
	std::shared_ptr<GateInstruction> h = std::make_shared<Hadamard>(0);
	c->accept(v);
	h->accept(v);
}
int main(int argc, char** argv) {
	    ::testing::InitGoogleTest(&argc, argv);
		    return RUN_ALL_TESTS();
}
