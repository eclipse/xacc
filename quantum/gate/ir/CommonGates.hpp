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
#include "Circuit.hpp"

namespace xacc {
namespace quantum {

class iSwap : public Gate {
public:
  iSwap() : Gate("iSwap") {}

  iSwap(std::size_t controlQubit, std::size_t targetQubit)
      : Gate("iSwap", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{}) {}

  iSwap(std::vector<std::size_t> qbits)
      : Gate("iSwap", qbits, std::vector<InstructionParameter>{}) {}
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(iSwap)
  DEFINE_VISITABLE()
};

class fSim : public Gate {
public:
  fSim() : Gate("fSim",std::vector<InstructionParameter>{0.0,0.0}) {}
  fSim(std::size_t controlQubit, std::size_t targetQubit)
      : Gate("fSim", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{0.0, 0.0}) {}
  fSim(std::size_t controlQubit, std::size_t targetQubit, double theta,
       double phi)
      : Gate("fSim", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{theta, phi}) {}
  fSim(std::vector<std::size_t> qbits)
      : Gate("fSim", qbits, std::vector<InstructionParameter>{0.0,0.0}) {}
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(fSim)
  DEFINE_VISITABLE()
};

class IfStmt : public Circuit {
protected:
  std::string bufferName;
  std::size_t bitIdx;

public:
  IfStmt() : Circuit("ifstmt") {}

  const std::vector<std::size_t> bits() override { return {bitIdx}; }
  void setBits(const std::vector<std::size_t> bits) override {
    bitIdx = bits[0];
  }

  const InstructionParameter
  getParameter(const std::size_t idx) const override {
    return InstructionParameter(bufferName);
  }
  void setParameter(const std::size_t idx, InstructionParameter &p) override {
    bufferName = p.toString();
  }
  std::vector<InstructionParameter> getParameters() override {
    return {bufferName};
  }
  const int nParameters() override { return 1; }

  const int nRequiredBits() const override { return 1; }

  void addInstruction(InstPtr instruction) override {
    instruction->disable();
    Circuit::addInstruction(instruction);
  }

  bool expand(const HeterogeneousMap &runtimeOptions) override;
  
  void disable() override {
    for (auto &i : instructions) {
      i->disable();
    }
  }

  const std::string toString() override; 
  DEFINE_CLONE(IfStmt)
  DEFINE_VISITABLE()
};

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

class AnnealingInstruction : public Gate {
public:
  AnnealingInstruction()
      : Gate("AnnealingInstruction",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  AnnealingInstruction(std::size_t qbit, InstructionParameter &&bias)
      : Gate("AnnealingInstruction", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{bias}) {}
  AnnealingInstruction(std::size_t qbit1, std::size_t qbit2,
                       InstructionParameter &&weight)
      : Gate("AnnealingInstruction", std::vector<std::size_t>{qbit1, qbit2},
             std::vector<InstructionParameter>{weight}) {}
  AnnealingInstruction(std::vector<std::size_t> qbits)
      : Gate("AnnealingInstruction", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {
    // make it easier to specify single qubit bias
    if (qbits.size() == 1) {
      qbits.push_back(qbits[0]);
    }
  }
  void setBits(const std::vector<std::size_t> bts) override {
    if (bts.size() == 1) {
      qbits.push_back(bts[0]);
      qbits.push_back(bts[0]);
    } else {
      qbits = bts;
    }
  }
  const int nRequiredBits() const override { return 2; }
  void
  setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override {
    std::vector<std::string> tmp;
    tmp.push_back(bufferNamesPerIdx[0]);
    if (bufferNamesPerIdx.size() == 1) {
      tmp.push_back(bufferNamesPerIdx[0]);
    } else {
      tmp.push_back(bufferNamesPerIdx[1]);
    }
    Gate::setBufferNames(tmp);
  }
  DEFINE_CLONE(AnnealingInstruction)
  DEFINE_VISITABLE()
};

class U1 : public Gate {
public:
  U1()
      : Gate("U1",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  U1(std::size_t qbit, InstructionParameter &&theta)
      : Gate("U1", std::vector<std::size_t>{qbit},
             std::vector<InstructionParameter>{theta}) {}
  U1(std::vector<std::size_t> qbits)
      : Gate("U1", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 1; }

  DEFINE_CLONE(U1)
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
  U(std::size_t qbit, std::vector<xacc::InstructionParameter> params)
      : Gate("U", params) {}
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
  bool hasClassicalRegAssignment() const {
    return buffer_names.size() > nRequiredBits();
  }

  void
  setBufferNames(const std::vector<std::string> bufferNamesPerIdx) override;

  const std::string toString() override {
    auto bufferVarName = "q";
    auto str = gateName;
    str += " ";

    int counter = 0;
    for (auto q : bits()) {
      str += (buffer_names.empty() ? "q" : getBufferName(counter)) +
             std::to_string(q) + ",";
      counter++;
    }

    // Remove trailing comma
    str = str.substr(0, str.length() - 1);

    if (hasClassicalRegAssignment()) {
      str +=
          (" -> " + getBufferName(1) + std::to_string(getClassicalBitIndex()));
    }

    return str;
  }
  DEFINE_CLONE(Measure)
  DEFINE_VISITABLE()
};

class CZ : public Gate {
public:
  CZ() : Gate("CZ") {}
  CZ(std::vector<std::size_t> qbits) : Gate("CZ", qbits) {}
  CZ(std::size_t srcqbit, std::size_t tgtqbit)
      : CZ(std::vector<std::size_t>{srcqbit, tgtqbit}) {}

  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CZ)
  DEFINE_VISITABLE()
};

class CPhase : public Gate {
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

  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CPhase)
  DEFINE_VISITABLE()
};

class XY : public Gate {
public:
  XY()
      : Gate("XY",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  XY(std::size_t controlQubit, std::size_t targetQubit, double theta)
      : Gate("XY", std::vector<std::size_t>{controlQubit, targetQubit},
             std::vector<InstructionParameter>{InstructionParameter(theta)}) {}
  XY(std::vector<std::size_t> qbits)
      : Gate("XY", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(XY)
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
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CH)
  DEFINE_VISITABLE()
};

class CRZ : public Gate {
public:
  CRZ()
      : Gate("CRZ",
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}

  CRZ(std::size_t src_qbit, std::size_t tgt_qbit,  double theta)
      : Gate("CRZ", std::vector<std::size_t>{src_qbit, tgt_qbit},
             std::vector<InstructionParameter>{InstructionParameter(theta)}) {}

  CRZ(std::vector<std::size_t> qbits)
      : Gate("CRZ", qbits,
             std::vector<InstructionParameter>{InstructionParameter(0.0)}) {}
  const int nRequiredBits() const override { return 2; }

  DEFINE_CLONE(CRZ)
  DEFINE_VISITABLE()
};

class Reset : public Gate {
public:
  Reset() : Gate("Reset") {}
  Reset(std::vector<std::size_t> qbit) : Gate("Reset", qbit) {}
  Reset(std::size_t qbit) : Reset(std::vector<std::size_t>{qbit}) {}
  const int nRequiredBits() const override { return 1; }
  DEFINE_CLONE(Reset)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc
#endif
