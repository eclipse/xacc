/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_
#define QUANTUM_GATE_GATEQIR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include <boost/lexical_cast.hpp>
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

class GateIRProvider;

/**
 */
class GateInstruction: public virtual Instruction, public Cloneable<GateInstruction> {

protected:

	friend class GateIRProvider;

	/**
	 * Reference to this instructions name
	 */
	std::string gateName;

	/**
	 * Reference to the qubits this instruction acts on
	 */
	std::vector<int> qbits;

	bool enabled = true;

	std::vector<InstructionParameter> parameters;

	virtual void setBits(std::vector<int> bits);

public:

	GateInstruction() {}

	GateInstruction(std::string name) : gateName(name) {}
	GateInstruction(std::string name, std::vector<InstructionParameter> params) : gateName(name), parameters(params) {}

	GateInstruction(std::vector<int> qubts) :
			gateName("UNKNOWN"), qbits(qubts), parameters(std::vector<InstructionParameter>{}) {
	}

	/**
	 * The constructor, takes the id, name, layer, and qubits
	 * this instruction acts on.
	 *
	 * @param id
	 * @param layer
	 * @param name
	 * @param qubts
	 */
	GateInstruction(std::string name, std::vector<int> qubts) :
			gateName(name), qbits(qubts), parameters(std::vector<InstructionParameter>{}) {
	}

	GateInstruction(std::string name, std::vector<int> qubts,
			std::vector<InstructionParameter> params) :
			gateName(name), qbits(qubts), parameters(params) {
	}

	GateInstruction(const GateInstruction& inst) :
			gateName(inst.gateName), qbits(inst.qbits), parameters(
					inst.parameters), enabled(inst.enabled) {
	}

	/**
	 * Return the instruction name.
	 * @return
	 */
	virtual const std::string name() const;

	virtual const std::string description() const;
	/**
	 * Return the list of qubits this instruction acts on.
	 * @return
	 */
	virtual const std::vector<int> bits();

	virtual const std::string getTag();

	virtual void mapBits(std::vector<int> bitMap);

	/**
	 * Return this instruction's assembly-like string
	 * representation.
	 * @param bufferVarName
	 * @return
	 */
	virtual const std::string toString(const std::string& bufferVarName);

	virtual bool isEnabled();

	virtual void disable();

	virtual void enable();

	virtual InstructionParameter getParameter(const int idx) const;

	virtual void setParameter(const int idx, InstructionParameter& p);

	virtual std::vector<InstructionParameter> getParameters();

	virtual bool isParameterized();

	virtual const int nParameters();

	DEFINE_VISITABLE()


	/**
	 * The destructor
	 */
	virtual ~GateInstruction() {
	}
};

}
}

#endif
