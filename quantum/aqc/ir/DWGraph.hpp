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
#ifndef QUANTUM_AQC_IR_DWGRAPH_HPP_
#define QUANTUM_AQC_IR_DWGRAPH_HPP_

#include "Graph.hpp"
#include "Accelerator.hpp"

namespace xacc {

namespace quantum {

/**
 * The DWVertex is a subclass of the XACCVertex that
 * keeps track of one vertex parameter - the qubit
 * bias parameter. XACCVertex already keeps track
 * of edge weights.
 */
class DWVertex: public XACCVertex<double> {
public:
	DWVertex() :
			XACCVertex() {
		propertyNames[0] = "bias";
	}
};

// Alias for Graphs of DWave Vertices
class DWGraph : public Graph<DWVertex> {
public:

	DWGraph(const int nVertices) : Graph(nVertices) {
	}

	std::shared_ptr<AcceleratorGraph> getAcceleratorGraph() {
		auto retGraph = std::make_shared<AcceleratorGraph>(order());
		for (int i = 0; i < order(); i++) {
			for (int j = 0; j < order(); j++) {
				if (i < j && edgeExists(i,j)) {
					retGraph->addEdge(i, j);
				}
			}
		}

		return retGraph;
	}

	std::string toKernelSource(const std::string& kernelName) {
		std::string src = "__qpu__ " + kernelName + "() {\n";
		for (int i = 0; i < order(); i++) {
			src += std::to_string(i) + " " + std::to_string(i) + " "
					+ std::to_string(getVertexProperty<0>(i)) + "\n";
		}
		for (int i = 0; i < order(); i++) {
			for (int j = 0; j < order(); j++) {
				if (i < j && edgeExists(i, j)) {
					src += std::to_string(i) + " " + std::to_string(j) + " "
							+ std::to_string(getEdgeWeight(i, j)) + "\n";
				}
			}
		}

		return src + "}\n";
	}

	virtual ~DWGraph() {}
};

class K44Bipartite : public DWGraph {
public:

	K44Bipartite() :
			DWGraph(8) {
		addEdge(0, 4);
		addEdge(0, 5);
		addEdge(0, 6);
		addEdge(0, 7);
		addEdge(1, 4);
		addEdge(1, 5);
		addEdge(1, 6);
		addEdge(1, 7);
		addEdge(2, 4);
		addEdge(2, 5);
		addEdge(2, 6);
		addEdge(2, 7);
		addEdge(3, 4);
		addEdge(3, 5);
		addEdge(3, 6);
		addEdge(3, 7);
	}

	virtual ~K44Bipartite() {}
};

class CompleteGraph : public DWGraph {
public:

	CompleteGraph(int nVertices) :
			DWGraph(nVertices) {
		for (int i = 0; i < nVertices; i++) {
			for (int j = 0; j < nVertices; j++) {
				if (i < j) {
					addEdge(i, j);
				}
			}
		}
	}

	virtual ~CompleteGraph() {}
};

class ChimeraGraph : public DWGraph {
public:

	ChimeraGraph(int gridSize) : DWGraph(8 * gridSize * gridSize) {
		// We are going to assign a tuple of ints to each vertex
		std::map<std::tuple<int, int, int, int>, int> vertexTuples;
		int i, j, k, l, v = 0;

		// Loop through the list of vertices and give
		// them the correct 4-tuple: (i,j) is unit cell in
		// the chimera grid, (k,l) is vertex in unit cell, k = 0-3,
		// l = 0,1 (left, right of bipartite graph)
		for (i = 0; i < gridSize; i++) {
			for (j = 0; j < gridSize; j++) {
				for (l = 0; l < 2; l++) {
					for (k = 0; k < 4; k++) {
						vertexTuples.insert(
								std::make_pair(std::make_tuple(i, j, k, l), v));
						v++;
					}
				}
			}
		}

		// Setup edges within each cell
		for (i = 0; i < gridSize; i++) {
			for (j = 0; j < gridSize; j++) {
				for (k = 0; k < 4; k++) {
					for (int k2 = 0; k2 < 4; k2++) {
						int v1 = vertexTuples[std::make_tuple(i, j, k, 0)];
						int v2 = vertexTuples[std::make_tuple(i, j, k2, 1)];
						addEdge(v1, v2);
					}

				}
			}
		}

		// Setup edges between cells
		for (i = 0; i < gridSize - 1; i++) {
			for (j = 0; j < gridSize; j++) {
				for (k = 0; k < 4; k++) {
					int v1 = vertexTuples[std::make_tuple(i, j, k, 0)];
					int v2 = vertexTuples[std::make_tuple(i + 1, j, k, 0)];
					addEdge(v1, v2);

				}
			}
		}

		for (i = 0; i < gridSize; i++) {
			for (j = 0; j < gridSize - 1; j++) {
				for (k = 0; k < 4; k++) {
					int v1 = vertexTuples[std::make_tuple(i, j, k, 1)];
					int v2 = vertexTuples[std::make_tuple(i, j + 1, k, 1)];
					addEdge(v1, v2);

				}
			}
		}
	}

	virtual ~ChimeraGraph() {}
};
}

}

#endif
