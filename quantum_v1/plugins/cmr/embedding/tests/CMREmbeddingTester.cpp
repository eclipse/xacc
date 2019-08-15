
#include <gtest/gtest.h>
#include "XACC.hpp"
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
