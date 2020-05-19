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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "ControlledGateApplicator.hpp"

namespace xacc {
std::shared_ptr<xacc::CompositeInstruction> ControlledGateApplicator::applyControl(const std::shared_ptr<xacc::CompositeInstruction>& in_program, int in_ctrlIdx)
{
    m_gateProvider = xacc::getService<xacc::IRProvider>("quantum");
    m_composite = m_gateProvider->createComposite("CTRL_" + in_program->name() + "_" + std::to_string(in_ctrlIdx));
    m_ctrlIdx = in_ctrlIdx;
    InstructionIterator it(in_program);
    while (it.hasNext())
    {
        auto nextInst = it.next();
        if (nextInst->isEnabled())
        {
            nextInst->accept(this);
        }
    }

    return m_composite;
}

void ControlledGateApplicator::visit(Hadamard& h) 
{
    if (h.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = h.bits()[0];
        // CH
        m_composite->addInstruction(m_gateProvider->createInstruction("CH", { m_ctrlIdx, targetIdx }));
    }
}

void ControlledGateApplicator::visit(X& x) 
{
    if (x.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = x.bits()[0];
        // CNOT
        m_composite->addInstruction(m_gateProvider->createInstruction("CX", { m_ctrlIdx, targetIdx }));
    }
}

void ControlledGateApplicator::visit(Y& y) 
{
    if (y.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = y.bits()[0];
        // CY
        m_composite->addInstruction(m_gateProvider->createInstruction("CY", { m_ctrlIdx, targetIdx }));
    }
}

void ControlledGateApplicator::visit(Z& z) 
{
    if (z.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = z.bits()[0];
        // CZ
        m_composite->addInstruction(m_gateProvider->createInstruction("CZ", { m_ctrlIdx, targetIdx }));
    }
}

void ControlledGateApplicator::visit(CNOT& cnot) 
{
   // TODO: CCNOT
}

void ControlledGateApplicator::visit(Rx& rx) 
{
    // CRn(theta) = Rn(theta/2) - CX - Rn(-theta/2) - CX
    if (rx.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = rx.bits()[0];
        const auto angle = InstructionParameterToDouble(rx.getParameter(0));
        m_composite->addInstruction(m_gateProvider->createInstruction("Rx", { targetIdx }, { angle/ 2.0 }));
        m_composite->addInstruction(m_gateProvider->createInstruction("CX", { m_ctrlIdx, targetIdx }));
        m_composite->addInstruction(m_gateProvider->createInstruction("Rx", { targetIdx }, { -angle/ 2.0 }));
        m_composite->addInstruction(m_gateProvider->createInstruction("CX", { m_ctrlIdx, targetIdx }));
    }
}

void ControlledGateApplicator::visit(Ry& ry) 
{
    // CRn(theta) = Rn(theta/2) - CX - Rn(-theta/2) - CX
    if (ry.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = ry.bits()[0];
        const auto angle = InstructionParameterToDouble(ry.getParameter(0));
        m_composite->addInstruction(m_gateProvider->createInstruction("Ry", { targetIdx }, { angle/ 2.0 }));
        m_composite->addInstruction(m_gateProvider->createInstruction("CX", { m_ctrlIdx, targetIdx }));
        m_composite->addInstruction(m_gateProvider->createInstruction("Ry", { targetIdx }, { -angle/ 2.0 }));
        m_composite->addInstruction(m_gateProvider->createInstruction("CX", { m_ctrlIdx, targetIdx }));
    }
}


void ControlledGateApplicator::visit(Rz& rz) 
{
    // CRn(theta) = Rn(theta/2) - CX - Rn(-theta/2) - CX
    if (rz.bits()[0] == m_ctrlIdx)
    {
        xacc::error("Control bit must be different from target qubit(s).");
    }
    else
    {
        const auto targetIdx = rz.bits()[0];
        const auto angle = InstructionParameterToDouble(rz.getParameter(0));
        // CRz
        m_composite->addInstruction(m_gateProvider->createInstruction("CRZ", { m_ctrlIdx, targetIdx }, { angle }));
    }
}

void ControlledGateApplicator::visit(S& s) 
{
    // S = Rz(pi/2)
    auto equivGate = m_gateProvider->createInstruction("Rz", { s.bits()[0] }, { M_PI_2 });
    equivGate->accept(this);
}

void ControlledGateApplicator::visit(Sdg& sdg) 
{
    // Sdg = Rz(-pi/2)
    auto equivGate = m_gateProvider->createInstruction("Rz", { sdg.bits()[0] }, { -M_PI_2 });
    equivGate->accept(this);
}

void ControlledGateApplicator::visit(T& t) 
{
    // T = Rz(pi/4)
    auto equivGate = m_gateProvider->createInstruction("Rz", { t.bits()[0] }, { M_PI_4 });
    equivGate->accept(this);
}

void ControlledGateApplicator::visit(Tdg& tdg) 
{
    // Tdg = Rz(-pi/4)
    auto equivGate = m_gateProvider->createInstruction("Rz", { tdg.bits()[0] }, { -M_PI_4 });
    equivGate->accept(this);
}

void ControlledGateApplicator::visit(U& u) 
{
    // TODO
}

void ControlledGateApplicator::visit(CY& cy) 
{
    // TODO
}

void ControlledGateApplicator::visit(CZ& cz) 
{
    // TODO
}

void ControlledGateApplicator::visit(CRZ& crz) 
{
    // TODO
}

void ControlledGateApplicator::visit(CH& ch) 
{
    // TODO
}

void ControlledGateApplicator::visit(Swap& s) 
{
   // TODO
}

void ControlledGateApplicator::visit(CPhase& cphase) 
{
   // TODO
}
}