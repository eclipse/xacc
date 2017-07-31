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
#ifndef QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_

#include "AlgorithmGenerator.hpp"

namespace xacc{
namespace quantum {

/**
 * QFT is a realization of the AlgorithmGenerator
 * interface that produces an XACC IR representation
 * of the Quantum Fourier Transform.
 *
 * @author Alex McCaskey
 */
class QFT : public AlgorithmGenerator {
public:

	/**
	 * This implementation returns a Function IR representation
	 * of the quantum fourier transform.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits);

	virtual const std::string name() const {
		return "QFT";
	}

	virtual const std::string description() const {
		return "The QFT Algorithm Generator generates IR representing "
				"an Quantum Fourier Transform.";
	}

	/**
	 * The destructor
	 */
	virtual ~QFT() {}
};
}
}


#endif
