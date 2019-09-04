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
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "CMREmbedding.hpp"
#include "Graph.hpp"
#include "xacc_service.hpp"

using namespace xacc;
using namespace xacc::quantum;
using namespace xacc::cmr;

TEST(CMREmbeddingTester, checkReduction) {
	auto problem = xacc::getService<Graph>("boost-ugraph");//std::make_shared<xacc::quantum::CompleteGraph>(5);

    for (int i = 0; i < 5; i++) problem->addVertex();

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (i < j) problem->addEdge(i,j);
        }
    }

	auto k44 = xacc::getService<Graph>("boost-ugraph");//std::make_shared<xacc::quantum::K44Bipartite>();
    for (int i = 0; i < 8; i++) k44->addVertex();

    k44->addEdge(0,4);
    k44->addEdge(0,5);
    k44->addEdge(0,6);
    k44->addEdge(0,7);
    k44->addEdge(1,4);
    k44->addEdge(1,5);
    k44->addEdge(1,6);
    k44->addEdge(1,7);
    k44->addEdge(2,4);
    k44->addEdge(2,5);
    k44->addEdge(2,6);
    k44->addEdge(2,7);
    k44->addEdge(3,4);
    k44->addEdge(3,5);
    k44->addEdge(3,6);
    k44->addEdge(3,7);

	auto algo = std::make_shared<CMREmbedding>();
	auto embedding = algo->embed(problem, k44);

	std::cout << "HEY WORLD: \n";
	for (auto& kv : embedding) {
		std::cout << kv.first << " : ";
		for (auto l : kv.second) {
			std::cout << l << " ";
		}
		std::cout << "\n";
	}
}

int main(int argc, char** argv) {
   xacc::Initialize();
   ::testing::InitGoogleTest(&argc, argv);
   auto ret = RUN_ALL_TESTS();
   xacc::Finalize();
   return ret;
}
