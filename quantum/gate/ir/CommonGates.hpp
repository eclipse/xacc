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
#ifndef QUANTUM_GATE_COMMONGATES_HPP_
#define QUANTUM_GATE_COMMONGATES_HPP_

#include "Gate.hpp"

namespace xacc {
namespace quantum {

class Identity : public Gate {
public:
  Identity() : Gate("I") {}
  Identity(std::vector<std::size_t> qbits) : Gate("I", qbits) {}
  Identity(std::size_t qbit) : Identity(std::vector<std::size_t>{qbit}) {}

  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Identity)
  DEFINE_VISITABLE()
};

class Hadamard : public Gate {
public:
  Hadamard() : Gate("H") {}
  Hadamard(std::vector<std::size_t> qbits) : Gate("H", qbits) {}
  Hadamard(std::size_t qbit) : Hadamard(std::vector<std::size_t>{qbit}) {}

  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Hadamard)
  DEFINE_VISITABLE()
};

class CNOT : public Gate {
public:
  CNOT() : Gate("CNOT") {}
  CNOT(std::vector<std::size_t> qbits) : Gate("CNOT", qbits) {}
  CNOT(std::size_t srcqbit, std::size_t tgtqbit)
      : CNOT(std::vector<std::size_t>{srcqbit, tgtqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CNOT)
  DEFINE_VISITABLE()
};

class Rx : public Gate {
public:
  Rx()
      : Gate("Rx",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  Rx(std::size_t qbit, InstructionParameter &&theta)
      : Gate("Rx", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{theta}) {}
  Rx(std::vector<std::size_t> qbits)
      : Gate("Rx", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Rx)
  DEFINE_VISITABLE()
};

class Ry : public Gate {
public:
  Ry()
      : Gate("Ry",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  Ry(std::size_t qbit, InstructionParameter &&theta)
      : Gate("Ry", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{theta}) {}

  Ry(std::vector<std::size_t> qbits)
      : Gate("Ry", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Ry)
  DEFINE_VISITABLE()
};

class Rz : public Gate {
public:
  Rz()
      : Gate("Rz",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  Rz(std::size_t qbit, InstructionParameter &&theta)
      : Gate("Rz", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{theta}) {}

  Rz(std::vector<std::size_t> qbits)
      : Gate("Rz", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Rz)
  DEFINE_VISITABLE()
};

class Swap : public Gate {
public:
  Swap() : Gate("Swap") {}

  Swap(std::size_t controlQubit, std::size_t targetQubit)
      : Gate("Swap", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{}) {}

  Swap(std::vector<std::size_t> qbits)
      : Gate("Swap", qbits, std::vector<InstructionParameter>{}) {}
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(Swap)
  DEFINE_VISITABLE()
};

class U : public Gate {
public:
  U()
      : Gate("U", std::vector<InstructionParameter>{
                      InstructionParameter(0.0), InstructionParameter(0.0),
                      InstructionParameter(0.0)}) {}

  U(std::size_t qbit, double theta, double phi, double lambda)
      : Gate("U", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{InstructionParameter(theta),
                                               InstructionParameter(phi),
                                               InstructionParameter(lambda)}) {}

  U(std::vector<std::size_t> qbits)
      : Gate("U", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0),
                                               InstructionParameter(0.0),
                                               InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(U)
  DEFINE_VISITABLE()
};

class X : public Gate {
public:
  X() : Gate("X") {}
  X(std::vector<std::size_t> qbit) : Gate("X", qbit) {}
  X(std::size_t qbit) : X(std::vector<std::size_t>{qbit}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(X)
  DEFINE_VISITABLE()
};

class Y : public Gate {
public:
  Y() : Gate("Y") {}
  Y(std::vector<std::size_t> qbit) : Gate("Y", qbit) {}
  Y(std::size_t qbit) : Y(std::vector<std::size_t>{qbit}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Y)
  DEFINE_VISITABLE()
};

class Z : public Gate {
public:
  Z() : Gate("Z") {}
  Z(std::vector<std::size_t> qbit) : Gate("Z", qbit) {}
  Z(std::size_t qbit) : Z(std::vector<std::size_t>{qbit}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Z)
  DEFINE_VISITABLE()
};

class Measure : public Gate {
public:
  Measure()
      : Gate("Measure",
             std::vector<InstructionParameter>{InstructionParameter(0)}) {}
  Measure(std::vector<std::size_t> qbit)
      : Gate("Measure", qbit,
             std::vector<InstructionParameter>{InstructionParameter(0)}) {}
  Measure(std::size_t qbit)
      : Gate("Measure", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{
                 InstructionParameter((int)qbit)}) {}
  Measure(std::size_t qbit, int classicalIdx)
      : Gate("Measure", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{
                 InstructionParameter(classicalIdx)}) {}

  int getClassicalBitIndex() { return mpark::get<int>(parameters[0]); }
  const int nRequiredBits() const override { return 1; }
  const std::string toString() override {
    auto bufferVarName = "q";
    auto str = gateName;
    str += " ";

    for (auto q : bits()) {
      str += bufferVarName + std::to_string(q) + ",";
    }

    // Remove trailing comma
    str = str.substr(0, str.length() - 1);

    return str;
  }
  DEFINE_CLONE(Measure)
  DEFINE_VISITABLE()
};

class CZ : public virtual Gate {
public:
  CZ() : Gate("CZ") {}
  CZ(std::vector<std::size_t> qbits) : Gate("CZ", qbits) {}
  CZ(std::size_t srcqbit, std::size_t tgtqbit)
      : CZ(std::vector<std::size_t>{srcqbit, tgtqbit}) {}

  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(CZ)
  DEFINE_VISITABLE()
};

class CPhase : public virtual Gate {
public:
  CPhase()
      : Gate("CPhase",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  CPhase(std::size_t controlQubit, std::size_t targetQubit, double theta)
      : Gate("CPhase", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{InstructionParameter(theta)}) {}
  CPhase(std::vector<std::size_t> qbits)
      : Gate("CPhase", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(CPhase)
  DEFINE_VISITABLE()
};

class S : public Gate {
public:
  S() : Gate("S") {}
  S(std::vector<std::size_t> qbits) : Gate("S", qbits) {}
  S(std::size_t qbit) : S(std::vector<std::size_t>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(S)
  DEFINE_VISITABLE()
};

class Sdg : public Gate {
public:
  Sdg() : Gate("Sdg") {}
  Sdg(std::vector<std::size_t> qbits) : Gate("Sdg", qbits) {}
  Sdg(std::size_t qbit) : Sdg(std::vector<std::size_t>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Sdg)
  DEFINE_VISITABLE()
};

class T : public Gate {
public:
  T() : Gate("T") {}
  T(std::vector<std::size_t> qbits) : Gate("T", qbits) {}
  T(std::size_t qbit) : T(std::vector<std::size_t>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(T)
  DEFINE_VISITABLE()
};

class Tdg : public Gate {
public:
  Tdg() : Gate("Tdg") {}
  Tdg(std::vector<std::size_t> qbits) : Gate("Tdg", qbits) {}
  Tdg(std::size_t qbit) : Tdg(std::vector<std::size_t>{qbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(Tdg)
  DEFINE_VISITABLE()
};

class CY : public Gate {
public:
  CY() : Gate("CY") {}
  CY(std::vector<std::size_t> qbits) : Gate("CY", qbits) {}
  CY(std::size_t sqbit, std::size_t tqbit)
      : CY(std::vector<std::size_t>{sqbit, tqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CY)
  DEFINE_VISITABLE()
};

class CH : public Gate {
public:
  CH() : Gate("CH") {}
  CH(std::vector<std::size_t> qbits) : Gate("CH", qbits) {}
  CH(std::size_t sqbit, std::size_t tqbit)
      : CH(std::vector<std::size_t>{sqbit, tqbit}) {}

  const std::string description() const override { return ""; }
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(CH)
  DEFINE_VISITABLE()
};

class CRZ : public virtual Gate {
public:
  CRZ()
      : Gate("CRZ",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  CRZ(std::size_t qbit, double theta)
      : Gate("CRZ", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

  CRZ(std::vector<std::size_t> qbits)
      : Gate("CRZ", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(CRZ)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc
#endif
