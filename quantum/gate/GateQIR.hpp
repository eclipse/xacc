/*
 * GateQIR.hpp
 *
 *  Created on: Apr 12, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_GATEQIR_HPP_
#define QUANTUM_GATE_GATEQIR_HPP_


#include "QIR.hpp"
#include "QuantumCircuit.hpp"

class GateQIR : public virtual QIR<xacc::quantum::CircuitNode> {

protected:

	std::vector<QInstruction> instructions;

	virtual ~GateQIR() {}
};


#endif
