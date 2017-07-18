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
#ifndef QUANTUM_AQC_COMPILER_HUBO_HPP_
#define QUANTUM_AQC_COMPILER_HUBO_HPP_

#include "DWGraph.hpp"

namespace xacc {
namespace quantum {

/**
 * HUBO is an interface for higher-order unconstrained
 * binary optimization problems. It provides a method
 * for subclasses to implement that takes a set of input
 * parameters and maps the problem down to a
 * quadratic unconstrained binary optimization problem,
 * in the form of a Graph.
 *
 * It takes a template parameter describing the
 * return type of the mapResults method - which
 * takes AQC QPU execution results and maps them to the
 * higher-order problem result.
 */
template<typename ResultType>
class HUBO {

public:

	/**
	 * Map this HUBO problem to a quadratic unconstrained
	 * binary optimization problem.
	 *
	 * @param parameters Any input parameters needed to set the problem up
	 * @return quboGraph The QUBO represented as a graph
	 */
	virtual std::shared_ptr<xacc::quantum::DWGraph> reduceToQubo(
			std::vector<InstructionParameter> parameters) = 0;

	/**
	 * Take the low-level AQC QPU result output and construct result of
	 * HUBO problem .
	 *
	 * @param resultBuffer The AcceleratorBuffer containing the AQC QPU results.
	 * @return results Custom Type representing the result.
	 */
	virtual ResultType mapResults(
			std::shared_ptr<AcceleratorBuffer> resultBuffer) = 0;

	/**
	 * The destructor
	 */
	virtual ~HUBO() {
	}
};

}

}

#endif
