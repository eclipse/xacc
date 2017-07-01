/*
 * QFT.hpp
 *
 *  Created on: Jul 1, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_

#include "AlgorithmGenerator.hpp"

namespace xacc{
namespace quantum {
class QFT : public AlgorithmGenerator {
public:

	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> qubits);

	virtual ~QFT() {}
};
}
}


#endif
