/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 *******************************************************************************/

// This example demonstrates XACC Optimal Control API usage.
// In particular, XACC has a GOAT optimization service that can optimize
// drive signal envelop (analytical form) to achieve a target Hamiltonian.
#include "xacc.hpp"

int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    const std::vector<double> initParams { 10.0 };
    // Max time that we need to achieve the target U
    const double tMax = 10000;
    // 2 qubits
    const int dimension = 2;

    // GOAT pulse optimization configs:
    xacc::HeterogeneousMap configs {
        std::make_pair("method", "GOAT"),
        std::make_pair("dimension", dimension),
        // Target unitary is a ZX (Z0X1) gate
        // (cross-resonant interaction)
        std::make_pair("target-U", "Z0X1"),
        // Control parameter (used in the control function)
        // For API demonstration purposes, we use the simplified Hamiltonian, i.e. eq. (14) of Rigetti and Devoret (2010)
        // Assuming we want to tune the width (sigma) of the Gaussian pulse
        std::make_pair("control-params", std::vector<std::string> { "sigma" }),
        // Parameters are from Rigetti and Devoret (2010): Delta = 3.3 GHz, omegaxx = 0.13 GHz
        std::make_pair("control-funcs", std::vector<std::string> { "(exp(-t^2/(2*sigma^2))*0.13/(4*3.3))" }),
        // The list of Hamiltonian terms that are modulated by the control functions
        std::make_pair("control-H", std::vector<std::string> { "Z0X1" }),
        // Initial params
        std::make_pair("initial-parameters", initParams),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("quantum-control", configs);
    const auto result = optimizer->optimize();

    xacc::Finalize();

  return 0;
}