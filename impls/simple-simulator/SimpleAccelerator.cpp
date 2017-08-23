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
#include "SimpleAccelerator.hpp"
#include <complex>
#include "SimpleAcceleratorVisitor.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {

std::shared_ptr<AcceleratorBuffer> SimpleAccelerator::createBuffer(
			const std::string& varId) {

	XACCError("NOT IMPLEMENTED YET");
}

std::shared_ptr<AcceleratorBuffer> SimpleAccelerator::createBuffer(
		const std::string& varId, const int size) {
	if (!isValidBufferSize(size)) {
		XACCError("Invalid buffer size.");
	}
	auto buffer = std::make_shared<SimulatedQubits>(varId, size);
	storeBuffer(varId, buffer);
	return buffer;
}

bool SimpleAccelerator::isValidBufferSize(const int NBits) {
	return NBits <= 10;
}

void SimpleAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer,
		const std::shared_ptr<xacc::Function> kernel) {

	// Cast to what we know should be SimulatedQubits
	auto qubits = std::static_pointer_cast<SimulatedQubits>(buffer);

	if (!qubits) {
		XACCError("Invalid derived AcceleratorBuffer passed to "
				"SimpleAccelerator. Must be of type SimulatedQubits.");
	}

	int nTrials = 1;
	if (xacc::optionExists("simple-n-trials")) {
		nTrials = std::stoi(xacc::getOption("simple-n-trials"));
	}

	for (int i = 0; i < nTrials; i++) {
		// Create a Visitor that will execute our lambdas when
		// we encounter one
		auto visitor = std::make_shared<SimpleAcceleratorVisitor>(qubits);

		// Our QIR is really a tree structure
		// so create a pre-order tree traversal
		// InstructionIterator to walk it
		InstructionIterator it(kernel);
		while (it.hasNext()) {
			// Get the next node in the tree
			auto nextInst = it.next();

			// If enabled, invoke the accept
			// method which kicks off the visitor
			// to execute the appropriate lambda.
			if (nextInst->isEnabled()) {
				nextInst->accept(visitor);
			}
		}

		boost::dynamic_bitset<> outcome(qubits->size());

		int countMeasurements = 0;
		int counter = qubits->size()-1;
		for (int i = 0; i < qubits->size(); i++) {
			auto s = qubits->getAcceleratorBitState(i);
			if (s == AcceleratorBitState::ONE) {
				 outcome[counter] = 1;
			}
			counter--;
		}

		qubits->appendMeasurement(outcome);
	}

}

}
}

