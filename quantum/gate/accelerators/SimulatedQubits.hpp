#ifndef QUANTUM_GATE_SIMULATEDQUBITS_HPP_
#define QUANTUM_GATE_SIMULATEDQUBITS_HPP_

#include "Accelerator.hpp"
#include <complex>
#include "Tensor.hpp"

namespace xacc {

namespace quantum {

using QubitState = fire::Tensor<1>;
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
			AcceleratorBuffer(str), bufferState(
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
	void applyUnitary(fire::Tensor<2>& U) {
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
			sum += bufferState(i) * bufferState(i);
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
	 * Print the state to the provided output stream.
	 *
	 * @param stream
	 */
	void printBufferState(std::ostream& stream) {
		for (int i = 0; i < bufferState.dimension(0); i++) {
			stream
					<< std::bitset<TotalNumberOfQubits>(i).to_string().substr(
							size(), TotalNumberOfQubits) << " -> "
					<< bufferState(i) << "\n";
		}
	}
};
}
}
#endif
