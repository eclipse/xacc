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

public:
	Qubits() :
			state((int) std::pow(2, NumberOfQubits)) {
	}

	void applyUnitary(Eigen::MatrixXcd& U) {
		state = U * state;
	}

	void printState(std::ostream& stream) {
		stream << state << "\n";
	}
};
}
}
#endif
