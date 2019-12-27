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

int main(int argc, char **argv) {
  xacc::Initialize(); 
  
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  
  xacc::Finalize(); 
  
  return result;
}
