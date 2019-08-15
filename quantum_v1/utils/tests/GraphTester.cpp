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
#include "Graph.hpp"

using namespace boost;
using namespace xacc;

class BadVertex {};

class FakeTensorVertex : public XACCVertex<std::string> {};

class FakeBiasVertex : public XACCVertex<double> {
public:
  FakeBiasVertex() : XACCVertex() { propertyNames[0] = "bias"; }
};

class FakeVertexFourProperties
    : public XACCVertex<std::string, double, int, float> {
public:
  FakeVertexFourProperties() : XACCVertex() {
    propertyNames[0] = "prop1";
    propertyNames[1] = "prop2";
    propertyNames[2] = "prop3";
    propertyNames[3] = "prop4";
  }
};

class FakeVertexFourPropertiesPrint2
    : public XACCVertex<std::string, double, int, float> {
public:
  FakeVertexFourPropertiesPrint2() : XACCVertex() {
    propertyNames[0] = "prop1";
    propertyNames[1] = "prop2";
  }
};

class FakeVertexWithVector : public XACCVertex<std::vector<int>> {
public:
  FakeVertexWithVector() : XACCVertex() { propertyNames[0] = "prop1"; }
};
TEST(GraphTester, checkDirected) {

  Graph<FakeBiasVertex> g;
  g.addVertex();
  g.addVertex();
  g.addVertex();

  g.addEdge(0, 1);
  g.addEdge(1, 2);

  auto n = g.getNeighborList(1);

  for (auto i : n) {
    std::cout << "NEIGHBOR: " << i << "\n";
  }
}

TEST(GraphTester, checkConstruction) {

  // Check our valid vertex functions...
  EXPECT_TRUE(!is_valid_vertex<BadVertex>::value);
  EXPECT_TRUE(is_valid_vertex<FakeTensorVertex>::value);

  // Create a blank Graph with good vertices
  Graph<FakeTensorVertex> g1;
  g1.addVertex();
  EXPECT_TRUE(g1.order() == 1);

  // Create with 5 vertices
  Graph<FakeTensorVertex> g2(5);
  EXPECT_TRUE(g2.order() == 5);

  // Create a 3 node graph
  Graph<FakeBiasVertex> graph(3);

  // Verify it's size
  EXPECT_TRUE(3 == graph.order());

  // Create a complete graph with
  // the given edge weights.
  graph.addEdge(0, 1, 2.0);
  graph.addEdge(1, 2, 3.0);
  graph.addEdge(2, 0, 1.0);

  EXPECT_TRUE(2.0 == graph.getEdgeWeight(0, 1));

  // Test that we can change a weight
  graph.setEdgeWeight(0, 1, 22.0);
  EXPECT_TRUE(22.0 == graph.getEdgeWeight(0, 1));

  // Verify that we can set vertex bias values
  EXPECT_TRUE(0.0 == graph.getVertexProperty<0>(0));
  EXPECT_TRUE(0.0 == graph.getVertexProperty<0>(1));
  graph.setVertexProperty<0>(0, 3.3);
  EXPECT_TRUE(3.3 == graph.getVertexProperty<0>(0));
  graph.setVertexProperty<0>(1, 33.3);
  EXPECT_TRUE(33.3 == graph.getVertexProperty<0>(1));

  graph.getNeighborList(0);
}

TEST(GraphTester, checkAddVertexWithProperties) {
  Graph<FakeBiasVertex> graph;
  graph.addVertex(22.2);
  EXPECT_TRUE(graph.getVertexProperty<0>(0) == 22.2);
}

TEST(GraphTester, checkEdgeExists) {
  // Create a 2 node graph
  Graph<FakeBiasVertex> graph(3);

  // Verify it's size
  EXPECT_EQ(3, graph.order());

  graph.addEdge(0, 1, 2.0);
  graph.addEdge(1, 2, 3.0);
  graph.addEdge(2, 0, 1.0);

  EXPECT_EQ(3, graph.size());

  EXPECT_TRUE(graph.edgeExists(0, 1));
  EXPECT_TRUE(graph.edgeExists(1, 0));
  EXPECT_TRUE(graph.edgeExists(1, 2));
  EXPECT_TRUE(graph.edgeExists(2, 1));
  EXPECT_TRUE(graph.edgeExists(2, 0));
  EXPECT_TRUE(graph.edgeExists(0, 2));
}

TEST(GraphTester, checkDegree) {
  Graph<FakeBiasVertex> complete5(5);
  complete5.addEdge(0, 1);
  complete5.addEdge(0, 2);
  complete5.addEdge(0, 3);
  complete5.addEdge(0, 4);
  complete5.addEdge(1, 2);
  complete5.addEdge(1, 3);
  complete5.addEdge(1, 4);
  complete5.addEdge(2, 3);
  complete5.addEdge(2, 4);
  complete5.addEdge(3, 4);

  for (int i = 0; i < 5; i++) {
    EXPECT_TRUE(4 == complete5.degree(i));
  }
}

TEST(GraphTester, checkDiameter) {
  // Create a Complete 5 node graph
  Graph<FakeBiasVertex> complete5(5);
  complete5.addEdge(0, 1);
  complete5.addEdge(0, 2);
  complete5.addEdge(0, 3);
  complete5.addEdge(0, 4);
  complete5.addEdge(1, 2);
  complete5.addEdge(1, 3);
  complete5.addEdge(1, 4);
  complete5.addEdge(2, 3);
  complete5.addEdge(2, 4);
  complete5.addEdge(3, 4);
  EXPECT_EQ(1, complete5.diameter());
}

TEST(GraphTester, checkWrite) {
  Graph<FakeBiasVertex> complete5(5);

  std::string expected = R"expected(graph G {
node [shape=box style=filled]
0 [label="bias=1"];
1 [label="bias=2"];
2 [label="bias=3"];
3 [label="bias=4"];
4 [label="bias=5"];
0--1 [label="weight=0.000000"];
0--2 [label="weight=0.000000"];
0--3 [label="weight=0.000000"];
0--4 [label="weight=0.000000"];
1--2 [label="weight=0.000000"];
1--3 [label="weight=0.000000"];
1--4 [label="weight=0.000000"];
2--3 [label="weight=0.000000"];
2--4 [label="weight=0.000000"];
3--4 [label="weight=0.000000"];
}
)expected";
  complete5.setVertexProperty<0>(0, 1.0);
  complete5.setVertexProperty<0>(1, 2.0);
  complete5.setVertexProperty<0>(2, 3.0);
  complete5.setVertexProperty<0>(3, 4.0);
  complete5.setVertexProperty<0>(4, 5.0);

  complete5.addEdge(0, 1);
  complete5.addEdge(0, 2);
  complete5.addEdge(0, 3);
  complete5.addEdge(0, 4);
  complete5.addEdge(1, 2);
  complete5.addEdge(1, 3);
  complete5.addEdge(1, 4);
  complete5.addEdge(2, 3);
  complete5.addEdge(2, 4);
  complete5.addEdge(3, 4);

  std::stringstream ss;
  std::cout << "WRITING:\n";
  complete5.write(ss);
  std::cout << ss.str() << "\nWROTE IT\n";
  EXPECT_TRUE(ss.str() == expected);

  Graph<FakeVertexFourProperties> complete5_4props(5);

  expected = R"expected(graph G {
node [shape=box style=filled]
0 [label="prop1=val1;prop2=1;prop3=1;prop4=1"];
1 [label="prop1=val2;prop2=2;prop3=2;prop4=2"];
2 [label="prop1=val3;prop2=3;prop3=3;prop4=3"];
3 [label="prop1=val4;prop2=4;prop3=4;prop4=4"];
4 [label="prop1=val5;prop2=5;prop3=5;prop4=5"];
0--1 [label="weight=0.000000"];
0--2 [label="weight=0.000000"];
0--3 [label="weight=0.000000"];
0--4 [label="weight=0.000000"];
1--2 [label="weight=0.000000"];
1--3 [label="weight=0.000000"];
1--4 [label="weight=0.000000"];
2--3 [label="weight=0.000000"];
2--4 [label="weight=0.000000"];
3--4 [label="weight=0.000000"];
}
)expected";

  complete5_4props.setVertexProperty<0>(0, "val1");
  complete5_4props.setVertexProperty<0>(1, "val2");
  complete5_4props.setVertexProperty<0>(2, "val3");
  complete5_4props.setVertexProperty<0>(3, "val4");
  complete5_4props.setVertexProperty<0>(4, "val5");

  complete5_4props.setVertexProperty<1>(0, 1.0);
  complete5_4props.setVertexProperty<1>(1, 2.0);
  complete5_4props.setVertexProperty<1>(2, 3.0);
  complete5_4props.setVertexProperty<1>(3, 4.0);
  complete5_4props.setVertexProperty<1>(4, 5.0);

  complete5_4props.setVertexProperty<2>(0, 1);
  complete5_4props.setVertexProperty<2>(1, 2);
  complete5_4props.setVertexProperty<2>(2, 3);
  complete5_4props.setVertexProperty<2>(3, 4);
  complete5_4props.setVertexProperty<2>(4, 5);

  complete5_4props.setVertexProperty<3>(0, 1.0);
  complete5_4props.setVertexProperty<3>(1, 2.0);
  complete5_4props.setVertexProperty<3>(2, 3.0);
  complete5_4props.setVertexProperty<3>(3, 4.0);
  complete5_4props.setVertexProperty<3>(4, 5.0);

  complete5_4props.addEdge(0, 1);
  complete5_4props.addEdge(0, 2);
  complete5_4props.addEdge(0, 3);
  complete5_4props.addEdge(0, 4);
  complete5_4props.addEdge(1, 2);
  complete5_4props.addEdge(1, 3);
  complete5_4props.addEdge(1, 4);
  complete5_4props.addEdge(2, 3);
  complete5_4props.addEdge(2, 4);
  complete5_4props.addEdge(3, 4);

  std::stringstream ss2;
  std::cout << "AGAIN\n";
  complete5_4props.write(ss2);
  std::cout << ss2.str() << "\nHOWDY\n";
  EXPECT_TRUE(ss2.str() == expected);

  // Create a 3 node graph
  Graph<FakeVertexFourPropertiesPrint2> graph(3);

  // Create a complete graph with
  // the given edge weights.
  graph.addEdge(0, 1, 2.0);
  graph.addEdge(1, 2, 3.0);
  graph.addEdge(2, 0, 1.0);

  graph.setVertexProperty<0>(0, "val1");
  graph.setVertexProperty<0>(1, "val2");
  graph.setVertexProperty<0>(2, "val3");

  graph.setVertexProperty<1>(0, 1.0);
  graph.setVertexProperty<1>(1, 2.0);
  graph.setVertexProperty<1>(2, 3.0);

  expected = "graph G {\n"
             "node [shape=box style=filled]\n"
             "0 [label=\"prop1=val1;prop2=1\"];\n"
             "1 [label=\"prop1=val2;prop2=2\"];\n"
             "2 [label=\"prop1=val3;prop2=3\"];\n"
             "0--1 [label=\"weight=2.000000\"];\n"
             "1--2 [label=\"weight=3.000000\"];\n"
             "2--0 [label=\"weight=1.000000\"];\n"
             "}\n";

  std::stringstream ss3;
  std::cout << "MADE IT\n";
  graph.write(ss3);
  std::cout << "MADE IT HERE\n";
  EXPECT_TRUE(ss3.str() == expected);

  // Create a 3 node graph
  Graph<FakeVertexWithVector> graph2(3);

  // Create a complete graph with
  // the given edge weights.
  graph2.addEdge(0, 1, 2.0);
  graph2.addEdge(1, 2, 3.0);
  graph2.addEdge(2, 0, 1.0);

  std::vector<int> hello{1, 2};
  graph2.setVertexProperty<0>(0, std::vector<int>{1, 2});
  graph2.setVertexProperty<0>(1, std::vector<int>{1, 2});
  graph2.setVertexProperty<0>(2, std::vector<int>{1, 2});

  expected = "graph G {\n"
             "node [shape=box style=filled]\n"
             "0 [label=\"prop1=[1, 2]\"];\n"
             "1 [label=\"prop1=[1, 2]\"];\n"
             "2 [label=\"prop1=[1, 2]\"];\n"
             "0--1 [label=\"weight=2.000000\"];\n"
             "1--2 [label=\"weight=3.000000\"];\n"
             "2--0 [label=\"weight=1.000000\"];\n"
             "}\n";

  std::stringstream ss4;
  std::cout << "FINALLY\n";
  graph2.write(ss4);
  std::cout << ss4.str() << "\nPROBS NOT\n";
//   EXPECT_TRUE(ss4.str() == expected);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
// TEST(GraphTester,checkRead) {
//	// Create a graph IR modeling a
//	// quantum teleportation kernel
//	std::string irstr = "graph G {\n"
//			"{\nnode [shape=box style=filled]\n"
//			"0 [label=\"Gate=InitialState,Circuit Layer=0,Gate
// Vertex Id=0,Gate Acting Qubits=[0 1 2 3 4]\"];\n" 			"1
// [label=\"Gate=x,Circuit Layer=1,Gate Vertex Id=1,Gate Acting
// Qubits=[0]\"];\n" "2 [label=\"Gate=h,Circuit Layer=1,Gate Vertex Id=2,Gate
// Acting
// Qubits=[1]\"];\n" 			"3 [label=\"Gate=cnot,Circuit
// Layer=2,Gate Vertex Id=3,Gate Acting Qubits=[1 2]\"];\n" "4
// [label=\"Gate=cnot,Circuit Layer=3,Gate Vertex Id=4,Gate Acting Qubits=[0
// 1]\"];\n" 			"5 [label=\"Gate=h,Circuit Layer=4,Gate Vertex
// Id=5,Gate Acting Qubits=[0]\"];\n" "6 [label=\"Gate=measure,Circuit
// Layer=5,Gate Vertex Id=6,Gate Acting Qubits=[0]\"];\n" "7
// [label=\"Gate=measure,Circuit
// Layer=5,Gate Vertex Id=7,Gate Acting
// Qubits=[1]\"];\n" 			"8 [label=\"Gate=h,Circuit Layer=6,Gate
// Vertex Id=8,Gate Acting Qubits=[2]\"];\n" 			"9
// [label=\"Gate=cnot,Circuit Layer=7,Gate Vertex Id=9,Gate Acting Qubits=[2
// 1]\"];\n" 			"10 [label=\"Gate=h,Circuit Layer=8,Gate Vertex
// Id=10,Gate Acting Qubits=[2]\"];\n" 			"11
// [label=\"Gate=cnot,Circuit Layer=9,Gate Vertex Id=11,Gate Acting Qubits=[2
// 0]\"];\n" 			"12 [label=\"Gate=FinalState,Circuit
// Layer=10,Gate Vertex Id=12,Gate Acting Qubits=[0 1 2 3 4]\"];\n"
//			"}\n"
//			"0--1 ;\n"
//			"0--2 ;\n"
//			"2--3 ;\n"
//			"0--3 ;\n"
//			"1--4 ;\n"
//			"3--4 ;\n"
//			"4--5 ;\n"
//			"5--6 ;\n"
//			"4--7 ;\n"
//			"3--8 ;\n"
//			"8--9 ;\n"
//			"7--9 ;\n"
//			"9--10 ;\n"
//			"10--11 ;\n"
//			"6--11 ;\n"
//			"11--12 ;\n"
//			"11--12 ;\n"
//			"9--12 ;\n"
//			"}";
//	std::istringstream iss(irstr);
//
//	// Create a 3 node graph
//	Graph<FakeVertexFourProperties> graph;
//	graph.read(iss);
//
//	EXPECT_TRUE(graph.order() == 13);
//
//	graph.write(std::cout);
//
//}
