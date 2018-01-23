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
#include "ReadoutErrorAcceleratorBufferPostprocessor.hpp"
#include <boost/algorithm/string.hpp>
#include "XACC.hpp"

namespace xacc {
namespace quantum {
std::vector<std::shared_ptr<AcceleratorBuffer>> ReadoutErrorAcceleratorBufferPostprocessor::process(
		std::vector<std::shared_ptr<AcceleratorBuffer>> buffers) {

	// Goal here is to get all ap01, ap10, ..., and fix all expectation values
	int nQubits = ir.maxBit() + 1;
	std::string zeroStr = "";
	for (int i = 0; i < nQubits; i++) zeroStr += "0";

	std::map<std::string, double> errorRates;
	for (auto& kv : measurements) {

		auto kernelIdx = kv.second;
		auto localBitStr = zeroStr;
		std::stringstream s;
		s << kv.first[1] << kv.first[2];
		if (boost::contains(s.str(), "01")) {

			auto kernel = ir.getKernels()[kernelIdx];
			auto bit = kernel->getInstruction(0)->bits()[0];

			localBitStr[nQubits - bit - 1] = '1';
		}

		std::cout << "HI: " << kv.first << ", " << kv.second << ", " << localBitStr << "\n";
		// get bit string from
		auto prob = buffers[kv.second]->computeMeasurementProbability(localBitStr);

		errorRates.insert({kv.first, std::isnan(prob) ? 0.0 : prob});
	}

	for (auto& kv : errorRates) {
		std::stringstream s, s2;
		s2 << kv.first[1] << kv.first[2];
		s << kv.first[0]
				<< (s2.str() == "01" ?
						" probability expected 0 but got 1 error rate = " :
						" probability expected 1 but got 0 error rate = ");
		xacc::info("Qubit " + s.str() + std::to_string(kv.second));
	}

	// Return new AcceleratorBuffers subtype, StaticExpValAcceleratorBuffer that has static
	// constant fixed expectation value from the calculation

	return std::vector<std::shared_ptr<AcceleratorBuffer>>{};
}
}
}



