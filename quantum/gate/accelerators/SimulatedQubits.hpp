#ifndef QUANTUM_GATE_SIMULATEDQUBITS_HPP_
#define QUANTUM_GATE_SIMULATEDQUBITS_HPP_

#include "Accelerator.hpp"
#include <complex>
#include <Eigen/Dense>

namespace xacc {

namespace quantum {

using QubitState = Eigen::VectorXcd;

/**
 *
 */
template<const int TotalNumberOfQubits>
class SimulatedQubits: public AcceleratorBuffer {
protected:

	QubitState bufferState;

public:

	SimulatedQubits(const std::string& str) :
			AcceleratorBuffer(str), bufferState((int) std::pow(2, TotalNumberOfQubits)) {
		// Initialize to |000...000> state
		bufferState.setZero();
		bufferState(0) = 1.0;
	}

	SimulatedQubits(const std::string& str, const int N) :
			AcceleratorBuffer(str, N), bufferState((int) std::pow(2, N)) {
		bufferState.setZero();
		bufferState(0) = 1.0;
	}

	template<typename ... Indices>
	SimulatedQubits(const std::string& str, int firstIndex, Indices ... indices) :
			AcceleratorBuffer(str, firstIndex, indices...), bufferState(
					(int) std::pow(2, sizeof...(indices) + 1)) {
	}

	void applyUnitary(Eigen::MatrixXcd& U) {
		bufferState = U * bufferState;
	}

	QubitState& getState() {
		return bufferState;
	}

	void setState(QubitState& st) {
		bufferState = st;
	}

	void printBufferState(std::ostream& stream) {
		for (int i = 0; i < bufferState.rows(); i++) {
			stream
					<< std::bitset<TotalNumberOfQubits>(i).to_string().substr(
							size(), TotalNumberOfQubits) << " -> "
					<< bufferState(i) << "\n";
		}
	}

	void mapBufferStateToSystemState() {

	}
};
}
}
#endif
