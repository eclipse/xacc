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
	xacc::info("Starting ReadoutErrorIRPreprocessor");

	// Get number of qubits, add 2*nqubit measurement kernels, add readout-error tag to each
	int nQubits = 0;//std::stoi(xacc::getOption("n-qubits")); //ir.maxBit() + 1;
	auto gateRegistry = GateInstructionRegistry::instance();


	// Get the true number of qubits
	std::set<int> qubits;
	for (auto kernel : ir.getKernels()) {
		for (auto inst : kernel->getInstructions()) {
			for (auto b : inst->bits()) {
				qubits.insert(b);
			}
		}
	}
	nQubits = qubits.size();

	// If we've mapped to different physical qubit indices,
	// we need to know about it
	std::vector<int> qubitMap;
	if (xacc::optionExists("qubit-map")) {
		auto mapStr = xacc::getOption("qubit-map");

		std::vector<std::string> split;
		boost::split(split, mapStr, boost::is_any_of(","));

		int counter = 0;
		for (auto s : split) {
			auto idx = std::stoi(s);
			qubitMap.push_back(idx);
		}
	} else {
		for (int i = 0; i < nQubits; i++) qubitMap.push_back(i);
	}

	// Create a reversed qubitMap so we can
	// create the sites map
	std::map<int, int> reversedQubitMap;
	int counter = 0;
	for (auto a : qubitMap) {
		reversedQubitMap.insert({a, counter});
		counter++;
	}

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
			auto bit = reversedQubitMap[inst->bits()[0]];

			if (allZTerm) {
				pauliTerm[bit] = "Z";
				pauliStr = "Z" + std::to_string(bit) + pauliStr;
				continue;
			}

			bool seen = false;
			if (inst->getName() == "H") {
				pauliStr = "X" + std::to_string(bit) + pauliStr;
				pauliTerm[bit] = "X";
				seen = true;
			} else if (inst->getName() == "Rx") {
				pauliStr = "Y" + std::to_string(bit) + pauliStr;
				pauliTerm[bit] = "Y";
				seen = true;
			} else if (inst->getName() == "Measure") {
				if (!boost::contains(pauliStr, "X" + std::to_string(bit))
						&& !boost::contains(pauliStr,
								"Y" + std::to_string(bit))) {
					pauliStr = pauliStr + "Z" + std::to_string(bit);
					pauliTerm[bit] = "Z";
				}
				continue;
			} else {
				xacc::error("ReadoutErrorIRPreprocessor only can be "
						"applied to kernels generated from a Pauli "
						"Hamiltonian, cannot have " + inst->getName() + " gate");
			}

		}

		if (!pauliStr.empty()) {
//			std::cout << "PTERM: " << pauliStr << "\n";
			orderedPauliTerms.push_back(pauliStr);
			pauliTerms.push_back(pauliTerm);
		}
	}

	// Use the above info to create the sites map
	std::map<std::string, std::vector<int>> sites;
	for (auto a : pauliTerms) {
		std::stringstream s;
		std::vector<int> tmp;
		for (auto& kv : a) {
			s << kv.second << kv.first;
			tmp.push_back(kv.first);
		}
		sites.insert({s.str(), tmp});
	}


//	std::cout << "SITES:\n";
//	for (auto & kv : sites) {
//		std::cout << kv.first << " -> (";
//		for (auto i : kv.second) {
//			std::cout << i << " ";
//		}
//		std::cout << ")\n";
//	}

	// Discover any extra kernels we need to compute
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

	// Add the extra kernels to the IR
	for (auto o : extraKernelsNeeded) {
//		std::cout << "EXTRA: " << o << "\n";
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

	// Now add the measurement kernels we need to compute
	// p01 and p10
	int qbit = 0;
	for (int i = 0; i < 2*nQubits; i+=2) {
		auto f01 = std::make_shared<GateFunction>(
				"measure0_qubit_" + std::to_string(qubitMap[qbit]), "readout-error");
		auto meas01 = gateRegistry->create("Measure", std::vector<int>{qubitMap[qbit]});
		InstructionParameter p(0);
		meas01->setParameter(0,p);
		f01->addInstruction(meas01);

		auto f10 = std::make_shared<GateFunction>(
				"measure1_qubit_" + std::to_string(qubitMap[qbit]), "readout-error");
		auto x = gateRegistry->create("X", std::vector<int>{qubitMap[qbit]});
		auto meas10 = gateRegistry->create("Measure", std::vector<int>{qubitMap[qbit]});
		InstructionParameter p2(0);
		meas10->setParameter(0,p2);
		f10->addInstruction(x);
		f10->addInstruction(meas01);

		ir.addKernel(f01);
		ir.addKernel(f10);

		qbit++;
	}

	xacc::info("Finished ReadoutErrorIRPreprocessor");

	return std::make_shared<ReadoutErrorAcceleratorBufferPostprocessor>(ir, sites, orderedPauliTerms);
}

}
}

