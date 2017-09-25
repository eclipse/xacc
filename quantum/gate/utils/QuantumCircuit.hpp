/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_UTILS_QUANTUMCIRCUIT_HPP_
#define QUANTUM_GATE_UTILS_QUANTUMCIRCUIT_HPP_

#include "Graph.hpp"
#include <boost/algorithm/string.hpp>

namespace xacc {
namespace quantum {
/**
 * CircuitNode subclasses QCIVertex to provide the following
 * parameters in the given order:
 *
 * Parameters: Gate, Layer (ie time sequence), Gate Vertex Id,
 * Qubit Ids that the gate acts on, enabled state, vector of parameters names
 */
class CircuitNode: public XACCVertex<std::string, int, int,
		std::vector<int>, bool, std::vector<std::string>> {
public:
	CircuitNode() :
			XACCVertex() {
		propertyNames[0] = "Gate";
		propertyNames[1] = "Circuit Layer";
		propertyNames[2] = "Gate Vertex Id";
		propertyNames[3] = "Gate Acting Qubits";
		propertyNames[4] = "Enabled";
		propertyNames[5] = "RuntimeParameters";

		// by default all circuit nodes
		// are enabled and
		std::get<4>(properties) = true;

	}
};

/**
 * The QuantumCircuit is a subclass of Graph that provides
 * its Vertex template parameter as a CircuitNode. It adds the
 * ability to read QuantumCircuits from a graphviz dot file.
 */
class QuantumCircuit : virtual public Graph<CircuitNode> {
public:

	virtual void read(std::istream& stream) {
		std::string content { std::istreambuf_iterator<char>(stream),
				std::istreambuf_iterator<char>() };

		std::vector<std::string> lines, sections;
		boost::split(sections, content, boost::is_any_of("}"));

		// Sections should be size 2 for a valid dot file
		boost::split(lines, sections[0], boost::is_any_of("\n"));
		for (auto line : lines) {
			if (boost::contains(line, "label")) {
				CircuitNode v;

				auto firstBracket = line.find_first_of('[');
				line = line.substr(firstBracket+1, line.size()-2);

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
						boost::split(elementsStr, qubitsStr,
								boost::is_any_of(" "));
						for (auto element : elementsStr) {
							qubits.push_back(std::stoi(element));
						}
						std::get<3>(v.properties) = qubits;
					} else if (eqsplit[0] == "Enabled") {
						std::get<4>(v.properties) = (bool) std::stoi(
								eqsplit[1]);
					}

				}
				addVertex(v);
			}
		}

		// Now add the edges
		lines.clear();
		boost::split(lines, sections[1], boost::is_any_of(";\n"));
		for (auto line : lines) {
			boost::trim(line);
			if (line == "}" || line.empty()) continue;
			boost::trim(line);
			boost::replace_all(line, "--", " ");
			std::vector<std::string> vertexPairs;
			boost::split(vertexPairs, line, boost::is_any_of(" "));
			addEdge(std::stoi(vertexPairs[0]), std::stoi(vertexPairs[1]));
		}
	}


	virtual ~QuantumCircuit() {}
};

}
}

#endif /* QUANTUM_GATE_UTILS_QUANTUMCIRCUIT_HPP_ */
