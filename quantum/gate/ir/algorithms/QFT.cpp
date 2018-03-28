/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "QFT.hpp"
#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "XACC.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<Function> QFT::generate(
		std::shared_ptr<AcceleratorBuffer> buffer,
		std::vector<InstructionParameter> parameters) {
	auto gateRegistry = xacc::getService<InstructionService>("gate");

	auto bitReversal =
			[&](std::vector<int> qubits) -> std::vector<std::shared_ptr<Instruction>> {
				std::vector<std::shared_ptr<Instruction>> swaps;
				auto endStart = qubits.size() - 1;
				for (auto i = 0; i < std::floor(qubits.size() / 2.0); ++i) {
					swaps.push_back(gateRegistry->create("Swap", std::vector<int> {qubits[i], qubits[endStart]}));
					endStart--;
				}

				return swaps;

			};

	std::function<std::vector<std::shared_ptr<Instruction>>(std::vector<int>&)> coreqft;
	coreqft =
			[&](std::vector<int>& qubits) -> std::vector<std::shared_ptr<Instruction>> {

				// Get the first qubit
				auto q = qubits[0];

				// If we have only one left, then
				// just return a hadamard, if not,
				// then we need to build up some cphase gates
				if (qubits.size() == 1) {
					auto hadamard = gateRegistry->create("H", std::vector<int> {q});
					return std::vector<std::shared_ptr<Instruction>> {hadamard};
				} else {

					// Get the 1 the N qubits
					std::vector<int> qs(qubits.begin()+1, qubits.end());

					// Compute the number of qubits
					auto n = 1 + qs.size();

					// Build up a list of cphase gates
					std::vector<std::shared_ptr<Instruction>> cphaseGates;
					int idx = 0;
					for (int i = n-1; i > 0; --i) {
						auto q_idx = qs[idx];
						auto angle = 3.1415926 / std::pow(2, n - i);
						InstructionParameter p(angle);
						auto cp = gateRegistry->create("CPhase", std::vector<int> {q, q_idx});
						cp->setParameter(0, p);
						cphaseGates.push_back(cp);
						idx++;
					}

					// Recursively build these up...
					auto insts = coreqft(qs);

					// Reverse the cphase gates
					std::reverse(cphaseGates.begin(), cphaseGates.end());

					// Add them to the return list
					for (auto cp : cphaseGates) {
						insts.push_back(cp);
					}

					// add a final hadamard...
					insts.push_back(gateRegistry->create("H", std::vector<int> {q}));

					// and return
					return insts;
				}
			};

	auto bufferSize = buffer->size();
	std::vector<int> qubits;
	for (int i = 0; i < bufferSize; i++) {
		qubits.push_back(i);
	}

	auto qftInstructions = coreqft(qubits);

	auto swaps = bitReversal(qubits);
	for (auto s : swaps) {
		qftInstructions.push_back(s);
	}

	auto qftKernel = std::make_shared<GateFunction>("qft");
	for (auto i : qftInstructions) {
		qftKernel->addInstruction(i);
	}

	return qftKernel;
}

}
}

