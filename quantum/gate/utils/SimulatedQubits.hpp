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

			XACCLogger::instance()->info(
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
