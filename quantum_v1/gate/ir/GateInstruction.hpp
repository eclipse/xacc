/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_IR_GATEINSTRUCTION_HPP_
#define QUANTUM_GATE_IR_GATEINSTRUCTION_HPP_

#include "Instruction.hpp"
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {

class GateIRProvider;

/**
 */
class GateInstruction : public Instruction, public Cloneable<GateInstruction> {

protected:
  /**
   * Reference to this instructions name
   */
  std::string gateName;

  /**
   * Reference to the qubits this instruction acts on
   */
  std::vector<int> qbits;

  /**
   * Reference to enabled or disabled instruction state.
   */
  bool enabled = true;

  /**
   * Reference to this Instructions vector of
   * parameters.
   */
  std::vector<InstructionParameter> parameters;

  /**
   * Reference to this Instruction's set of options.
   */
  std::map<std::string, InstructionParameter> options;
  
  /**
   * This method is intended for subclasses. It is
   * a protected method that will be called at construction
   * to describe this Instructions available options map.
   */
  virtual void describeOptions() {}

public:
  /**
   * Nullary constructor
   */
  GateInstruction();

  /**
   * Constructor, construct with gate name
   */
  GateInstruction(std::string name);

  /**
   * Constructor, construct with gate name and instruction parameters.
   */
  GateInstruction(std::string name, std::vector<InstructionParameter> params);

  /**
   * The constructor, takes the name and qubits
   * this instruction acts on.
   */
  GateInstruction(std::string name, std::vector<int> qubts);

  /**
   * The constructor, takes the name, bits operated on, and parameters.
   */
  GateInstruction(std::string name, std::vector<int> qubts,
                  std::vector<InstructionParameter> params);

  /**
   * The copy constructor
   */
  GateInstruction(const GateInstruction &inst);
  void setBits(const std::vector<int> bits) override { qbits = bits; }

  /**
   * Return the instruction name.
   *
   * @return
   */
  const std::string name() const override;

  /**
   * Return this instruction's description.
   *
   * @return desc The description.
   */
  const std::string description() const override;

  /**
   * Return the list of qubits this instruction acts on.
   *
   * @return bits The qubits this instruction operates on.
   */
  const std::vector<int> bits() override;

  /**
   * Map this Instruction's qubit indices to the
   * given physical qubits.
   *
   * @param bitMap The physical bits to map to.
   */
  void mapBits(std::vector<int> bitMap) override;

  /**
   * Return this instruction's assembly-like string
   * representation.
   *
   * @return qasm The instruction as qasm
   */
  const std::string toString() override;

  /**
   * Return this instruction's assembly-like string
   * representation.

   * @param bufferVarName The qubit register name
   * @return qasm The instruction as qasm
   */
  const std::string toString(const std::string &bufferVarName) override;

  /**
   * Return true if this Instruction is enabled.
   *
   * @return enabled True if this Instruction is enabled.
   */
  bool isEnabled() override;

  /**
   * Disable this Instruction.
   */
  void disable() override;

  /**
   * Enable this Instruction.
   */
  void enable() override;

  /**
   * Return the parameter at the given index.
   *
   * @param idx The parameter index
   * @return param The parameter.
   */
  InstructionParameter getParameter(const int idx) const override;

  /**
   * Set the parameter at the given index.
   *
   * @param idx The parameter index
   * @param param The parameter.
   */
  void setParameter(const int idx, InstructionParameter &p) override;

  /**
   * Return this Instructions Parameters.
   *
   * @return params The Instruction Parameters.
   */
  std::vector<InstructionParameter> getParameters() override;

  /**
   * Return true if this Instruction is parameterized.
   *
   * @return isParameterized True if parameterized.
   */
  bool isParameterized() override;

  /**
   * Return the number of parameters this Instruction has.
   *
   * @return nParams The number of parameters.
   */
  const int nParameters() override;

  /**
   * Return true if this Instruction has
   * customizable options.
   *
   * @return hasOptions
   */
  bool hasOptions() override;

  /**
   * Set the value of an option with the given name.
   *
   * @param optName The name of the option.
   * @param option The value of the option
   */
  void setOption(const std::string optName,
                 InstructionParameter option) override;
  /**
   * Get the value of an option with the given name.
   *
   * @param optName Then name of the option.
   * @return option The value of the option.
   */
  InstructionParameter getOption(const std::string optName) override;

  /**
   * Return all the Instructions options as a map.
   *
   * @return optMap The options map.
   */
  std::map<std::string, InstructionParameter> getOptions() override;

  /**
   * This configures this Instruction with the appropriate
   * accept() method for the XACC IR InstructionVisitor pattern.
   */
  DEFINE_VISITABLE()

  /**
   * The destructor
   */
  virtual ~GateInstruction() {}
};

#define DEFINE_CLONE(CLASS)                                      \
  std::shared_ptr<GateInstruction> clone() override {            \
    return std::make_shared<CLASS>();                           \
  }

} // namespace quantum
} // namespace xacc

#endif
