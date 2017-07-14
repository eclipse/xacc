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
#define BOOST_TEST_MODULE TrivialEmbeddingAlgorithm

#include <boost/test/included/unit_test.hpp>
#include "TrivialEmbeddingAlgorithm.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkSimpleEmbed) {

	TrivialEmbeddingAlgorithm algo;
	auto k44 = std::make_shared<K44Bipartite>();

	// Set some params
	std::map<std::string, std::string> params;
	params.insert(std::make_pair("failhard", "false"));

	auto triviallyMappedProblem = std::make_shared<DWGraph>(7);

	triviallyMappedProblem->setVertexProperties(0, 20);
	triviallyMappedProblem->setVertexProperties(1, 50);
	triviallyMappedProblem->setVertexProperties(2, 60);
	triviallyMappedProblem->setVertexProperties(4, 50);
	triviallyMappedProblem->setVertexProperties(5, 60);
	triviallyMappedProblem->setVertexProperties(6, -160);

	triviallyMappedProblem->addEdge(2, 5);// -10000);
	triviallyMappedProblem->setEdgeWeight(2,5, -10000);
	triviallyMappedProblem->addEdge(1,4);
	triviallyMappedProblem->setEdgeWeight(2,5, -1000);

	triviallyMappedProblem->addEdge(0,4);
	triviallyMappedProblem->setEdgeWeight(2,5, -14);
	triviallyMappedProblem->addEdge(0,5);
	triviallyMappedProblem->setEdgeWeight(2,5, -12);

	triviallyMappedProblem->addEdge(0, 6);
	triviallyMappedProblem->setEdgeWeight(2,5, 32);
	triviallyMappedProblem->addEdge(1, 5);
	triviallyMappedProblem->setEdgeWeight(2,5, 68);
	triviallyMappedProblem->addEdge(1, 6);
	triviallyMappedProblem->setEdgeWeight(2,5, -128);
	triviallyMappedProblem->addEdge(2,6);
	triviallyMappedProblem->setEdgeWeight(2,5, -128);

	auto embedding = algo.embed(triviallyMappedProblem, k44->getAcceleratorGraph(), params);

	int i = 0;
	for (auto& kv : embedding) {
		std::cout << kv.first << ": ";
		for (auto h : kv.second) {
			std::cout << h << " ";
			BOOST_VERIFY(h == i);
		}
		std::cout << "\n";
		BOOST_VERIFY(kv.first == i);
		BOOST_VERIFY(kv.second.size() == 1);
		i++;
	}

	auto complete5 = std::make_shared<CompleteGraph>(5);

	embedding = algo.embed(complete5, k44->getAcceleratorGraph(), params);

	BOOST_VERIFY(embedding.empty());
}




