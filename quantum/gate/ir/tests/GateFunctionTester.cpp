
/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#define BOOST_TEST_MODULE GateFunctionTester

#include <boost/test/included/unit_test.hpp>
#include "GateFunction.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
#include "InstructionIterator.hpp"

using namespace xacc::quantum;

const std::string expectedQasm =
		"H qreg1\n"
		"CNOT qreg1,qreg2\n"
		"CNOT qreg0,qreg1\n"
		"H qreg0\n";

BOOST_AUTO_TEST_CASE(checkFunctionMethods) {

	GateFunction f("foo");
	BOOST_VERIFY(f.getName() == "foo");
	BOOST_VERIFY(f.nInstructions() == 0);

	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);

	f.addInstruction(h);
	f.addInstruction(cn1);
	f.addInstruction(cn2);
	f.addInstruction(h2);
	BOOST_VERIFY(f.nInstructions() == 4);
	BOOST_VERIFY(f.toString("qreg") == expectedQasm);

	f.removeInstruction(0);
	BOOST_VERIFY(f.nInstructions() == 3);

	const std::string removeExpectedQasm =
			"CNOT qreg1,qreg2\n"
			"CNOT qreg0,qreg1\n"
			"H qreg0\n";
	BOOST_VERIFY(f.toString("qreg") == removeExpectedQasm);


	f.replaceInstruction(0, h);
	const std::string replaceExpectedQasm = "H qreg1\n"
			"CNOT qreg0,qreg1\n"
			"H qreg0\n";
	BOOST_VERIFY(f.toString("qreg") == replaceExpectedQasm);

	f.replaceInstruction(2, cn1);
	const std::string replaceExpectedQasm2 = "H qreg1\n"
			"CNOT qreg0,qreg1\n"
			"CNOT qreg1,qreg2\n";
	BOOST_VERIFY(f.toString("qreg") == replaceExpectedQasm2);

}

BOOST_AUTO_TEST_CASE(checkWalkFunctionTree) {
	auto f = std::make_shared<GateFunction>("foo");
	auto g = std::make_shared<GateFunction>("goo");

	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);

	g->addInstruction(cn1);
	g->addInstruction(cn2);

	f->addInstruction(h);
	f->addInstruction(g);
	f->addInstruction(h2);

	xacc::InstructionIterator it(f);
	while(it.hasNext()) {
		auto inst = it.next();
		if (!inst->isComposite()) std::cout << inst->getName() << "\n";
	}

}

BOOST_AUTO_TEST_CASE(checkEvaluateVariables) {

	xacc::InstructionParameter p("phi");
	xacc::InstructionParameter fParam("phi");

	BOOST_VERIFY(p == fParam);

	auto rz = std::make_shared<Rz>(std::vector<int>{0});
	rz->setParameter(0, p);

	GateFunction f("foo", std::vector<xacc::InstructionParameter>{fParam});

	f.addInstruction(rz);

	std::cout << f.toString("qreg") << "\n";

	xacc::InstructionParameter runtimeValue(3.1415);

	f.evaluateVariableParameters(std::vector<xacc::InstructionParameter>{runtimeValue});

	BOOST_VERIFY(boost::get<double>(f.getInstruction(0)->getParameter(0)) == 3.1415);

	std::cout << "ParamSet:\n" << f.toString("qreg") << "\n";

	xacc::InstructionParameter runtimeValue2(6.28);

	f.evaluateVariableParameters(std::vector<xacc::InstructionParameter>{runtimeValue2});

	std::cout << "ParamSet:\n" << f.toString("qreg") << "\n";


}
