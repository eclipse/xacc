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

#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(ASwapTester, checkSimple) 
{
    {
        auto tmp = xacc::getService<Instruction>("ASWAP");
        auto aswap = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
        aswap->expand({ std::make_pair("nbQubits", 2), std::make_pair("nbParticles", 1), std::make_pair("timeReversalSymmetry", false) });
        // 2 variables (theta and phi) because we don't enable timeReversalSymmetry
        EXPECT_EQ(aswap->getVariables().size(), 2);
        // A-gate block has 7 instructions + 1 initial X gate
        EXPECT_EQ(aswap->nInstructions(), 8);
    }
    {
        auto tmp = xacc::getService<Instruction>("ASWAP");
        auto aswap = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
        aswap->expand({ std::make_pair("nbQubits", 2), std::make_pair("nbParticles", 1), std::make_pair("timeReversalSymmetry", true) });
        // only 1 variable if we enable timeReversalSymmetry
        EXPECT_EQ(aswap->getVariables().size(), 1);
        // A-gate block has 7 instructions + 1 initial X gate
        EXPECT_EQ(aswap->nInstructions(), 8);
    }
    {
        auto tmp = xacc::getService<Instruction>("ASWAP");
        auto aswap = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
        aswap->expand({ std::make_pair("nbQubits", 4), std::make_pair("nbParticles", 2), std::make_pair("timeReversalSymmetry", true) });
        // See FIG3: for the 4 qubits/2 particles case, we have a total of 6 A blocks + 2 X gates.
        const int nbInstrPerA = 7;
        const int nbA = 6;
        EXPECT_EQ(aswap->nInstructions(), nbInstrPerA*nbA + 2);
        // Expect: C(4,2) - 1 = 5 params
        EXPECT_EQ(aswap->getVariables().size(), 5);
    }
    {
        auto tmp = xacc::getService<Instruction>("ASWAP");
        auto aswap = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
        aswap->expand({ std::make_pair("nbQubits", 4), std::make_pair("nbParticles", 2), std::make_pair("timeReversalSymmetry", false) });
        // Expect: 2 * (C(4,2) - 1) = 10 params
        EXPECT_EQ(aswap->getVariables().size(), 10);
    }
}

TEST(ASwapTester, checkDeuteron2)
{
    // use our Qpp backend 
    auto accelerator = xacc::getAccelerator("qpp");
    auto H_N_2 = xacc::quantum::getObservable(
        "pauli", std::string("5.907 - 2.1433 X0X1 "
                            "- 2.1433 Y0Y1"
                            "+ .21829 Z0 - 6.125 Z1"));

    auto optimizer = xacc::getOptimizer("nlopt");
    // Use the ASWAP circuit as the ansatz
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters t0
        .qbit q
        ASWAP(q, t0, {{"nbQubits", 2}, {"nbParticles", 1}});
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

TEST(ASwapTester, checkDeuteron3)
{
    auto accelerator = xacc::getAccelerator("qpp");

    // Create the N=3 deuteron Hamiltonian
    auto H_N_3 = xacc::quantum::getObservable(
        "pauli",
        std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                    "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

    auto optimizer = xacc::getOptimizer("nlopt");

    // ASWAP with 3 qubits (orbitals) and 1 particle (electron)
    xacc::qasm(R"(
    .compiler xasm
    .circuit deuteron_ansatz_h3
    .parameters t0, t1
    .qbit q
    ASWAP(q, t0, t1, {{"nbQubits", 3}, {"nbParticles", 1}});
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz_h3");
    EXPECT_EQ(ansatz->getVariables().size(), 2);
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

TEST(ASwapTester, checkDeuteronH3) {
  auto accelerator = xacc::getAccelerator("qpp");

  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
      "pauli",
      std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                  "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  auto optimizer = xacc::getOptimizer("nlopt");
  auto tmp = xacc::getService<Instruction>("ASWAP");
  auto aswap = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
  aswap->expand({{"nbQubits", 3}, {"nbParticles", 1}});
  EXPECT_EQ(aswap->getVariables().size(), 2);
  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", aswap),
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
    xacc::Initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}