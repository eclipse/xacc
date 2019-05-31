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
#ifndef QUANTUM_GATE_DIGITALGATES_HPP_
#define QUANTUM_GATE_DIGITALGATES_HPP_

#include "GateInstruction.hpp"
#include "GateFunction.hpp"

namespace xacc {
namespace quantum {

class Identity : public GateInstruction {
public:
  Identity() : GateInstruction("I") {}
  Identity(std::vector<int> qbits) : GateInstruction("I", qbits) {}
  Identity(int qbit) : Identity(std::vector<int>{qbit}) {}

  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Identity)
  DEFINE_VISITABLE()
};

class Hadamard : public GateInstruction {
public:
  Hadamard() : GateInstruction("H") {}
  Hadamard(std::vector<int> qbits) : GateInstruction("H", qbits) {}
  Hadamard(int qbit) : Hadamard(std::vector<int>{qbit}) {}

  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Hadamard)
  DEFINE_VISITABLE()
};

class CNOT : public GateInstruction {
public:

  CNOT() : GateInstruction("CNOT") {}
  CNOT(std::vector<int> qbits) : GateInstruction("CNOT", qbits) {}
  CNOT(int srcqbit, int tgtqbit) : CNOT(std::vector<int>{srcqbit, tgtqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 2;}

  DEFINE_CLONE(CNOT)
  DEFINE_VISITABLE()
};

class Rx : public GateInstruction {
public:
  Rx()
      : GateInstruction("Rx", std::vector<InstructionParameter>{
                                  InstructionParameter(0.0)}) {}
  Rx(int qbit, double theta)
      : GateInstruction(
            "Rx", std::vector<int>{qbit},
            std::vector<InstructionParameter>{InstructionParameter(theta)}) {}
  Rx(std::vector<int> qbits)
      : GateInstruction(
            "Rx", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Rx)
  DEFINE_VISITABLE()
};

class Ry : public GateInstruction {
public:
  Ry()
      : GateInstruction("Ry", std::vector<InstructionParameter>{
                                  InstructionParameter(0.0)}) {}

  Ry(int qbit, double theta)
      : GateInstruction(
            "Ry", std::vector<int>{qbit},
            std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

  Ry(std::vector<int> qbits)
      : GateInstruction(
            "Ry", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Ry)
  DEFINE_VISITABLE()
};

class Rz : public GateInstruction {
public:
  Rz()
      : GateInstruction("Rz", std::vector<InstructionParameter>{
                                  InstructionParameter(0.0)}) {}

  Rz(int qbit, double theta)
      : GateInstruction(
            "Rz", std::vector<int>{qbit},
            std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

  Rz(std::vector<int> qbits)
      : GateInstruction(
            "Rz", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Rz)
  DEFINE_VISITABLE()
};

class Swap : public GateInstruction {
public:
  Swap() : GateInstruction("Swap") {}

  Swap(int controlQubit, int targetQubit)
      : GateInstruction("Swap", std::vector<int>{controlQubit, targetQubit},
                        std::vector<InstructionParameter>{}) {}

  Swap(std::vector<int> qbits)
      : GateInstruction("Swap", qbits, std::vector<InstructionParameter>{}) {}
  const int nRequiredBits() const override {return 2;}

  DEFINE_CLONE(Swap)
  DEFINE_VISITABLE()
};

class U : public GateInstruction {
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
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(U)
  DEFINE_VISITABLE()
};

class X : public GateInstruction {
public:
  X() : GateInstruction("X") {}
  X(std::vector<int> qbit) : GateInstruction("X", qbit) {}
  X(int qbit) : X(std::vector<int>{qbit}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(X)
  DEFINE_VISITABLE()
};

class Y : public GateInstruction {
public:
  Y() : GateInstruction("Y") {}
  Y(std::vector<int> qbit) : GateInstruction("Y", qbit) {}
  Y(int qbit) : Y(std::vector<int>{qbit}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Y)
  DEFINE_VISITABLE()
};

class Z : public GateInstruction {
public:
  Z() : GateInstruction("Z") {}
  Z(std::vector<int> qbit) : GateInstruction("Z", qbit) {}
  Z(int qbit) : Z(std::vector<int>{qbit}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Z)
  DEFINE_VISITABLE()
};

class Measure : public GateInstruction {
public:
  Measure()
      : GateInstruction("Measure", std::vector<InstructionParameter>{
                                       InstructionParameter(0)}) {}
  Measure(std::vector<int> qbit)
      : GateInstruction(
            "Measure", qbit,
            std::vector<InstructionParameter>{InstructionParameter(0)}) {}
  Measure(int qbit, int classicalIdx)
      : GateInstruction("Measure", std::vector<int>{qbit},
                        std::vector<InstructionParameter>{
                            InstructionParameter(classicalIdx)}) {}

  const std::string toString(const std::string &bufferVarName) override {
    return gateName + " " + bufferVarName + std::to_string(bits()[0]);
  }

  int getClassicalBitIndex() { return mpark::get<int>(parameters[0]); }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Measure)
  DEFINE_VISITABLE()
};

class CZ : public virtual GateInstruction {
public:
  CZ() : GateInstruction("CZ") {}
  CZ(std::vector<int> qbits) : GateInstruction("CZ", qbits) {}
  CZ(int srcqbit, int tgtqbit) : CZ(std::vector<int>{srcqbit, tgtqbit}) {}

  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(CZ)
  DEFINE_VISITABLE()
};

class CPhase : public virtual GateInstruction {
public:
  CPhase()
      : GateInstruction("CPhase", std::vector<InstructionParameter>{
                                      InstructionParameter(0.0)}) {}
  CPhase(int controlQubit, int targetQubit, double theta)
      : GateInstruction(
            "CPhase", std::vector<int>{controlQubit, targetQubit},
            std::vector<InstructionParameter>{InstructionParameter(theta)}) {}
  CPhase(std::vector<int> qbits)
      : GateInstruction(
            "CPhase", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(CPhase)
  DEFINE_VISITABLE()
};

class S : public GateInstruction {
public:
  S() : GateInstruction("S") {}
  S(std::vector<int> qbits) : GateInstruction("S", qbits) {}
  S(int qbit) : S(std::vector<int>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(S)
  DEFINE_VISITABLE()
};

class Sdg : public GateInstruction {
public:

  Sdg() : GateInstruction("Sdg") {}
  Sdg(std::vector<int> qbits) : GateInstruction("Sdg", qbits) {}
  Sdg(int qbit) : Sdg(std::vector<int>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Sdg)
  DEFINE_VISITABLE()
};

class T : public GateInstruction {
public:

  T() : GateInstruction("T") {}
  T(std::vector<int> qbits) : GateInstruction("T", qbits) {}
  T(int qbit) : T(std::vector<int>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(T)
  DEFINE_VISITABLE()
};

class Tdg : public GateInstruction {
public:

  Tdg() : GateInstruction("Tdg") {}
  Tdg(std::vector<int> qbits) : GateInstruction("Tdg", qbits) {}
  Tdg(int qbit) : Tdg(std::vector<int>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(Tdg)
  DEFINE_VISITABLE()
};

class CY : public GateInstruction {
public:

  CY() : GateInstruction("CY") {}
  CY(std::vector<int> qbits) : GateInstruction("CY", qbits) {}
  CY(int sqbit, int tqbit) : CY(std::vector<int>{sqbit,tqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 2;}

  DEFINE_CLONE(CY)
  DEFINE_VISITABLE()
};

class CH : public GateInstruction {
public:

  CH() : GateInstruction("CH") {}
  CH(std::vector<int> qbits) : GateInstruction("CH", qbits) {}
  CH(int sqbit, int tqbit) : CH(std::vector<int>{sqbit,tqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(CH)
  DEFINE_VISITABLE()
};

class CRZ : public virtual GateInstruction {
public:
  CRZ()
      : GateInstruction("CRZ", std::vector<InstructionParameter>{
                                  InstructionParameter(0.0)}) {}

  CRZ(int qbit, double theta)
      : GateInstruction(
            "CRZ", std::vector<int>{qbit},
            std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

  CRZ(std::vector<int> qbits)
      : GateInstruction(
            "CRZ", qbits,
            std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override {return 1;}

  DEFINE_CLONE(CRZ)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc
#endif
