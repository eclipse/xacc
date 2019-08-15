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
#include "TrivialEmbeddingAlgorithm.hpp"

using namespace xacc::quantum;

TEST(TrivialEmbeddingAlgorithmTester, checkSimpleEmbed) {

//   TrivialEmbeddingAlgorithm algo;
//   auto k44 = std::make_shared<K44Bipartite>();

//   // Set some params
//   std::map<std::string, std::string> params;
//   params.insert(std::make_pair("failhard", "false"));

//   auto triviallyMappedProblem = std::make_shared<DWGraph>(7);

//   triviallyMappedProblem->setVertexProperties(0, 20);
//   triviallyMappedProblem->setVertexProperties(1, 50);
//   triviallyMappedProblem->setVertexProperties(2, 60);
//   triviallyMappedProblem->setVertexProperties(4, 50);
//   triviallyMappedProblem->setVertexProperties(5, 60);
//   triviallyMappedProblem->setVertexProperties(6, -160);

//   triviallyMappedProblem->addEdge(2, 5); // -10000);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -10000);
//   triviallyMappedProblem->addEdge(1, 4);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -1000);

//   triviallyMappedProblem->addEdge(0, 4);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -14);
//   triviallyMappedProblem->addEdge(0, 5);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -12);

//   triviallyMappedProblem->addEdge(0, 6);
//   triviallyMappedProblem->setEdgeWeight(2, 5, 32);
//   triviallyMappedProblem->addEdge(1, 5);
//   triviallyMappedProblem->setEdgeWeight(2, 5, 68);
//   triviallyMappedProblem->addEdge(1, 6);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -128);
//   triviallyMappedProblem->addEdge(2, 6);
//   triviallyMappedProblem->setEdgeWeight(2, 5, -128);

//   auto embedding =
//       algo.embed(triviallyMappedProblem, k44->getAcceleratorGraph(), params);

//   int i = 0;
//   for (auto &kv : embedding) {
//     std::cout << kv.first << ": ";
//     for (auto h : kv.second) {
//       std::cout << h << " ";
//       EXPECT_TRUE(h == i);
//     }
//     std::cout << "\n";
//     EXPECT_TRUE(kv.first == i);
//     EXPECT_TRUE(kv.second.size() == 1);
//     i++;
//   }

//   auto complete5 = std::make_shared<CompleteGraph>(5);

//   embedding = algo.embed(complete5, k44->getAcceleratorGraph(), params);

//   EXPECT_TRUE(embedding.empty());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
