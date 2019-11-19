/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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

#include "Cloneable.hpp"
#include "InstructionVisitor.hpp"
#include "heterogeneous.hpp"

namespace xacc {
using InstructionParameter = Variant<int,double,std::string>;
class CompositeInstruction;

// The Instruction interface exposes an API for describing a general
// post-Moore's law low-level assembly instruction. Each Instruction
// exposes a unique string name, the vector of bit indices that it operates on,
// and a vector of InstructionParameters describing any runtime
// parameters (like rotation angle for a gate-model quantum computing
// instruction) that it requires. Clients can get the name of the
// Instruction, get and set the bits it operates on, and get and set
// any of the InstructionParameters it exposes.
//
// Instructions can be enabled or disabled, and expose whether
// it is composite or not (see CompositeInstruction).
//
// Instructions are Cloneable, which is required for use with the
// xacc service registry. Therefore, all Instructions are meant to
// be provided services within the XACC framework, making it easy
// to inject new instructions to an existing framework install.
//
// Instruction can be visited, enabling type-specific information
// at runtime to custom InstructionVisitors.
class Instruction : public BaseInstructionVisitable,
                    public Identifiable,
                    public Cloneable<Instruction> {

public:
  virtual const std::string toString() = 0;

  virtual const std::vector<std::size_t> bits() = 0;
  virtual void setBits(const std::vector<std::size_t> bits) = 0;
  virtual void mapBits(std::vector<std::size_t> bitMap) = 0;

  virtual const InstructionParameter getParameter(const std::size_t idx) const = 0;
  virtual std::vector<InstructionParameter> getParameters() = 0;

  virtual void setParameter(const std::size_t idx, InstructionParameter &inst) = 0;
  virtual void setParameter(const std::size_t idx, InstructionParameter &&inst) {
    setParameter(idx, inst);
  }

  virtual const int nParameters() = 0;
  virtual bool isParameterized() { return false; }

  virtual bool isComposite() { return false; }

  // The following accomadate pulse-level instructions
  virtual std::string channel() {return "default";}
  virtual void setChannel(const std::string ch) {return;}
  virtual std::size_t start() {return 0;}
  virtual void setStart(const std::size_t s) { return; }
  virtual std::size_t duration() { return 0; }
  virtual void setDuration(const std::size_t d) { return; }
  virtual void setSamples(const std::vector<std::vector<double>> samples) {return;}
  virtual std::vector<std::vector<double>> getSamples() {return {};}

  // The following accomodate conditional instructions
  virtual bool isEnabled() { return true; }
  virtual void disable() {}
  virtual void enable() {}

  virtual void customVisitAction(BaseInstructionVisitor &visitor) {
    /* do nothing at this level */
  }

  virtual const bool isAnalog() const { return false; }
  virtual const int nRequiredBits() const = 0;

  virtual ~Instruction() {}
};

} // namespace xacc
#endif
