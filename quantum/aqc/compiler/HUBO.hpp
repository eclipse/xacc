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
