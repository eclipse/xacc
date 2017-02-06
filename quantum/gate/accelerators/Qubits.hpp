#ifndef QUANTUM_GATE_QUBITS_HPP_
#define QUANTUM_GATE_QUBITS_HPP_

#include "Accelerator.hpp"
#include <complex>
#include <Eigen/Dense>

namespace xacc {

namespace quantum {

using QubitState = Eigen::VectorXcd;

/**
 *
 */
template<const int NumberOfQubits>
class Qubits: public AcceleratorBits<NumberOfQubits> {
protected:

	QubitState state;

	QubitState subState;

	bool usingSubstate;

public:

	Qubits() :
			state((int) std::pow(2, NumberOfQubits)), usingSubstate(false) {
		// Initialize to |000...000> state
		state.setZero();
		state(0) = 1.0;
	}

	template<typename ... ActiveBits>
	Qubits(QubitState& currentState, ActiveBits ... activeBits) :
			state(currentState), subState((int) std::pow(2, sizeof...(ActiveBits))),
			usingSubstate(true) {

	}

	template<typename... SubBits>
	auto allocateSubset(
			SubBits... subset) -> decltype(std::shared_ptr<Qubits<sizeof...(SubBits)>>()) {
		auto qubits = std::make_shared<Qubits<sizeof...(SubBits)>>(state, subset...);
		return qubits;
	}

	void applyUnitary(Eigen::MatrixXcd& U) {
		state = U * state;
	}

	QubitState& getState() {
		return state;
	}

	void setState(QubitState& st) {
		state = st;
	}

	void printState(std::ostream& stream) {
		for (int i = 0; i < state.rows(); i++) {
			stream << std::bitset<NumberOfQubits>(i).to_string() << " -> " << state(i) << "\n";
		}
	}
};
}
}
#endif
