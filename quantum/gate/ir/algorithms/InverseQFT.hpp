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
#ifndef QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_

#include "AlgorithmGenerator.hpp"

namespace xacc{
namespace quantum {

/**
 * InverseQFT is a realization of the AlgorithmGenerator
 * interface that produces an XACC IR representation
 * of the Inverse Quantum Fourier Transform.
 */
class InverseQFT : public AlgorithmGenerator {
public:

	/**
	 * This implementation returns a Function IR representation
	 * of the inverse quantum fourier transform.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits);

	virtual const std::string name() const {
		return "InverseQFT";
	}

	virtual const std::string description() const {
		return "The Inverse QFT Algorithm Generator generates IR representing "
				"an inverse Quantum Fourier Transform.";
	}

	/**
	 * The destructor
	 */
	virtual ~InverseQFT() {}
};
}
}


#endif
