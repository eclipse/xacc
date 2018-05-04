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
#include "KernelReplacementPreprocessor.hpp"
#include <boost/algorithm/string.hpp>
#include "IRGenerator.hpp"
#include "XACC.hpp"
#include <numeric>

namespace xacc {
namespace quantum {

KernelReplacementPreprocessor::KernelReplacementPreprocessor() {}

const std::string KernelReplacementPreprocessor::process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator) {

	XACCLogger::instance()->info("Running Kernel Replacement Preprocessor.\n");

	// Find any occurrences of xacc::FUNCTION
	// Get the first occurrence
	std::string search("xacc::"), newSource = source;
	auto pos = source.find(search);
	std::vector<std::size_t> allPositions;

	// Repeat till end is reached
	while (pos != std::string::npos) {
		// Add position to the vector
		allPositions.push_back(pos);

		// Get the next occurrence from the current position
		pos = source.find(search, pos + search.size());
	}

	for (auto position : allPositions) {

		auto funcString = source.substr(position, source.find("\n", position)-position);
		boost::trim(funcString);

		std::vector<std::string> split;
		boost::split(split, funcString, boost::is_any_of("::"));

		auto funcName = split[2].substr(0, split[2].find("("));

		std::string bufName = "";
		auto parenLocation = split[2].find("(")+1;
		if (boost::contains(split[2], ",")) {
			bufName = split[2].substr(parenLocation, split[2].find(",")-parenLocation);
		} else {
			bufName = split[2].substr(parenLocation, split[2].find(")")-parenLocation);
		}

		auto buffer = accelerator->getBuffer(bufName);
		auto algoGen = xacc::getService<IRGenerator>(funcName);

		auto algoKernel = algoGen->generate(buffer);

		auto translation = compiler->translate(bufName, algoKernel);

		boost::replace_all(translation, "\n", "\n   "
				"");
		boost::replace_all(newSource, funcString, translation);
	}

	return newSource;
}

}
}
