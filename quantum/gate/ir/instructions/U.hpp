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
#ifndef QUANTUM_GATE_GATEQIR_INSTRUCTIONS_U1_HPP_
#define QUANTUM_GATE_GATEQIR_INSTRUCTIONS_U1_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {
class U : public virtual GateInstruction {
public:
  U()
      : GateInstruction("U", std::vector<InstructionParameter>{
                                  InstructionParameter(0.0), InstructionParameter(0.0), InstructionParameter(0.0)}) {}

  U(int qbit, double theta, double phi, double lambda)
      : GateInstruction(
            "U", std::vector<int>{qbit},
            std::vector<InstructionParameter>{InstructionParameter(theta), InstructionParameter(phi), InstructionParameter(lambda)}) {}

  U(std::vector<int> qbits)
      : GateInstruction(
            "U", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0), InstructionParameter(0.0), InstructionParameter(0.0)}) {}

  std::shared_ptr<GateInstruction> clone() override {
    return std::make_shared<U>();
  }

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  const std::string description() const override { return ""; }

  DEFINE_VISITABLE()
};

// class U2 : public virtual GateInstruction {
// public:
//   U2()
//       : GateInstruction("U2", std::vector<InstructionParameter>{
//                                   InstructionParameter(0.0), InstructionParameter(0.0)}) {}

//   U2(int qbit, double phi, double lambda)
//       : GateInstruction(
//             "U2", std::vector<int>{qbit},
//             std::vector<InstructionParameter>{InstructionParameter(phi), InstructionParameter(lambda)}) {}

//   U2(std::vector<int> qbits)
//       : GateInstruction(
//             "U2", qbits,
//             std::vector<InstructionParameter>{InstructionParameter(0.0), InstructionParameter(0.0)}) {}

//   std::shared_ptr<GateInstruction> clone() override {
//     return std::make_shared<U2>();
//   }

//   /**
//    * Return the description of this instance
//    * @return description The description of this object.
//    */
//   const std::string description() const override { return ""; }

//   DEFINE_VISITABLE()
// };

// class U1 : public virtual GateInstruction {
// public:
//   U1()
//       : GateInstruction("U1", std::vector<InstructionParameter>{
//                                   InstructionParameter(0.0)}) {}

//   U1(int qbit, double theta)
//       : GateInstruction(
//             "U1", std::vector<int>{qbit},
//             std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

//   U1(std::vector<int> qbits)
//       : GateInstruction(
//             "U1", qbits,
//             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

//   std::shared_ptr<GateInstruction> clone() override {
//     return std::make_shared<U1>();
//   }

//   /**
//    * Return the description of this instance
//    * @return description The description of this object.
//    */
//   const std::string description() const override { return ""; }

//   DEFINE_VISITABLE()
// };
} // namespace quantum
} // namespace xacc

#endif
