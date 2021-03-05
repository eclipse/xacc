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

TEST(QppAcceleratorTester, testDeuteron)
{
    auto accelerator = xacc::getAccelerator("qpp");
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

TEST(QppAcceleratorTester, testDeuteronVqeH2)
{
    // Use Qpp accelerator
    auto accelerator = xacc::getAccelerator("qpp");
    EXPECT_EQ(accelerator->name(), "qpp");

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

TEST(QppAcceleratorTester, testDeuteronVqeH3)
{
    // Use Qpp accelerator
    auto accelerator = xacc::getAccelerator("qpp");
    EXPECT_EQ(accelerator->name(), "qpp");

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

TEST(QppAcceleratorTester, testShots)
{
    const int nbShots = 100;
    {
        auto accelerator = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
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
        auto accelerator = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
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
        auto accelerator = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
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

// Port DDCL test suite to QPP
/*TEST(QppAcceleratorTester, testDDCL)
{
    // Set up
    {
        const std::string src =
            R"rucc(__qpu__ void f(qbit q, double t0, double t1, double t2) {
            Rx(q[0], t0);
            Ry(q[0], t1);
            Rx(q[0], t2);
        })rucc";
        auto acc = xacc::getAccelerator("qpp");
        auto compiler = xacc::getCompiler("xasm");
        // compile source to the compilation DB
        auto ir = compiler->compile(src, acc);
    }

    // Use a reasonable number of shots to save test time
    const int nbShots = 128;
    // checkJSSimpleGradientFree
    {
        auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(1);

        auto simple = xacc::getCompiled("f");

        // get cobyla optimizer
        auto optimizer = xacc::getOptimizer(
            "nlopt", xacc::HeterogeneousMap{std::make_pair("initial-parameters", std::vector<double>{1.3, 1.4, -.05}), std::make_pair("nlopt-maxeval", 50)});

        std::vector<double> target{.5, .5};

        auto ddcl = xacc::getService<xacc::Algorithm>("ddcl");
        EXPECT_TRUE(ddcl->initialize(
            {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
            std::make_pair("target_dist", target), std::make_pair("loss", "js"),
            std::make_pair("optimizer", optimizer)}));
        ddcl->execute(buffer);

        auto loss = buffer->getInformation("opt-val").as<double>();
        EXPECT_NEAR(loss, 0.0, 1e-2);
        auto a = (*buffer)["opt-params"].as<std::vector<double>>();
        for (auto& aa : a) std::cout << aa << " ";
        std::cout << std::endl;
    }

    //  checkMMDSimpleGradientFree)
    {
        auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(1);

        auto simple = xacc::getCompiled("f");

        // get cobyla optimizer
        auto optimizer = xacc::getOptimizer(
                                            "nlopt", xacc::HeterogeneousMap{std::make_pair("nlopt-maxeval", 50)});

        std::vector<double> target{.5, .5};

        auto ddcl = xacc::getService<xacc::Algorithm>("ddcl");
        EXPECT_TRUE(ddcl->initialize(
                                    {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
                                    std::make_pair("target_dist", target), std::make_pair("loss", "mmd"),
                                    std::make_pair("optimizer", optimizer)}));
        ddcl->execute(buffer);

        auto loss = buffer->getInformation("opt-val").as<double>();
        EXPECT_NEAR(loss, 0.0, 1e-2);
        auto a = (*buffer)["opt-params"].as<std::vector<double>>();
        for (auto& aa : a) std::cout << aa << " ";
        std::cout << std::endl;
    }

    // checkJSSimpleWithGradient)
    {
        auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(1);

        auto simple = xacc::getCompiled("f");

        // get cobyla optimizer
        auto optimizer = xacc::getOptimizer(
            "nlopt", xacc::HeterogeneousMap{std::make_pair("nlopt-maxeval", 50), std::make_pair("nlopt-ftol", 1e-4), std::make_pair("initial-parameters", std::vector<double>{1.5, 0, 1.5}),
                                    std::make_pair("nlopt-optimizer", "l-bfgs")});

        std::vector<double> target{.5, .5};

        auto ddcl = xacc::getService<xacc::Algorithm>("ddcl");
        EXPECT_TRUE(ddcl->initialize(
            {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
            std::make_pair("target_dist", target), std::make_pair("loss", "js"),
            std::make_pair("gradient", "js-parameter-shift"),
            std::make_pair("optimizer", optimizer)}));
        ddcl->execute(buffer);

        auto loss = buffer->getInformation("opt-val").as<double>();
        EXPECT_NEAR(loss, 0.0, 1e-2);
    }

    // checkMMDSimpleWithGradient
    {
        auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
        auto buffer = xacc::qalloc(1);

        auto simple = xacc::getCompiled("f");

        // get cobyla optimizer
        auto optimizer = xacc::getOptimizer(
            "nlopt", xacc::HeterogeneousMap{std::make_pair("nlopt-maxeval", 50), std::make_pair("nlopt-ftol", 1e-4), std::make_pair("initial-parameters", std::vector<double>{1.5, 0, 1.5}),
                                    std::make_pair("nlopt-optimizer", "l-bfgs")});

        std::vector<double> target{.5, .5};

        auto ddcl = xacc::getService<xacc::Algorithm>("ddcl");
        EXPECT_TRUE(ddcl->initialize(
            {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
            std::make_pair("target_dist", target), std::make_pair("loss", "mmd"),
            std::make_pair("gradient", "mmd-parameter-shift"),
            std::make_pair("optimizer", optimizer)}));
        ddcl->execute(buffer);

        auto loss = buffer->getInformation("opt-val").as<double>();
        EXPECT_NEAR(loss, 0.0, 1e-2);
    }
}
*/
TEST(QppAcceleratorTester, testConditional)
{
    // Get reference to the Accelerator
    xacc::set_verbose(true);
    const int nbTests = 100;
    auto accelerator =  xacc::getAccelerator("qpp", { std::make_pair("shots", nbTests) });
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

TEST(QppAcceleratorTester, testISwap)
{
    // Get reference to the Accelerator
    xacc::set_verbose(false);
    const int nbShots = 100;
    auto accelerator =  xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
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

TEST(QppAcceleratorTester, testFsim)
{
    // Get reference to the Accelerator
    const int nbShots = 1000;
    auto accelerator =  xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
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

TEST(QppAcceleratorTester, testDeuteronVqeH3Shots)
{
    // Use Qpp accelerator
    const int nbShots = 10000;
    auto accelerator = xacc::getAccelerator("qpp", { std::make_pair("shots", nbShots) });
    EXPECT_EQ(accelerator->name(), "qpp");

    // Create the N=3 deuteron Hamiltonian
    auto H_N_3 = xacc::quantum::getObservable(
        "pauli",
        std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                    "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

    auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-maxeval", 100)});

    // JIT map Quil QASM Ansatz to IR
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h3_2
        .parameters t0, t1
        .qbit q
        X(q[0]);
        exp_i_theta(q, t0, {{"pauli", "X0 Y1 - Y0 X1"}});
        exp_i_theta(q, t1, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz_h3_2");

    // Get the VQE Algorithm and initialize it
    auto vqe = xacc::getAlgorithm("vqe");
    vqe->initialize({std::make_pair("ansatz", ansatz),
                    std::make_pair("observable", H_N_3),
                    std::make_pair("accelerator", accelerator),
                    std::make_pair("optimizer", optimizer)});

    xacc::set_verbose(true);
    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(3);
    vqe->execute(buffer);

    // Expected result: -2.04482
    // Tol: 0.25 (~10% of the true value)
    // (since we are using shots, hence will introduce randomness to the optimizer)
    std::cout << "Energy = " << (*buffer)["opt-val"].as<double>() << "\n";
    // EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -2.04482, 0.25);
}

TEST(QppAcceleratorTester, testVqeMode)
{
    // Use Qpp accelerator
    auto accelerator = xacc::getAccelerator("qpp", {{"vqe-mode", true}});
    EXPECT_EQ(accelerator->name(), "qpp");

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

TEST(QppAcceleratorTester, testExecutionInfo)
{
    auto accelerator = xacc::getAccelerator("qpp");
    xacc::qasm(R"(
        .compiler xasm
        .circuit test_bell_exe
        .qbit q
        H(q[0]);
        CNOT(q[0],q[1]);
    )");
    auto bell = xacc::getCompiled("test_bell_exe");

    // Allocate some qubits and execute
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, bell);

    auto exeInfo = accelerator->getExecutionInfo();
    EXPECT_GT(exeInfo.size(), 0);
    auto waveFn =
        accelerator->getExecutionInfo<xacc::ExecutionInfo::WaveFuncPtrType>(
            xacc::ExecutionInfo::WaveFuncKey);
    // for (const auto &elem : *waveFn) {
    //   std::cout << elem << "\n";
    // }
    // 2 qubits => 4 elements
    EXPECT_EQ(waveFn->size(), 4);
}

// Check the runtime resolve of multiple if statements
// referring to a measurement result (pointed to by a creg)
TEST(QppAcceleratorTester, checkMultipleIfStmts) 
{
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void iqpe(qbit q) {
H(q[0]);
X(q[1]);
// Prepare the state:
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
H(q[0]);
// Measure and reset
Measure(q[0], cReg[0]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
// Conditional rotation
if(cReg[0]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[1]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
if(cReg[0]) {
  Rz(q[0], -pi/4);
}
if(cReg[1]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[2]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
if(cReg[0]) {
  Rz(q[0], -pi/8);
}
if(cReg[1]) {
  Rz(q[0], -pi/4);
}
if(cReg[2]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[3]);
})");
  auto accelerator = xacc::getAccelerator("qpp", {{"shots", 1024}});
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, ir->getComposites()[0]);
  buffer->print();
  // Expect: 1101 (lsb) = 11 (decimal)
  EXPECT_EQ(buffer->getMeasurementCounts()["1101"], 1024);
}

TEST(QppAcceleratorTester, testFtqcApply)
{
    auto provider = xacc::getIRProvider("quantum");
    const int nbShots = 1024;
    int nb00 = 0;
    int nb11 = 0;
    for (int i = 0; i < nbShots; ++i) {
      auto accelerator = xacc::getAccelerator("qpp");
      auto buffer = xacc::qalloc(1);
      auto hGate = provider->createInstruction("H", 0);
      accelerator->apply(buffer, hGate);
      buffer->setSize(2);
      auto cxGate = provider->createInstruction("CX", {0, 1});
      accelerator->apply(buffer, cxGate);
      accelerator->apply(buffer, provider->createInstruction("Measure", 0));
      accelerator->apply(buffer, provider->createInstruction("Measure", 1));
      const auto bitString = buffer->single_measurements_to_bitstring();
      EXPECT_TRUE(bitString == "00" || bitString == "11");
      if (bitString == "00") {
        nb00++;
      } else {
        nb11++;
      }
    }
    // Make sure we have both values.
    EXPECT_TRUE(nb00 > 100 && nb11 > 100);
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
