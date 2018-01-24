/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "ReadoutErrorIRPreprocessor.hpp"
#include "XACC.hpp"
#include "GateFunction.hpp"
#include "GateInstruction.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "Measure.hpp"
#include <boost/math/constants/constants.hpp>
#include "ReadoutErrorAcceleratorBufferPostprocessor.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<AcceleratorBufferPostprocessor> ReadoutErrorIRPreprocessor::process(IR& ir) {

	// Get number of qubits, add 2*nqubit measurement kernels, add readout-error tag to each
	int nQubits = ir.maxBit() + 1;
	auto gateRegistry = GateInstructionRegistry::instance();

	// Search IR Functions and construct Pauli Term strings, then add any Pauli that is not there
	std::vector<std::string> orderedPauliTerms;
	std::vector<std::map<int, std::string>> pauliTerms;
	for (auto kernel : ir.getKernels()) {
		std::string pauliStr = "";
		CountGatesOfTypeVisitor<Measure> v(kernel);
		bool allZTerm = false;
		if (kernel->nInstructions() == v.countGates()) {
			allZTerm = true;
		}

		std::map<int, std::string> pauliTerm;
		for (auto inst : kernel->getInstructions()) {
			auto bit = inst->bits()[0];

			if (allZTerm) {
				pauliTerm[bit] = "Z";
				pauliStr = "Z" + std::to_string(bit) + pauliStr;
				continue;
			}

			if (inst->getName() == "H") {
				pauliStr = "X" + std::to_string(bit) + pauliStr;
				pauliTerm[bit] = "X";
			} else if (inst->getName() == "Rx") {
				pauliStr = "X" + std::to_string(bit) + pauliStr;
				pauliTerm[bit] = "Y";
			} else if (inst->getName() == "Measure") {
				// do nothing
				continue;
			} else {
				xacc::error("ReadoutErrorIRPreprocessor only can be "
						"applied to kernels generated from a Pauli "
						"Hamiltonian, cannot have " + inst->getName() + " gate");
			}
		}

		orderedPauliTerms.push_back(pauliStr);
		pauliTerms.push_back(pauliTerm);
	}

	std::cout << "TERMSMaps:\n";
	std::map<std::string, std::vector<int>> sites;
	for (auto a : pauliTerms) {
		std::stringstream s;
		std::vector<int> tmp;
		for (auto& kv : a) {
			s << kv.second << kv.first;
			tmp.push_back(kv.first);
			std::cout << kv.second << kv.first << " ";
		}
		sites.insert({s.str(), tmp});
		std::cout << "\n";
	}


	std::cout << "SITES:\n";
	for (auto & kv : sites) {
		std::cout << kv.first << " -> (";
		for (auto i : kv.second) {
			std::cout << i << " ";
		}
		std::cout << ")\n";
	}

	std::vector<std::string> extraKernelsNeeded;
	for (auto t : pauliTerms) {
		if (t.size() > 1) {
			for (auto& kv : t) {
				auto termStr = kv.second + std::to_string(kv.first);
				if (!sites.count(termStr)) {
					extraKernelsNeeded.push_back(termStr);
				}
			}
		}
	}

	std::sort( extraKernelsNeeded.begin(), extraKernelsNeeded.end() );
	extraKernelsNeeded.erase( std::unique( extraKernelsNeeded.begin(), extraKernelsNeeded.end() ), extraKernelsNeeded.end() );

	for (auto o : extraKernelsNeeded) {
		std::cout << "EXTRA: " << o << "\n";
		int nKernels = ir.getKernels().size();
		auto extraKernel = std::make_shared<GateFunction>(o, "readout-error-extra");

		std::stringstream sg, sb;
		sg << o[0];
		sb << o[1];
		auto gate = sg.str();
		auto bit = std::stoi(sb.str());

		if (gate == "X") {
			auto h = gateRegistry->create("H", std::vector<int>{bit});
			extraKernel->addInstruction(h);
		} else if (gate == "Y") {
			auto rx = gateRegistry->create("Rx", std::vector<int>{bit});
			InstructionParameter p(boost::math::constants::pi<double>() / 2.0);
			rx->setParameter(0,p);
			extraKernel->addInstruction(rx);
		}

		auto meas = gateRegistry->create("Measure", std::vector<int> { bit });
		InstructionParameter m(0);
		meas->setParameter(0, m);
		extraKernel->addInstruction(meas);

		ir.addKernel(extraKernel);

		orderedPauliTerms.push_back(o);
	}

	int nKernels = ir.getKernels().size();

	int qbit = 0;
	for (int i = 0; i < 2*nQubits; i+=2) {
		int nKernels = ir.getKernels().size();
		auto f01 = std::make_shared<GateFunction>(
				"measure0_qubit_" + std::to_string(qbit), "readout-error");
		auto meas01 = gateRegistry->create("Measure", std::vector<int>{qbit});
		InstructionParameter p(0);
		meas01->setParameter(0,p);
		f01->addInstruction(meas01);

		auto f10 = std::make_shared<GateFunction>(
				"measure1_qubit_" + std::to_string(qbit), "readout-error");
		auto x = gateRegistry->create("X", std::vector<int>{qbit});
		auto meas10 = gateRegistry->create("Measure", std::vector<int>{qbit});
		InstructionParameter p2(0);
		meas10->setParameter(0,p2);
		f10->addInstruction(x);
		f10->addInstruction(meas01);

		ir.addKernel(f10);
		ir.addKernel(f01);

		qbit++;
	}

	// Construct a ReadoutErrorABPostprocessor
	return std::make_shared<ReadoutErrorAcceleratorBufferPostprocessor>(ir, sites, orderedPauliTerms);
}

}
}

