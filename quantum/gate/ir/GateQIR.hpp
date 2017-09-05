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
		for (auto f : kernels) {
			if (f->getName() == name) {
				return f;
			}
		}
		XACCError("Invalid kernel name.");
	}

	virtual bool kernelExists(const std::string& name) {
		return std::any_of(kernels.cbegin(), kernels.cend(),
				[=](std::shared_ptr<Function> i) {return i->getName() == name;});
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
