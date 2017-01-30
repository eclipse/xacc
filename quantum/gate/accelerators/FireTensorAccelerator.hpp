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
#ifndef QUANTUM_GATE_ACCELERATORS_FIRETENSORACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_FIRETENSORACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "Tensor.hpp"
#include "Graph.hpp"
#include "QasmToGraph.hpp"
#include "GraphIR.hpp"

namespace xacc {
namespace quantum {

using namespace fire;

double sqrt2 = std::sqrt(2.0);

/**
 *
 */
class FireTensorAccelerator : public Accelerator {
public:

	FireTensorAccelerator() {
		Tensor<2> h(2,2), cnot(4,4);
		h.setValues({{1.0/sqrt2,1.0/sqrt2},{1.0/sqrt2,-1.0/sqrt2}});
		cnot.setValues({{1, 0, 0, 0},{0, 1, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}});
		gates.emplace("h",h);
		gates.emplace("cnot",cnot);
	}
	virtual AcceleratorType getType() {
		return AcceleratorType::qpu_gate;
	}

	virtual std::vector<xacc::IRTransformation> getIRTransformations() {
		std::vector<xacc::IRTransformation> v;
		return v;
	}

	virtual void execute(const std::shared_ptr<xacc::IR> ir) {

		using GraphType = qci::common::Graph<CircuitNode>;

		// Cast to a GraphIR, if we can...
		auto graphir = std::dynamic_pointer_cast<xacc::GraphIR<GraphType>>(ir);
		if (!graphir) {
			QCIError("Invalid IR - this Accelerator on accepts GraphIR<Graph<CircuitNode>>.");
		}

		std::vector<CircuitNode> gateOperations;
		// Get the Graph
		auto graph = graphir->getGraph();
		int nNodes = graph.order(), layer = 1;
		int finalLayer = graph.getVertexProperty<1>(nNodes - 1);

		for (int i = 0; i < nNodes; i++) {
			CircuitNode n;
			n.properties = graph.getVertexProperties(i);
			gateOperations.emplace_back(n);
		}

		while (layer < finalLayer) {

			std::vector<CircuitNode> currentLayerGates;
			std::copy_if(gateOperations.begin(), gateOperations.end(),
					std::back_inserter(currentLayerGates),
					[&](const CircuitNode& c) {return std::get<1>(c.properties) == layer;});

			// Can parallize this...
			for (auto n : currentLayerGates) {
				auto gateName = std::get<0>(n.properties);
				auto actingQubits = std::get<3>(n.properties);
			}


			layer++;
		}


	}

	virtual ~FireTensorAccelerator() {
	}

protected:
	bool canAllocate(const int N) {
		return true;
	}

	std::map<std::string, Tensor<2>> gates;
};
}
}



#endif /* QUANTUM_GATE_ACCELERATORS_FIRETENSORACCELERATOR_HPP_ */
