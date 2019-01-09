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
#ifndef QUANTUM_AQC_IR_DWQMI_HPP_
#define QUANTUM_AQC_IR_DWQMI_HPP_

#include "Instruction.hpp"

namespace xacc {

namespace quantum {

/**
 * The DWQMI (D=Wave Quantum Machine Instruction) class is an XACC
 * Instruction that models a logical problem or hardware bias or
 * connection for an optimization problem to be solved on
 * the D-Wave QPU. It keeps track of 2 bits indices, if both
 * are the same index then this DWQMI Instruction models
 * a bias value, and if they are different indices,
 * then this Instruction models a logical coupling.
 *
 * Note that this class can model both problem bias/couplings and
 * hardware bias/couplings. The hardware bias/couplings result from
 * a minor graph embedding computation.
 *
 */
class DWQMI : public Instruction {

protected:
  /**
   * The qubits involved in this Instruction
   */
  std::vector<int> qubits;

  /**
   * The bias or coupling value.
   */
  InstructionParameter parameter;

  /**
   * Is this Instruction enabled or disabled.
   *
   */
  bool enabled = true;

public:
  /**
   * The Constructor, takes one qubit
   * indicating this is a bias value, initialized to 0.0
   *
   * @param qbit The bit index
   */
  DWQMI(int qbit) : qubits(std::vector<int>{qbit, qbit}), parameter(0.0) {}

  /**
   * The Constructor, takes one qubit
   * indicating this is a bias value, but
   * set to the provided bias.
   *
   * @param qbit The bit index
   * @param param The bias value
   */
  DWQMI(int qbit, double param)
      : qubits(std::vector<int>{qbit, qbit}), parameter(param) {}

  /**
   * The Constructor, takes two qubit indices
   * to indicate that this is a coupler, with
   * value given by the provided parameter.
   *
   * @param qbit1 The bit index
   * @param qbit2 The bit index
   * @param param The coupling weight
   */
  DWQMI(int qbit1, int qbit2, double param)
      : qubits(std::vector<int>{qbit1, qbit2}), parameter(param) {}

  DWQMI(int qbit1, int qbit2, InstructionParameter param)
      : qubits(std::vector<int>{qbit1, qbit2}), parameter(param) {}

  DWQMI(std::vector<int> bits, InstructionParameter param)
      : qubits(bits), parameter(param) {}
  /**
   * Return the name of this Instruction
   *
   * @return name The name of this Instruction
   */
  const std::string name() const override { return "dw-qmi"; }

  const std::string description() const override { return ""; }

  void mapBits(std::vector<int> bitMap) override {}
  const int nRequiredBits() const override {return 2;}

  /**
   * Persist this Instruction to an assembly-like
   * string.
   *
   * @param bufferVarName The name of the AcceleratorBuffer
   * @return str The assembly-like string.
   */
  const std::string toString(const std::string &bufferVarName) override {
    std::stringstream ss;
    ss << bits()[0] << " " << bits()[1] << " " << getParameter(0).toString();
    return ss.str();
  }

  const std::string toString() override {
      return toString("");
  }

  /**
   * Return the indices of the bits that this Instruction
   * operates on.
   *
   * @return bits The bits this Instruction operates on.
   */
  const std::vector<int> bits() override { return qubits; }

  /**
   * Return this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter.
   * @return param The InstructionParameter at the given index.
   */
  InstructionParameter getParameter(const int idx) const override {
    return parameter;
  }

  /**
   * Return all of this Instruction's parameters.
   *
   * @return params This instructions parameters.
   */
  std::vector<InstructionParameter> getParameters() override {
    return std::vector<InstructionParameter>{parameter};
  }

  /**
   * Set this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter
   * @param inst The instruction.
   */
  void setParameter(const int idx, InstructionParameter &inst) override {
    parameter = inst;
  }

  /**
   * Return the number of InstructionParameters this Instruction contains.
   *
   * @return nInsts The number of instructions.
   */
  const int nParameters() override { return 1; }

  /**
   * Return true if this Instruction is parameterized.
   *
   * @return parameterized True if this Instruction has parameters.
   */
  bool isParameterized() override { return true; }
  
  /**
   * Returns true if this Instruction is composite,
   * ie, contains other Instructions.
   *
   * @return isComposite True if this is a composite Instruction
   */
  bool isComposite() override { return false; }

  /**
   * Returns true if this Instruction is enabled
   *
   * @return enabled True if this Instruction is enabled.
   */
  bool isEnabled() override { return enabled; }

  /**
   * Disable this Instruction
   */

  void disable() override { enabled = false; }

  /**
   * Enable this Instruction.
   */
  void enable() override { enabled = true; }

  /**
   * Return true if this Instruction has
   * customizable options.
   *
   * @return hasOptions
   */
  bool hasOptions() override {
      return false;
  }

  /**
   * Set the value of an option with the given name.
   *
   * @param optName The name of the option.
   * @param option The value of the option
   */
  void setOption(const std::string optName,
                 InstructionParameter option) override {
      XACCLogger::instance()->error("setOption not implemented for DWQMI."); 
      return;              
  }
  
  /**
   * Get the value of an option with the given name.
   *
   * @param optName Then name of the option.
   * @return option The value of the option.
   */
  InstructionParameter getOption(const std::string optName) override {
       XACCLogger::instance()->error("getOption not implemented for DWQMI.");  
       return InstructionParameter(0);             
  }

  /**
   * Return all the Instructions options as a map.
   *
   * @return optMap The options map.
   */
  std::map<std::string, InstructionParameter> getOptions() override {
       XACCLogger::instance()->error("getOptions not implemented for DWQMI."); 
       return std::map<std::string,InstructionParameter>();              
  }

  EMPTY_DEFINE_VISITABLE()
};

/**
 * The DWQMI (D=Wave Quantum Machine Instruction) class is an XACC
 * Instruction that models a logical problem or hardware bias or
 * connection for an optimization problem to be solved on
 * the D-Wave QPU. It keeps track of 2 bits indices, if both
 * are the same index then this DWQMI Instruction models
 * a bias value, and if they are different indices,
 * then this Instruction models a logical coupling.
 *
 * Note that this class can model both problem bias/couplings and
 * hardware bias/couplings. The hardware bias/couplings result from
 * a minor graph embedding computation.
 *
 */
class Anneal : public Instruction {
protected:
  /**
   * The initial ramp up time
   */
  std::vector<InstructionParameter> times;

  /**
   * Is this Instruction enabled or disabled.
   *
   */
  bool enabled = true;

public:
  /**
   * The Constructor, takes the annealing times
   *
   * @param qbit The bit index
   */
  Anneal(InstructionParameter _ts, InstructionParameter _tp,
         InstructionParameter _tq, InstructionParameter direction)
      : times({_ts, _tp, _tq, direction}) {}

  Anneal(std::vector<InstructionParameter> p) : times(p) {
    if (times.size() != 4)
      XACCLogger::instance()->error(
          "Invalid number of instruction parameters for Anneal Instruction, " +
          std::to_string(times.size()));
  }

  /**
   * Return the name of this Instruction
   *
   * @return name The name of this Instruction
   */
  const std::string name() const override { return "anneal"; }

  const std::string description() const override {
    return "A description of the annealing schedule";
  }

  void mapBits(std::vector<int> bitMap) override {}
  const int nRequiredBits() const override {return 0;}

  /**
   * Persist this Instruction to an assembly-like
   * string.
   *
   * @param bufferVarName The name of the AcceleratorBuffer
   * @return str The assembly-like string.
   */
  const std::string toString(const std::string &bufferVarName) override {
    std::stringstream ss;
    ss << "anneal ta = " << times[0].toString() << ", tp = " << times[1].toString()
       << ", tq = " << times[2].toString() << ", ";
    if (times.size() > 3) {
      ss << times[3].toString();
    } else {
      ss << "forward";
    }
    return ss.str();
  }

  const std::string toString() override {
      return toString("");
  }
  /**
   * Return the indices of the bits that this Instruction
   * operates on.
   *
   * @return bits The bits this Instruction operates on.
   */
  const std::vector<int> bits() override { return std::vector<int>{}; }

  /**
   * Return this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter.
   * @return param The InstructionParameter at the given index.
   */
  InstructionParameter getParameter(const int idx) const override {
    return times[idx];
  }

  /**
   * Return all of this Instruction's parameters.
   *
   * @return params This instructions parameters.
   */
  std::vector<InstructionParameter> getParameters() override { return times; }

  /**
   * Set this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter
   * @param inst The instruction.
   */
  void setParameter(const int idx, InstructionParameter &inst) override {
    times.at(idx) = inst;
  }

  /**
   * Return the number of InstructionParameters this Instruction contains.
   *
   * @return nInsts The number of instructions.
   */
  const int nParameters() override { return 4; }

  /**
   * Return true if this Instruction is parameterized.
   *
   * @return parameterized True if this Instruction has parameters.
   */
  bool isParameterized() override { return true; }
  /**
   * Returns true if this Instruction is composite,
   * ie, contains other Instructions.
   *
   * @return isComposite True if this is a composite Instruction
   */
  bool isComposite() override { return false; }

  /**
   * Returns true if this Instruction is enabled
   *
   * @return enabled True if this Instruction is enabled.
   */
  bool isEnabled() override { return enabled; }

  /**
   * Disable this Instruction
   */

  void disable() override { enabled = false; }

  /**
   * Enable this Instruction.
   */
  void enable() override { enabled = true; }

 /**
   * Return true if this Instruction has
   * customizable options.
   *
   * @return hasOptions
   */
  bool hasOptions() override {
      return false;
  }

  /**
   * Set the value of an option with the given name.
   *
   * @param optName The name of the option.
   * @param option The value of the option
   */
  void setOption(const std::string optName,
                 InstructionParameter option) override {
      XACCLogger::instance()->error("setOption not implemented for Anneal."); 
      return;              
  }
  
  /**
   * Get the value of an option with the given name.
   *
   * @param optName Then name of the option.
   * @return option The value of the option.
   */
  InstructionParameter getOption(const std::string optName) override {
       XACCLogger::instance()->error("getOption not implemented for Anneal.");  
       return InstructionParameter(0);             
  }

  /**
   * Return all the Instructions options as a map.
   *
   * @return optMap The options map.
   */
  std::map<std::string, InstructionParameter> getOptions() override {
       XACCLogger::instance()->error("getOptions not implemented for Anneal."); 
       return std::map<std::string,InstructionParameter>();              
  }

  EMPTY_DEFINE_VISITABLE()
};

} // namespace quantum

} // namespace xacc

#endif
