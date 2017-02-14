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
#ifndef QUANTUM_GATE_QASMTOGRAPH_HPP_
#define QUANTUM_GATE_QASMTOGRAPH_HPP_

#include "QuantumCircuit.hpp"
#include <regex>

namespace xacc {
namespace quantum {

/**
 * The QasmToGraph class provides a static
 * utility method that maps a flat qasm string
 * to a QCI Common Graph data structure.
 */
class QasmToGraph {

public:

	/**
	 * Create a Graph data structure that models a quantum
	 * circuit from the provided qasm string.
	 *
	 * @param flatQasmStr The qasm to be converted to a Graph.
	 * @return graph Graph modeling a quantum circuit.
	 */
	static QuantumCircuit getCircuitGraph(
			const std::string& flatQasmStr) {

		// Local Declarations
		using namespace qci::common;
		QuantumCircuit graph;
		std::map<std::string, int> qubitVarNameToId;
		std::vector<std::string> qasmLines;
		std::vector<int> allQbitIds;
		std::regex newLineDelim("\n"), spaceDelim(" ");
		std::regex qubitDeclarations("\\s*qubit\\s*\\w+");
		std::sregex_token_iterator first{flatQasmStr.begin(), flatQasmStr.end(), newLineDelim, -1}, last;
		int nQubits = 0, qbitId = 0, layer = 1, gateId = 1;
		std::string qubitVarName;
		qasmLines = {first, last};

		// Let's now loop over the qubit declarations,
		// and construct a mapping of qubit var names to integer id,
		// and get the total number of qubits
		for (auto i = std::sregex_iterator(flatQasmStr.begin(), flatQasmStr.end(),
						qubitDeclarations); i != std::sregex_iterator(); ++i) {
			std::string qubitLine = (*i).str();
			qubitLine.erase(std::remove(qubitLine.begin(), qubitLine.end(), '\n'), qubitLine.end());
			std::sregex_token_iterator first{qubitLine.begin(), qubitLine.end(), spaceDelim, -1}, last;
			std::vector<std::string> splitQubitLine = {first, last};
			qubitVarNameToId[splitQubitLine[1]] = qbitId;
			splitQubitLine[1].erase(
			  std::remove_if(splitQubitLine[1].begin(), splitQubitLine[1].end(), &isdigit),
			  splitQubitLine[1].end());
			qubitVarName = splitQubitLine[1];
			allQbitIds.push_back(qbitId);
			qbitId++;
		}

		// Set the number of qubits
		nQubits = qubitVarNameToId.size();

		// First create a starting node for the initial
		// wave function - it should have nQubits outgoing
		// edges
		graph.addVertex("InitialState", 0, 0, allQbitIds, true, std::vector<std::string>{});

		std::vector<CircuitNode> gateOperations;
		for (auto line : qasmLines) {
			// If this is a gate line...
			if (!boost::contains(line, "qubit") && !boost::contains(line, "cbit")) {

				// Create a new CircuitNode
				CircuitNode node;

				// Split the current qasm command at the spaces
				std::sregex_token_iterator first { line.begin(),
						line.end(), spaceDelim, -1 }, last;
				std::vector<std::string> gateCommand = {first, last};

				// Set the gate as a lowercase gate name string
				auto g = boost::to_lower_copy(gateCommand[0]);
				boost::trim(g);
				if (g == "measz") g = "measure";
				std::get<0>(node.properties) = g;

				// If not a 2 qubit gate, and if the acting
				// qubit is different than the last one, then
				// keep the layer the same, otherwise increment
				if (incrementLayer(gateCommand, qubitVarNameToId,
						gateOperations, layer)) {
					layer++;
				}

				// Set the current layer
				std::get<1>(node.properties) = layer;

				// Set the gate vertex id
				std::get<2>(node.properties) = gateId;
				gateId++;

				// Set the qubits this gate acts on
				std::vector<int> actingQubits;
				if (!boost::contains(gateCommand[1], ",")) {
					actingQubits.push_back(qubitVarNameToId[gateCommand[1]]);
				} else {


					// FIXME Need to differentiate between qubits and parameters here
					// First we need the qubit register variable name


					std::vector<std::string> splitComma;
					boost::split(splitComma, gateCommand[1], boost::is_any_of(","));
					for (auto segment : splitComma) {
						if (boost::contains(segment, qubitVarName)) {
							actingQubits.push_back(qubitVarNameToId[segment]);
						} else {
							// This is not a qubit, it must be a parameter for gate
						}
					}
				}

				// Set the acting qubits
				std::get<3>(node.properties) = actingQubits;

				// Add this gate to the local vector
				// and to the graph
				gateOperations.push_back(node);
				graph.addVertex(node);
			}
		}

		// Add a final layer for the graph sink
		CircuitNode finalNode;
		std::get<0>(finalNode.properties) = "FinalState";
		std::get<1>(finalNode.properties) = layer+1;
		std::get<2>(finalNode.properties) = gateId;
		std::get<3>(finalNode.properties) = allQbitIds;
		std::get<4>(finalNode.properties) = true;

		graph.addVertex(finalNode);
		gateOperations.push_back(finalNode);

		// Set how many layers are in this circuit
		int maxLayer = layer+1;

		generateEdgesFromLayer(1, graph, gateOperations, 0);

		return graph;
	}

	/**
	 * Create connecting conditional nodes that link the main
	 * circuit graph to subsequent conditional graphs. The conditional
	 * nodes can be used by Accelerators to figure out if the condition
	 * code should be executed or not.
	 * s
	 * @param mainGraph
	 * @param conditionalGraphs
	 */
	static void linkConditionalQasm(QuantumCircuit& mainGraph,
			std::vector<QuantumCircuit>& conditionalGraphs,
			std::vector<int>& conditionalQubits) {

		// At this point we have a main circuit graph,
		// and one or more smaller conditional graphs (each with
		// initial and final state nodes.

		// We need to loop through the conditional graphs and
		// pull out the gate vertices (skip initial final state nodes)
		// and add them to the main graph after some conditional nodes

		assert (conditionalQubits.size() == conditionalGraphs.size());

		int counter = 0;
		int finalIdMainGraph = mainGraph.getVertexProperty<2>(mainGraph.order() - 1);
		int finalLayerMainGraph = mainGraph.getVertexProperty<1>(mainGraph.order() - 1);
		int layer = finalLayerMainGraph + 1;
		int id = finalIdMainGraph + 1;

		// By default all conditional graph nodes should be disabled
		// Conditional nodes should have acting qubits set to the
		// measured qubit, and gate vertex id set to the measurement
		// gate vertex id.
		for (auto g : conditionalGraphs) {
			CircuitNode node;
			std::get<0>(node.properties) = "conditional";
			std::get<2>(node.properties) = id;
			std::get<3>(node.properties) = std::vector<int> {conditionalQubits[counter]};
			mainGraph.addVertex(node);

			// Connect the main graph to the cond node
			mainGraph.addEdge(finalIdMainGraph, id);

			id++;

			std::vector<CircuitNode> newGates;
			int initialStateId, initialStateLayer;
			for (int i = 0; i < g.order(); i++) {
				CircuitNode newVert;
				newVert.properties = g.getVertexProperties(i);
				std::get<0>(newVert.properties) = g.getVertexProperty<0>(i);
				std::get<1>(newVert.properties) = layer+g.getVertexProperty<1>(i);
				std::get<2>(newVert.properties) = id;
				std::get<3>(newVert.properties) = g.getVertexProperty<3>(i);
				std::get<4>(newVert.properties) = false;
				mainGraph.addVertex(newVert);
				newGates.push_back(newVert);

				// Connect conditional node with first node here
				if(i == 0) mainGraph.addEdge(id - 1, id);
				if(i == 0) initialStateId = id;
				if (i == 0) initialStateLayer = std::get<1>(newVert.properties);

				id++;
			}

			generateEdgesFromLayer(initialStateLayer+1, mainGraph, newGates, initialStateId);

			// Set the new layer for the next conditional node
			layer = mainGraph.getVertexProperty<1>(id - 1) + 1;
			counter++;
		}




	}

private:

	/**
	 * Generate all edges for the circuit graph starting at
	 * the given layer.
	 *
	 * @param layer
	 * @param graph
	 * @param gateOperations
	 * @param initialStateId
	 */
	static void generateEdgesFromLayer(const int layer, QuantumCircuit& graph,
			std::vector<CircuitNode>& gateOperations, int initialStateId) {

		int nQubits = std::get<3>(graph.getVertexProperties(0)).size();
		int maxLayer = std::get<1>(
				gateOperations[gateOperations.size() - 1].properties);

		std::map<int,int> qubitToCurrentGateId;
		for (int i = 0 ; i < nQubits; i++) {
			qubitToCurrentGateId[i] = initialStateId;
		}

		int currentLayer = layer;
		while (currentLayer <= maxLayer) {
			std::vector<CircuitNode> currentLayerGates;
			std::copy_if(gateOperations.begin(), gateOperations.end(),
					std::back_inserter(currentLayerGates),
					[&](const CircuitNode& c) {return std::get<1>(c.properties) == currentLayer;});

			for (auto n : currentLayerGates) {
				std::vector<int> actingQubits = std::get<3>(n.properties);
				for (auto qubit : actingQubits) {
					int currentQubitGateId = qubitToCurrentGateId[qubit];
					graph.addEdge(currentQubitGateId, std::get<2>(n.properties));
					qubitToCurrentGateId[qubit] = std::get<2>(n.properties);
				}
			}

			currentLayer++;
		}
	}
	/**
	 * This method determines if a new layer should be added to the circuit.
	 *
	 * @param gateCommand
	 * @param qubitVarNameToId
	 * @param gates
	 * @param currentLayer
	 * @return
	 */
	static bool incrementLayer(const std::vector<std::string>& gateCommand,
			std::map<std::string, int>& qubitVarNameToId,
			const std::vector<CircuitNode>& gates, const int& currentLayer) {

		bool oneQubitGate = !boost::contains(gateCommand[1], ","), noGateAtQOnL = true;
		auto g = boost::to_lower_copy(gateCommand[0]);
		boost::trim(g);
		if (g == "measz") g = "measure";


		std::vector<CircuitNode> thisLayerGates;
		std::copy_if(gates.begin(), gates.end(),
				std::back_inserter(thisLayerGates),
				[&](const CircuitNode& c) {return std::get<1>(c.properties) == currentLayer;});

		for (auto layerGate : thisLayerGates) {
			std::vector<int> qubits = std::get<3>(layerGate.properties);
			for (auto q : qubits) {
				if (qubitVarNameToId[gateCommand[1]] == q) {
					noGateAtQOnL = false;
				}
			}
		}

		if (!oneQubitGate) {
			return true;
		} else if (!noGateAtQOnL) {
			return true;
		} else if (!gates.empty()
				&& (std::get<0>(gates[gates.size() - 1].properties)
						== "measure") && g != "measure") {
			return true;
		}

		return false;
	}
};
}
}

#endif /* QUANTUM_GATE_QASMTOGRAPH_HPP_ */
