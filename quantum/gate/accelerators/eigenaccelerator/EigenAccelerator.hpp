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
#include <unsupported/Eigen/KroneckerProduct>
#include <random>

namespace xacc {
namespace quantum {

double sqrt2 = std::sqrt(2.0);
using GraphType = qci::common::Graph<CircuitNode>;
using QuantumGraphIR = xacc::GraphIR<GraphType>;

/**
 *
 */
template<const int NQubits>
class EigenAccelerator : public QPUGate<NQubits> {
public:

	/**
	 * The constructor, create tensor gates
	 */
	EigenAccelerator() {
		Eigen::MatrixXcd h(2,2), cnot(4,4), I(2,2), x(2,2), p0(2,2), p1(2,2);
		h << 1.0/sqrt2,1.0/sqrt2, 1.0/sqrt2,-1.0/sqrt2;
		cnot << 1, 0, 0, 0,0, 1, 0, 0,0, 0, 0, 1, 0, 0, 1, 0;
		x << 0, 1, 1, 0;
		I << 1,0,0,1;
		p0 << 1, 0, 0, 0;
		p1 << 0, 0, 0, 1;
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("h",h));
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("cnot",cnot));
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("I",I));
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("x",x));
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("p0",p0));
		gates.insert(std::map<std::string, Eigen::MatrixXcd>::value_type("p1",p1));
	}

	/**
	 *
	 * @param ir
	 */
	virtual void execute(const std::shared_ptr<xacc::IR> ir) {

		auto qubits = std::dynamic_pointer_cast<Qubits<NQubits>>(this->bits);

		// Cast to a GraphIR, if we can...
		auto graphir = std::dynamic_pointer_cast<QuantumGraphIR>(ir);
		if (!graphir) {
			QCIError("Invalid IR - this Accelerator only accepts GraphIR<Graph<CircuitNode>>.");
		}

		// Get the Graph
		std::vector<CircuitNode> gateOperations;
		std::map<int, int> qubitIdToMeasuredResult;
		auto graph = graphir->getGraph();
		int nNodes = graph.order(), layer = 1;
		int finalLayer = graph.getVertexProperty<1>(nNodes - 1);

		for (int i = 0; i < nNodes; i++) {
			CircuitNode n;
			n.properties = graph.getVertexProperties(i);
			gateOperations.emplace_back(n);
		}

		std::cout << "Initial State:\n";
		qubits->printState(std::cout);
		std::cout << "\n";

		for (auto gate : gateOperations) {

			// Skip disabled gates...
			if (!std::get<4>(gate.properties)) {
				continue;
			}

			// Create a list of nQubits Identity gates
			std::vector<Eigen::MatrixXcd> productList(NQubits);
			for (int i = 0; i < NQubits; i++) {
				productList[i] = gates["I"];
			}

			// Create a local U gate
			Eigen::MatrixXcd localU;

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
						if (graph.getVertexProperty<0>(i) == "FinalState") {
							break;
						}
						std::cout << "Enabling " << graph.getVertexProperty<0>(i) << "\n";
						graph.setVertexProperty<4>(i, true);
					}
				}

			} else if (gateName == "measure") {

				// get rho
				auto rho = qubits->getState() * qubits->getState().transpose();

				productList[actingQubits[0]] = gates["p0"];
				// Create a total unitary for this layer of the circuit
				auto temp = productList[0];
				for (int i = 1; i < productList.size(); i++) {
					temp = kroneckerProduct(temp, productList[i]).eval();
				}

				auto temp2 = temp * rho;
				auto probZero = temp2.trace();
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> dist(0, 1.0);
				int result;
				auto val = dist(mt);
				std::cout << "Val: " << val << "\n";
				if (val < std::real(probZero)) {
					result = 0;
					Eigen::VectorXcd newState = (temp * qubits->getState());
					newState.normalize();
					qubits->setState(newState);
				} else {
					result = 1;
					productList[actingQubits[0]] = gates["p1"];
					// Create a total unitary for this layer of the circuit
					auto temp = productList[0];
					for (int i = 1; i < productList.size(); i++) {
						temp = kroneckerProduct(temp, productList[i]).eval();
					}
					Eigen::VectorXcd newState = (temp * qubits->getState());
					newState.normalize();
					qubits->setState(newState);
				}

				std::cout << "Measured qubit " << actingQubits[0] << " to be a " << result << ": prob was " << probZero << "\n";
				qubitIdToMeasuredResult.insert(std::make_pair(actingQubits[0], result));

			} else {
				if (gateName != "FinalState" && gateName != "InitialState") {
					// Regular Gate operations...

					if (actingQubits.size() == 1) {

						// If this is a one qubit gate, just replace
						// the currect I in the list with the gate
						productList[actingQubits[0]] = gates[gateName];

						// Create a total unitary for this layer of the circuit
						localU = productList[0];
						for (int i = 1; i < productList.size(); i++) {
							localU =
									kroneckerProduct(localU, productList[i]).eval();
						}

					} else if (actingQubits.size() == 2) {
						// If this is a 2 qubit gate, then we need t
						// to construct Kron(P0, I, ..., I) + Kron(P1, I, ..., Gate, ..., I)
						productList[actingQubits[0]] = gates["p0"];
						localU = productList[0];
						for (int i = 1; i < productList.size(); i++) {
							localU =
									kroneckerProduct(localU, productList[i]).eval();
						}

						// Now create the second term in the sum
						productList[actingQubits[0]] = gates["p1"];
						productList[actingQubits[1]] = gates[gateName == "cnot" ? "x" : "I"];
						auto temp = productList[0];
						for (int i = 1; i < productList.size(); i++) {
							temp =
									kroneckerProduct(temp, productList[i]).eval();
						}

						// Sum them up
						localU += temp;
					} else {
						QCIError("Can only simulate one and two qubit gates.");
					}

					// Make sure that localU is the correct size
					assert(localU.rows() == std::pow(2, NQubits) && localU.cols() == std::pow(2,NQubits));

					qubits->applyUnitary(localU);

					std::cout << "Current State after " << gateName << ":\n";
					qubits->printState(std::cout);
					std::cout << "\n" << localU << "\n";
				}
			}
		}
//
//		while (layer < finalLayer) {
//
//			std::vector<CircuitNode> currentLayerGates;
//			std::copy_if(gateOperations.begin(), gateOperations.end(),
//					std::back_inserter(currentLayerGates),
//					[&](const CircuitNode& c) {return std::get<1>(c.properties) == layer;});
//
//			std::vector<Eigen::MatrixXcd> productList(NQubits);
//			for (int i = 0; i < NQubits; i++) {
//				productList[i] = gates["I"];
//			}
//
//			// Can parallize this...
//			for (auto n : currentLayerGates) {
//
//				auto gateName = std::get<0>(n.properties);
//				auto actingQubits = std::get<3>(n.properties);
//
//				if (gateName != "measure" || gateName != "FinalState" || gateName != "InitialState") {
//					auto gate = gates[gateName];
//
//					if (actingQubits.size() == 1) {
//						productList[actingQubits[0]] = gate;
//					} else if (actingQubits.size() == 2) {
//						productList[actingQubits[0]] = gate;
//						productList.erase(productList.begin() + actingQubits[1]);
//					} else {
//						QCIError("Can only simulate one and two qubit gates.");
//					}
//				} else {
//
//					if (gateName == "conditional") {
//
//					} else if (gateName == "measure") {
//
//					}
//				}
//
//				// Create a total unitary for this layer of the circuit
//				Eigen::MatrixXcd result = productList[0];
//				for (int i = 1; i < productList.size(); i++) {
//					result = kroneckerProduct(result, productList[i]).eval();
//				}
//				assert(result.rows() == std::pow(2, NQubits) && result.cols() == std::pow(2,NQubits));
//
//				// Update the circuit unitary matrix
//				U = result * U;
//
//			}
//
//			layer++;
//		}
//
//		qubitsType->applyUnitary(U);
	}

	/**
	 * The destructor
	 */
	virtual ~EigenAccelerator() {
	}

protected:

	std::map<std::string, Eigen::MatrixXcd> gates;
};
}
}



#endif
