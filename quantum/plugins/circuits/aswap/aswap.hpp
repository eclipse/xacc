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

#pragma once

#include "Circuit.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace circuits {

class ASWAP : public xacc::quantum::Circuit 
{
public:
    ASWAP();
    bool expand(const xacc::HeterogeneousMap& runtimeOptions) override;
    const std::vector<std::string> requiredKeys() override;
    const std::vector<std::string>& getParamList() const { return m_paramList; }
    DEFINE_CLONE(ASWAP);
private:
    void initializeParamList();
    void constructCircuit();
    // A few short-hand funcs to help circuit construction
    void CNOT(int in_q1, int in_q2);
    void X(int in_qubit);
    void H(int in_qubit);
    void Rx(int in_qubit, const std::string& in_param);
    void Rx(int in_qubit, double in_angle);
    void Ry(int in_qubit, const std::string& in_param);
    void Ry(int in_qubit, double in_angle);
    void Rz(int in_qubit, const std::string& in_param);
    void Rz(int in_qubit, double in_angle);

    // Add an A gate (see FIG. 2) between two arbitrary qubits
    // The rotation gates are parameterized by the variable names provided.
    void addAGate(int in_q1, int in_q2, const std::string& in_thetaParam, const std::string& in_phiParam);
private:
    int m_nbQubits;
    size_t m_nbParticles;
    bool m_timeReversalSymmetry;
    double m_szSymmetry;
    std::vector<std::string> m_paramList;
    std::shared_ptr<xacc::IRProvider> m_gateRegistry;    
};

} // namespace circuits
} // namespace xacc