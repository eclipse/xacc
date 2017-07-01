#ifndef QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_

#include "AlgorithmGenerator.hpp"

namespace xacc{
namespace quantum {
class InverseQFT : public AlgorithmGenerator {
public:

	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits);

	virtual ~InverseQFT() {}
};
}
}


#endif
