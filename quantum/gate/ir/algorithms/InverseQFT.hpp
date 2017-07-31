#ifndef QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_

#include "AlgorithmGenerator.hpp"

namespace xacc{
namespace quantum {

/**
 * InverseQFT is a realization of the AlgorithmGenerator
 * interface that produces an XACC IR representation
 * of the Inverse Quantum Fourier Transform.
 */
class InverseQFT : public AlgorithmGenerator {
public:

	/**
	 * This implementation returns a Function IR representation
	 * of the inverse quantum fourier transform.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits);

	virtual const std::string name() const {
		return "InverseQFT";
	}

	virtual const std::string description() const {
		return "The Inverse QFT Algorithm Generator generates IR representing "
				"an inverse Quantum Fourier Transform.";
	}

	/**
	 * The destructor
	 */
	virtual ~InverseQFT() {}
};
}
}


#endif
