/*******************************************************************************
 * Copyright (c) 2019-2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *******************************************************************************/

#include "QrackVisitor.hpp"
#include "xacc.hpp"

#define MAKE_ENGINE(num_qubits, perm) Qrack::CreateQuantumInterface(qIType1, qIType2, qIType3, num_qubits, perm, nullptr, Qrack::CMPLX_DEFAULT_ARG, doNormalize, false, false, device_id, true, zero_threshold)

namespace xacc {
namespace quantum {
    void QrackVisitor::initialize(std::shared_ptr<AcceleratorBuffer> buffer, int shots, bool use_opencl, bool use_qunit, bool use_opencl_multi, bool use_stabilizer, int device_id, bool doNormalize, double zero_threshold)
    {
        m_buffer = std::move(buffer);
        m_measureBits.clear();
        m_shots = shots;
        m_shotsMode = shots > 1;

        Qrack::QInterfaceEngine qIType1, qIType2, qIType3;
        if (use_qunit) {
            qIType1 = use_opencl_multi ? Qrack::QINTERFACE_QUNIT_MULTI : Qrack::QINTERFACE_QUNIT;
            qIType2 = use_stabilizer ? Qrack::QINTERFACE_STABILIZER_HYBRID : (use_opencl ? Qrack::QINTERFACE_OPTIMAL_SCHROEDINGER : Qrack::QINTERFACE_CPU);
            qIType3 = use_opencl ? (use_stabilizer ? Qrack::QINTERFACE_OPTIMAL_SCHROEDINGER : Qrack::QINTERFACE_OPTIMAL_SINGLE_PAGE ) : Qrack::QINTERFACE_CPU;
        } else {
            qIType1 = use_stabilizer ? Qrack::QINTERFACE_STABILIZER_HYBRID : (use_opencl ? Qrack::QINTERFACE_OPTIMAL_SCHROEDINGER : Qrack::QINTERFACE_CPU);
            qIType2 = use_opencl ? (use_stabilizer ? Qrack::QINTERFACE_OPTIMAL_SCHROEDINGER : Qrack::QINTERFACE_OPTIMAL_SINGLE_PAGE) : Qrack::QINTERFACE_CPU;
            qIType3 = Qrack::QINTERFACE_OPTIMAL_SINGLE_PAGE;
        }

        m_qReg = MAKE_ENGINE(m_buffer->size(), 0);
    }

    double QrackVisitor::calcExpectationValueZ() const
    {
        const auto hasEvenParity = [](size_t x, const std::vector<bitLenInt>& in_qubitIndices) -> bool {
            size_t count = 0;
            for (const auto& bitIdx : in_qubitIndices)
            {
                if (x & (1ULL << bitIdx))
                {
                    count++;
                }
            }
            return (count & 1) == 0;
        };

        std::vector<Qrack::complex> wfn((bitCapIntOcl)m_qReg->GetMaxQPower());
        m_qReg->GetQuantumState(&(wfn[0]));

        double result = 0.0;
        for(uint64_t i = 0; i < wfn.size(); ++i)
        {
            result += (hasEvenParity(i, m_measureBits) ? 1.0 : -1.0) * std::norm(wfn[i]);
        }

        return result;
    }

    std::map<bitCapInt, int> QrackVisitor::measure_shots() {
        bitLenInt bitCount = m_measureBits.size();
        bitCapInt* qPowers = new bitCapInt[bitCount];
        for (bitLenInt i = 0; i < bitCount; i++) {
            qPowers[i] = Qrack::pow2(m_measureBits[i]);
        }

        std::map<bitCapInt, int> result = m_qReg->MultiShotMeasureMask(qPowers, bitCount, m_shots);

        delete[] qPowers;

        return result;
    }

    void QrackVisitor::finalize()
    {
        if (m_shots < 0)
        {
            const double expectedValueZ = calcExpectationValueZ();
            m_buffer->addExtraInfo("exp-val-z", expectedValueZ);
            m_measureBits.clear();
            return;
        }

        if (!m_shotsMode)
        {
            m_buffer->appendMeasurement(m_bitString);
            m_bitString.clear();
            return;
        }

        auto measureDist = measure_shots();

        for (auto it = measureDist.begin(); it != measureDist.end(); it++) {
            std::string bitString = "";
            for (int i = 0; i < m_measureBits.size(); i++) {
                bitString.append((Qrack::pow2(i) & it->first) ? "1" : "0"); 
            }
            for (int j = 0; j < it->second; j++) {
                m_buffer->appendMeasurement(bitString);
            }
        }

        m_measureBits.clear();
    }

    void QrackVisitor::visit(Hadamard& h)
    {
       m_qReg->H(h.bits()[0]);
    }

    void QrackVisitor::visit(CNOT& cnot)
    {
        m_qReg->CNOT(cnot.bits()[0], cnot.bits()[1]);
    }

    void QrackVisitor::visit(Rz& rz)
    {
        const auto angleTheta = InstructionParameterToDouble(rz.getParameter(0));
        m_qReg->RZ(angleTheta, rz.bits()[0]);
    }

    void QrackVisitor::visit(Ry& ry)
    {
        const auto angleTheta = InstructionParameterToDouble(ry.getParameter(0));
        m_qReg->RY(angleTheta, ry.bits()[0]);
    }

    void QrackVisitor::visit(Rx& rx)
    {
        const auto angleTheta = InstructionParameterToDouble(rx.getParameter(0));
        m_qReg->RX(angleTheta, rx.bits()[0]);
    }

    void QrackVisitor::visit(X& x)
    {
        m_qReg->X(x.bits()[0]);
    }

    void QrackVisitor::visit(Y& y)
    {
        m_qReg->Y(y.bits()[0]);
    }

    void QrackVisitor::visit(Z& z)
    {
        m_qReg->Z(z.bits()[0]);
    }

    void QrackVisitor::visit(CY& cy)
    {
        m_qReg->CY(cy.bits()[0], cy.bits()[1]);
    }

    void QrackVisitor::visit(CZ& cz)
    {
        m_qReg->CZ(cz.bits()[0], cz.bits()[1]);
    }

    void QrackVisitor::visit(Swap& s)
    {
        m_qReg->Swap(s.bits()[0], s.bits()[1]);
    }

    void QrackVisitor::visit(CRZ& crz)
    {
        const auto angleTheta = InstructionParameterToDouble(crz.getParameter(0));
        m_qReg->CRZ(angleTheta, crz.bits()[0], crz.bits()[1]);
    }

    void QrackVisitor::visit(CH& ch)
    {
        m_qReg->CH(ch.bits()[0], ch.bits()[1]);
    }

    void QrackVisitor::visit(S& s)
    {
        m_qReg->S(s.bits()[0]);
    }

    void QrackVisitor::visit(Sdg& sdg)
    {
        m_qReg->IS(sdg.bits()[0]);
    }

    void QrackVisitor::visit(T& t)
    {
        m_qReg->T(t.bits()[0]);
    }

    void QrackVisitor::visit(Tdg& tdg)
    {
        m_qReg->IT(tdg.bits()[0]);
    }

    void QrackVisitor::visit(CPhase& cphase)
    {
        m_qReg->CS(cphase.bits()[0], cphase.bits()[1]);
    }

    void QrackVisitor::visit(Identity& i)
    {
        // Intentionally left blank
    }

    void QrackVisitor::visit(U& u)
    {
        const auto theta = InstructionParameterToDouble(u.getParameter(0));
        const auto phi = InstructionParameterToDouble(u.getParameter(1));
        const auto lambda = InstructionParameterToDouble(u.getParameter(2));
        m_qReg->U(u.bits()[0], theta, phi, lambda);
    }

    void QrackVisitor::visit(iSwap& in_iSwapGate) 
    {
        m_qReg->ISwap(in_iSwapGate.bits()[0], in_iSwapGate.bits()[1]);
    }

    void QrackVisitor::visit(fSim& in_fsimGate) 
    {
        const auto theta = InstructionParameterToDouble(in_fsimGate.getParameter(0));
        const auto phi = InstructionParameterToDouble(in_fsimGate.getParameter(1));
        m_qReg->FSim(theta, phi, in_fsimGate.bits()[0], in_fsimGate.bits()[1]);
    }

    void QrackVisitor::visit(Measure& measure)
    {
        if (m_shotsMode || (m_shots < 1)) {
            m_measureBits.push_back(measure.bits()[0]);
            return;
        }

        bool result = m_qReg->M(measure.bits()[0]);
        m_bitString.append(result ? "1" : "0");

        // Add the measurement data to the acceleration buffer (e.g. for conditional execution branching)
        m_buffer->measure(measure.bits()[0], result);
    }

    void QrackVisitor::visit(IfStmt& ifStmt)
    {
        ifStmt.expand({});

        if (xacc::verbose)
        {
            std::cout << "If statement expanded to: " << ifStmt.toString() << "\n";
        }
    }
}}
