/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_IR_CompositeInstruction_HPP_
#define XACC_IR_CompositeInstruction_HPP_

#include <vector>
#include <complex>
#include <set>

#include "Graph.hpp"
#include "Instruction.hpp"
#include "Persistable.hpp"

namespace xacc {

using InstPtr = std::shared_ptr<Instruction>;

// CompositeInstructions are Instructions that contain further Instructions (which
// of course can be other CompositeInstructions). This forms the familiar tree, or
// composite pattern, where nodes are CompositeInstructions and leaves are concrete
// Instruction instances. CompositeInstructions are also Persistable, meaning a
// CompositeInstruction can be persisted (read) from an output (input) stream.

// Adding Instructions:
// Implementations of CompositeInstruction are responsible for providing
// an appropriate container for Instructions. When adding parameterized
// Instructions (which may contain a string variable),
// implementations must verify that this CompositeInstruction exposes
// a variable string that corresponds to that same variable name.
// i.e. adding Ry(theta) to CompositeInstruction f, f must expose a variable
// with name theta.

// CompositeInstructions can exist without children. These are considered
// terminating nodes in Instruction tree, and represent composite
// Instructions that can not be known at compile time, but are
// parameterized by a map of options. Clients can query if the
// CompositeInstruction hasChildren(), and if not, expand the CompositeInstruction to a
// list of its constituent children at runtime given a valid
// set of options. i.e., a CompositeInstruction that can generate a UCCSD circuit
// for gate model quantum computing needs to know the number of
// qubits and number of electrons, which may be unknown at CompositeInstruction
// build time. Later, clients can invoke f->expand({{"ne",2},{"nq",4}})
// to expand this CompositeInstruction and make it a composite node instead of a
// terminating node. In this way, developers can contribute dynamic
// runtime parameterized Instructions.

// Note, expanding a CompositeInstruction node should expand all sub-nodes.

// These runtime, expanded CompositeInstructions are intended to be provided
// through the xacc ServiceRegistry.

// The operator()() evaluate method assumes that clients provide
// a vector of doubles, where the ith double corresponds to the
// ith variable.

// Oftentimes, collections of instructions may have a coefficient
// associated with them, e.g. a term in a Hamiltonian may have a weight,
// but we map that term to a set of measurement instructions. Therefore,
// we also expose a coefficient complex value on the CompositeInstruction.

class CompositeInstruction : public Instruction, public Persistable {
public:

  // This should return the number
  // of concrete leaves in the tree
  virtual const int nInstructions() = 0;

  // This should return the number of
  // direct children
  virtual const int nChildren() = 0;

  virtual InstPtr getInstruction(const std::size_t idx) = 0;
  virtual std::vector<InstPtr> getInstructions() = 0;
  virtual void removeInstruction(const std::size_t idx) = 0;
  virtual void replaceInstruction(const std::size_t idx, InstPtr newInst) = 0;
  virtual void insertInstruction(const std::size_t idx, InstPtr newInst) = 0;

  virtual void addInstruction(InstPtr instruction) = 0;
  virtual void addInstructions(std::vector<InstPtr>& instruction) = 0;
  virtual void addInstructions(const std::vector<InstPtr>& instruction) = 0;
  virtual void addInstructions(const std::vector<InstPtr> &&insts) {addInstructions(insts);}

  virtual bool hasChildren() const = 0;
  virtual bool expand(const HeterogeneousMap& runtimeOptions) = 0;
  virtual const std::vector<std::string> requiredKeys() = 0;

  virtual void addVariable(const std::string variableName) = 0;
  virtual void addVariables(const std::vector<std::string>& variables) = 0;
  virtual void addVariables(const std::vector<std::string>&& variables) {addVariables(variables);}
  virtual const std::vector<std::string> getVariables() = 0;
  virtual void replaceVariable(const std::string variable, const std::string newVariable) = 0;
  virtual const std::size_t nVariables() = 0;

  virtual const int depth() = 0;
  virtual const std::string persistGraph() = 0;
  virtual std::shared_ptr<Graph> toGraph() = 0;

  bool isComposite() override { return true; }

  virtual const std::size_t nLogicalBits() = 0;
  virtual const std::size_t nPhysicalBits() = 0;
  virtual const std::set<std::size_t> uniqueBits() = 0;

  virtual std::shared_ptr<CompositeInstruction> enabledView() = 0;

  virtual void setCoefficient(const std::complex<double> coefficient) = 0;
  virtual const std::complex<double> getCoefficient() = 0;

  virtual std::shared_ptr<CompositeInstruction>
  operator()(const std::vector<double> &params) = 0;

  virtual const std::string accelerator_signature() = 0;
  virtual void set_accelerator_signature(const std::string signature) = 0;

  virtual ~CompositeInstruction() {}
};

} // namespace xacc
#endif
