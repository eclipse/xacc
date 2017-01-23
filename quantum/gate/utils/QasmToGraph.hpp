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
 *   * Neither the name of the <organization> nor the
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

enum SupportedGates {
	H, CNot, C_Z, C_X, Measure, X, Y, Z, T, S, ZZ, SS, Swap, Toffoli, InitialState
};

const boost::unordered_map<std::string, SupportedGates> strToGate =
		map_list_of("h", H)("cnot", CNot)("c_z", C_Z)("c_x", C_X)("measure",
				Measure)("x", X)("y", Y)("z", Z)("t", T)("s", S)("zz", ZZ)("ss",
				SS)("swap", Swap)("toffoli", Toffoli);

const boost::unordered_map<SupportedGates, std::string> gateToStr =
		map_list_of(H, "h")(CNot, "cnot")(C_Z, "c_z")(C_X, "c_x")(Measure,
				"measure")(X, "x")(Y, "y")(Z, "z")(T, "t")(S, "s")(ZZ, "zz")(SS,
						"ss")(Swap, "swap")(Toffoli, "toffoli");
/**
 * Params - gate name, layer (ie time sequence), vertex id,
 * qubit ids that gate acts on
 */
class CircuitNode : public qci::common::QCIVertex<SupportedGates, int, int, std::vector<int>> {};

class QasmToGraph {

public:

	static qci::common::Graph<CircuitNode> getCircuitGraph(
			const std::string& flatQasmStr) {

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

		nQubits = qubitVarNameToId.size();

		std::cout << "Number of Qubits is " << nQubits << std::endl;

		// Fill the Graph...

		// First create a starting node for the initial
		// wave function - it should have nQubits outgoing
		// edges
		graph.addVertex(SupportedGates::InitialState, 0, 0, allQbitIds);

		std::vector<CircuitNode> gateOperations;
		for (auto line : qasmLines) {
			// If this is a gate line...
			if (!boost::contains(line, "qubit")) {
				std::sregex_token_iterator first { line.begin(),
						line.end(), spaceDelim, -1 }, last;
				std::vector<std::string> gateCommand = {first, last};

				// If we have a > 2 qubit gate, make a new layer
				if (boost::contains(gateCommand[1], ",")) {
					layer++;
				}

				// Create a new CircuitNode
				CircuitNode node;

				// Set the gate as a lowercase gate name string
				auto g = boost::to_lower_copy(gateCommand[0]);
				boost::trim(g);
				auto s = strToGate.at(g);
				std::get<0>(node.properties) = s;

				// Set the current layer
				std::get<1>(node.properties) = layer;

				std::get<2>(node.properties) = gateId;
				gateId++;

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

				gateOperations.push_back(node);
				graph.addVertex(node);
			}
		}

		// Set how many layers are in this circuit
		int maxLayer = layer;

		// Print info...
		for (auto cn : gateOperations) {
			std::cout << "Gate Operation: \n";
			std::cout << "\tName: " << gateToStr.at(std::get<0>(cn.properties)) << "\n";
			std::cout << "\tLayer: " << std::get<1>(cn.properties) << "\n";
			std::cout << "\tGate Vertex Id: " << std::get<2>(cn.properties) << "\n";
			std::cout << "\tActing Qubits: ";
			std::vector<int> qubits = std::get<3>(cn.properties);
			for (auto v : qubits) {
				if (!(v == *qubits.end())) {
					std::cout << v << ", ";
				} else {
					std::cout << v;
				}
			}
			std::cout << "\n\n";
		}

		// Add an edge between the initial state node
		// and the layer 1 gates
		std::map<int,int> qubitToCurrentGateId;
		for (int i = 0 ; i < nQubits; i++) {
			qubitToCurrentGateId[i] = 0;
		}
//		std::vector<CircuitNode> layerOneGates;
//		std::copy_if(gateOperations.begin(), gateOperations.end(),
//				std::back_inserter(layerOneGates),
//				[](const CircuitNode& c) {return std::get<1>(c.properties) == 1;});
//		for (auto i : layerOneGates) {
//			std::vector<int> actingQubits = std::get<3>(i.properties);
//			for (auto qubit : actingQubits) {
//				graph.addEdge(0, std::get<2>(i.properties));
//				qubitToCurrentGateId[qubit] = std::get<2>(i.properties);
//			}
//		}

		int currentLayer = 1;
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


		// Then we can go through and assign edges
		// to all vertices

		return graph;
	}

};
}
}

#endif /* QUANTUM_GATE_QASMTOGRAPH_HPP_ */
