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
#ifndef QUANTUM_GATE_GATEQIR_HPP_
#define QUANTUM_GATE_GATEQIR_HPP_

#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "InstructionIterator.hpp"
#include "Graph.hpp"
#include "IR.hpp"

#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "X.hpp"
#include "Z.hpp"
#include "ConditionalFunction.hpp"
#include "Rz.hpp"
#include "Measure.hpp"

namespace xacc {
namespace quantum {

/**
 * CircuitNode subclasses QCIVertex to provide the following
 * parameters in the given order:
 *
 * Parameters: Gate, Layer (ie time sequence), Gate Vertex Id,
 * Qubit Ids that the gate acts on, enabled state, vector of parameters names
 */
class CircuitNode: public XACCVertex<std::string, int, int, std::vector<int>,
		bool, std::vector<std::string>> {
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
 * The GateQIR is an implementation of the QIR for gate model quantum
 * computing. It provides a Graph node type that models a quantum
 * circuit gate (CircuitNode).
 *
 */
class GateQIR: public xacc::IR, public Graph<CircuitNode> {

public:

	/**
	 * The nullary Constructor
	 */
	GateQIR() {
	}

	/**
	 * This method takes the list of quantum instructions that this
	 * QIR contains and creates a graph representation of the
	 * quantum circuit.
	 */
	virtual void generateGraph(const std::string& kernelName);

	virtual const int maxBit() {
		int maxBit = 0;
		for (auto k : kernels) {
			for (auto inst : k->getInstructions()) {
				for (auto b : inst->bits()) {
					if (b > maxBit) {
						maxBit = b;
					}
				}
			}
		}
		return maxBit;
	}

	/**
	 * Add a quantum function to this intermediate representation.
	 * @param kernel
	 */
	virtual void addKernel(std::shared_ptr<Function> kernel) {
		kernels.push_back(kernel);
	}

	virtual const int numberOfKernels() {
		return kernels.size();
	}

	virtual std::shared_ptr<Function> getKernel(const std::string& name) {
		std::shared_ptr<Function> ret;
		for (auto f : kernels) {
			if (f->getName() == name) {
				ret = f;
			}
		}
		if (!ret) {
			xacc::error("Invalid kernel name.");
		}
		return ret;
	}

	virtual bool kernelExists(const std::string& name) {
		return std::any_of(kernels.cbegin(), kernels.cend(),
				[=](std::shared_ptr<Function> i) {return i->getName() == name;});
	}

	virtual void mapBits(std::vector<int> bitMap) {
		for (auto k : kernels) {
			k->mapBits(bitMap);
		}
	}

	/**
	 * Return a string representation of this
	 * intermediate representation
	 * @return
	 */
	virtual std::string toAssemblyString(const std::string& kernelName, const std::string& accBufferVarName);

	/**
	 * Persist this IR instance to the given
	 * output stream.
	 *
	 * @param outStream
	 */
	virtual void persist(std::ostream& outStream);

	/**
	 * Create this IR instance from the given input
	 * stream.
	 *
	 * @param inStream
	 */
	virtual void load(std::istream& inStream);

	/**
	 * This is the implementation of the Graph.read method...
	 *
	 * Read in a graphviz dot graph from the given input
	 * stream. This is left for subclasses.
	 *
	 * @param stream
	 */
	virtual void read(std::istream& stream);

	virtual std::vector<std::shared_ptr<Function>> getKernels() {
		return kernels;
	}

	/**
	 * The destructor
	 */
	virtual ~GateQIR() {
	}

protected:

	/**
	 * Reference to this QIR's list of quantum functions
	 */
	std::vector<std::shared_ptr<Function>> kernels;

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
	bool incrementLayer(const std::vector<std::string>& gateCommand,
			std::map<std::string, int>& qubitVarNameToId,
			const std::vector<CircuitNode>& gates, const int& currentLayer);

	/**
	 * Generate all edges for the circuit graph starting at
	 * the given layer.
	 *
	 * @param layer
	 * @param graph
	 * @param gateOperations
	 * @param initialStateId
	 */
	void generateEdgesFromLayer(const int layer,
			std::vector<CircuitNode>& gateOperations, int initialStateId);

	/**
	 * Create connecting conditional nodes that link the main
	 * circuit graph to subsequent conditional graphs. The conditional
	 * nodes can be used by Accelerators to figure out if the condition
	 * code should be executed or not.
	 * s
	 * @param mainGraph
	 * @param conditionalGraphs
	 */
//	static void linkConditionalQasm(QuantumCircuit& mainGraph,
//			std::vector<QuantumCircuit>& conditionalGraphs,
//			std::vector<int>& conditionalQubits);
};
}
}
#endif
