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
#define BOOST_TEST_MODULE ScaffoldIRToSrcVisitorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "ScaffoldIRToSrcVisitor.hpp"
#include "InstructionIterator.hpp"

using namespace xacc;
using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkIRToScaffold) {

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

	auto rz = std::make_shared<Rz>(0, 3.1415);
	auto swap = std::make_shared<Swap>(0, 1);
	auto cphase = std::make_shared<CPhase>(1, 2, 3.1415926);

	InstructionParameter p("theta");
	auto varCphase = std::make_shared<CPhase>(std::vector<int>{1,2});
	varCphase->setParameter(0, p);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(cond1);
	f->addInstruction(cond2);
	f->addInstruction(rz);
	f->addInstruction(swap);
	f->addInstruction(cphase);
	f->addInstruction(varCphase);

	// Create the Instruction Visitor that is going
	// to map our IR to Quil.
	auto visitor = std::make_shared<ScaffoldIRToSrcVisitor>("qreg");

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

	std::string expectedStr = "cbit creg[2];\n"
			"X(qreg[0]);\n"
			"H(qreg[1]);\n"
			"CNOT(qreg[1],qreg[2]);\n"
			"CNOT(qreg[0],qreg[1]);\n"
			"H(qreg[0]);\n"
			"creg[0] = MeasZ(qreg[0]);\n"
			"creg[1] = MeasZ(qreg[1]);\n"
			"if (creg[0] == 1) {\n"
			"   Z(qreg[2]);\n"
			"}\n"
			"if (creg[1] == 1) {\n"
			"   X(qreg[2]);\n"
			"}\n"
			"Rz(qreg[0],3.1415);\n"
			"// BEGIN SWAP 0 1\n"
			"CNOT(qreg[1],qreg[0]);\n"
			"CNOT(qreg[0],qreg[1]);\n"
			"CNOT(qreg[1],qreg[0]);\n"
			"// END SWAP 0 1\n"
			"// BEGIN CPHASE GATE\n"
			"Rz(qreg[2],1.5708);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"Rz(qreg[2],-1.5708);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"// END CPHASE GATE\n"
			"// BEGIN CPHASE GATE\n"
			"Rz(qreg[2],0.5 * theta);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"Rz(qreg[2],-1 * 0.5 * theta);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"// END CPHASE GATE\n";

	BOOST_VERIFY(expectedStr == visitor->getScaffoldString());
}

BOOST_AUTO_TEST_CASE(checkQFT) {

	auto h1 = std::make_shared<Hadamard>(2);
	auto cphase1 = std::make_shared<CPhase>(1, 2, 1.5707963);
	auto h2 = std::make_shared<Hadamard>(2);
	auto cphase2 = std::make_shared<CPhase>(0, 2, 0.78539815);
	auto cphase3 = std::make_shared<CPhase>(0, 1, 1.5707963);
	auto h3 = std::make_shared<Hadamard>(0);
	auto swap = std::make_shared<Swap>(0, 2);

	auto expectedF = std::make_shared<GateFunction>("qft");
	expectedF->addInstruction(h1);
	expectedF->addInstruction(cphase1);
	expectedF->addInstruction(h2);
	expectedF->addInstruction(cphase2);
	expectedF->addInstruction(cphase3);
	expectedF->addInstruction(h3);
	expectedF->addInstruction(swap);

	auto visitor = std::make_shared<ScaffoldIRToSrcVisitor>("qreg");
	InstructionIterator it(expectedF);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();
		if (nextInst->isEnabled())
			nextInst->accept(visitor);
	}

	std::string expected = "H(qreg[2]);\n"
			"// BEGIN CPHASE GATE\n"
			"Rz(qreg[2],0.785398);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"Rz(qreg[2],-0.785398);\n"
			"CNOT(qreg[2],qreg[1]);\n"
			"// END CPHASE GATE\n"
			"H(qreg[2]);\n"
			"// BEGIN CPHASE GATE\n"
			"Rz(qreg[2],0.392699);\n"
			"CNOT(qreg[2],qreg[0]);\n"
			"Rz(qreg[2],-0.392699);\n"
			"CNOT(qreg[2],qreg[0]);\n"
			"// END CPHASE GATE\n"
			"// BEGIN CPHASE GATE\n"
			"Rz(qreg[1],0.785398);\n"
			"CNOT(qreg[1],qreg[0]);\n"
			"Rz(qreg[1],-0.785398);\n"
			"CNOT(qreg[1],qreg[0]);\n"
			"// END CPHASE GATE\n"
			"H(qreg[0]);\n"
			"// BEGIN SWAP 0 2\n"
			"CNOT(qreg[2],qreg[0]);\n"
			"CNOT(qreg[0],qreg[2]);\n"
			"CNOT(qreg[2],qreg[0]);\n"
			"// END SWAP 0 2\n";

	BOOST_VERIFY(expected == visitor->getScaffoldString());
}
