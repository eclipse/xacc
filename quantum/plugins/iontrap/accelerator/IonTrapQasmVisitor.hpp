/*******************************************************************************
 * Copyright (c) 2019-2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *   Austin Adams - adaption for GTRI testbed
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPQASMVISITOR_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPQASMVISITOR_HPP_

#include "Identifiable.hpp"
#include "AllGateVisitor.hpp"
#include "AcceleratorBuffer.hpp"
#include "OptionsProvider.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

typedef std::set<std::pair<std::size_t, std::size_t>> IonTrapCouplingMap;

class IonTrapQasmVisitor : public AllGateVisitor, public OptionsProvider, public xacc::Cloneable<IonTrapQasmVisitor> {
public:
    IonTrapQasmVisitor(std::size_t nQubits_, IonTrapCouplingMap &couplingMap_, std::ostream &qasmFile_) : nQubits(nQubits_), couplingMap(couplingMap_), qasmFile(qasmFile_) {}
    void writeHeader();

    void visit(Hadamard& h) override;
    void visit(CNOT& cnot) override;
    void visit(Rz& rz) override;
    void visit(Ry& ry) override;
    void visit(Rx& rx) override;
    void visit(X& x) override;
    void visit(Y& y) override;
    void visit(Z& z) override;
    void visit(CRZ& crz) override;
    void visit(CH& ch) override;
    void visit(S& s) override;
    void visit(Sdg& sdg) override;
    void visit(T& t) override;
    void visit(Tdg& tdg) override;
    void visit(CPhase& cphase) override;
    void visit(Identity& i) override;
    void visit(U& u) override;
    void visit(iSwap& in_iSwapGate) override;
    void visit(fSim& in_fsimGate) override;
    void visit(IfStmt& ifStmt) override;
    void visit(Measure &meas) override;

    virtual std::shared_ptr<IonTrapQasmVisitor> clone() { return std::make_shared<IonTrapQasmVisitor>(nQubits, couplingMap, qasmFile); }

private:
    std::size_t nQubits;
    IonTrapCouplingMap &couplingMap;
    std::ostream &qasmFile;
};
}}

#endif
