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
#include "KernelReplacementPreprocessor.hpp"
#include <boost/algorithm/string.hpp>
#include "AlgorithmGenerator.hpp"
#include "ServiceRegistry.hpp"
#include <numeric>

namespace xacc {
namespace quantum {

KernelReplacementPreprocessor::KernelReplacementPreprocessor() {}

const std::string KernelReplacementPreprocessor::process(const std::string& source,
			std::shared_ptr<Compiler> compiler,
			std::shared_ptr<Accelerator> accelerator) {

	XACCInfo("Running Kernel Replacement Preprocessor.\n");

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
		auto bufferSize = buffer->size();
		std::vector<int> qubits(bufferSize);
		std::iota(qubits.begin(), qubits.end(), 0);

		auto algoGen = ServiceRegistry::instance()->getService<AlgorithmGenerator>(funcName);

		auto algoKernel = algoGen->generateAlgorithm(qubits);

		auto translation = compiler->translate(bufName, algoKernel);

		boost::replace_all(translation, "\n", "\n   "
				"");
		boost::replace_all(newSource, funcString, translation);
	}

	return newSource;
}

}
}
