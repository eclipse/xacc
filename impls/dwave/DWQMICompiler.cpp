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
#include "RuntimeOptions.hpp"

namespace xacc {

namespace quantum {

DWQMICompiler::DWQMICompiler() {
}

std::shared_ptr<IR> DWQMICompiler::compile(const std::string& src,
		std::shared_ptr<Accelerator> acc) {

	auto runtimeOptions = RuntimeOptions::instance();
	auto hardwareGraph = acc->getAcceleratorConnectivity();
	std::set<int> qubits;
	std::vector<std::shared_ptr<DWQMI>> instructions;
	int nHardwareVerts = hardwareGraph->order();

	// Set the Kernel Source code
	kernelSource = src;


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
		if (!qmi.empty()) {
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

	// Get an embedding algorithm to execute
	if (!runtimeOptions->exists("dwave-embedding")) {
		// For now, this is an error
		XACCError("You must specify an embedding algorithm");
	}
	auto algoStr = (*runtimeOptions)["dwave-embedding"];
	auto embeddingAlgorithm = EmbeddingAlgorithmRegistry::instance()->create(algoStr);

	// Compute the minor graph embedding
	auto embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);

	auto countEdgesBetweenSubTrees = [&](std::list<int> Ti, std::list<int> Tj) -> int {
		int nEdges = 0;
		for (auto i : Ti) {
			for (auto j : Tj) {
				if (hardwareGraph->edgeExists(i, j)) {
					nEdges++;
				}
			}
		}
		return nEdges;
	};

	auto subTreeContains = [](std::list<int> tree, int i) -> bool {
		return std::find(tree.begin(), tree.end(), i) != tree.end();
	};

	// Setup the hardware bias values
	for (auto& embKv : embedding) {
		auto probVert = embKv.first;
		auto hardwareMapping = embKv.second;
		auto newBias = std::get<0>(problemGraph->getVertexProperties(probVert)) / hardwareMapping.size();
		for (auto h : hardwareMapping) {
			auto embeddedInst = std::make_shared<DWQMI>(h, h, newBias);
			dwKernel->addInstruction(embeddedInst);
		}
	}

	for (int i = 0; i < nHardwareVerts; i++) {
		for (int j = 0; j < nHardwareVerts; j++) {
			if (hardwareGraph->edgeExists(i, j) && i < j && i != j) {
				for (int pi = 0; pi < problemGraph->order(); pi++) {
					for (int pj = 0; pj < problemGraph->order(); pj++) {

						auto Ti = embedding[pi];
						auto Tj = embedding[pj];

						if (subTreeContains(Ti, i) && subTreeContains(Tj, j)) {
							double newWeight = 0.0;
							if (pi != pj) {
								// If problem edge does not exist,
								// Graph.getEdgeWeight retusn 0.0;
								newWeight = problemGraph->getEdgeWeight(pi,
										pj)
										/ countEdgesBetweenSubTrees(Ti, Tj);
							} else {
								// ferro-magnetic coupling parameter that ensures that physical
								// qubits representing one logical qubit remain highly correlated.
								for (auto neighbor : problemGraph->getNeighborList(
										pi)) {
									newWeight += std::fabs(
											problemGraph->getEdgeWeight(pi,
													neighbor));
								}
								newWeight = std::get<0>(
										problemGraph->getVertexProperties(pi))
										+ newWeight - 1.0;
							}

							if (std::fabs(newWeight) > 1e-4) {
								auto embeddedInst = std::make_shared<DWQMI>(i,
										j, newWeight);
								dwKernel->addInstruction(embeddedInst);
							}

						}
					}
				}
			}
		}
	}

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
