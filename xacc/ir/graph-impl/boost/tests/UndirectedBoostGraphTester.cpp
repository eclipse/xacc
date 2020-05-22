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
#include "UndirectedBoostGraph.hpp"

using namespace boost;
using namespace xacc;

TEST(UndirectedGraphTester, checkConstruction) {

  // Create a blank Graph with good vertices
  UndirectedBoostGraph g1;
  g1.addVertex();
  EXPECT_TRUE(g1.order() == 1);

  // Create with 5 vertices
  UndirectedBoostGraph g2(5);
  EXPECT_TRUE(g2.order() == 5);

  // Create a 3 node graph
  UndirectedBoostGraph graph;
  graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});
    graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});
  graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});

  // Verify it's size
  EXPECT_TRUE(3 == graph.order());

  // Create a complete graph with
  // the given edge weights.
  graph.addEdge(0, 1, 2.0);
  graph.addEdge(1, 2, 3.0);
  graph.addEdge(2, 0, 1.0);

  EXPECT_EQ(2.0 , graph.getEdgeWeight(0, 1));

  // Test that we can change a weight
  graph.setEdgeWeight(0, 1, 22.0);
  EXPECT_EQ(22.0 , graph.getEdgeWeight(0, 1));

  // Verify that we can set vertex bias values
  EXPECT_EQ(0.0 , graph.getVertexProperties(0).get<double>("bias"));//.as<double>());
  EXPECT_EQ(0.0 , graph.getVertexProperties(1).get<double>( "bias"));//as<double>());
  graph.getVertexProperties(0).insert("bias", 3.3);
  EXPECT_EQ(3.3 , graph.getVertexProperties(0).get<double>("bias"));//.as<double>());
   graph.getVertexProperties(1).insert("bias", 33.3);
  EXPECT_EQ(33.3 , graph.getVertexProperties(1).get<double>("bias"));
//   graph.setVertexProperty(1, "bias", 33.3);
//   EXPECT_EQ(33.3 , graph.getVertexProperty(1, "bias").as<double>());

  auto n = graph.getNeighborList(0);
  EXPECT_EQ(2, n.size());

  std::stringstream ss;
  graph.write(ss);
  std::cout << "U\n" << ss.str() << "\n";
}

TEST(UndirectedGraphTester, checkEdgeExists) {
  // Create a 2 node graph
  UndirectedBoostGraph graph;
  graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});
  graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});
  graph.addVertex(HeterogeneousMap{std::make_pair("bias",0.0)});

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

TEST(UndirectedGraphTester, checkDegree) {
  UndirectedBoostGraph complete5(5);

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
    EXPECT_EQ(4 , complete5.degree(i));
  }
}

TEST(UndirectedGraphTester, checkDiameter) {
  // Create a Complete 5 node graph
  UndirectedBoostGraph complete5(5);
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

// TEST(UndirectedGraphTester, checkWrite) {
//   Graph<FakeBiasVertex> complete5(5);

//   std::string expected = R"expected(graph G {
// node [shape=box style=filled]
// 0 [label="bias=1"];
// 1 [label="bias=2"];
// 2 [label="bias=3"];
// 3 [label="bias=4"];
// 4 [label="bias=5"];
// 0--1 [label="weight=0.000000"];
// 0--2 [label="weight=0.000000"];
// 0--3 [label="weight=0.000000"];
// 0--4 [label="weight=0.000000"];
// 1--2 [label="weight=0.000000"];
// 1--3 [label="weight=0.000000"];
// 1--4 [label="weight=0.000000"];
// 2--3 [label="weight=0.000000"];
// 2--4 [label="weight=0.000000"];
// 3--4 [label="weight=0.000000"];
// }
// )expected";
//   complete5.setVertexProperty<0>(0, 1.0);
//   complete5.setVertexProperty<0>(1, 2.0);
//   complete5.setVertexProperty<0>(2, 3.0);
//   complete5.setVertexProperty<0>(3, 4.0);
//   complete5.setVertexProperty<0>(4, 5.0);

//   complete5.addEdge(0, 1);
//   complete5.addEdge(0, 2);
//   complete5.addEdge(0, 3);
//   complete5.addEdge(0, 4);
//   complete5.addEdge(1, 2);
//   complete5.addEdge(1, 3);
//   complete5.addEdge(1, 4);
//   complete5.addEdge(2, 3);
//   complete5.addEdge(2, 4);
//   complete5.addEdge(3, 4);

//   std::stringstream ss;
//   std::cout << "WRITING:\n";
//   complete5.write(ss);
//   std::cout << ss.str() << "\nWROTE IT\n";
//   EXPECT_TRUE(ss.str() == expected);

//   Graph<FakeVertexFourProperties> complete5_4props(5);

//   expected = R"expected(graph G {
// node [shape=box style=filled]
// 0 [label="prop1=val1;prop2=1;prop3=1;prop4=1"];
// 1 [label="prop1=val2;prop2=2;prop3=2;prop4=2"];
// 2 [label="prop1=val3;prop2=3;prop3=3;prop4=3"];
// 3 [label="prop1=val4;prop2=4;prop3=4;prop4=4"];
// 4 [label="prop1=val5;prop2=5;prop3=5;prop4=5"];
// 0--1 [label="weight=0.000000"];
// 0--2 [label="weight=0.000000"];
// 0--3 [label="weight=0.000000"];
// 0--4 [label="weight=0.000000"];
// 1--2 [label="weight=0.000000"];
// 1--3 [label="weight=0.000000"];
// 1--4 [label="weight=0.000000"];
// 2--3 [label="weight=0.000000"];
// 2--4 [label="weight=0.000000"];
// 3--4 [label="weight=0.000000"];
// }
// )expected";

//   complete5_4props.setVertexProperty<0>(0, "val1");
//   complete5_4props.setVertexProperty<0>(1, "val2");
//   complete5_4props.setVertexProperty<0>(2, "val3");
//   complete5_4props.setVertexProperty<0>(3, "val4");
//   complete5_4props.setVertexProperty<0>(4, "val5");

//   complete5_4props.setVertexProperty<1>(0, 1.0);
//   complete5_4props.setVertexProperty<1>(1, 2.0);
//   complete5_4props.setVertexProperty<1>(2, 3.0);
//   complete5_4props.setVertexProperty<1>(3, 4.0);
//   complete5_4props.setVertexProperty<1>(4, 5.0);

//   complete5_4props.setVertexProperty<2>(0, 1);
//   complete5_4props.setVertexProperty<2>(1, 2);
//   complete5_4props.setVertexProperty<2>(2, 3);
//   complete5_4props.setVertexProperty<2>(3, 4);
//   complete5_4props.setVertexProperty<2>(4, 5);

//   complete5_4props.setVertexProperty<3>(0, 1.0);
//   complete5_4props.setVertexProperty<3>(1, 2.0);
//   complete5_4props.setVertexProperty<3>(2, 3.0);
//   complete5_4props.setVertexProperty<3>(3, 4.0);
//   complete5_4props.setVertexProperty<3>(4, 5.0);

//   complete5_4props.addEdge(0, 1);
//   complete5_4props.addEdge(0, 2);
//   complete5_4props.addEdge(0, 3);
//   complete5_4props.addEdge(0, 4);
//   complete5_4props.addEdge(1, 2);
//   complete5_4props.addEdge(1, 3);
//   complete5_4props.addEdge(1, 4);
//   complete5_4props.addEdge(2, 3);
//   complete5_4props.addEdge(2, 4);
//   complete5_4props.addEdge(3, 4);

//   std::stringstream ss2;
//   std::cout << "AGAIN\n";
//   complete5_4props.write(ss2);
//   std::cout << ss2.str() << "\nHOWDY\n";
//   EXPECT_TRUE(ss2.str() == expected);

//   // Create a 3 node graph
//   Graph<FakeVertexFourPropertiesPrint2> graph(3);

//   // Create a complete graph with
//   // the given edge weights.
//   graph.addEdge(0, 1, 2.0);
//   graph.addEdge(1, 2, 3.0);
//   graph.addEdge(2, 0, 1.0);

//   graph.setVertexProperty<0>(0, "val1");
//   graph.setVertexProperty<0>(1, "val2");
//   graph.setVertexProperty<0>(2, "val3");

//   graph.setVertexProperty<1>(0, 1.0);
//   graph.setVertexProperty<1>(1, 2.0);
//   graph.setVertexProperty<1>(2, 3.0);

//   expected = "graph G {\n"
//              "node [shape=box style=filled]\n"
//              "0 [label=\"prop1=val1;prop2=1\"];\n"
//              "1 [label=\"prop1=val2;prop2=2\"];\n"
//              "2 [label=\"prop1=val3;prop2=3\"];\n"
//              "0--1 [label=\"weight=2.000000\"];\n"
//              "1--2 [label=\"weight=3.000000\"];\n"
//              "2--0 [label=\"weight=1.000000\"];\n"
//              "}\n";

//   std::stringstream ss3;
//   std::cout << "MADE IT\n";
//   graph.write(ss3);
//   std::cout << "MADE IT HERE\n";
//   EXPECT_TRUE(ss3.str() == expected);

//   // Create a 3 node graph
//   Graph<FakeVertexWithVector> graph2(3);

//   // Create a complete graph with
//   // the given edge weights.
//   graph2.addEdge(0, 1, 2.0);
//   graph2.addEdge(1, 2, 3.0);
//   graph2.addEdge(2, 0, 1.0);

//   std::vector<int> hello{1, 2};
//   graph2.setVertexProperty<0>(0, std::vector<int>{1, 2});
//   graph2.setVertexProperty<0>(1, std::vector<int>{1, 2});
//   graph2.setVertexProperty<0>(2, std::vector<int>{1, 2});

//   expected = "graph G {\n"
//              "node [shape=box style=filled]\n"
//              "0 [label=\"prop1=[1, 2]\"];\n"
//              "1 [label=\"prop1=[1, 2]\"];\n"
//              "2 [label=\"prop1=[1, 2]\"];\n"
//              "0--1 [label=\"weight=2.000000\"];\n"
//              "1--2 [label=\"weight=3.000000\"];\n"
//              "2--0 [label=\"weight=1.000000\"];\n"
//              "}\n";

//   std::stringstream ss4;
//   std::cout << "FINALLY\n";
//   graph2.write(ss4);
//   std::cout << ss4.str() << "\nPROBS NOT\n";
// //   EXPECT_TRUE(ss4.str() == expected);
// }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}