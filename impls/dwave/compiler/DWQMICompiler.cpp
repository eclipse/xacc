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
#include <regex>
#include <boost/algorithm/string.hpp>
#include "DWQMICompiler.hpp"
#include "DWKernel.hpp"
#include "RuntimeOptions.hpp"
#include "AQCAcceleratorBuffer.hpp"
#include "ParameterSetter.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<IR> DWQMICompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {

	auto runtimeOptions = RuntimeOptions::instance();
	auto hardwareGraph = acc->getAcceleratorConnectivity();
	std::set<int> qubits;
	std::vector<std::shared_ptr<DWQMI>> instructions;
	int nHardwareVerts = hardwareGraph->order();

	// Set the Kernel Source code
	kernelSource = src;

	// Here we assume, there is just one allocation
	// of qubits for the D-Wave -- all of them.
	auto bufName = acc->getAllocatedBufferNames()[0];
	auto buffer = acc->getBuffer(bufName);
	auto aqcBuffer = std::dynamic_pointer_cast<AQCAcceleratorBuffer>(buffer);
	if (!aqcBuffer) {
		XACCError("Invalid AcceleratorBuffer passed to DW QMI Compiler. Must be an AQCAcceleratorBuffer.");
	}

	// Here we expect we have a kernel, only one kernel,
	// and that it is just machine level instructions
	// So just pick out where the opening and closing
	// brackets are, and then get the text between them.

	// First off, split the string into lines
	std::vector<std::string> lines, fLineSpaces;
	boost::split(lines, src, boost::is_any_of("\n"));
	auto functionLine = lines[0];
	boost::split(fLineSpaces, functionLine, boost::is_any_of(" "));
	auto fName = fLineSpaces[1];
	boost::trim(fName);
	fName = fName.substr(0, fName.find_first_of("("));
	auto dwKernel = std::make_shared<DWKernel>(fName);
	auto firstCodeLine = lines.begin() + 1;
	auto lastCodeLine = lines.end() - 1;
	std::vector<std::string> qmiStrVec(firstCodeLine, lastCodeLine);

	// Loop over the lines to create DWQMI
	for (auto qmi : qmiStrVec) {
		boost::trim(qmi);
		if (!qmi.empty() && (std::string::npos != qmi.find_first_of("0123456789"))) {
			std::vector<std::string> splitOnSpaces;
			boost::split(splitOnSpaces, qmi, boost::is_any_of(" "));
			auto qbit1 = std::stoi(splitOnSpaces[0]);
			auto qbit2 = std::stoi(splitOnSpaces[1]);
			auto weight = std::stod(splitOnSpaces[2]);
			qubits.insert(qbit1);
			qubits.insert(qbit2);
			instructions.emplace_back(std::make_shared<DWQMI>(qbit1, qbit2, weight));
		}
	}

	// Now we have a qubits set. If we used 0-N qubits,
	// then this set size will be N, but if we skipped some
	// bits, then it will be < N. Get the maximum int in this set
	// so we can see if any were skipped.
	int maxBitIdx = *qubits.rbegin();
	maxBitIdx++;

	// Create a graph representation of the problem
	auto problemGraph = std::make_shared<DWGraph>(maxBitIdx);
	for (auto inst : instructions) {
		auto qbit1 = inst->bits()[0];
		auto qbit2 = inst->bits()[1];
		double weightOrBias = boost::get<double>(inst->getParameter(0));
		if (qbit1 == qbit2) {
			problemGraph->setVertexProperties(qbit1, weightOrBias);
		} else {
			problemGraph->addEdge(qbit1, qbit2,
					weightOrBias);
		}
	}

	Embedding embedding;
	// Get an embedding algorithm to execute
	if (!runtimeOptions->exists("dwave-embedding")
			&& !runtimeOptions->exists("dwave-load-embedding")) {
		// For now, this is an error
		XACCError("You must specify an embedding algorithm or embedding file.");
	}

	if (runtimeOptions->exists("dwave-load-embedding")) {
		std::ifstream ifs((*runtimeOptions)["dwave-load-embedding"]);
		embedding.load(ifs);
	} else {
		auto algoStr = (*runtimeOptions)["dwave-embedding"];
		auto embeddingAlgorithm =
				ServiceRegistry::instance()->getService<EmbeddingAlgorithm>(algoStr);

		// Compute the minor graph embedding
		embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);

		if (runtimeOptions->exists("dwave-persist-embedding")) {
			auto fileName = (*runtimeOptions)["dwave-persist-embedding"];
			std::ofstream ofs(fileName);
			embedding.persist(ofs);
		}
	}

	// Add the embedding to the AcceleratorBuffer
	aqcBuffer->setEmbedding(embedding);

	// Get the ParameterSetter
	std::shared_ptr<ParameterSetter> parameterSetter;
	if (runtimeOptions->exists("dwave-parameter-setter")) {
		parameterSetter = ServiceRegistry::instance()->getService<
				ParameterSetter>((*runtimeOptions)["dwave-parameter-setter"]);
	} else {
		parameterSetter = ServiceRegistry::instance()->getService<
				ParameterSetter>("default");
	}

	// Set the parameters
	auto insts = parameterSetter->setParameters(problemGraph, hardwareGraph,
			embedding);

	// Add the instructions to the Kernel
	for (auto i : insts) {
		dwKernel->addInstruction(i);
	}

	// Create and return the IR
	auto ir = std::make_shared<DWIR>();
	ir->addKernel(dwKernel);
	return ir;
}

std::shared_ptr<IR> DWQMICompiler::compile(const std::string& src) {
	XACCError("Cannot compile D-Wave program without "
			"information about Accelerator connectivity.");
}

}

}
