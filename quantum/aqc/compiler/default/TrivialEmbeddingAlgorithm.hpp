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
#ifndef QUANTUM_AQC_COMPILER_TRIVIALEMBEDDINGALGORITHM_HPP_
#define QUANTUM_AQC_COMPILER_TRIVIALEMBEDDINGALGORITHM_HPP_

#include "EmbeddingAlgorithm.hpp"

namespace xacc {
namespace quantum {

/**
 * The EmbeddingAlgorithm class provides an interface
 * for minor graph embedding algorithms.
 *
 */
class TrivialEmbeddingAlgorithm : public EmbeddingAlgorithm {

public:

	/**
	 * The Constructor
	 */
	TrivialEmbeddingAlgorithm() {}

	/**
	 * The Destructor
	 */
	virtual ~TrivialEmbeddingAlgorithm() {}

	/**
	 * Implementations of EmbeddingAlgorithm implement this method to
	 * provide a valid minor graph embedding of the given problem
	 * graph into the given hardware graph.
	 *
	 * @param problem The problem graph to be embedded into the hardware graph
	 * @param hardware The hardware graph.
	 * @param params Any key-value string parameters to influence the algorithm.
	 * @return embedding A mapping of problem vertex indices to the list of hardware vertices they map to
	 */
	virtual Embedding embed(std::shared_ptr<DWGraph> problem,
			std::shared_ptr<AcceleratorGraph> hardware,
			std::map<std::string, std::string> params = std::map<std::string,
					std::string>());

	virtual const std::string name() const {
		return "trivial";
	}

	virtual const std::string description() const {
		return "This Embedding Algorithm trivially maps a problem vertex "
				"to a hardware vertex.";
	}

};

}

}

#endif
