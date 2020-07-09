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

#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "QuantumNaturalGradient.hpp"

using namespace xacc;
using namespace xacc::algorithm;

TEST(QuantumNatualGradientTester, checkLayers) 
{
    // Single layer
    {
        auto xasmCompiler = xacc::getCompiler("xasm");
        auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz1(qbit q, double t) {
            X(q[0]);
            Ry(q[1], t);
            CX(q[1], q[0]);
            H(q[0]);
            H(q[1]);
        })", nullptr);

        auto program = ir->getComposite("ansatz1");
        auto layers = ParametrizedCircuitLayer::toParametrizedLayers(program);
        EXPECT_EQ(layers.size(), 1);
        EXPECT_EQ(layers[0].ops.size(), program->nInstructions());
        EXPECT_EQ(layers[0].paramInds.size(), 1);
        EXPECT_EQ(layers[0].paramInds[0], 0);
    }
    // Two layers
    {
        auto xasmCompiler = xacc::getCompiler("xasm");
        auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz2(qbit q, double t0, double t1, double t2, double t3) {
            // State-prep 
            Ry(q[0], pi/4);
            Ry(q[1], pi/3);
            Ry(q[2], pi/2);
            // Parametrized gates (layer 1)
            Rz(q[0], t0);
            Rz(q[1], t1);
            CX(q[0], q[1]);
            CX(q[1], q[2]);
            // Parametrized gates (layer 2)
            Ry(q[1], t2);
            Rx(q[2], t3);
            CX(q[0], q[1]);
            CX(q[1], q[2]);
        })", nullptr);

        auto program = ir->getComposite("ansatz2");
        auto layers = ParametrizedCircuitLayer::toParametrizedLayers(program);
        EXPECT_EQ(layers.size(), 2);
        EXPECT_EQ(layers[0].ops.size(), 7);
        EXPECT_EQ(layers[1].ops.size(), 4);
        EXPECT_EQ(layers[0].paramInds.size(), 2);
        EXPECT_EQ(layers[1].paramInds.size(), 2);
        // Direct compare (via stringtify) the layered circuit vs. the original
        auto gateRegistry = xacc::getService<IRProvider>("quantum");
        auto reconstructedKernel = gateRegistry->createComposite("test_recon");
        reconstructedKernel->addVariable("t0");
        reconstructedKernel->addVariable("t1");
        reconstructedKernel->addVariable("t2");
        reconstructedKernel->addVariable("t3");
        reconstructedKernel->addInstructions(layers[0].ops);
        reconstructedKernel->addInstructions(layers[1].ops);
        EXPECT_EQ(reconstructedKernel->toString(), program->toString());
    }
}

int main(int argc, char **argv) 
{
    xacc::Initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}