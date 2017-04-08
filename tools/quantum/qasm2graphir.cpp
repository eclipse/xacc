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
#include "QasmToGraph.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include "GraphIR.hpp"

using namespace boost::program_options;

int main(int argc, char** argv) {
	// Declare the supported options.
	options_description desc("QASM to XACC Graph IR Utility");
	desc.add_options()("help",
			"The following arguments are valid for this utility.")("input",
			value<std::string>()->required(), "Provide the name of the input qasm file.")(
			"output", value<std::string>()->default_value("graphIR.xir"),
			"Provide the name of the output file *.xir file name - "
			"default is graphIR.xir");
	// Get the user-specified compiler parameters as a map
	variables_map compileParameters;
	store(command_line_parser(argc, argv).options(desc).run(),
			compileParameters);

	auto inputFileName = compileParameters["input"].as<std::string>();
	auto outputFileName = (
			compileParameters.count("output") ?
					compileParameters["output"].as<std::string>() :
					"graphIR.xir");

	std::ifstream inputStream(inputFileName);
	std::ofstream outputStream(outputFileName);
	std::stringstream ss;
	if (!inputStream) {
		XACCError("Invalid input qasm file");
	}

	ss << inputStream.rdbuf();
	inputStream.close();

	auto graph = xacc::quantum::QasmToGraph::getCircuitGraph(ss.str());

	xacc::GraphIR<decltype(graph)> xir(graph);
	xir.persist(outputStream);

	return 0;
}
