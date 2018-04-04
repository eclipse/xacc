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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ReadoutErrorAcceleratorBufferPostprocessorTester

#include <boost/test/included/unit_test.hpp>
#include "ReadoutErrorAcceleratorBufferPostprocessor.hpp"
#include "GateIR.hpp"
#include <boost/math/constants/constants.hpp>
#include "XACC.hpp"
#include "GateFunction.hpp"
#include "GateInstruction.hpp"
#include "GateIRProvider.hpp"

using namespace xacc;

using namespace xacc::quantum;

using Term = std::map<int, std::string>;

std::shared_ptr<IR> createXACCIR(std::unordered_map<std::string, Term> terms) {
// Create a new GateQIR to hold the spin based terms
	auto newIr = std::make_shared<xacc::quantum::GateIR>();
	int counter = 0;
	auto pi = boost::math::constants::pi<double>();

	GateIRProvider gateRegistry;

	// Populate GateQIR now...
	for (auto& inst : terms) {

		Term spinInst = inst.second;

		// Create a GateFunction and specify that it has
		// a parameter that is the Spin Instruction coefficient
		// that will help us get it to the user for their purposes.
		auto gateFunction = std::make_shared<xacc::quantum::GateFunction>(
				"term" + std::to_string(counter));

		// Loop over all terms in the Spin Instruction
		// and create instructions to run on the Gate QPU.
		std::vector<std::shared_ptr<xacc::Instruction>> measurements;

		std::vector<std::pair<int, std::string>> terms;
		for (auto& kv : spinInst) {
			if (kv.second != "I" && !kv.second.empty()) {
				terms.push_back( { kv.first, kv.second });
			}
		}

		for (int i = terms.size() - 1; i >= 0; i--) {
			auto qbit = terms[i].first;
			auto gateName = terms[i].second;
			auto meas = gateRegistry.createInstruction("Measure",
					std::vector<int> { qbit });
			xacc::InstructionParameter classicalIdx(qbit);
			meas->setParameter(0, classicalIdx);
			measurements.push_back(meas);

			if (gateName == "X") {
				auto hadamard = gateRegistry.createInstruction("H", std::vector<int> {
						qbit });
				gateFunction->addInstruction(hadamard);
			} else if (gateName == "Y") {
				auto rx = gateRegistry.createInstruction("Rx", std::vector<int> { qbit });
				InstructionParameter p(pi / 2.0);
				rx->setParameter(0, p);
				gateFunction->addInstruction(rx);
			}

		}

		for (auto m : measurements) {
			gateFunction->addInstruction(m);
		}

		newIr->addKernel(gateFunction);
		counter++;
	}

	return newIr;
}

class FakeTermAB : public AcceleratorBuffer {
public:

	FakeTermAB(const std::string& str, const int N) : AcceleratorBuffer(str, N) {}
	virtual const double getExpectationValueZ() {
		return .88;
	}
};

class FakeMeasureAB : public AcceleratorBuffer {
public:

	FakeMeasureAB(const std::string& str, const int N) : AcceleratorBuffer(str, N) {}
	virtual double computeMeasurementProbability(const std::string& bitStr) {
		if (bitStr == "1") {
			return .08;
		} else {
			return .01;
		}
	}

};

BOOST_AUTO_TEST_CASE(checkSimple) {

	xacc::Initialize();
	// Simple example H = X0
	std::vector<std::string> orderedTerms{"Z0"};

	auto gateRegistry = xacc::getService<IRProvider>("gate");

	// Construct IR for this
	auto ir = createXACCIR( { { "Z0", Term { { 0, "Z" } } } });
	auto f01 = std::make_shared<GateFunction>("measure0_qubit_0",
			"readout-error");
	auto meas01 = gateRegistry->createInstruction("Measure", std::vector<int> { 0 });
	InstructionParameter p(0);
	meas01->setParameter(0, p);
	f01->addInstruction(meas01);

	auto f10 = std::make_shared<GateFunction>("measure1_qubit_0",
			"readout-error");
	auto x = gateRegistry->createInstruction("X", std::vector<int> { 0 });
	auto meas10 = gateRegistry->createInstruction("Measure", std::vector<int> { 0 });
	InstructionParameter p2(0);
	meas10->setParameter(0, p2);
	f10->addInstruction(x);
	f10->addInstruction(meas01);

	ir->addKernel(f10);
	ir->addKernel(f01);

	std::cout << "IR: " << ir->getKernels().size() << "\n";
	// Construct sites map
	std::map<std::string, std::vector<int>> sites{{"Z0",{0}}};

	auto buff = std::make_shared<FakeTermAB>("buff", 1);

	auto mb1 = std::make_shared<FakeMeasureAB>("m1", 1);
	auto mb2 = std::make_shared<FakeMeasureAB>("m2", 1);

	std::vector<std::shared_ptr<AcceleratorBuffer>> buffers {buff, mb1, mb2};

	ReadoutErrorAcceleratorBufferPostprocessor processor(*ir, sites, orderedTerms);

	auto fixed = processor.process(buffers);

	std::cout << "HELLO: " << fixed[0]->getExpectationValueZ() << "\n";

	xacc::Finalize();
//	BOOST_VERIFY(std::fabs(fixed[0]->getExpectationValueZ() - .911111) < 1e-6);

}
