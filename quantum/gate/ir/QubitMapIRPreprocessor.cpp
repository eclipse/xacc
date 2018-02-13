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
#include "QubitMapIRPreprocessor.hpp"
#include "XACC.hpp"

namespace xacc {
namespace quantum {
std::shared_ptr<AcceleratorBufferPostprocessor> QubitMapIRPreprocessor::process(IR& ir) {
	if (xacc::optionExists("qubit-map")) {
		std::vector<int> qubitMap;
		auto mapStr = xacc::getOption("qubit-map");

		std::vector<std::string> split;
		boost::split(split, mapStr, boost::is_any_of(","));

		int counter = 0;
		for (auto s : split) {
			auto idx = std::stoi(s);
			qubitMap.push_back(idx);
		}

		ir.mapBits(qubitMap);

		int newNQubits = *std::max_element(qubitMap.begin(), qubitMap.end()) + 1;
		xacc::setOption("n-qubits",std::to_string(newNQubits));

	}

	// Construct a ReadoutErrorABPostprocessor
	return std::make_shared<NullAcceleratorBufferPostprocessor>();
}

}
}

