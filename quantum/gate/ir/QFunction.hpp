/*
 * QFunction.hpp
 *
 *  Created on: Apr 12, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_IR_QFUNCTION_HPP_
#define QUANTUM_GATE_IR_QFUNCTION_HPP_

#include "QInstruction.hpp"

class QFunction : public virtual QInstruction {

protected:

protected:

	std::string name;

	int layer;

	std::vector<int> qbits;

	std::vector<std::shared_ptr<QInstruction>> instructions;

public:

	virtual std::string getName() {
		return name;
	}

	virtual int layer() {
		return 0;
	}

	virtual std::vector<int> qubits() {
		return qbits;
	}

	virtual void addInstruction(std::shared_ptr<QInstruction> inst) {
		instructions.push_back(inst);
	}
};

#endif /* QUANTUM_GATE_IR_QFUNCTION_HPP_ */
