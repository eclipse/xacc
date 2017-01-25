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

#include "Graph.hpp"
#include <regex>
#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

namespace xacc {
namespace quantum {

using boost::assign::map_list_of;

/**
 * Enumeration of gates we support
 */
enum SupportedGates {
	H, CNot, C_Z, C_X, Measure, X, Y, Z,
	T, S, ZZ, SS, Swap, Toffoli, InitialState,
	FinalState
};

/**
 * Create a string to SupportedGates mapping
 */
const boost::unordered_map<std::string, SupportedGates> strToGate =
		map_list_of("h", H)("cnot", CNot)("c_z", C_Z)("c_x", C_X)("measure",
				Measure)("x", X)("y", Y)("z", Z)("t", T)("s", S)("zz", ZZ)("ss",
				SS)("swap", Swap)("toffoli", Toffoli);

/**
 * Create a SupportedGates to string mapping
 */
const boost::unordered_map<SupportedGates, std::string> gateToStr =
		map_list_of(H, "h")(CNot, "cnot")(C_Z, "c_z")(C_X, "c_x")(Measure,
				"measure")(X, "x")(Y, "y")(Z, "z")(T, "t")(S, "s")(ZZ, "zz")(SS,
						"ss")(Swap, "swap")(Toffoli, "toffoli")(FinalState, "FinalState")(InitialState, "InitialState");
/**
 * CircuitNode subclasses QCIVertex to provide the following
 * parameters in the given order:
 *
 * Parameters: Gate, Layer (ie time sequence), Gate Vertex Id,
 * Qubit Ids that the gate acts on
 */
class CircuitNode: public qci::common::QCIVertex<std::string, int, int,
		std::vector<int>> {
public:
	CircuitNode() :
			QCIVertex() {
		propertyNames[0] = "Gate";
		propertyNames[1] = "Circuit Layer";
		propertyNames[2] = "Gate Vertex Id";
		propertyNames[3] = "Gate Acting Qubits";
	}
};

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
	static qci::common::Graph<CircuitNode> getCircuitGraph(
			const std::string& flatQasmStr) {

		// Local Declarations
		using namespace qci::common;
		Graph<CircuitNode> graph;
		std::map<std::string, int> qubitVarNameToId;
		std::vector<std::string> qasmLines;
		std::vector<int> allQbitIds;
		std::regex newLineDelim("\n"), spaceDelim(" ");
		std::regex qubitDeclarations("\\s*qubit\\s*\\w+");
		std::sregex_token_iterator first{flatQasmStr.begin(), flatQasmStr.end(), newLineDelim, -1}, last;
		int nQubits = 0, qbitId = 0, layer = 1, gateId = 1;
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
			allQbitIds.push_back(qbitId);
			qbitId++;
		}

		// Set the number of qubits
		nQubits = qubitVarNameToId.size();

		std::cout << "Number of Qubits is " << nQubits << std::endl;

		// First create a starting node for the initial
		// wave function - it should have nQubits outgoing
		// edges
		graph.addVertex(gateToStr.at(SupportedGates::InitialState), 0, 0, allQbitIds);

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
//				auto s = strToGate.at(g);
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
					std::vector<std::string> qbits;
					boost::split(qbits, gateCommand[1], boost::is_any_of(","));
					for (auto q : qbits) {
						actingQubits.push_back(qubitVarNameToId[q]);
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
		graph.addVertex(gateToStr.at(SupportedGates::FinalState), layer+1, gateId, allQbitIds);

		// Set how many layers are in this circuit
		int maxLayer = layer;

		// Print info...
		for (auto cn : gateOperations) {
			std::cout << "Gate Operation: \n";
			std::cout << "\tName: " << std::get<0>(cn.properties) << "\n";
			std::cout << "\tLayer: " << std::get<1>(cn.properties) << "\n";
			std::cout << "\tGate Vertex Id: " << std::get<2>(cn.properties) << "\n";
			std::cout << "\tActing Qubits: ";
			std::vector<int> qubits = std::get<3>(cn.properties);
			for (auto v : qubits) {
				std::cout << v << ", ";
			}
			std::cout << "\n\n";
		}

		// Add an edge between the initial state node
		// and the layer 1 gates
		std::map<int,int> qubitToCurrentGateId;
		for (int i = 0 ; i < nQubits; i++) {
			qubitToCurrentGateId[i] = 0;
		}

		int currentLayer = 0;
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

		// Add a graph sink - ie a final node representing
		// the final system state
		int counter = 0;

		// Walk the list downward, skip first node since is
		// the graph sink
		for (int i = gateOperations.size() - 1; i >= 0; i--) {
			auto gate = gateOperations[i];
			int currentGateId = std::get<2>(gate.properties);
			int nQubitsActing = std::get<3>(gate.properties).size();
			int gateDegree = graph.degree(currentGateId);
			for (int j = gateDegree; j < 2 * nQubitsActing; j++) {
				graph.addEdge(currentGateId, gateId);
				counter++;
			}

			// Break early if we can...
			if (counter == nQubits)
				break;
		}

		return graph;
	}

private:

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
