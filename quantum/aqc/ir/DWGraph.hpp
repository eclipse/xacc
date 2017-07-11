/*
 * DWGraph.hpp
 *
 *  Created on: Jul 11, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_AQC_IR_DWGRAPH_HPP_
#define QUANTUM_AQC_IR_DWGRAPH_HPP_

#include "Graph.hpp"

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
				if (i < j) {
					addEdge(i, j);
				}
			}
		}

		return retGraph;
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

class Chimera : public DWGraph {
public:

	Chimera(int gridSize) : DWGraph(8 * gridSize * gridSize) {
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

	virtual ~Chimera() {}
};
}

}

#endif
