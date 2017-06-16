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
#ifndef QUANTUM_GATE_SIMULATEDQUBITS_HPP_
#define QUANTUM_GATE_SIMULATEDQUBITS_HPP_

#include "AcceleratorBuffer.hpp"
#include <complex>
#include "Tensor.hpp"
#include <bitset>

namespace xacc {

namespace quantum {

using QubitState = fire::Tensor<1, fire::EigenProvider, std::complex<double>>;
using IndexPair = std::pair<int,int>;

/**
 * SimulatedQubits is an AcceleratorBuffer that
 * models simulated qubits. As such, it keeps track of the
 * state of the qubit buffer using a Rank 1 Fire Tensor.
 *
 * It provides an interface for applying unitary operations on the
 * qubit buffer state.
 */
template<const int TotalNumberOfQubits>
class SimulatedQubits: public AcceleratorBuffer {
protected:

	/**
	 * The qubit buffer state.
	 */
	QubitState bufferState;

public:

	/**
	 * The Constructor, creates a state of size
	 * 2^TotalNumberOfQubits.
	 *
	 * @param str
	 */
	SimulatedQubits(const std::string& str) :
			AcceleratorBuffer(str, TotalNumberOfQubits), bufferState(
					(int) std::pow(2, TotalNumberOfQubits)) {
		// Initialize to |000...000> state
		bufferState(0) = 1.0;
	}

	/**
	 * The Constructor, creates a state with given size
	 * N.
	 * @param str
	 * @param N
	 */
	SimulatedQubits(const std::string& str, const int N) :
			AcceleratorBuffer(str, N), bufferState((int) std::pow(2, N)) {
		bufferState(0) = 1.0;
	}

	template<typename ... Indices>
	SimulatedQubits(const std::string& str, int firstIndex, Indices ... indices) :
			AcceleratorBuffer(str, firstIndex, indices...), bufferState(
					(int) std::pow(2, sizeof...(indices) + 1)) {
	}

	/**
	 * Apply the given unitary matrix to this qubit
	 * buffer state.
	 *
	 * @param U
	 */
	void applyUnitary(fire::Tensor<2, fire::EigenProvider, std::complex<double>>& U) {
		assert(
				U.dimension(0) == bufferState.dimension(0)
						&& U.dimension(1) == bufferState.dimension(0));
		std::array<IndexPair, 1> contractionIndices;
		contractionIndices[0] = std::make_pair(1, 0);
		bufferState = U.contract(bufferState, contractionIndices);
	}

	/**
	 * Normalize the state.
	 */
	void normalize() {
		double sum = 0.0;
		for (int i = 0; i < bufferState.dimension(0); i++)
			sum += std::real(bufferState(i) * bufferState(i));
		for (int i = 0; i < bufferState.dimension(0); i++)
			bufferState(i) = bufferState(i) / std::sqrt(sum);
	}

	/**
	 * Return the current state
	 *
	 * @return
	 */
	QubitState& getState() {
		return bufferState;
	}

	/**
	 * Set the state.
	 * @param st
	 */
	void setState(QubitState& st) {
		bufferState = st;
	}

	/**
	 * Allocating this buffer type is only valid
	 * if N <= TotalNumberOfQubits.
	 * @param N
	 * @return
	 */
	virtual bool isValidBufferSize(const int N) {
		return N <= TotalNumberOfQubits;
	}

	/**
	 * Print the state to the provided output stream.
	 *
	 * @param stream
	 */
	virtual void print(std::ostream& stream) {
		if (size() < TotalNumberOfQubits) {
			for (int i = 0; i < bufferState.dimension(0); i++) {
				stream
						<< std::bitset<TotalNumberOfQubits>(i).to_string().substr(
								TotalNumberOfQubits - size(), TotalNumberOfQubits) << " -> "
						<< std::fabs(bufferState(i)) << "\n";
			}
		} else {
			for (int i = 0; i < bufferState.dimension(0); i++) {

				stream << std::bitset<TotalNumberOfQubits>(i).to_string()
						<< " -> " << std::fabs(bufferState(i)) << "\n";
			}
		}
	}

	virtual void print() {
		if (size() < TotalNumberOfQubits) {
			for (int i = 0; i < bufferState.dimension(0); i++) {
				XACCInfo(
						std::bitset<TotalNumberOfQubits>(i).to_string().substr(
								TotalNumberOfQubits - size(),
								TotalNumberOfQubits) + " -> "
								+ std::to_string(std::real(std::fabs(bufferState(i)))));
			}
		} else {
			for (int i = 0; i < bufferState.dimension(0); i++) {

				XACCInfo(
						std::bitset<TotalNumberOfQubits>(i).to_string() + " -> "
								+ std::to_string(std::real(std::fabs(bufferState(i)))));
			}
		}
	}

	virtual ~SimulatedQubits() {}
};
}
}
#endif
