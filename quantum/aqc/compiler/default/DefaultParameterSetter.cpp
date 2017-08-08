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
#include "DefaultParameterSetter.hpp"

namespace xacc {
namespace quantum {

std::list<std::shared_ptr<DWQMI>> DefaultParameterSetter::setParameters(
		std::shared_ptr<DWGraph> problemGraph,
		std::shared_ptr<AcceleratorGraph> hardwareGraph,
		Embedding embedding) {

	std::list<std::shared_ptr<DWQMI>> instList;
	auto nHardwareVerts = hardwareGraph->order();
	auto nProblemVerts = problemGraph->order();

	auto countEdgesBetweenSubTrees =
			[&](std::vector<int> Ti, std::vector<int> Tj) -> int {
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

	auto subTreeContains = [](std::vector<int> tree, int i) -> bool {
		return std::find(tree.begin(), tree.end(), i) != tree.end();
	};

	// Setup the hardware bias values
	for (auto& embKv : embedding) {
		auto probVert = embKv.first;
		auto hardwareMapping = embKv.second;
		auto newBias = std::get<0>(problemGraph->getVertexProperties(probVert)) / hardwareMapping.size();
		for (auto h : hardwareMapping) {
			auto embeddedInst = std::make_shared<DWQMI>(h, h, newBias);
			instList.push_back(embeddedInst);
		}
	}

	for (int i = 0; i < nHardwareVerts; i++) {
		for (int j = 0; j < nHardwareVerts; j++) {
			if (hardwareGraph->edgeExists(i, j) && i < j && i != j) {
				for (int pi = 0; pi < nProblemVerts; pi++) {
					for (int pj = 0; pj < nProblemVerts; pj++) {

						auto Ti = embedding[pi];
						auto Tj = embedding[pj];

						if (subTreeContains(Ti, i) && subTreeContains(Tj, j)) {
							double newWeight = 0.0;
							if (pi != pj) {
								// If problem edge does not exist,
								// Graph.getEdgeWeight retusn 0.0;
								newWeight = problemGraph->getEdgeWeight(pi, pj)
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
								newWeight += std::get<0>(
										problemGraph->getVertexProperties(pi));
								newWeight *= -1.0;
							}

							if (std::fabs(newWeight) > 1e-4) {
								auto embeddedInst = std::make_shared<DWQMI>(i,
										j, newWeight);
								instList.push_back(embeddedInst);
							}

						}
					}
				}
			}
		}
	}

	return instList;
}
}
}
