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
#include "GateFunction.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
#include "Ry.hpp"
#include "Rx.hpp"
#include "InstructionIterator.hpp"

using namespace xacc::quantum;

const std::string expectedQasm =
		"H qreg1\n"
		"CNOT qreg1,qreg2\n"
		"CNOT qreg0,qreg1\n"
		"H qreg0\n";

TEST(GateFunctionTester,checkFunctionMethods) {

	GateFunction f("foo");
	EXPECT_TRUE(f.name() == "foo");
	EXPECT_TRUE(f.nInstructions() == 0);

	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);

	f.addInstruction(h);
	f.addInstruction(cn1);
	f.addInstruction(cn2);
	f.addInstruction(h2);
	EXPECT_TRUE(f.nInstructions() == 4);
	EXPECT_TRUE(f.toString("qreg") == expectedQasm);

	f.removeInstruction(0);
	EXPECT_TRUE(f.nInstructions() == 3);

	const std::string removeExpectedQasm =
			"CNOT qreg1,qreg2\n"
			"CNOT qreg0,qreg1\n"
			"H qreg0\n";
	EXPECT_TRUE(f.toString("qreg") == removeExpectedQasm);


	f.replaceInstruction(0, h);
	const std::string replaceExpectedQasm = "H qreg1\n"
			"CNOT qreg0,qreg1\n"
			"H qreg0\n";
	EXPECT_TRUE(f.toString("qreg") == replaceExpectedQasm);

	f.replaceInstruction(2, cn1);
	const std::string replaceExpectedQasm2 = "H qreg1\n"
			"CNOT qreg0,qreg1\n"
			"CNOT qreg1,qreg2\n";
	EXPECT_TRUE(f.toString("qreg") == replaceExpectedQasm2);

}

TEST(GateFunctionTester,checkWalkFunctionTree) {
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
		if (!inst->isComposite()) std::cout << inst->name() << "\n";
	}

}

TEST(GateFunctionTester,checkEvaluateVariables) {

	xacc::Initialize();
	xacc::InstructionParameter p("phi");
	xacc::InstructionParameter fParam("phi");

	EXPECT_TRUE(p == fParam);

	auto rz = std::make_shared<Rz>(std::vector<int>{0});
	rz->setParameter(0, p);

	GateFunction f("foo", std::vector<xacc::InstructionParameter>{fParam});

	f.addInstruction(rz);

	std::cout << f.toString("qreg") << "\n";

	Eigen::VectorXd v(1);
	v(0) = 3.1415;

	auto evaled = f(v);

	EXPECT_TRUE(boost::get<double>(evaled->getInstruction(0)->getParameter(0)) == 3.1415);

	std::cout << "ParamSet:\n" << f.toString("qreg") << "\n";

	v(0) = 6.28;

	evaled = f(v);

	std::cout << "ParamSet:\n" << evaled->toString("qreg") << "\n";

	xacc::Finalize();
}

TEST(GateFunctionTester, checkParameterInsertion){
    
    xacc::Initialize();
    xacc::InstructionParameter p("phi");
    xacc::InstructionParameter p2("psi");

    auto ry = std::make_shared<Ry>(std::vector<int>{0});
    auto rz = std::make_shared<Rz>(std::vector<int>{1});

    ry->setParameter(0, p);
    rz->setParameter(0, p2);

    GateFunction f("foo");
    f.addInstruction(ry);
    f.addInstruction(rz);
    EXPECT_TRUE(f.nParameters() == 2);
    EXPECT_TRUE(f.getParameter(0).which() == 3);
    EXPECT_TRUE(f.getParameter(1).which() == 3);
    EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "phi");
    EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

    auto rx = std::make_shared<Rx>(std::vector<int>{2});

    rx->setParameter(0, p);

    f.addInstruction(rx);
    
    EXPECT_TRUE(f.nParameters() == 2);

    f.removeInstruction(1);
    
    EXPECT_TRUE(f.nInstructions() == 2);
    EXPECT_TRUE(f.nParameters() == 1);
    EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "phi");
    
    xacc::Finalize();
    
    
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
