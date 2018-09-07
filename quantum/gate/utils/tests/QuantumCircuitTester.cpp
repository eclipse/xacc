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
#include <gtest/gtest.h>
#include "QuantumCircuit.hpp"

using namespace xacc::quantum;

TEST(QuantumCircuitTester, checkReadGraph) {

  // Create a graph IR modeling a
  // quantum teleportation kernel
  std::string irstr = "graph G {\n"
                      "{\n"
                      "node [shape=box style=filled]\n"
                      "0 [label=\"Gate=InitialState,Circuit Layer=0,Gate "
                      "Vertex Id=0,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
                      "1 [label=\"Gate=x,Circuit Layer=1,Gate Vertex Id=1,Gate "
                      "Acting Qubits=[0],Enabled=1\"];\n"
                      "2 [label=\"Gate=h,Circuit Layer=1,Gate Vertex Id=2,Gate "
                      "Acting Qubits=[1],Enabled=1\"];\n"
                      "3 [label=\"Gate=cnot,Circuit Layer=2,Gate Vertex "
                      "Id=3,Gate Acting Qubits=[1 2],Enabled=1\"];\n"
                      "4 [label=\"Gate=cnot,Circuit Layer=3,Gate Vertex "
                      "Id=4,Gate Acting Qubits=[0 1],Enabled=1\"];\n"
                      "5 [label=\"Gate=h,Circuit Layer=4,Gate Vertex Id=5,Gate "
                      "Acting Qubits=[0],Enabled=1\"];\n"
                      "6 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex "
                      "Id=6,Gate Acting Qubits=[0],Enabled=1\"];\n"
                      "7 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex "
                      "Id=7,Gate Acting Qubits=[1],Enabled=1\"];\n"
                      "8 [label=\"Gate=FinalState,Circuit Layer=6,Gate Vertex "
                      "Id=8,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
                      "9 [label=\"Gate=conditional,Circuit Layer=0,Gate Vertex "
                      "Id=9,Gate Acting Qubits=[0],Enabled=1\"];\n"
                      "10 [label=\"Gate=InitialState,Circuit Layer=7,Gate "
                      "Vertex Id=10,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
                      "11 [label=\"Gate=z,Circuit Layer=8,Gate Vertex "
                      "Id=11,Gate Acting Qubits=[2],Enabled=0\"];\n"
                      "12 [label=\"Gate=FinalState,Circuit Layer=9,Gate Vertex "
                      "Id=12,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
                      "13 [label=\"Gate=conditional,Circuit Layer=0,Gate "
                      "Vertex Id=13,Gate Acting Qubits=[1],Enabled=1\"];\n"
                      "14 [label=\"Gate=InitialState,Circuit Layer=10,Gate "
                      "Vertex Id=14,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
                      "15 [label=\"Gate=x,Circuit Layer=11,Gate Vertex "
                      "Id=15,Gate Acting Qubits=[2],Enabled=0\"];\n"
                      "16 [label=\"Gate=FinalState,Circuit Layer=12,Gate "
                      "Vertex Id=16,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
                      "}\n"
                      "0--1 ;\n"
                      "0--2 ;\n"
                      "2--3 ;\n"
                      "0--3 ;\n"
                      "1--4 ;\n"
                      "3--4 ;\n"
                      "4--5 ;\n"
                      "5--6 ;\n"
                      "4--7 ;\n"
                      "6--8 ;\n"
                      "7--8 ;\n"
                      "3--8 ;\n"
                      "8--9 ;\n"
                      "9--10 ;\n"
                      "10--11 ;\n"
                      "10--12 ;\n"
                      "10--12 ;\n"
                      "11--12 ;\n"
                      "8--13 ;\n"
                      "13--14 ;\n"
                      "14--15 ;\n"
                      "14--16 ;\n"
                      "14--16 ;\n"
                      "15--16 ;\n"
                      "}";
  std::istringstream iss(irstr);

  QuantumCircuit circuit;
  circuit.read(iss);

  std::map<int, std::vector<int>> expectedQubits;
  expectedQubits.insert(std::make_pair(0, std::vector<int>{0, 1, 2}));
  expectedQubits.insert(std::make_pair(1, std::vector<int>{0}));
  expectedQubits.insert(std::make_pair(2, std::vector<int>{1}));
  expectedQubits.insert(std::make_pair(3, std::vector<int>{1, 2}));
  expectedQubits.insert(std::make_pair(4, std::vector<int>{0, 1}));
  expectedQubits.insert(std::make_pair(5, std::vector<int>{0}));
  expectedQubits.insert(std::make_pair(6, std::vector<int>{0}));
  expectedQubits.insert(std::make_pair(7, std::vector<int>{1}));
  expectedQubits.insert(std::make_pair(8, std::vector<int>{0, 1, 2}));
  expectedQubits.insert(std::make_pair(9, std::vector<int>{0}));
  expectedQubits.insert(std::make_pair(10, std::vector<int>{0, 1, 2}));
  expectedQubits.insert(std::make_pair(11, std::vector<int>{2}));
  expectedQubits.insert(std::make_pair(12, std::vector<int>{0, 1, 2}));
  expectedQubits.insert(std::make_pair(13, std::vector<int>{1}));
  expectedQubits.insert(std::make_pair(14, std::vector<int>{0, 1, 2}));
  expectedQubits.insert(std::make_pair(15, std::vector<int>{2}));
  expectedQubits.insert(std::make_pair(16, std::vector<int>{0, 1, 2}));

  EXPECT_TRUE(circuit.order() == 17);
  EXPECT_TRUE(circuit.size() == 24);

  EXPECT_TRUE(circuit.getVertexProperty<0>(0) == "InitialState");
  EXPECT_TRUE(circuit.getVertexProperty<0>(1) == "x");
  EXPECT_TRUE(circuit.getVertexProperty<0>(2) == "h");
  EXPECT_TRUE(circuit.getVertexProperty<0>(3) == "cnot");
  EXPECT_TRUE(circuit.getVertexProperty<0>(4) == "cnot");
  EXPECT_TRUE(circuit.getVertexProperty<0>(5) == "h");
  EXPECT_TRUE(circuit.getVertexProperty<0>(6) == "measure");
  EXPECT_TRUE(circuit.getVertexProperty<0>(7) == "measure");
  EXPECT_TRUE(circuit.getVertexProperty<0>(8) == "FinalState");
  EXPECT_TRUE(circuit.getVertexProperty<0>(9) == "conditional");
  EXPECT_TRUE(circuit.getVertexProperty<0>(10) == "InitialState");
  EXPECT_TRUE(circuit.getVertexProperty<0>(11) == "z");
  EXPECT_TRUE(circuit.getVertexProperty<0>(12) == "FinalState");
  EXPECT_TRUE(circuit.getVertexProperty<0>(13) == "conditional");
  EXPECT_TRUE(circuit.getVertexProperty<0>(14) == "InitialState");
  EXPECT_TRUE(circuit.getVertexProperty<0>(15) == "x");
  EXPECT_TRUE(circuit.getVertexProperty<0>(16) == "FinalState");

  for (int i = 0; i < 17; i++) {
    EXPECT_TRUE(circuit.getVertexProperty<2>(i) == i);
    EXPECT_TRUE(circuit.getVertexProperty<3>(i) == expectedQubits[i]);
  }

  EXPECT_TRUE(circuit.getVertexProperty<1>(0) == 0);
  EXPECT_TRUE(circuit.getVertexProperty<1>(1) == 1);
  EXPECT_TRUE(circuit.getVertexProperty<1>(2) == 1);
  EXPECT_TRUE(circuit.getVertexProperty<1>(3) == 2);
  EXPECT_TRUE(circuit.getVertexProperty<1>(4) == 3);
  EXPECT_TRUE(circuit.getVertexProperty<1>(5) == 4);
  EXPECT_TRUE(circuit.getVertexProperty<1>(6) == 5);
  EXPECT_TRUE(circuit.getVertexProperty<1>(7) == 5);
  EXPECT_TRUE(circuit.getVertexProperty<1>(8) == 6);
  EXPECT_TRUE(circuit.getVertexProperty<1>(9) == 0);
  EXPECT_TRUE(circuit.getVertexProperty<1>(10) == 7);
  EXPECT_TRUE(circuit.getVertexProperty<1>(11) == 8);
  EXPECT_TRUE(circuit.getVertexProperty<1>(12) == 9);
  EXPECT_TRUE(circuit.getVertexProperty<1>(13) == 0);
  EXPECT_TRUE(circuit.getVertexProperty<1>(14) == 10);
  EXPECT_TRUE(circuit.getVertexProperty<1>(15) == 11);
  EXPECT_TRUE(circuit.getVertexProperty<1>(16) == 12);

  EXPECT_TRUE(circuit.getVertexProperty<4>(0));
  EXPECT_TRUE(circuit.getVertexProperty<4>(1));
  EXPECT_TRUE(circuit.getVertexProperty<4>(2));
  EXPECT_TRUE(circuit.getVertexProperty<4>(3));
  EXPECT_TRUE(circuit.getVertexProperty<4>(4));
  EXPECT_TRUE(circuit.getVertexProperty<4>(5));
  EXPECT_TRUE(circuit.getVertexProperty<4>(6));
  EXPECT_TRUE(circuit.getVertexProperty<4>(7));
  EXPECT_TRUE(circuit.getVertexProperty<4>(8));
  EXPECT_TRUE(circuit.getVertexProperty<4>(9));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(10));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(11));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(12));
  EXPECT_TRUE(circuit.getVertexProperty<4>(13));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(14));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(15));
  EXPECT_TRUE(!circuit.getVertexProperty<4>(16));
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
