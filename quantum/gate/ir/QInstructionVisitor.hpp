/*
 * QInstructionVisitor.hpp
 *
 *  Created on: Apr 12, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_IR_QINSTRUCTIONVISITOR_HPP_
#define QUANTUM_GATE_IR_QINSTRUCTIONVISITOR_HPP_

#include "QInstruction.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"

class QInstructionVisitor {
public:
	virtual void visit(QFunction* function) = 0;
	virtual void visit(Hadamard* function) = 0;
	virtual void visit(CNOT* function) = 0;
	virtual void visit(Rz* function) = 0;
	virtual ~QInstructionVisitor() {}
};



#endif /* QUANTUM_GATE_IR_QINSTRUCTIONVISITOR_HPP_ */
