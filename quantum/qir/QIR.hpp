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
#ifndef QUANTUM_QIR_HPP_
#define QUANTUM_QIR_HPP_

#include "Graph.hpp"
#include "IR.hpp"
#include "Function.hpp"

namespace xacc {
namespace quantum {

/**
 * QIR represents a quantum intermediate representation. It is
 * an abstract class. It does not implement the IR interface methods, but leaves them for
 * further implementations of this class.
 * It also specifies a method for adding quantum kernels, represented
 * as the QIR QFunction class, to this IR instance.
 *
 * Furthermore, QIR is a Graph whose node type can be provided
 * at compile time with the VertexType template parameter. All quantum
 * intermediate representations have a dual list/graph nature. QIR therefore
 * specifies a generateGraph method for subclasses to implement to produce a graph
 * representation of the instruction-like intermediate representation.
 */
template<typename VertexType>
class QIR: public virtual xacc::Graph<VertexType>, public virtual xacc::IR {
public:

	/**
	 * The nullary constructor
	 */
	QIR() {
	}

	/**
	 * The constructor, takes the AcceleratorBuffer
	 * this IR works on.
	 * @param buf
	 */
	QIR(std::shared_ptr<AcceleratorBuffer> buf) :
			IR(buf) {
	}

	/**
	 * From this IR's list of instructions, construct an
	 * equivalent graph representation.
	 */
	virtual void generateGraph() = 0;

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
	/**
	 * The destructor
	 */
	virtual ~QIR() {
	}

protected:

	/**
	 * Reference to this QIR's list of quantum functions
	 */
	std::vector<std::shared_ptr<Function>> kernels;

};
}
}
#endif
