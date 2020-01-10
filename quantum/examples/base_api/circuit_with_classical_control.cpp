/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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

#include "xacc.hpp"

int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    xacc::set_verbose(true);
    // Get reference to the Accelerator
    auto accelerator =  xacc::getAccelerator("qpp");

    // Allocate some qubits
    auto buffer = xacc::qalloc(3);
    // Create a classical buffer to store measurement results (for conditional)
    buffer->setName("q");
    xacc::storeBuffer(buffer);
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void teleport(qbit q) {
        // State preparation (Bob)
        Rx(q[0], 0.6);
        // Bell channel setup
        H(q[1]);
        CX(q[1], q[2]);
        // Alice Bell measurement
        CX(q[0], q[1]);
        H(q[0]);
        Measure(q[0]);
        Measure(q[1]);
        // Correction
        if (q[0])
        {
            Z(q[2]);
        }
        if (q[1])
        {
            X(q[2]);
        }
        // Measure teleported qubit
        Measure(q[2]);
    })", accelerator);

    accelerator->execute(buffer, ir->getComposites()[0]);

    buffer->print();

    xacc::Finalize();

    return 0;
}
