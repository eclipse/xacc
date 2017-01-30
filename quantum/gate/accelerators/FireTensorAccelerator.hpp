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

/**
 *
 */
class FireTensorAccelerator : public Accelerator {
public:

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

	}

	virtual ~FireTensorAccelerator() {
	}

protected:
	bool canAllocate(const int N) {
		return true;
	}

};
}
}



#endif /* QUANTUM_GATE_ACCELERATORS_FIRETENSORACCELERATOR_HPP_ */
