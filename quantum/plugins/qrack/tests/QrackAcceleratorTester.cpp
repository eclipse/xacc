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
#include <gtest/gtest.h>
#include <string>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"

namespace {
    template <typename T>
    std::vector<T> linspace(T a, T b, size_t N)
    {
        T h = (b - a) / static_cast<T>(N - 1);
        std::vector<T> xs(N);
        typename std::vector<T>::iterator x;
        T val;
        for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        {
            *x = val;
        }
        return xs;
    }
}

TEST(QrackAcceleratorTester, testDeuteron)
{
    auto accelerator = xacc::getAccelerator("qrack");
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      X(q[0]);
      Ry(q[1], t);
      CX(q[1], q[0]);
      H(q[0]);
      H(q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })", accelerator);

    auto program = ir->getComposite("ansatz");
    // Expected results from deuteron_2qbit_xasm_X0X1
    const std::vector<double> expectedResults {
        0.0,
        -0.324699,
        -0.614213,
        -0.837166,
        -0.9694,
        -0.996584,
        -0.915773,
        -0.735724,
        -0.475947,
        -0.164595,
        0.164595,
        0.475947,
        0.735724,
        0.915773,
        0.996584,
        0.9694,
        0.837166,
        0.614213,
        0.324699,
        0.0
    };

    const auto angles = linspace(-xacc::constants::pi, xacc::constants::pi, 20);
    for (size_t i = 0; i < angles.size(); ++i)
    {
        auto buffer = xacc::qalloc(2);
        auto evaled = program->operator()({ angles[i] });
        accelerator->execute(buffer, evaled);
        EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResults[i], 1e-6);
    }
}

TEST(QrackAcceleratorTester, testDeuteronVqeH2)
{
    // Use Qrack accelerator
    auto accelerator = xacc::getAccelerator("qrack");
    EXPECT_EQ(accelerator->name(), "qrack");

    // Create the N=2 deuteron Hamiltonian
    auto H_N_2 = xacc::quantum::getObservable(
        "pauli", std::string("5.907 - 2.1433 X0X1 "
                            "- 2.1433 Y0Y1"
                            "+ .21829 Z0 - 6.125 Z1"));

    auto optimizer = xacc::getOptimizer("nlopt");
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters theta
        .qbit q
        X(q[0]);
        Ry(q[1], theta);
        CNOT(q[1],q[0]);
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz");

    // Get the VQE Algorithm and initialize it
    auto vqe = xacc::getAlgorithm("vqe");
    vqe->initialize({std::make_pair("ansatz", ansatz),
                    std::make_pair("observable", H_N_2),
                    std::make_pair("accelerator", accelerator),
                    std::make_pair("optimizer", optimizer)});

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(2);
    vqe->execute(buffer);

    // Expected result: -1.74886
    EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 1e-4);
}

TEST(QrackAcceleratorTester, testDeuteronVqeH3)
{
    // Use Qrack accelerator
    auto accelerator = xacc::getAccelerator("qrack");
    EXPECT_EQ(accelerator->name(), "qrack");

    // Create the N=3 deuteron Hamiltonian
    auto H_N_3 = xacc::quantum::getObservable(
        "pauli",
        std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                    "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

    auto optimizer = xacc::getOptimizer("nlopt");

    // JIT map Quil QASM Ansatz to IR
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h3
        .parameters t0, t1
        .qbit q
        X(q[0]);
        exp_i_theta(q, t0, {{"pauli", "X0 Y1 - Y0 X1"}});
        exp_i_theta(q, t1, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz_h3");

    // Get the VQE Algorithm and initialize it
    auto vqe = xacc::getAlgorithm("vqe");
    vqe->initialize({std::make_pair("ansatz", ansatz),
                    std::make_pair("observable", H_N_3),
                    std::make_pair("accelerator", accelerator),
                    std::make_pair("optimizer", optimizer)});

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(3);
    vqe->execute(buffer);

    // Expected result: -2.04482
    EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -2.04482, 1e-4);
}

TEST(QrackAcceleratorTester, testShots)
{
    const int nbShots = 100;
    {
        auto accelerator = xacc::getAccelerator("qrack", { std::make_pair("shots", nbShots) });
        // Allocate some qubits
        auto buffer = xacc::qalloc(2);
        auto quilCompiler = xacc::getCompiler("quil");
        // Nothing, should be 00
        auto ir = quilCompiler->compile(R"(__qpu__ void test1(qbit q) {
MEASURE 0 [0]
MEASURE 1 [1]
})", accelerator);
        accelerator->execute(buffer, ir->getComposites()[0]);
        EXPECT_EQ(buffer->getMeasurementCounts().size(), 1);
        EXPECT_EQ(buffer->getMeasurementCounts()["00"], nbShots);
        buffer->print();
    }
    {
        auto accelerator = xacc::getAccelerator("qrack", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(2);
        auto quilCompiler = xacc::getCompiler("quil");
        // Expected "11"
        auto ir = quilCompiler->compile(R"(__qpu__ void test2(qbit q) {
X 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})", accelerator);
        accelerator->execute(buffer, ir->getComposites()[0]);

        EXPECT_EQ(buffer->getMeasurementCounts().size(), 1);
        EXPECT_EQ(buffer->getMeasurementCounts()["11"], nbShots);
    }
    {
        auto accelerator = xacc::getAccelerator("qrack", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(2);
        auto quilCompiler = xacc::getCompiler("quil");
        // Bell states
        auto ir = quilCompiler->compile(R"(__qpu__ void test3(qbit q) {
H 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})", accelerator);
        accelerator->execute(buffer, ir->getComposites()[0]);
        EXPECT_EQ(buffer->getMeasurementCounts().size(), 2);
        // Only 00 and 11 states
        EXPECT_EQ(buffer->getMeasurementCounts()["11"] + buffer->getMeasurementCounts()["00"], nbShots);
    }
}

TEST(QrackAcceleratorTester, testConditional)
{
    // Get reference to the Accelerator
    xacc::set_verbose(true);
    const int nbTests = 100;
    auto accelerator =  xacc::getAccelerator("qrack", { std::make_pair("shots", nbTests) });
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void teleport(qbit q) {
        // State preparation (Bob)
        // Rotate an arbitrary angle
        Rx(q[0], -0.123);
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
        // Rotate back (-theta) on the teleported qubit
        Rx(q[2], 0.123);
        // Measure teleported qubit
        // This should always be zero (perfect teleportation)
        Measure(q[2]);
    })", accelerator);

    auto program = ir->getComposite("teleport");
    // Allocate some qubits
    auto buffer = xacc::qalloc(3);
    // Create a classical buffer to store measurement results (for conditional)
    buffer->setName("q");
    xacc::storeBuffer(buffer);
    accelerator->execute(buffer, program);

    int resultCount = 0;

    for (const auto& bitStrToCount : buffer->getMeasurementCounts())
    {
        const std::string& bitString = bitStrToCount.first;
        // Last bit must be zero (teleported)
        EXPECT_TRUE(bitString.back() == '0');
        resultCount += bitStrToCount.second;
    }

    EXPECT_EQ(resultCount, nbTests);
}

TEST(QrackAcceleratorTester, testISwap)
{
    // Get reference to the Accelerator
    xacc::set_verbose(false);
    const int nbShots = 100;
    auto accelerator =  xacc::getAccelerator("qrack", { std::make_pair("shots", nbShots) });
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void testISwap(qbit q) {
        X(q[0]);
        iSwap(q[0], q[3]);
        Measure(q[0]);
        Measure(q[1]);
        Measure(q[2]);
        Measure(q[3]);
        Measure(q[4]);
    })", accelerator);

    auto program = ir->getComposite("testISwap");
    // Allocate some qubits (5)
    auto buffer = xacc::qalloc(5);
    accelerator->execute(buffer, program);
    // 10000 => i00010 after iswap
    buffer->print();
    EXPECT_EQ(buffer->getMeasurementCounts()["00010"], nbShots);
}

TEST(QrackAcceleratorTester, testFsim)
{
    // Get reference to the Accelerator
    const int nbShots = 1000;
    auto accelerator =  xacc::getAccelerator("qrack", { std::make_pair("shots", nbShots) });
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto ir = xasmCompiler->compile(R"(__qpu__ void testFsim(qbit q, double x, double y) {
        X(q[0]);
        fSim(q[0], q[2], x, y);
        Measure(q[0]);
        Measure(q[2]);
    })", accelerator);

    auto program = ir->getComposites()[0]; 
    const auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 10);

    for (const auto& a : angles) 
    {
        auto buffer = xacc::qalloc(3);
        auto evaled = program->operator()({ a, 0.0 });
        accelerator->execute(buffer, evaled);
        const auto expectedProb = std::sin(a) * std::sin(a);
        std::cout << "Angle = " << a << "\n";
        buffer->print();
        // fSim mixes 01 and 10 states w.r.t. the theta angle.
        EXPECT_NEAR(buffer->computeMeasurementProbability("01"), expectedProb, 0.1);
        EXPECT_NEAR(buffer->computeMeasurementProbability("10"), 1.0 - expectedProb, 0.1);
    }
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
