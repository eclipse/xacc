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
#include "Observable.hpp"

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
        EXPECT_EQ(layers[0].ops.size(), 1);
        EXPECT_EQ(layers[0].preOps.size(), 1);
        EXPECT_EQ(layers[0].postOps.size(), 3);
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
        EXPECT_EQ(layers[0].ops.size(), 2);
        EXPECT_EQ(layers[1].ops.size(), 2);
        EXPECT_EQ(layers[0].paramInds.size(), 2);
        EXPECT_EQ(layers[1].paramInds.size(), 2);
        // Direct compare (via stringtify) the layered circuit vs. the original
        auto gateRegistry = xacc::getService<IRProvider>("quantum");
        auto reconstructedKernel = gateRegistry->createComposite("test_recon");
        reconstructedKernel->addVariable("t0");
        reconstructedKernel->addVariable("t1");
        reconstructedKernel->addVariable("t2");
        reconstructedKernel->addVariable("t3");
        
        // Test layer 1
        reconstructedKernel->addInstructions(layers[0].preOps);
        reconstructedKernel->addInstructions(layers[0].ops);
        reconstructedKernel->addInstructions(layers[0].postOps);
        EXPECT_EQ(reconstructedKernel->toString(), program->toString());
        EXPECT_EQ(layers[0].preOps.size(), 3);

        // Test layer 2
        reconstructedKernel->clear();
        reconstructedKernel->addInstructions(layers[1].preOps);
        reconstructedKernel->addInstructions(layers[1].ops);
        reconstructedKernel->addInstructions(layers[1].postOps);
        EXPECT_EQ(reconstructedKernel->toString(), program->toString());
        EXPECT_EQ(layers[1].preOps.size(), 7);
    }
}

TEST(QuantumNatualGradientTester, checkCircuitGen)
{
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz3(qbit q, double t0, double t1, double t2, double t3) {
        // State-prep 
        Ry(q[0], pi/4);
        Ry(q[1], pi/3);
        Ry(q[2], pi/7);
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

    auto program = ir->getComposite("ansatz3");
    std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
    observable->fromString("Y0");

    auto qng = xacc::getService<AlgorithmGradientStrategy>("quantum-natural-gradient");
    EXPECT_TRUE(qng->initialize({std::make_pair("observable", observable)}));
    const std::vector<double> params(4, 0.0);
    auto kernels = qng->getGradientExecutions(program, params);
    // Ki = 4
    // KiKj = 4*4 = 16
    // out of 16 KiKj terms, 4 KiKi terms are identity terms, hence no circuits.
    EXPECT_EQ(kernels.size(), 20 - 4);
} 

TEST(QuantumNatualGradientTester, checkVQE)
{
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz4(qbit q, double t0, double t1, double t2, double t3) {
        // State-prep 
        Ry(q[0], pi/4);
        Ry(q[1], pi/3);
        Ry(q[2], pi/7);
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

    auto program = ir->getComposite("ansatz4");
    std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
    observable->fromString("Y0");

    auto qng = xacc::getService<AlgorithmGradientStrategy>("quantum-natural-gradient");
    EXPECT_TRUE(qng->initialize({std::make_pair("observable", observable)}));
    const std::vector<double> params { 0.432, -0.123, 0.543, 0.233 };
    auto kernels = qng->getGradientExecutions(program, params);

    auto optimizer = xacc::getOptimizer("mlpack");
    auto vqe = xacc::getService<Algorithm>("vqe");
    auto acc = xacc::getAccelerator("qpp");
    EXPECT_TRUE(vqe->initialize({std::make_pair("ansatz", program),
                                std::make_pair("accelerator", acc),
                                std::make_pair("observable", observable),
                                std::make_pair("optimizer", optimizer),
                                std::make_pair("gradient_strategy", "quantum-natural-gradient")}));
    auto buffer = xacc::qalloc(4);
    vqe->execute(buffer);
    const double finalCostValue = (*buffer)["opt-val"].as<double>();
    std::cout << "Energy = " << finalCostValue << "\n";
    // Check this plot: ~ -0.6
    // https://pennylane.ai/qml/demos/tutorial_quantum_natural_gradient.html#quantum-natural-gradient-optimization
    EXPECT_NEAR(finalCostValue, -0.6, 0.1);
} 

// Numerical tests
TEST(QuantumNatualGradientTester, testSingleQubitRotations)
{
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void testRotation(qbit q, double t0, double t1) {
        Rx(q[0], t0);
        Ry(q[0], t1);
    })", nullptr);

    auto program = ir->getComposite("testRotation");
    std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
    observable->fromString("Z0");    
    auto qng = xacc::getService<AlgorithmGradientStrategy>("quantum-natural-gradient");
    EXPECT_TRUE(qng->initialize({std::make_pair("observable", observable)}));
    const std::vector<double> params { 0.11, 0.12 };
    auto optimizer = xacc::getOptimizer("mlpack", { std::make_pair("initial-parameters", params) });
    auto vqe = xacc::getService<Algorithm>("vqe");
    auto acc = xacc::getAccelerator("qpp");
    EXPECT_TRUE(vqe->initialize({std::make_pair("ansatz", program),
                                std::make_pair("accelerator", acc),
                                std::make_pair("observable", observable),
                                std::make_pair("optimizer", optimizer),
                                std::make_pair("gradient_strategy", "quantum-natural-gradient")}));
    auto buffer = xacc::qalloc(1);
    vqe->execute(buffer);
    const double finalCostValue = (*buffer)["opt-val"].as<double>();
    std::cout << "Energy = " << finalCostValue << "\n";
    // TODO: we have analytical results for this, we need a way to test this.  
    EXPECT_NEAR(finalCostValue, -1.0, 0.1);
} 

int main(int argc, char **argv) 
{
    xacc::Initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}