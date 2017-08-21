/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#define BOOST_TEST_MODULE RigettiAcceleratorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "RigettiAccelerator.hpp"
#include "JsonVisitor.hpp"

using namespace xacc::quantum;
BOOST_AUTO_TEST_CASE(checkKernelExecution) {

	/*auto options = RuntimeOptions::instance();
	options->insert(std::make_pair("rigetti-api-key", "fakekey"));
	options->insert(std::make_pair("rigetti-type", "faketype"));

	auto client = std::make_shared<FakeHttpClient>();
	RigettiAccelerator acc(client);

	auto qreg1 = acc.createBuffer("qreg", 3);
	auto f = std::make_shared<GateFunction>("foo");

	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);

	auto cond1 = std::make_shared<ConditionalFunction>(0);
	auto z = std::make_shared<Z>(2);
	cond1->addInstruction(z);
	auto cond2 = std::make_shared<ConditionalFunction>(1);
	auto x2 = std::make_shared<X>(2);
	cond2->addInstruction(x2);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(cond1);
	f->addInstruction(cond2);

	acc.execute(qreg1, f);

	BOOST_VERIFY(client->postOccured);*/
}

BOOST_AUTO_TEST_CASE(buildQFT) {

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
					return std::vector<std::shared_ptr<Instruction>>{hadamard};
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

	std::vector<int> qbits {0, 1, 2};
	auto qftInstructions = coreqft(qbits);

	auto swaps = bitReversal(qbits);
	for (auto s : swaps) {
		qftInstructions.push_back(s);
	}

	auto qftKernel = std::make_shared<GateFunction>("foo");
	for (auto i : qftInstructions) {
		qftKernel->addInstruction(i);
	}

//	JsonVisitor v(qftKernel);
//	std::cout << v.write() << "\n";

	std::string expectedQuil(
			"H 2\n"
			"CPHASE(1.5708) 1 2\n"
			"H 1\n"
			"CPHASE(0.785398) 0 2\n"
			"CPHASE(1.5708) 0 1\n"
			"H 0\n"
			"SWAP 0 2\n");

	auto quilV = std::make_shared<QuilVisitor>();

	InstructionIterator it(qftKernel);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();

		// If enabled, invoke the accept
		// method which kicks off the visitor
		// to execute the appropriate lambda.
		if (nextInst->isEnabled()) {
			nextInst->accept(quilV);
		}
	}
	std::cout << quilV->getQuilString() << "\n" << expectedQuil << "\n";

	BOOST_VERIFY(quilV->getQuilString() == expectedQuil);

	expectedQuil =
			"H 4\n"
			"CPHASE(1.5708) 3 4\n"
			"H 3\n"
			"CPHASE(0.785398) 2 4\n"
			"CPHASE(1.5708) 2 3\n"
			"H 2\n"
			"CPHASE(0.392699) 1 4\n"
			"CPHASE(0.785398) 1 3\n"
			"CPHASE(1.5708) 1 2\n"
			"H 1\n"
			"CPHASE(0.19635) 0 4\n"
			"CPHASE(0.392699) 0 3\n"
			"CPHASE(0.785398) 0 2\n"
			"CPHASE(1.57078) 0 1\n"
			"H 0\n"
			"SWAP 0 4\n"
			"SWAP 1 3\n";

	std::vector<int> qbits5 {0, 1, 2, 3, 4};
	auto qft5Instructions = coreqft(qbits5);

	swaps = bitReversal(qbits5);
	for (auto s : swaps) {
		qft5Instructions.push_back(s);
	}

	auto qft5Kernel = std::make_shared<GateFunction>("foo");
	for (auto i : qft5Instructions) {
		qft5Kernel->addInstruction(i);
	}


//	JsonVisitor v5(qft5Kernel);
//	std::cout << v5.write() << "\n";

	auto quilV5 = std::make_shared<QuilVisitor>();

	InstructionIterator it5(qft5Kernel);
	while (it5.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it5.next();

		// If enabled, invoke the accept
		// method which kicks off the visitor
		// to execute the appropriate lambda.
		if (nextInst->isEnabled()) {
			nextInst->accept(quilV5);
		}
	}

	std::cout << quilV5->getQuilString() << "\n" << expectedQuil << "\n";
}


