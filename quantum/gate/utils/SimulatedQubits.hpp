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
#include <bitset>
#include <unsupported/Eigen/KroneckerProduct>

namespace xacc {

namespace quantum {

using QubitState = Eigen::VectorXcd;
using IndexPair = std::pair<int,int>;

/**
 * SimulatedQubits is an AcceleratorBuffer that
 * models simulated qubits. As such, it keeps track of the
 * state of the qubit buffer using a Rank 1 Fire Tensor.
 *
 * It provides an interface for applying unitary operations on the
 * qubit buffer state.
 */
class SimulatedQubits: public AcceleratorBuffer {
protected:

	/**
	 * The qubit buffer state.
	 */
	QubitState bufferState;

	Eigen::MatrixXcd Z;

public:

	/**
	 * The Constructor, creates a state with given size
	 * N.
	 * @param str Variable name of this buffer
	 * @param N The number of qubits to model
	 */
	SimulatedQubits(const std::string& str, const int N) :
			AcceleratorBuffer(str, N), bufferState((int) std::pow(2, N)) {
		bufferState.setZero();
		bufferState(0) = 1.0;
		Eigen::MatrixXcd z(2,2), temp;
		z << 1, 0, 0, -1;

		temp = z;
		for (int i = 0; i < N-1; i++) {
			temp = Eigen::kroneckerProduct(z, temp).eval();
		}

		Z = temp;

		assert(temp.rows() == std::pow(2,N));
	}

	/**
	 * The constructor
	 */
	template<typename ... Indices>
	SimulatedQubits(const std::string& str, int firstIndex, Indices ... indices) :
			AcceleratorBuffer(str, firstIndex, indices...), bufferState(
					(int) std::pow(2, sizeof...(indices) + 1)) {
	}

	void applyUnitary(Eigen::MatrixXcd& U) {
		bufferState = U * bufferState;
	}

	/**
	 * Normalize the state.
	 */
	void normalize() {
		bufferState.normalize();
	}

	/**
	 * Return the current state
	 *
	 * @return state This state vector
	 */
	QubitState& getState() {
		return bufferState;
	}

	/**
	 * Set the state.
	 * @param st The state to set on this buffer.
	 */
	void setState(QubitState& st) {
		bufferState = st;
	}

	virtual void resetBuffer() {
		bufferState.setZero();
		bufferState(0) = 1;
		measurements.clear();
	}

	virtual const double getExpectationValueZ() {
		if (measurements.empty()) {
			return std::real(bufferState.conjugate().transpose().dot(Z*bufferState));
		} else {
			return AcceleratorBuffer::getExpectationValueZ();
		}
	}

	/**
	 * Print the state to the provided output stream.
	 *
	 * @param stream The output stream to print to
	 */
	virtual void print(std::ostream& stream) {

		std::vector<std::string> bitStrings;
		std::function<void(std::string, int)> getStrings;
		getStrings = [&](std::string s, int digitsLeft) {
			if( digitsLeft == 0 ) {
				bitStrings.push_back( s );
			} else {
				getStrings( s + "0", digitsLeft - 1 );
				getStrings( s + "1", digitsLeft - 1 );
			}
		};

		getStrings("", size());

		for (int i = 0; i < bufferState.size(); i++) {

			stream << bitStrings[i] << " -> " << bufferState(i) << "\n";
		}
	}

	/**
	 * Print to XACC Info
	 */
	virtual void print() {

		std::vector<std::string> bitStrings;
		std::function<void(std::string, int)> getStrings;
		getStrings = [&](std::string s, int digitsLeft) {
			if( digitsLeft == 0 ) {
				bitStrings.push_back( s );
			} else {
				getStrings( s + "0", digitsLeft - 1 );
				getStrings( s + "1", digitsLeft - 1 );
			}
		};

		getStrings("", size());

		for (int i = 0; i < bufferState.size(); i++) {

			XACCInfo(
					bitStrings[i] + " -> ("
							+ std::to_string(std::real(bufferState(i))) + ", "
							+ std::to_string(std::imag(bufferState(i))) + ")");
		}
	}

	/**
	 * The destructor
	 */
	virtual ~SimulatedQubits() {}
};
}
}
#endif
