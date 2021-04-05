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
using InstructionParameter = Variant<int, double, std::string>;

// Util func to get a double parameter from an InstructionParameter variant.
// Note: most of the cases, we have double-type parameters (e.g. rotation
// angles). This also handles int->double and string->double conversion if
// necessary.
static double
InstructionParameterToDouble(const xacc::InstructionParameter &in_parameter) {
  if (in_parameter.which() == 0) {
    // Convert int to double
    return static_cast<double>(in_parameter.as<int>());
  } else if (in_parameter.which() == 1) {
    return in_parameter.as<double>();
  } else {
    // Check if this string parameter can be converted to a double
    const auto isNumber = [](const std::string &in_string,
                             double &out_double) -> bool {
      char *end = 0;
      double val = strtod(in_string.c_str(), &end);
      // This whole string must be a valid double number
      const bool isConversionOkay =
          (end != in_string.c_str()) && (*end == '\0') &&
          (val != std::numeric_limits<double>::infinity());
      if (isConversionOkay) {
        out_double = val;
      }

      return isConversionOkay;
    };

    const std::string paramString = in_parameter.toString();
    double paramDouble = 0.0;
    if (isNumber(paramString, paramDouble)) {
      return paramDouble;
    }
  }

  // Cannot find a way to convert, returns 0.0
  return 0.0;
}

class CompositeInstruction;

class CompositeArgument {
public:
  std::string name;
  std::string type;
  HeterogeneousMap runtimeValue;
  CompositeArgument(const std::string n, const std::string t)
      : name(n), type(t) {}
  CompositeArgument(const std::string n, const std::string t,
                    HeterogeneousMap &map)
      : name(n), type(t), runtimeValue(map) {}
  CompositeArgument(const CompositeArgument &other)
      : name(other.name), type(other.type), runtimeValue(other.runtimeValue) {}
};

const std::string INTERNAL_ARGUMENT_VALUE_KEY = "__xacc_internal_value_key";

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
protected:
  HeterogeneousMap metadata;

public:
  // Instructions can carry domain-specific metadata as a 
  // key-value dictionary of information. Here we provide 
  // a method for attaching metadata
  virtual void attachMetadata(HeterogeneousMap&& m) {
    metadata = m;
  }
  // Retrieve this intruction's metadata. 
  virtual HeterogeneousMap getMetadata() {return metadata;}

  virtual void applyRuntimeArguments() = 0;
  virtual void addArgument(std::shared_ptr<CompositeArgument> arg,
                           const int idx_of_inst_param) = 0;

  // This method is a helper for associating InstructionParameter indices 
  // to other indices like std::vector<double> indices. Does nothing here
  // to be implemented by subclasses.
  virtual void addIndexMapping(const int idx_1, const int idx_2) {
      return;
  }
  virtual const std::string toString() = 0;

  virtual const std::vector<std::size_t> bits() = 0;
  virtual void setBits(const std::vector<std::size_t> bits) = 0;
  virtual void mapBits(std::vector<std::size_t> bitMap) = 0;

  // For the case where the bit indices for this Instruction are
  // some general expression, Clients should set the bit to -1 and provide
  // the bit expression as a string. The example here would be
  // H(q[i]) (bit_idx=0, expr=i), or C(q[i],q[i+1]) (bit_idx=0, expr=i and
  // bit_idx=1,expr=i+1)
  virtual void setBitExpression(const std::size_t bit_idx,
                                const std::string expr) = 0;
  virtual std::string getBitExpression(const std::size_t bit_idx) = 0;

  // Return the name of the AcceleratorBuffer that this
  // Instruction operates on at the given bitIdx. E.g.
  // CX(q[0], r[1]), this would return 'q' for bit 0 and 'r'
  // for bit 1
  // Additionally, set enables one to provide these buffer names
  // input vector should be same size as bits()
  virtual std::string getBufferName(const std::size_t bitIdx) = 0;
  virtual std::vector<std::string> getBufferNames() = 0;
  virtual void
  setBufferNames(const std::vector<std::string> bufferNamesPerIdx) = 0;

  virtual const InstructionParameter
  getParameter(const std::size_t idx) const = 0;
  virtual std::vector<InstructionParameter> getParameters() = 0;

  virtual void setParameter(const std::size_t idx,
                            InstructionParameter &inst) = 0;
  virtual void setParameter(const std::size_t idx,
                            InstructionParameter &&inst) {
    setParameter(idx, inst);
  }

  virtual const int nParameters() = 0;
  virtual bool isParameterized() { return false; }

  virtual bool isComposite() { return false; }

  // The following accomadate pulse-level instructions
  virtual std::string channel() { return "default"; }
  virtual void setChannel(const std::string ch) { return; }
  virtual std::size_t start() { return 0; }
  virtual void setStart(const std::size_t s) { return; }
  virtual std::size_t duration() { return 0; }
  virtual void setDuration(const std::size_t d) { return; }
  virtual void setSamples(const std::vector<std::vector<double>> samples) {
    return;
  }
  virtual std::vector<std::vector<double>> getSamples() { return {}; }
  virtual void setPulseParams(const HeterogeneousMap &in_pulseParams) {}
  virtual HeterogeneousMap getPulseParams() { return {}; }

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
