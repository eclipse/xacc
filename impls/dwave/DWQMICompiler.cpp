/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#include <regex>
#include <boost/algorithm/string.hpp>
#include "DWQMICompiler.hpp"
#include "DWKernel.hpp"

namespace xacc {

namespace quantum {

DWQMICompiler::DWQMICompiler() {
}

std::shared_ptr<IR> DWQMICompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {

	// Set the Kernel Source code
	kernelSource = src;

	return std::make_shared<DWIR>();
}

std::shared_ptr<IR> DWQMICompiler::compile(const std::string& src) {

	kernelSource = src;

	// Here we expect we have a kernel, only one kernel,
	// and that it is just machine level instructions
	// So just pick out where the opening and closing
	// brackets are, and then get the text between them.

	// First off, split the string into lines
	std::vector<std::string> lines;

	boost::split(lines, src, boost::is_any_of("\n"));

	auto firstCodeLine = lines.begin() + 1;
	auto lastCodeLine = lines.end() - 1;
	std::vector<std::string> qmiStrVec(firstCodeLine, lastCodeLine);

	auto dwKernel = std::make_shared<DWKernel>();

	for (auto qmi : qmiStrVec) {
		std::cout << "LINE: " << qmi << "\n";

		boost::trim(qmi);

		if (!qmi.empty()) {
			std::vector<std::string> splitOnSpaces;
			boost::split(splitOnSpaces, qmi, boost::is_any_of(" "));

			auto qbit1 = std::stoi(splitOnSpaces[0]);
			auto qbit2 = std::stoi(splitOnSpaces[1]);
			auto weight = std::stod(splitOnSpaces[2]);

			auto dwqmi = std::make_shared<DWQMI>(qbit1, qbit2, weight);

			dwKernel->addInstruction(dwqmi);
		}

	}

	auto ir = std::make_shared<DWIR>();

//	ir->addKernel(ir);

	return ir;

}

}

}
