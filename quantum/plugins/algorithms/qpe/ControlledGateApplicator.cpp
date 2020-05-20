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
namespace circuits {

bool ControlledU::expand(const xacc::HeterogeneousMap& runtimeOptions) 
{
    if (!runtimeOptions.keyExists<int>("control-idx")) 
    {
        xacc::error("'control-idx' is required.");
        return false;
    }

    const auto ctrlIdx = runtimeOptions.get<int>("control-idx");
    
    if (!runtimeOptions.pointerLikeExists<CompositeInstruction>("U")) 
    {
        xacc::error("'U' composite is required.");
        return false;
    }
    
    auto uComposite = std::shared_ptr<CompositeInstruction>(
        runtimeOptions.getPointerLike<CompositeInstruction>("U"), 
        xacc::empty_delete<CompositeInstruction>());
    auto ctrlU = applyControl(uComposite, ctrlIdx);

    for (int instId = 0; instId < ctrlU->nInstructions(); ++instId)
    {
        addInstruction(ctrlU->getInstruction(instId)->clone());
    }
    return true;
}

std::shared_ptr<xacc::CompositeInstruction> ControlledU::applyControl(const std::shared_ptr<xacc::CompositeInstruction>& in_program, int in_ctrlIdx)
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

void ControlledU::visit(Hadamard& h) 
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

void ControlledU::visit(X& x) 
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

void ControlledU::visit(Y& y) 
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

void ControlledU::visit(Z& z) 
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

void ControlledU::visit(CNOT& cnot) 
{
    // CCNOT gate:
    // We now has two control bits
    const auto ctrlIdx1 = cnot.bits()[0];
    const auto ctrlIdx2 = m_ctrlIdx;
    // Target qubit
    const auto targetIdx = cnot.bits()[1];
    
    m_composite->addInstruction(m_gateProvider->createInstruction("H", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx1, targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", { targetIdx }));    
    m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx2, targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("T", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx1, targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx2, targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("T", { ctrlIdx1 }));
    m_composite->addInstruction(m_gateProvider->createInstruction("T", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("H", { targetIdx }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx2, ctrlIdx1 }));
    m_composite->addInstruction(m_gateProvider->createInstruction("T", { ctrlIdx2 }));
    m_composite->addInstruction(m_gateProvider->createInstruction("Tdg", { ctrlIdx1 }));
    m_composite->addInstruction(m_gateProvider->createInstruction("CX", { ctrlIdx2, ctrlIdx1 }));
}

void ControlledU::visit(Rx& rx) 
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

void ControlledU::visit(Ry& ry) 
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


void ControlledU::visit(Rz& rz) 
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

void ControlledU::visit(S& s) 
{
    // Ctrl-S = CPhase(pi/2)
    const auto targetIdx = s.bits()[0];
    m_composite->addInstruction(m_gateProvider->createInstruction("CPhase", { m_ctrlIdx, targetIdx }, { M_PI_2 }));
}

void ControlledU::visit(Sdg& sdg) 
{
    // Ctrl-Sdg = CPhase(-pi/2)
    const auto targetIdx = sdg.bits()[0];
    m_composite->addInstruction(m_gateProvider->createInstruction("CPhase", { m_ctrlIdx, targetIdx }, { -M_PI_2 }));
}

void ControlledU::visit(T& t) 
{
    // Ctrl-T = CPhase(pi/4)
    const auto targetIdx = t.bits()[0];
    m_composite->addInstruction(m_gateProvider->createInstruction("CPhase", { m_ctrlIdx, targetIdx }, { M_PI_4 }));
}

void ControlledU::visit(Tdg& tdg) 
{
    // Ctrl-Tdg = CPhase(-pi/4)
    const auto targetIdx = tdg.bits()[0];
    m_composite->addInstruction(m_gateProvider->createInstruction("CPhase", { m_ctrlIdx, targetIdx }, { -M_PI_4 }));
}

void ControlledU::visit(Swap& s) 
{
    CNOT c1(s.bits()), c2(s.bits()[1],s.bits()[0]), c3(s.bits());
    visit(c1);
    visit(c2);
    visit(c3);
}

void ControlledU::visit(U& u) 
{
    xacc::error("Unsupported!");
}

void ControlledU::visit(CY& cy) 
{
    xacc::error("Unsupported!");
}

void ControlledU::visit(CZ& cz) 
{
    xacc::error("Unsupported!");
}

void ControlledU::visit(CRZ& crz) 
{
    xacc::error("Unsupported!");
}

void ControlledU::visit(CH& ch) 
{
    xacc::error("Unsupported!");
}

void ControlledU::visit(CPhase& cphase) 
{
    xacc::error("Unsupported!");
}
}}