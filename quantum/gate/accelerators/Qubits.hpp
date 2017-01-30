#ifndef QUANTUM_GATE_QUBITS_HPP_
#define QUANTUM_GATE_QUBITS_HPP_

#include "Accelerator.hpp"
#include <complex>

namespace xacc {

namespace quantum {

using QubitState = std::vector<std::complex<double>>;

/**
 *
 */
template<const int NumberOfQubits>
class Qubits: public AcceleratorBits<NumberOfQubits> {

	QubitState state;

public:

	QubitState& getState() {
		return state;
	}
};

}

}
#endif
