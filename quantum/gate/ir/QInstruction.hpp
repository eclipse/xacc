/*
 * QInstruction.hpp
 *
 *  Created on: Apr 12, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_IR_QINSTRUCTION_HPP_
#define QUANTUM_GATE_IR_QINSTRUCTION_HPP_

#include <string>
#include <vector>

class QInstruction {
public:
	virtual std::string getName() = 0;
	virtual int layer() = 0;
	virtual std::vector<int> qubits() = 0;
	virtual ~QInstruction() {}
};

#endif
