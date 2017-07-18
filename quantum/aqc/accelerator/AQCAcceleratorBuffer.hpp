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
#ifndef QUANTUM_AQC_ACCELERATOR_AQCACCELERATORBUFFER_HPP_
#define QUANTUM_AQC_ACCELERATOR_AQCACCELERATORBUFFER_HPP_

#include "AcceleratorBuffer.hpp"

namespace xacc {
namespace quantum {

/**
 * The AQCAcceleratorBuffer is an AcceleratorBuffer that keeps
 * track of the problem-specific embedding into the
 * hardware graph. It also tracks AQC QPU computed
 * energies.
 */
class AQCAcceleratorBuffer : public AcceleratorBuffer {

protected:

	/**
	 * The minor graph embedding for the problem these
	 * results represent.
	 */
	std::map<int, std::list<int>> embedding;

	/**
	 * The energies computed as part of this execution.
	 */
	std::vector<double> energies;

public:

	/**
	 * The constructor
	 * @param str The name of this buffer
	 * @param N The number of bits represented by this buffer
	 */
	AQCAcceleratorBuffer(const std::string& str, const int N) :
			AcceleratorBuffer(str, N) {
	}

	/**
	 * The constructor
	 * @param str
	 * @param firstIndex
	 * @param indices
	 */
	template<typename ... Indices>
	AQCAcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			AcceleratorBuffer(str, firstIndex, indices...) {
	}

	/**
	 * Set the minor graph embedding for the problem
	 * solved during this execution .
	 *
	 * @param emb The minor graph embedding
	 */
	void setEmbedding(std::map<int, std::list<int>> emb) {
		embedding = emb;
	}

	/**
	 * Return the minor graph embedding.
	 *
	 * @return emb The minor graph embedding
	 */
	std::map<int, std::list<int>> getEmbedding() {
		return embedding;
	}

};

}
}
#endif
