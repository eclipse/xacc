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

#include "QppVisitor.hpp"
#include "xacc.hpp"

namespace {
    // Add gate matrix for iSwap and fSim gates
    qpp::cmat iSwapGateMat()
    {
        qpp::cmat gateMat(4, 4);  
        gateMat << 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, std::complex<double>(0, 1.0), 0.0,
        0.0, std::complex<double>(0, 1.0), 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0;
        
        return gateMat; 
    }

    qpp::cmat fSimGateMat(double in_theta, double in_phi)
    {
        qpp::cmat gateMat(4, 4);  
        gateMat << 
        1.0, 0.0, 0.0, 0.0,
        0.0, std::cos(in_theta), std::complex<double>(0, -std::sin(in_theta)), 0.0,
        0.0, std::complex<double>(0, -std::sin(in_theta)), std::cos(in_theta), 0.0,
        0.0, 0.0, 0.0, std::exp(std::complex<double>(0, -in_phi));
        
        return gateMat; 
    }

    qpp::cmat u3GateMat(double in_theta, double in_phi, double in_lambda) 
    {
      qpp::cmat gateMat(2, 2);
      gateMat << std::cos(in_theta / 2.0),
          -std::exp(std::complex<double>(0, in_lambda)) *
              std::sin(in_theta / 2.0),
          std::exp(std::complex<double>(0, in_phi)) * std::sin(in_theta / 2.0),
          std::exp(std::complex<double>(0, in_phi + in_lambda)) *
              std::cos(in_theta / 2.0);

      return gateMat;
    }
}

namespace xacc {
namespace quantum {
    void QppVisitor::initialize(std::shared_ptr<AcceleratorBuffer> buffer, bool shotsMode)
    {
        m_buffer = std::move(buffer);
        const std::vector<qpp::idx> initialState(m_buffer->size(), 0);
        m_stateVec = qpp::mket(initialState);
        const std::vector<qpp::idx> dims(m_buffer->size(), 2);
        m_dims = std::move(dims);
        m_measureBits.clear();
        m_shotsMode = shotsMode;
        m_initialized = true;
    }

    void QppVisitor::finalize()
    {
        if (m_shotsMode)
        {
            m_buffer->appendMeasurement(m_bitString);
            m_bitString.clear();
        }
        m_stateVec.resize(0);
        m_initialized = false;
    }

    qpp::idx QppVisitor::xaccIdxToQppIdx(size_t in_idx) const
    {
        assert(in_idx < m_buffer->size());
        // QPP is using a different *endian* than the one of XACC,
        // i.e. q0 is the MSB in QPP vs. LSB as the convention used by XACC.
        // Hence, we convert the indices before passing to QPP so that
        // the state vector is indexed according to the XACC convention.
        return m_buffer->size() - in_idx - 1;
    }

    double QppVisitor::calcExpectationValueZ(const KetVectorType& in_stateVec, const std::vector<qpp::idx>& in_bits)
    {
        const auto hasEvenParity = [](size_t x, const std::vector<size_t>& in_qubitIndices) -> bool {
            size_t count = 0;
            for (const auto& bitIdx : in_qubitIndices)
            {
                if (x & (1ULL << bitIdx))
                {
                    count++;
                }
            }
            return (count % 2) == 0;
        };


        double result = 0.0;
        for(uint64_t i = 0; i < in_stateVec.size(); ++i)
        {
            result += (hasEvenParity(i, in_bits) ? 1.0 : -1.0) * std::norm(in_stateVec[i]);
        }

        return result;
    }

    void QppVisitor::visit(Hadamard& h)
    {
       const auto qubitIdx = xaccIdxToQppIdx(h.bits()[0]);
       m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().H, { qubitIdx });
    }

    void QppVisitor::visit(CNOT& cnot)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(cnot.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(cnot.bits()[1]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().CNOT, { ctrlIdx,  targetIdx}, m_dims);
    }

    void QppVisitor::visit(Rz& rz)
    {
        const auto qubitIdx = xaccIdxToQppIdx(rz.bits()[0]);
        const auto angleTheta = InstructionParameterToDouble(rz.getParameter(0));
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().RZ(angleTheta), { qubitIdx });
    }

    void QppVisitor::visit(Ry& ry)
    {
        const auto qubitIdx = xaccIdxToQppIdx(ry.bits()[0]);
        const auto angleTheta = InstructionParameterToDouble(ry.getParameter(0));
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().RY(angleTheta), { qubitIdx });
    }

    void QppVisitor::visit(Rx& rx)
    {
        const auto qubitIdx = xaccIdxToQppIdx(rx.bits()[0]);
        const auto angleTheta = InstructionParameterToDouble(rx.getParameter(0));
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().RX(angleTheta), { qubitIdx });
    }

    void QppVisitor::visit(X& x)
    {
        const auto qubitIdx = xaccIdxToQppIdx(x.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().X, { qubitIdx });
    }

    void QppVisitor::visit(Y& y)
    {
        const auto qubitIdx = xaccIdxToQppIdx(y.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().Y, { qubitIdx });
    }

    void QppVisitor::visit(Z& z)
    {
        const auto qubitIdx = xaccIdxToQppIdx(z.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().Z, { qubitIdx });
    }

    void QppVisitor::visit(CY& cy)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(cy.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(cy.bits()[1]);
        m_stateVec = qpp::applyCTRL(m_stateVec, qpp::Gates::get_instance().Y, { ctrlIdx } ,  { targetIdx });
    }

    void QppVisitor::visit(CZ& cz)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(cz.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(cz.bits()[1]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().CZ, { ctrlIdx,  targetIdx}, m_dims);
    }

    void QppVisitor::visit(Swap& s)
    {
        const auto qIdx1 = xaccIdxToQppIdx(s.bits()[0]);
        const auto qIdx2 = xaccIdxToQppIdx(s.bits()[1]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().SWAP, { qIdx1, qIdx2 });
    }

    void QppVisitor::visit(CRZ& crz)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(crz.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(crz.bits()[1]);
        const auto angleTheta = InstructionParameterToDouble(crz.getParameter(0));
        m_stateVec = qpp::applyCTRL(m_stateVec, qpp::Gates::get_instance().RZ(angleTheta), { ctrlIdx } ,  { targetIdx });
    }

    void QppVisitor::visit(CH& ch)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(ch.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(ch.bits()[1]);
        m_stateVec = qpp::applyCTRL(m_stateVec, qpp::Gates::get_instance().H, { ctrlIdx } ,  { targetIdx });
    }

    void QppVisitor::visit(S& s)
    {
        const auto qubitIdx = xaccIdxToQppIdx(s.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().S, { qubitIdx });
    }

    void QppVisitor::visit(Sdg& sdg)
    {
        const auto qubitIdx = xaccIdxToQppIdx(sdg.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().S.adjoint(), { qubitIdx });
    }

    void QppVisitor::visit(T& t)
    {
        const auto qubitIdx = xaccIdxToQppIdx(t.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().T, { qubitIdx });
    }

    void QppVisitor::visit(Tdg& tdg)
    {
        const auto qubitIdx = xaccIdxToQppIdx(tdg.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().T.adjoint(), { qubitIdx });
    }

    void QppVisitor::visit(CPhase& cphase)
    {
        const auto ctrlIdx = xaccIdxToQppIdx(cphase.bits()[0]);
        const auto targetIdx = xaccIdxToQppIdx(cphase.bits()[1]);
        const auto angleTheta = InstructionParameterToDouble(cphase.getParameter(0));
        qpp::cmat gateMat { qpp::cmat::Zero(2, 2)};       
        gateMat << 1, 0, 0, std::exp(std::complex<double>(0.0, angleTheta));
        m_stateVec = qpp::applyCTRL(m_stateVec, gateMat, { ctrlIdx } ,  { targetIdx });
    }

    void QppVisitor::visit(Identity& i)
    {
        const auto qubitIdx = xaccIdxToQppIdx(i.bits()[0]);
        m_stateVec = qpp::apply(m_stateVec, qpp::Gates::get_instance().Id2, { qubitIdx });
    }

    void QppVisitor::visit(U& u)
    {
        const auto qubitIdx = xaccIdxToQppIdx(u.bits()[0]);
        const auto theta = InstructionParameterToDouble(u.getParameter(0));
        const auto phi = InstructionParameterToDouble(u.getParameter(1));
        const auto lambda = InstructionParameterToDouble(u.getParameter(2));
        m_stateVec = qpp::apply(m_stateVec, u3GateMat(theta, phi, lambda), { qubitIdx });
    }

    void QppVisitor::visit(iSwap& in_iSwapGate) 
    {
        const auto qIdx1 = xaccIdxToQppIdx(in_iSwapGate.bits()[0]);
        const auto qIdx2 = xaccIdxToQppIdx(in_iSwapGate.bits()[1]);

        m_stateVec = qpp::apply(m_stateVec, iSwapGateMat(), { qIdx1, qIdx2 });
    }

    void QppVisitor::visit(fSim& in_fsimGate) 
    {
        const auto qIdx1 = xaccIdxToQppIdx(in_fsimGate.bits()[0]);
        const auto qIdx2 = xaccIdxToQppIdx(in_fsimGate.bits()[1]);
        const auto theta = InstructionParameterToDouble(in_fsimGate.getParameter(0));
        const auto phi = InstructionParameterToDouble(in_fsimGate.getParameter(1));
        m_stateVec = qpp::apply(m_stateVec, fSimGateMat(theta, phi), { qIdx1, qIdx2 });
    }

    void QppVisitor::visit(Measure& measure)
    {
        if (xacc::verbose)
        {
            std::cout << ">> Applying Measure @ q[" << measure.bits()[0] << "] \n";
            std::cout << ">> State before measurement: " << qpp::disp(m_stateVec, ", ") << "\n";
        }

        const auto qubitIdx = xaccIdxToQppIdx(measure.bits()[0]);
        const auto measured = qpp::measure(m_stateVec, qpp::Gates::get_instance().Id2, { qubitIdx }, 2,  false);
        const auto& measProbs = std::get<qpp::PROB>(measured);
        const auto& postMeasStates = std::get<qpp::ST>(measured);
        const auto randomSelectedResult = std::get<qpp::RES>(measured);

        assert(measProbs.size() == 2 && postMeasStates.size() == 2 && randomSelectedResult < 2);
        m_measureBits.emplace_back(measure.bits()[0]);
        if (!m_shotsMode)
        {
            // Not running a shot simulation, calculate the expectation value.
            const double expectedValueZ = calcExpectationValueZ(m_stateVec, m_measureBits);
            m_buffer->addExtraInfo("exp-val-z", expectedValueZ);
        }
        else
        {
            m_bitString.append(std::to_string(randomSelectedResult));
        }

        if (xacc::verbose)
        {
            std::cout << ">> Probability of all results: ";
            std::cout << qpp::disp(measProbs, ", ") << "\n";
            std::cout << ">> Measurement result is: " << randomSelectedResult << "\n";
        }

        const auto& collapsedState = postMeasStates[randomSelectedResult];
        m_stateVec = Eigen::Map<const qpp::ket>(collapsedState.data(), collapsedState.size());
        if (xacc::verbose)
        {
            std::cout << ">> State after measurement: " << qpp::disp(m_stateVec, ", ") << "\n";
        }

        if (measure.hasClassicalRegAssignment()) 
        {
          // Store the measurement to the corresponding classical buffer.
          m_buffer->measure(measure.getBufferNames()[1],
                            measure.getClassicalBitIndex(),
                            randomSelectedResult);
        } 
        else 
        {
          // Add the measurement data to the acceleration buffer (e.g. for
          // conditional execution branching)
          m_buffer->measure(measure.bits()[0], randomSelectedResult);
        }
    }
    
    void QppVisitor::visit(Reset& in_resetGate) 
    {
        if (xacc::verbose)
        {
            std::cout << ">> Applying Reset @ q[" << in_resetGate.bits()[0] << "] \n";
            std::cout << ">> State before reset: " << qpp::disp(m_stateVec, ", ") << "\n";
        }

        const auto qubitIdx = xaccIdxToQppIdx(in_resetGate.bits()[0]);
        m_stateVec = qpp::reset(m_stateVec, { qubitIdx });
        
        if (xacc::verbose)
        {
            std::cout << ">> State after reset: " << qpp::disp(m_stateVec, ", ") << "\n";
        }
    }

    void QppVisitor::visit(IfStmt& ifStmt)
    {
        ifStmt.expand({});

        if (xacc::verbose)
        {
            std::cout << "If statement expanded to: " << ifStmt.toString() << "\n";
        }
    }
    
    void QppVisitor::applyGate(Gate& in_gate) 
    {
        if (in_gate.name() == "Measure")
        {
            xacc::error("Only unitary gates are allowed.");
        }
        in_gate.accept(this);
    }

    bool QppVisitor::measure(size_t in_bit) 
    {
        const auto qubitIdx = xaccIdxToQppIdx(in_bit);
        const auto measured = qpp::measure(m_stateVec, qpp::Gates::get_instance().Id2, { qubitIdx }, 2,  false);
        const auto& measProbs = std::get<qpp::PROB>(measured);
        const auto& postMeasStates = std::get<qpp::ST>(measured);
        const auto randomSelectedResult = std::get<qpp::RES>(measured);

        assert(measProbs.size() == 2 && postMeasStates.size() == 2 && randomSelectedResult < 2);
       
        if (xacc::verbose)
        {
            std::cout << ">> Probability of all results: ";
            std::cout << qpp::disp(measProbs, ", ") << "\n";
            std::cout << ">> Measurement result is: " << randomSelectedResult << "\n";
        }

        const auto& collapsedState = postMeasStates[randomSelectedResult];
        m_stateVec = Eigen::Map<const qpp::ket>(collapsedState.data(), collapsedState.size());
        if (xacc::verbose)
        {
            std::cout << ">> State after measurement: " << qpp::disp(m_stateVec, ", ") << "\n";
        }
        // 0 -> False; 1 -> True
        return (randomSelectedResult == 1);
    }

    double QppVisitor::getExpectationValueZ(std::shared_ptr<CompositeInstruction> in_composite) 
    {
        auto cachedStateVec = m_stateVec;
        std::vector<size_t> measureBitIdxs;

        InstructionIterator it(in_composite);
        while (it.hasNext()) 
        {
            auto nextInst = it.next();
            if (nextInst->isEnabled() && !nextInst->isComposite()) 
            {
                if (nextInst->name() == "Measure") 
                {
                    measureBitIdxs.emplace_back(nextInst->bits()[0]);
                }
                else
                {
                    // Apply change-of-basis gates (if any)
                    nextInst->accept(this);
                }
            }
        }
        
        const double result = calcExpectationValueZ(m_stateVec, measureBitIdxs);
        // Restore the state vector
        m_stateVec = cachedStateVec;
        return result;
    }

    void QppVisitor::allocateQubits(size_t in_nbQubits) 
    {
        assert(in_nbQubits > 0);
        if (xacc::verbose)
        {
            std::cout << ">> Allocate : " << in_nbQubits << " qubits.\n";
            std::cout << ">> State before allocate: " << qpp::disp(m_stateVec, ", ") << "\n";
        }
        const std::vector<qpp::idx> initialState(in_nbQubits, 0);
        auto new_stateVec = qpp::mket(initialState);
        m_stateVec = qpp::kron(new_stateVec, m_stateVec);
        const std::vector<qpp::idx> new_dims(in_nbQubits, 2);
        m_dims.insert(m_dims.end(), new_dims.begin(), new_dims.end());
        if (xacc::verbose)
        {
            std::cout << ">> State after allocate: " << qpp::disp(m_stateVec, ", ") << "\n";
        }
    }
}}
