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
#ifndef QUANTUM_GATE_ACCELERATORS_EIGENACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_EIGENACCELERATOR_HPP_

#include "QPUGate.hpp"
#include "QasmToGraph.hpp"
#include "GraphIR.hpp"
#include "SimulatedQubits.hpp"
#include <random>

namespace xacc {
namespace quantum {

double sqrt2 = std::sqrt(2.0);
using GraphType = qci::common::Graph<CircuitNode>;
using QuantumGraphIR = xacc::GraphIR<GraphType>;

/**
 * The FireTensorAccelerator is an XACC Accelerator that simulates
 * gate based quantum computing circuits. It models the QPUGate Accelerator
 * with SimulatedQubit AcceleratorBuffer. It relies on the Fire Scientific Computing
 * Framework's tensor module to model a specific set of quantum gates. It uses these
 * tensors to build up the unitary matrix described by the circuit.
 */
template<const int NQubits>
class FireTensorAccelerator : virtual public QPUGate<SimulatedQubits<NQubits>> {
public:

	/**
	 * The constructor, create tensor gates
	 */
	FireTensorAccelerator() {
		fire::Tensor<2> h(2,2), cnot(4,4), I(2,2), x(2,2), p0(2,2), p1(2,2), z(2,2);
		h.setValues({{1.0/sqrt2, 1.0/sqrt2},{1.0/sqrt2,-1.0/sqrt2}});
		cnot.setValues({{1,0,0,0},{0,0,1,0},{0,0,0,1},{0,0,1,0}});
		x.setValues({{0, 1},{1, 0}});
		I.setValues({{1,0},{0,1}});
		p0.setValues({{1,0},{0,0}});
		p1.setValues({{0,0},{0,1}});
		z.setValues({{1,0},{0,-1}});
		gates.insert(std::make_pair("h",h));
		gates.insert(std::make_pair("cont",cnot));
		gates.insert(std::make_pair("x",x));
		gates.insert(std::make_pair("I",I));
		gates.insert(std::make_pair("p0",p0));
		gates.insert(std::make_pair("p1",p1));
		gates.insert(std::make_pair("z",z));
	}

	/**
	 * Execute the simulation. Requires both a valid SimulatedQubits buffer and
	 * Graph IR instance modeling the quantum circuit.
	 *
	 * @param ir
	 */
	virtual void execute(const std::string& bufferId, const std::shared_ptr<xacc::IR> ir) {

		// Get the requested qubit buffer
		auto qubits = this->allocatedBuffers[bufferId];
		if (!qubits) {
			QCIError("Invalid buffer id. Could not get qubit buffer.");
		}

		// Set the size
		int nQubits = qubits->size();

		// Cast to a GraphIR, if we can...
		auto graphir = std::dynamic_pointer_cast<QuantumGraphIR>(ir);
		if (!graphir) {
			QCIError("Invalid IR - this Accelerator only accepts GraphIR<Graph<CircuitNode>>.");
		}

		// Get the Graph and related info
		std::vector<CircuitNode> gateOperations;
		std::map<int, int> qubitIdToMeasuredResult;
		auto graph = graphir->getGraph();
		int nNodes = graph.order(), layer = 1;
		int finalLayer = graph.getVertexProperty<1>(nNodes - 1);

		// Get a vector of all gates
		for (int i = 0; i < nNodes; i++) {
			CircuitNode n;
			n.properties = graph.getVertexProperties(i);
			gateOperations.emplace_back(n);
		}

		// Loop over all gates in the circuit
		for (auto gate : gateOperations) {

			// Skip disabled gates...
			if (!std::get<4>(gate.properties)) {
				continue;
			}

			// Create a list of nQubits Identity gates
			std::vector<fire::Tensor<2>> productList;
			for (int i = 0; i < nQubits; i++) {
				productList.push_back(gates.at("I"));
			}

			// Create a local U gate, initialized to identity
			fire::Tensor<2> localU = gates.at("I");

			// Get the current gate anme
			auto gateName = std::get<0>(gate.properties);

			// Get the qubits we're acting on...
			auto actingQubits = std::get<3>(gate.properties);

			if (gateName == "conditional") {

				// If we hit a measured result then we
				// need to figure out the measured qubit it
				// depends on, then if its a 1, enable the disabled
				// gates from this node to the next FinalState node
				auto qubitWeNeed = std::get<3>(gate.properties)[0];
				auto qubitFound = qubitIdToMeasuredResult.find(qubitWeNeed);
				if (qubitFound == qubitIdToMeasuredResult.end()) {
					QCIError("Invalid conditional node - this qubit has not been measured.");
				}

				auto result = qubitIdToMeasuredResult[qubitWeNeed];

				auto currentNodeId = std::get<2>(gate.properties);
				if (result == 1) {
					// Walk over the next nodes until we hit a FinalState node
					// set their enabled state to true
					for (int i = currentNodeId+1; i < nNodes; i++) {
						// Once we hit the next final state node, then break out
						if (std::get<0>(gateOperations[i].properties) == "FinalState") {
							break;
						}
						std::get<4>(gateOperations[i].properties) = true;
					}
				}

			} else if (gateName == "measure") {

				// get rho - outer product of state with itself
				auto rho = qubits->getState() * qubits->getState();

				// Create a total unitary for this layer of the circuit
				productList.at(actingQubits[0]) = gates.at("p0");
				auto Pi0 = productList.at(0);
				for (int i = 1; i < productList.size(); i++) {
					Pi0 = Pi0.kronProd(productList.at(i));
				}

				// Get probability qubit is a 0
				double probZero = 0.0;
				std::array<IndexPair, 1> contractionIndices;
				contractionIndices[0] = std::make_pair(1, 0);
				auto Prob0 = Pi0.contract(rho, contractionIndices);
				for (int i = 0; i < Prob0.dimension(0); i++) probZero += Prob0(i,i);

				// Make the measurement random...
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> dist(0, 1.0);
				int result;
				auto val = dist(mt);
				if (val < std::real(probZero)) {
					result = 0;
					qubits->applyUnitary(Pi0);
					qubits->normalize();
				} else {
					result = 1;
					productList.at(actingQubits[0]) = gates.at("p1");
					// Create a total unitary for this layer of the circuit
					auto Pi1 = productList.at(0);
					for (int i = 1; i < productList.size(); i++) {
						Pi1 = Pi1.kronProd(productList.at(i));
					}
					qubits->applyUnitary(Pi1);
					qubits->normalize();
				}

				// Record the measurement result
				qubitIdToMeasuredResult.insert(std::make_pair(actingQubits[0], result));

			} else {
				if (gateName != "FinalState" && gateName != "InitialState") {
					// Regular Gate operations...

					if (actingQubits.size() == 1) {

						// If this is a one qubit gate, just replace
						// the currect I in the list with the gate
						productList.at(actingQubits[0]) = gates.at(gateName);

						// Create a total unitary for this layer of the circuit
						localU = productList.at(0);
						for (int i = 1; i < productList.size(); i++) {
							localU = localU.kronProd(productList.at(i));
						}

					} else if (actingQubits.size() == 2) {
						// If this is a 2 qubit gate, then we need t
						// to construct Kron(P0, I, ..., I) + Kron(P1, I, ..., Gate, ..., I)
						productList.at(actingQubits[0]) = gates.at("p0");
						localU = productList.at(0);
						for (int i = 1; i < productList.size(); i++) {
							localU = localU.kronProd(productList.at(i));
						}

						// Now create the second term in the sum
						productList.at(actingQubits[0]) = gates.at("p1");
						productList.at(actingQubits[1]) = gates.at(gateName == "cnot" ? "x" : "I");
						auto temp = productList.at(0);
						for (int i = 1; i < productList.size(); i++) {
							temp = temp.kronProd(productList.at(i));
						}

						// Sum them up
						localU = localU + temp;
					} else {
						QCIError("Can only simulate one and two qubit gates.");
					}

					// Make sure that localU is the correct size
					assert(
							localU.dimension(0) == std::pow(2, nQubits)
									&& localU.dimension(1)
											== std::pow(2, nQubits));

					// Appy the unitary and update th state
					qubits->applyUnitary(localU);

				}
			}
		}
	}

	/**
	 * The destructor
	 */
	virtual ~FireTensorAccelerator() {}

protected:

	/**
	 * Mapping of gate names to actual gate matrices.
	 */
	std::map<std::string, fire::Tensor<2>> gates;
};
}
}




#endif
