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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <memory>
#include <gtest/gtest.h>
#include "OpenQasmVisitor.hpp"
#include "InstructionIterator.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(OpenQasmVisitorTester,checkIRToOpenQasm) {

	auto f = std::make_shared<GateFunction>("foo");

	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);

	auto cond1 = std::make_shared<ConditionalFunction>(0);
	auto z = std::make_shared<Z>(2);
	cond1->addInstruction(z);
	auto cond2 = std::make_shared<ConditionalFunction>(1);
	auto x2 = std::make_shared<X>(2);
	cond2->addInstruction(x2);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(cond1);
	f->addInstruction(cond2);


	// Create the Instruction Visitor that is going
	// to map our IR to Quil.
	auto visitor = std::make_shared<OpenQasmVisitor>(3);

	// Our QIR is really a tree structure
	// so create a pre-order tree traversal
	// InstructionIterator to walk it
	InstructionIterator it(f);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled())
			nextInst->accept(visitor);
	}

	std::cout << visitor->getOpenQasmString() << "\n";

	const std::string expected = R"expected(
include \"qelib1.inc\";
qreg q[3];
u3(3.14159, 0, 3.14159) q[0];
u2(0, 3.14159) q[1];
cx q[1], q[2];
cx q[0], q[1];
u2(0, 3.14159) q[0];
creg c0[1];
measure q[0] -> c0[0];
creg c1[1];
measure q[1] -> c1[0];
if (c0 == 1) u1(3.14159) q[2];
if (c1 == 1) u3(3.14159, 0, 3.14159) q[2];
)expected";

	std::cout << "EXPECTED: " << expected << "\n";
	EXPECT_TRUE(expected == visitor->getOpenQasmString());

//	EXPECT_TRUE()
}


int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
