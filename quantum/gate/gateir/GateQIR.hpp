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

#include "../../qir/QIR.hpp"

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
 *
 */
class GateQIR: public virtual xacc::quantum::QIR<xacc::quantum::CircuitNode> {

public:

	GateQIR() : QFunction() {}

	GateQIR(int id, const std::string name) : QFunction(id, name) {}

	/**
	 *
	 */
	virtual void generateGraph();

	/**
	 * Return a string representation of this
	 * intermediate representation
	 * @return
	 */
	virtual std::string toString();

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

	/**
	 * The destructor
	 */
	virtual ~GateQIR() {
	}

};
}
}
#endif
