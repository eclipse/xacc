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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE QuilVisitorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "QuilVisitor.hpp"
#include "InstructionIterator.hpp"

using namespace xacc;
using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkIRToQuil) {

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
	auto visitor = std::make_shared<QuilVisitor>();

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

	std::string expectedQuil =
			"X 0\n"
			"H 1\n"
			"CNOT 1 2\n"
			"CNOT 0 1\n"
			"H 0\n"
			"MEASURE 0 [0]\n"
			"MEASURE 1 [1]\n"
			"JUMP-UNLESS @conditional_0 [0]\n"
			"Z 2\n"
			"LABEL @conditional_0\n"
			"JUMP-UNLESS @conditional_1 [1]\n"
			"X 2\n"
			"LABEL @conditional_1\n";

	BOOST_VERIFY(expectedQuil == visitor->getQuilString());
}

