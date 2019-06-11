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
#ifndef XACC_IR_FUNCTION_HPP_
#define XACC_IR_FUNCTION_HPP_

#include <list>

#include "Graph.hpp"
#include "Instruction.hpp"
#include "Persistable.hpp"

namespace xacc {

using InstPtr = std::shared_ptr<Instruction>;

/**
 * The Function is an Instruction that contains further
 * child Instructions. Functions, like Instructions, can be
 * parameterized. These parameters represent Function arguments.
 *
 * @author Alex McCaskey
 */
class Function : public Instruction, public Persistable {
public:

  virtual void expandIRGenerators(std::map<std::string, InstructionParameter> irGenMap) {
      return;
  }

  virtual bool hasIRGenerators() {
      return false;
  }

  /**
   * Return the number of Instructions that this Function contains.
   *
   * @return nInst The number of instructions
   */
  virtual const int nInstructions() = 0;

  virtual void setBitMap(const std::vector<int> bitMap) = 0;
  virtual const std::vector<int> getBitMap() = 0;
  virtual bool hasBeenBitMapped() {return false;}
  
  void setBits(const std::vector<int> bits) override {
      return;
  }

  /**
   * Return the Instruction at the given index.
   *
   * @param idx The desired Instruction index
   * @return inst The instruction at the given index.
   */
  virtual InstPtr getInstruction(const int idx) = 0;

  /**
   * Return all Instructions in this Function
   *
   * @return insts The list of this Function's Instructions
   */
  virtual std::list<InstPtr> getInstructions() = 0;

  /**
   * Remove the Instruction at the given index.
   *
   * @param idx The index of the Instruction to remove.
   */
  virtual void removeInstruction(const int idx) = 0;

  /**
   * Replace the Instruction at the given index with the given new Instruction.
   *
   * @param idx The index of the Instruction to replace.
   * @param newInst The new Instruction to replace with.
   */
  virtual void replaceInstruction(const int idx, InstPtr newInst) = 0;

  /**
   * Insert a new Instruction at the given index. All previous
   * instructions are pushed back, ie their new indices are
   * currentIndex + 1.
   *
   * @param idx The index where the new instruction should be inserted
   * @param newInst The new Instruction to insert.
   */
  virtual void insertInstruction(const int idx, InstPtr newInst) = 0;

  /**
   * Add an Instruction to this Function.
   *
   * @param instruction The instruction to add.
   */
  virtual void addInstruction(InstPtr instruction) = 0;

  /**
   * Add a parameter to this Function.
   *
   * @ param The parameter to add to this Function.
   */
  virtual void addParameter(InstructionParameter instParam) = 0;

  /**
   * Return the depth of this Function, applicable
   * for Functions that represent quantum circuits.
   *
   * @return depth The depth of the list of instructions.
   */
  virtual const int depth() = 0;

  /**
   * Persist this Function to a graph representation.
   *
   * @return graph The graph represented as a string, like a DOT file
   */
  virtual const std::string persistGraph() = 0;

  /**
   * Return true always to indicate that the
   * Function is composite.
   *
   * @return composite True indicating this is a composite Instruction.
   */
  bool isComposite() override { return true; }

  /**
   * Return the number of logical qubits.
   *
   * @return nLogical The number of logical qubits.
   */
  virtual const int nLogicalBits() = 0;

  /**
   * Return the number of physical qubits.
   *
   * @return nPhysical The number of physical qubits.
   */
  virtual const int nPhysicalBits() = 0;

  /**
   * Return a view of this Function that only
   * contains enabled instructions.
   *
   * @return enabledFunction The Function of all enabled instructions.
   */
  virtual std::shared_ptr<Function> enabledView() = 0;

  /**
   * Evaluate this parameterized function at the
   * given concrete parameters.
   *
   * @param params A vector of parameters
   */
  virtual std::shared_ptr<Function>
  operator()(const std::vector<double> &params) = 0;

  virtual std::shared_ptr<Graph> toGraph() = 0;

  /**
   * The destructor
   */
  virtual ~Function() {}
};

} // namespace xacc
#endif
