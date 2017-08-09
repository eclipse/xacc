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
#include "Embedding.hpp"

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
	Embedding embedding;

	/**
	 * The energies computed as part of this execution.
	 */
	std::vector<double> energies;

	/**
	 * The number of occurrences for each energy/measurement
	 */
	std::vector<int> numOccurrences;

	/**
	 * This list of active qubit indices.
	 */
	std::vector<int> activeVarIndices;

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
	void setEmbedding(Embedding& emb) {
		embedding = emb;
	}

	/**
	 * Return the minor graph embedding.
	 *
	 * @return emb The minor graph embedding
	 */
	const Embedding& getEmbedding() const {
		return embedding;
	}

	/**
	 * Set the energies produced by the AQC Accelator execution.
	 *
	 * @param en The energies observed
	 */
	void setEnergies(std::vector<double>& en) {
		energies = en;
	}

	/**
	 * Return the energies.
	 *
	 * @return energes The energies
	 */
	const std::vector<double>& getEnergies() const {
		return energies;
	}

	/**
	 * Set the number of occurrences of each bit state and energe.
	 * @param nOcc The number of occurrences
	 */
	void setNumberOfOccurrences(std::vector<int>& numOcc) {
		numOccurrences = numOcc;
	}

	/**
	 * Return the number of occurrences of each bit state / energy.
	 *
	 * @return numOccurrences The number of occurrences
	 */
	const std::vector<int>& getNumberOfOccurrences() const {
		return numOccurrences;
	}

	/**
	 * Set the active qubit variable indices.
	 *
	 * @param activeVars The active indices.
	 */
	void setActiveVariableIndices(std::vector<int>& activeVars) {
		activeVarIndices = activeVars;
	}

	/**
	 * Get the active variable indices.
	 *
	 * @return activeVars The active indices
	 */
	const std::vector<int>& getActiveVariableIndices() const {
		return activeVarIndices;
	}

	/**
	 * Return the number of executions.
	 *
	 * @return numExec The number of executions
	 *
	 */
	const int getNumberOfExecutions() const {
		return std::accumulate(numOccurrences.rbegin(), numOccurrences.rend(), 0);
	}

	/**
	 * Return the bit string corresponding to the lowest observed energy.
	 *
	 * @return bitStr The bits corresponding to the lowest energy
	 */
	const boost::dynamic_bitset<>& getLowestEnergyMeasurement() {
		auto minIt = std::min_element(energies.begin(), energies.end());
		return measurements[std::distance(energies.begin(), minIt)];
	}

	/**
	 * Return the lowest energy observed.
	 *
	 * @return energy The lowest energy
	 */
	const double getLowestEnergy() const {
		return *std::min_element(energies.begin(), energies.end());
	}

	/**
	 * Return the most probable energy.
	 *
	 * @return energy The most probable energy.
	 */
	const double getMostProbableEnergy() const {
		auto maxIt = std::max_element(numOccurrences.begin(), numOccurrences.end());
		return energies[std::distance(numOccurrences.begin(), maxIt)];
	}

	/**
	 * Return the most probable bit string
	 *
	 * @return bitStr The bits
	 */
	const boost::dynamic_bitset<>& getMostProbableMeasurement() const {
		auto maxIt = std::max_element(numOccurrences.begin(), numOccurrences.end());
		return measurements[std::distance(numOccurrences.begin(), maxIt)];
	}
};

}
}
#endif
