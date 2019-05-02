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
#ifndef XACC_IR_INSTRUCTION_HPP_
#define XACC_IR_INSTRUCTION_HPP_
#include <memory>

#include "InstructionVisitor.hpp"
#include "InstructionParameter.hpp"

namespace xacc {

/**
 * The Instruction interface is the base of all
 * XACC Intermediate Representation Instructions for
 * post-Moore's law accelerated computing. The Instruction, at
 * its core, provides an Instruction name and a set of
 * next-gen bits that the Instruction operates on. Instructions
 * can also be enabled or disabled. Instructions implement
 * BaseInstructionVisitable to enable visitor pattern
 * functionality across all Instruction subclasses.
 *
 * Instruction can also expose 0 to N InstructionParameters.
 * InstructionParameters can be an int, double, float,
 * or string.
 *
 */
class Instruction : public BaseInstructionVisitable, public Identifiable {

public:
  /**
   * Persist this Instruction to an assembly-like
   * string with a given bit buffer variable name.
   *
   * @param bufferVarName The name of the AcceleratorBuffer
   * @return str The assembly-like string.
   */
  virtual const std::string toString(const std::string &bufferVarName) = 0;

  /**
   * Persist this Instruction to an assembly-like
   * string.
   *
   * @return str The assembly-like string.
   */
  virtual const std::string toString() = 0;

  /**
   * Return the indices of the bits that this Instruction
   * operates on.
   *
   * @return bits The bits this Instruction operates on.
   */
  virtual const std::vector<int> bits() = 0;
  virtual void setBits(const std::vector<int> bits) = 0;

  /**
   * Return this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter.
   * @return param The InstructionParameter at the given index.
   */
  virtual InstructionParameter getParameter(const int idx) const = 0;

  /**
   * Return all of this Instruction's parameters.
   *
   * @return params This instructions parameters.
   */
  virtual std::vector<InstructionParameter> getParameters() = 0;

  /**
   * Set this Instruction's parameter at the given index.
   *
   * @param idx The index of the parameter
   * @param inst The instruction.
   */
  virtual void setParameter(const int idx, InstructionParameter &inst) = 0;

  /**
   * Return the number of InstructionParameters this Instruction contains.
   *
   * @return nInsts The number of instructions.
   */
  virtual const int nParameters() = 0;

  /**
   * Return true if this Instruction is parameterized.
   *
   * @return parameterized True if this Instruction has parameters.
   */
  virtual bool isParameterized() { return false; }

  /**
   * Map bits [0,1,2,...] to [bitMap[0],bitMap[1],...]
   *
   * @param bitMap The bits to map to
   */
  virtual void mapBits(std::vector<int> bitMap) = 0;

  /**
   * Returns true if this Instruction is composite,
   * ie, contains other Instructions.
   *
   * @return isComposite True if this is a composite Instruction
   */
  virtual bool isComposite() { return false; }

  /**
   * Returns true if this Instruction is enabled
   *
   * @return enabled True if this Instruction is enabled.
   */
  virtual bool isEnabled() { return true; }

  /**
   * Disable this Instruction
   */

  virtual void disable() {}

  /**
   * Enable this Instruction.
   */
  virtual void enable() {}

  /**
   * Return true if this Instruction has
   * customizable options.
   *
   * @return hasOptions
   */
  virtual bool hasOptions() {return false;}

  /**
   * Set the value of an option with the given name.
   *
   * @param optName The name of the option.
   * @param option The value of the option
   */
  virtual void setOption(const std::string optName, InstructionParameter option) = 0;

  /**
   * Get the value of an option with the given name.
   *
   * @param optName Then name of the option.
   * @return option The value of the option.
   */
  virtual InstructionParameter getOption(const std::string optName) = 0;

  /**
   * Return all the Instructions options as a map.
   *
   * @return optMap The options map.
   */
  virtual std::map<std::string, InstructionParameter> getOptions() = 0;

  /**
   * Provide a mechanism for contributing custom
   * visit actions. This can be used to dynamically add
   * new visit() methods to existing InstructionVisitor
   * classes.
   *
   * @param visitor The visitor visiting this instruction.
   */
  virtual void customVisitAction(BaseInstructionVisitor& visitor) {
    /* do nothing at this level */
  }

  virtual const bool isAnalog() const { return false; }
  virtual const int nRequiredBits() const = 0;

  /**
   * The destructor
   */
  virtual ~Instruction() {}
};

} // namespace xacc
#endif
