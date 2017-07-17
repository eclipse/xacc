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
#include "GateQIR.hpp"
#include <boost/algorithm/string.hpp>
#include <regex>
#include "JsonVisitor.hpp"

namespace xacc {
namespace quantum {

void GateQIR::generateGraph(const std::string& kernelName) {

	auto flatQasmStr = toAssemblyString(kernelName, "qreg");

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
	addVertex("InitialState", 0, 0, allQbitIds, true, std::vector<std::string>{});

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

				int counter = 0;
				std::vector<std::string> splitComma, props;
				boost::split(splitComma, gateCommand[1], boost::is_any_of(","));
				for (auto segment : splitComma) {
					if (boost::contains(segment, qubitVarName)) {
						actingQubits.push_back(qubitVarNameToId[segment]);
					} else {
						// This is not a qubit, it must be a parameter for gate
						props.push_back("PARAM_" + std::to_string(counter));
						std::get<5>(node.properties) = props;
						counter++;
					}
				}
			}

			// Set the acting qubits
			std::get<3>(node.properties) = actingQubits;

			// Add this gate to the local vector
			// and to the graph
			gateOperations.push_back(node);
			addVertex(node);
		}
	}

	// Add a final layer for the graph sink
	CircuitNode finalNode;
	std::get<0>(finalNode.properties) = "FinalState";
	std::get<1>(finalNode.properties) = layer+1;
	std::get<2>(finalNode.properties) = gateId;
	std::get<3>(finalNode.properties) = allQbitIds;
	std::get<4>(finalNode.properties) = true;

	addVertex(finalNode);
	gateOperations.push_back(finalNode);

	// Set how many layers are in this circuit
	int maxLayer = layer+1;

	generateEdgesFromLayer(1, gateOperations, 0);

	return;
}

void GateQIR::generateEdgesFromLayer(const int layer,
		std::vector<CircuitNode>& gateOperations, int initialStateId) {

	int nQubits = std::get<3>(getVertexProperties(0)).size();
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
				addEdge(currentQubitGateId, std::get<2>(n.properties));
				qubitToCurrentGateId[qubit] = std::get<2>(n.properties);
			}
		}

		currentLayer++;
	}
}

bool GateQIR::incrementLayer(const std::vector<std::string>& gateCommand,
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


std::string GateQIR::toAssemblyString(const std::string& kernelName, const std::string& accBufferVarName) {
	std::string retStr = "";
	auto kernel = getKernel(kernelName);

	std::set<int> qubitsUsed;
	InstructionIterator it(kernel);
	while (it.hasNext()) {
		// Get the next node in the tree
		auto nextInst = it.next();

		// If enabled, invoke the accept
		// method which kicks off the visitor
		// to execute the appropriate lambda.
		if (nextInst->isEnabled() && !nextInst->isComposite()) {

			for (auto qi : nextInst->bits()) {
				qubitsUsed.insert(qi);
			}
		}
	}

	for (auto qi : qubitsUsed) {
		retStr += "qubit " + accBufferVarName + std::to_string(qi) + "\n";
	}
	for (auto f : kernels) {
		retStr += f->toString(accBufferVarName);
	}
	return retStr;
}

void GateQIR::persist(std::ostream& outStream) {

	JsonVisitor visitor(kernels);
	outStream << visitor.write();

//	StringBuffer sb;
//	PrettyWriter<StringBuffer> writer(sb);
//
//	serializeJson(writer);
//
//	outStream << sb.GetString();

	return;
}

// FOR IR
void GateQIR::load(std::istream& inStream) {
	read(inStream);
}

// FOR GRAPH
void GateQIR::read(std::istream& stream) {

	// IMPLEMENT GRAPH READ STUFF

	std::string content { std::istreambuf_iterator<char>(stream),
			std::istreambuf_iterator<char>() };

	std::vector<std::string> lines, sectionOne, sectionTwo;
	boost::split(lines, content, boost::is_any_of("\n"));

	for (auto l : lines) {
		if (boost::contains(l, "label") && boost::contains(l, "--")){
			sectionTwo.push_back(l);
		} else if (boost::contains(l, "label")) {
			sectionOne.push_back(l);
		}
	}

	// Sections should be size 2 for a valid dot file
//	boost::split(lines, sections[0], boost::is_any_of("\n"));
	for (auto line : sectionOne) {
		if (boost::contains(line, "label")) {
			CircuitNode v;

			auto firstBracket = line.find_first_of('[');
			line = line.substr(firstBracket + 1, line.size() - 2);

			boost::replace_all(line, ";", "");
			boost::replace_all(line, "[", "");
			boost::replace_all(line, "]", "");
			boost::replace_all(line, "\"", "");
			boost::replace_all(line, "label=", "");
			boost::trim(line);
			std::vector<std::string> labelLineSplit, removeId;
			boost::split(labelLineSplit, line, boost::is_any_of(","));

			for (auto a : labelLineSplit) {
				std::vector<std::string> eqsplit;
				boost::split(eqsplit, a, boost::is_any_of("="));
				if (eqsplit[0] == "Gate") {
					std::get<0>(v.properties) = eqsplit[1];
				} else if (eqsplit[0] == "Circuit Layer") {
					std::get<1>(v.properties) = std::stoi(eqsplit[1]);
				} else if (eqsplit[0] == "Gate Vertex Id") {
					std::get<2>(v.properties) = std::stoi(eqsplit[1]);
				} else if (eqsplit[0] == "Gate Acting Qubits") {
					auto qubitsStr = eqsplit[1];
					boost::replace_all(qubitsStr, "[", "");
					boost::replace_all(qubitsStr, "[", "");
					std::vector<std::string> elementsStr;
					std::vector<int> qubits;
					boost::split(elementsStr, qubitsStr, boost::is_any_of(" "));
					for (auto element : elementsStr) {
						qubits.push_back(std::stoi(element));
					}
					std::get<3>(v.properties) = qubits;
				} else if (eqsplit[0] == "Enabled") {
					std::get<4>(v.properties) = (bool) std::stoi(eqsplit[1]);
				}

			}
			addVertex(v);
		}
	}

	// Now add the edges
	lines.clear();
//	boost::split(lines, sections[1], boost::is_any_of(";\n"));
	for (auto line : sectionTwo) {
		boost::trim(line);
		if (line == "}" || line.empty())
			continue;
		boost::trim(line);
		boost::replace_all(line, "--", " ");
		std::vector<std::string> vertexPairs;
		boost::split(vertexPairs, line, boost::is_any_of(" "));
		addEdge(std::stoi(vertexPairs[0]), std::stoi(vertexPairs[1]));
	}

}

}
}

