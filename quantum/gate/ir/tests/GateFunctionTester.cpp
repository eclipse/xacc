/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "CNOT.hpp"
#include "GateFunction.hpp"
#include "Hadamard.hpp"
#include "InstructionIterator.hpp"
#include "Rx.hpp"
#include "Ry.hpp"
#include "Rz.hpp"
#include "X.hpp"
#include <gtest/gtest.h>
#include "Graph.hpp"

using namespace xacc::quantum;

const std::string expectedQasm = "H qreg1\n"
                                 "CNOT qreg1,qreg2\n"
                                 "CNOT qreg0,qreg1\n"
                                 "H qreg0\n";

TEST(GateFunctionTester, checkFunctionMethods) {

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

  const std::string removeExpectedQasm = "CNOT qreg1,qreg2\n"
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

TEST(GateFunctionTester, checkWalkFunctionTree) {
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
  while (it.hasNext()) {
    auto inst = it.next();
    if (!inst->isComposite())
      std::cout << inst->name() << "\n";
  }
}

TEST(GateFunctionTester, checkEvaluateVariables) {

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

  EXPECT_TRUE(boost::get<double>(evaled->getInstruction(0)->getParameter(0)) ==
              3.1415);

  std::cout << "ParamSet:\n" << f.toString("qreg") << "\n";

  v(0) = 6.28;

  evaled = f(v);

  std::cout << "ParamSet:\n" << evaled->toString("qreg") << "\n";
}

TEST(GateFunctionTester, checkParameterInsertion) {

  xacc::InstructionParameter p1("theta");
  xacc::InstructionParameter p2("1 * theta");
  xacc::InstructionParameter p3("0.5 * psi");

  auto rz = std::make_shared<Rz>(std::vector<int>{1});
  auto ry = std::make_shared<Ry>(std::vector<int>{2});
  auto rx = std::make_shared<Rx>(std::vector<int>{3});

  rz->setParameter(0, p1);
  ry->setParameter(0, p2);
  rx->setParameter(0, p3);

  GateFunction f("foo");

  f.addInstruction(rz);
  f.addInstruction(ry);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRy(1 * theta) qreg2\n");

  // replace duplicated parameter with a unique one
  f.replaceInstruction(1, rx);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRx(0.5 * psi) qreg3\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  // replace unique Instruction parameter with a duplicated parameter
  f.replaceInstruction(1, ry);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRy(1 * theta) qreg2\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  // make sure parameters get removed completely
  f.removeInstruction(0);
  EXPECT_TRUE(f.nParameters() == 1);
  f.removeInstruction(0);

  EXPECT_TRUE(f.nInstructions() == 0);
  EXPECT_TRUE(f.nParameters() == 0);

  auto x = std::make_shared<X>(std::vector<int>{2});

  f.addInstruction(x);

  EXPECT_TRUE(f.nParameters() == 0);
  EXPECT_TRUE(f.nInstructions() == 1);

  f.addInstruction(rz);
  // replace an Instruction without a parameter with a parameterized Instruction
  f.replaceInstruction(0, rx);

  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.toString("qreg") == "Rx(0.5 * psi) qreg3\nRz(theta) qreg1\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  // replace a parameterized Instruction with one that does not have a parameter
  f.replaceInstruction(0, x);

  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.toString("qreg") == "X qreg2\nRz(theta) qreg1\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");

  f.replaceInstruction(0, rx);

  f.addInstruction(ry);

  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.nInstructions() == 3);
  EXPECT_TRUE(f.toString("qreg") ==
              "Rx(0.5 * psi) qreg3\nRz(theta) qreg1\nRy(1 * theta) qreg2\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  auto rx2 = std::make_shared<Rx>(std::vector<int>{4});
  xacc::InstructionParameter p5("1 * phi");
  rx2->setParameter(0, p5);
  // replacing duplicated parameter with a brand new parameter
  f.replaceInstruction(1, rx2);

  EXPECT_TRUE(f.nParameters() == 3);
  EXPECT_TRUE(f.nInstructions() == 3);
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(2)) == "phi");

  EXPECT_TRUE(f.toString("qreg") ==
              "Rx(0.5 * psi) qreg3\nRx(1 * phi) qreg4\nRy(1 * theta) qreg2\n");
}

// TEST(GateFunctionTester,checkReadGraph) {

// 	// Create a graph IR modeling a
// 	// quantum teleportation kernel
// 	std::string irstr = "graph G {\n"
// 			"node [shape=box style=filled]\n"
// 			"0 [label=\"Gate=InitialState,Circuit Layer=0,Gate Vertex Id=0,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
// 			"1 [label=\"Gate=x,Circuit Layer=1,Gate Vertex Id=1,Gate Acting Qubits=[0],Enabled=1\"];\n"
// 			"2 [label=\"Gate=h,Circuit Layer=1,Gate Vertex Id=2,Gate Acting Qubits=[1],Enabled=1\"];\n"
// 			"3 [label=\"Gate=cnot,Circuit Layer=2,Gate Vertex Id=3,Gate Acting Qubits=[1 2],Enabled=1\"];\n"
// 			"4 [label=\"Gate=cnot,Circuit Layer=3,Gate Vertex Id=4,Gate Acting Qubits=[0 1],Enabled=1\"];\n"
// 			"5 [label=\"Gate=h,Circuit Layer=4,Gate Vertex Id=5,Gate Acting Qubits=[0],Enabled=1\"];\n"
// 			"6 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex Id=6,Gate Acting Qubits=[0],Enabled=1\"];\n"
// 			"7 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex Id=7,Gate Acting Qubits=[1],Enabled=1\"];\n"
// 			"8 [label=\"Gate=FinalState,Circuit Layer=6,Gate Vertex Id=8,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
// 			"9 [label=\"Gate=conditional,Circuit Layer=0,Gate Vertex Id=9,Gate Acting Qubits=[0],Enabled=1\"];\n"
// 			"10 [label=\"Gate=InitialState,Circuit Layer=7,Gate Vertex Id=10,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
// 			"11 [label=\"Gate=z,Circuit Layer=8,Gate Vertex Id=11,Gate Acting Qubits=[2],Enabled=0\"];\n"
// 			"12 [label=\"Gate=FinalState,Circuit Layer=9,Gate Vertex Id=12,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
// 			"13 [label=\"Gate=conditional,Circuit Layer=0,Gate Vertex Id=13,Gate Acting Qubits=[1],Enabled=1\"];\n"
// 			"14 [label=\"Gate=InitialState,Circuit Layer=10,Gate Vertex Id=14,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
// 			"15 [label=\"Gate=x,Circuit Layer=11,Gate Vertex Id=15,Gate Acting Qubits=[2],Enabled=0\"];\n"
// 			"16 [label=\"Gate=FinalState,Circuit Layer=12,Gate Vertex Id=16,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
// 			"0--1 [label=\"weight=0.000000\"];\n"
// 			"0--2 [label=\"weight=0.000000\"];\n"
// 			"2--3 [label=\"weight=0.000000\"];\n"
// 			"0--3 [label=\"weight=0.000000\"];\n"
// 			"1--4 [label=\"weight=0.000000\"];\n"
// 			"3--4 [label=\"weight=0.000000\"];\n"
// 			"4--5 [label=\"weight=0.000000\"];\n"
// 			"5--6 [label=\"weight=0.000000\"];\n"
// 			"4--7 [label=\"weight=0.000000\"];\n"
// 			"6--8 [label=\"weight=0.000000\"];\n"
// 			"7--8 [label=\"weight=0.000000\"];\n"
// 			"3--8 [label=\"weight=0.000000\"];\n"
// 			"8--9 [label=\"weight=0.000000\"];\n"
// 			"9--10 [label=\"weight=0.000000\"];\n"
// 			"10--11 [label=\"weight=0.000000\"];\n"
// 			"10--12 [label=\"weight=0.000000\"];\n"
// 			"10--12 [label=\"weight=0.000000\"];\n"
// 			"11--12 [label=\"weight=0.000000\"];\n"
// 			"8--13 [label=\"weight=0.000000\"];\n"
// 			"13--14 [label=\"weight=0.000000\"];\n"
// 			"14--15 [label=\"weight=0.000000\"];\n"
// 			"14--16 [label=\"weight=0.000000\"];\n"
// 			"14--16 [label=\"weight=0.000000\"];\n"
// 			"15--16 [label=\"weight=0.000000\"];\n"
// 			"}";
// 	std::istringstream iss(irstr);

//     GateFunction f("foo");
//     f.fromGraph(iss);
    
// 	auto circuit = f.toGraph();

// 	std::map<int, std::vector<int>> expectedQubits;
// 	expectedQubits.insert(std::make_pair(0, std::vector<int>{0, 1, 2}));
// 	expectedQubits.insert(std::make_pair(1, std::vector<int>{0}));
// 	expectedQubits.insert(std::make_pair(2, std::vector<int>{1}));
// 	expectedQubits.insert(std::make_pair(3, std::vector<int>{1, 2}));
// 	expectedQubits.insert(std::make_pair(4, std::vector<int>{0, 1}));
// 	expectedQubits.insert(std::make_pair(5, std::vector<int>{0}));
// 	expectedQubits.insert(std::make_pair(6, std::vector<int>{0}));
// 	expectedQubits.insert(std::make_pair(7, std::vector<int>{1}));
// 	expectedQubits.insert(std::make_pair(8, std::vector<int>{0, 1, 2}));
// 	expectedQubits.insert(std::make_pair(9, std::vector<int>{0}));
// 	expectedQubits.insert(std::make_pair(10, std::vector<int>{0, 1, 2}));
// 	expectedQubits.insert(std::make_pair(11, std::vector<int>{2}));
// 	expectedQubits.insert(std::make_pair(12, std::vector<int>{0, 1, 2}));
// 	expectedQubits.insert(std::make_pair(13, std::vector<int>{1}));
// 	expectedQubits.insert(std::make_pair(14, std::vector<int>{0, 1, 2}));
// 	expectedQubits.insert(std::make_pair(15, std::vector<int>{2}));
// 	expectedQubits.insert(std::make_pair(16, std::vector<int>{0, 1, 2}));

// 	EXPECT_TRUE(circuit.order() == 17);
// 	EXPECT_TRUE(circuit.size() == 24);

// 	EXPECT_TRUE(circuit.getVertexProperty<0>(0) == "InitialState");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(1) == "x");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(2) == "h");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(3) == "cnot");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(4) == "cnot");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(5) == "h");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(6) == "measure");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(7) == "measure");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(8) == "FinalState");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(9) == "conditional");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(10) == "InitialState");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(11) == "z");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(12) == "FinalState");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(13) == "conditional");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(14) == "InitialState");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(15) == "x");
// 	EXPECT_TRUE(circuit.getVertexProperty<0>(16) == "FinalState");

// 	for (int i = 0; i < 17; i++) {
// 		EXPECT_TRUE(circuit.getVertexProperty<2>(i) == i);
// 		EXPECT_TRUE(circuit.getVertexProperty<3>(i) == expectedQubits[i]);
// 	}

// 	EXPECT_TRUE(circuit.getVertexProperty<1>(0) == 0);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(1) == 1);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(2) == 1);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(3) == 2);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(4) == 3);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(5) == 4);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(6) == 5);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(7) == 5);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(8) == 6);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(9) == 0);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(10) == 7);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(11) == 8);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(12) == 9);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(13) == 0);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(14) == 10);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(15) == 11);
// 	EXPECT_TRUE(circuit.getVertexProperty<1>(16) == 12);

// 	EXPECT_TRUE(circuit.getVertexProperty<4>(0));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(1));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(2));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(3));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(4));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(5));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(6));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(7));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(8));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(9));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(10));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(11));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(12));
// 	EXPECT_TRUE(circuit.getVertexProperty<4>(13));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(14));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(15));
// 	EXPECT_TRUE(!circuit.getVertexProperty<4>(16));
// }

TEST(GateFunctionTester,checkGenerateGraph) {

	auto f = std::make_shared<GateFunction>("foo");
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
    auto rz = std::make_shared<Rz>(2,3.1415);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
    f->addInstruction(rz);
    
	auto g = f->toGraph();

	std::stringstream ss;
	g.write(ss);

	std::string expected = R"expected(digraph G
node [shape=box style=filled] {
0 [label="Gate=InitialState,Circuit Layer=0,Gate Vertex Id=0,Gate Acting Qubits=[0 1 2],Enabled=1,RuntimeParameters=[]"];
1 [label="Gate=H,Circuit Layer=0,Gate Vertex Id=1,Gate Acting Qubits=[1],Enabled=1,RuntimeParameters=[]"];
2 [label="Gate=CNOT,Circuit Layer=0,Gate Vertex Id=2,Gate Acting Qubits=[1 2],Enabled=1,RuntimeParameters=[]"];
3 [label="Gate=CNOT,Circuit Layer=0,Gate Vertex Id=3,Gate Acting Qubits=[0 1],Enabled=1,RuntimeParameters=[]"];
4 [label="Gate=H,Circuit Layer=0,Gate Vertex Id=4,Gate Acting Qubits=[0],Enabled=1,RuntimeParameters=[]"];
5 [label="Gate=Rz,Circuit Layer=0,Gate Vertex Id=5,Gate Acting Qubits=[2],Enabled=1,RuntimeParameters=[]"];
6 [label="Gate=FinalState,Circuit Layer=0,Gate Vertex Id=6,Gate Acting Qubits=[],Enabled=1,RuntimeParameters=[]"];
0->1 ;
0->2 ;
0->3 ;
1->2 ;
2->3 ;
2->5 ;
3->4 ;
3->6 ;
4->6 ;
5->6 ;
})expected";

	std::cout << ss.str() << "\n\n" << expected << "\n";
	EXPECT_TRUE(expected == ss.str());

}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
