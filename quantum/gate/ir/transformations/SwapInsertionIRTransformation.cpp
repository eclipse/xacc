#include "SwapInsertionIRTransformation.hpp"

namespace xacc {
namespace quantum {

std::shared_ptr<IR> SwapInsertionIRTransformation::transform(std::shared_ptr<IR> ir) {

	auto newir = std::make_shared<GateQIR>();
	for (auto kernel : ir->getKernels()) {

		for (auto inst : kernel->getInstructions()) {
			auto qbits = inst->bits();

			// If we have 2 qubit gate that does not have a corresponding
			// connection in the hardware, then we need to add swaps
			if (qbits.size() == 2 && !graph->edgeExists(qbits[0], qbits[1])) {

				std::vector<int> paths;
				std::vector<double> distances;

				graph->computeShortestPath(qbits[0], distances, paths);

			}
		}

	}

}
}
}
