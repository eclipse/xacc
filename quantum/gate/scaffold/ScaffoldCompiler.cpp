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
#include "ScaffoldCompiler.hpp"
#include "GraphIR.hpp"
#include <regex>
#include "ScaffCCAPI.hpp"

namespace xacc {

namespace quantum {

void ScaffoldCompiler::modifySource() {

	// Here we assume we've been given just
	// the body of the quantum code, as part
	// of an xacc __qpu__ kernel function.

	// First off, replace __qpu__ with 'module '
	kernelSource.erase(kernelSource.find("__qpu__"), 7);
	kernelSource = std::string("module ") + kernelSource;

	// Get the kernel name
	std::regex functionName("((\\w+)\\s*\\()\\s*");
	auto begin = std::sregex_iterator(kernelSource.begin(), kernelSource.end(),
			functionName);
	std::string fName = (*begin).str();

	// Now wrap in a main function for ScaffCC
	kernelSource = kernelSource + std::string("\nint main() {\n   ") + fName
			+ std::string(");\n}");

	std::cout << "\n" << kernelSource << "\n";
}

std::shared_ptr<IR> ScaffoldCompiler::compile() {

	// Create an instance of our ScaffCC API
	// so that we can interact with a locally installed
	// scaffcc executable
	scaffold::ScaffCCAPI scaffcc;

	// Compile the source code and return the QASM form
	// This will throw if it fails.
	auto qasm = scaffcc.getFlatQASMFromSource(kernelSource);

	// Generate a GraphIR instance, ie a graph
	// tensor references making up this QASM
	std::cout << "Flat QASM: \n" << qasm << "\n";
//return std::make_shared<GraphIR>();
}

} // end namespace quantum

} // end namespace xacc

// Required in CPP file to be discovered by factory pattern
REGISTER_QCIOBJECT_WITH_QCITYPE(xacc::quantum::ScaffoldCompiler, "compiler",
		"scaffold");

