#include "QFT.hpp"
#include "GateInstruction.hpp"
#include "GateFunction.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<Function> QFT::generateAlgorithm(std::vector<int> qubits) {

	auto bitReversal =
			[](std::vector<int> qubits) -> std::vector<std::shared_ptr<Instruction>> {
				std::vector<std::shared_ptr<Instruction>> swaps;
				auto endStart = qubits.size() - 1;
				for (auto i = 0; i < std::floor(qubits.size() / 2.0); ++i) {
					swaps.push_back(GateInstructionRegistry::instance()->create("Swap", std::vector<int> {qubits[i], qubits[endStart]}));
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
					auto hadamard = GateInstructionRegistry::instance()->create("H", std::vector<int> {q});
					return std::vector<std::shared_ptr<Instruction>> {hadamard};
				} else {

					// Get the 1 the N qubits
					std::vector<int> qs(qubits.begin()+1, qubits.end());

					// Compute the number of qubits
					auto n = 1 + qs.size();

					// Build up a list of cphase gates
					std::vector<std::shared_ptr<GateInstruction>> cphaseGates;
					int idx = 0;
					for (int i = n-1; i > 0; --i) {
						auto q_idx = qs[idx];
						auto angle = 3.1415926 / std::pow(2, n - i);
						InstructionParameter p(angle);
						auto cp = GateInstructionRegistry::instance()->create("CPhase", std::vector<int> {q, q_idx});
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
					insts.push_back(GateInstructionRegistry::instance()->create("H", std::vector<int> {q}));

					// and return
					return insts;
				}
			};

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

