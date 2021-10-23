/*******************************************************************************
 * Copyright (c) 2019-2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *   Austin Adams - adaption for GTRI testbed
 *******************************************************************************/
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IonTrapTwoQubitPass.hpp"

namespace {
// Give us some nice breathing room
const double ERR = 1e-4;
}

//
// Two-qubit pass tests
//

TEST(IonTrapTwoQubitPassTester, twoQubitPassDecomposesCnot)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void my_bell(qbit q) {
        H(q[0]);
        X(q[1]);
        CX(q[0],q[1]);
        Measure(q[0]);
        Measure(q[1]);
    })")->getComposites()[0];
    auto twoQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-2q-pass");

    xacc::quantum::IonTrapMSPhaseMap msPhases({{{0, 1}, {0.0, 0.0}}});
    twoQubitPass->apply(ir, nullptr, {{"ms-phases", &msPhases}});

    std::vector<std::size_t> bits0({0});
    std::vector<std::size_t> bits1({1});
    std::vector<std::size_t> bits01({0,1});

    EXPECT_EQ(9, ir->nInstructions());

    EXPECT_EQ("H", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());

    EXPECT_EQ("X", ir->getInstruction(1)->name());
    EXPECT_EQ(bits1, ir->getInstruction(1)->bits());

    EXPECT_EQ("Ry", ir->getInstruction(2)->name());
    EXPECT_EQ(bits0, ir->getInstruction(2)->bits());
    EXPECT_NEAR(-M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(2)->getParameter(0)),
                ERR);

    EXPECT_EQ("XX", ir->getInstruction(3)->name());
    EXPECT_EQ(bits01, ir->getInstruction(3)->bits());
    EXPECT_NEAR(M_PI/4.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(3)->getParameter(0)),
                ERR);

    EXPECT_EQ("Ry", ir->getInstruction(4)->name());
    EXPECT_EQ(bits0, ir->getInstruction(4)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(4)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rz", ir->getInstruction(5)->name());
    EXPECT_EQ(bits0, ir->getInstruction(5)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(5)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rx", ir->getInstruction(6)->name());
    EXPECT_EQ(bits1, ir->getInstruction(6)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(6)->getParameter(0)),
                ERR);

    EXPECT_EQ("Measure", ir->getInstruction(7)->name());
    EXPECT_EQ(bits0, ir->getInstruction(7)->bits());

    EXPECT_EQ("Measure", ir->getInstruction(8)->name());
    EXPECT_EQ(bits1, ir->getInstruction(8)->bits());
}

TEST(IonTrapTwoQubitPassTester, twoQubitPassDecomposesCZ)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void my_cz(qbit q) {
        CZ(q[1],q[0]);
    })")->getComposites()[0];
    auto twoQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-2q-pass");

    xacc::quantum::IonTrapMSPhaseMap msPhases({{{0, 1}, {0.0, 0.0}}});
    twoQubitPass->apply(ir, nullptr, {{"ms-phases", &msPhases}});

    std::vector<std::size_t> bits0({0});
    std::vector<std::size_t> bits1({1});
    std::vector<std::size_t> bits10({1,0});

    EXPECT_EQ(7, ir->nInstructions());

    EXPECT_EQ("H", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());

    EXPECT_EQ("Ry", ir->getInstruction(1)->name());
    EXPECT_EQ(bits1, ir->getInstruction(1)->bits());
    EXPECT_NEAR(-M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(1)->getParameter(0)),
                ERR);

    EXPECT_EQ("XX", ir->getInstruction(2)->name());
    EXPECT_EQ(bits10, ir->getInstruction(2)->bits());
    EXPECT_NEAR(M_PI/4.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(2)->getParameter(0)),
                ERR);

    EXPECT_EQ("Ry", ir->getInstruction(3)->name());
    EXPECT_EQ(bits1, ir->getInstruction(3)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(3)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rz", ir->getInstruction(4)->name());
    EXPECT_EQ(bits1, ir->getInstruction(4)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(4)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rx", ir->getInstruction(5)->name());
    EXPECT_EQ(bits0, ir->getInstruction(5)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(5)->getParameter(0)),
                ERR);

    EXPECT_EQ("H", ir->getInstruction(6)->name());
    EXPECT_EQ(bits0, ir->getInstruction(6)->bits());
}

TEST(IonTrapTwoQubitPassTester, twoQubitPassDecomposesSwap)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void my_swap(qbit q) {
        Swap(q[0],q[1]);
    })")->getComposites()[0];
    auto twoQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-2q-pass");

    xacc::quantum::IonTrapMSPhaseMap msPhases({{{0, 1}, {0.0, 0.0}}});
    twoQubitPass->apply(ir, nullptr, {{"ms-phases", &msPhases}});

    std::vector<std::size_t> bits0({0});
    std::vector<std::size_t> bits1({1});
    std::vector<std::size_t> bits01({0,1});
    std::vector<std::size_t> bits10({1,0});

    EXPECT_EQ(15, ir->nInstructions());

    // CNOT 0,1
    EXPECT_EQ("Ry", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());
    EXPECT_NEAR(-M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);

    EXPECT_EQ("XX", ir->getInstruction(1)->name());
    EXPECT_EQ(bits01, ir->getInstruction(1)->bits());
    EXPECT_NEAR(M_PI/4.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(1)->getParameter(0)),
                ERR);

    EXPECT_EQ("Ry", ir->getInstruction(2)->name());
    EXPECT_EQ(bits0, ir->getInstruction(2)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(2)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rz", ir->getInstruction(3)->name());
    EXPECT_EQ(bits0, ir->getInstruction(3)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(3)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rx", ir->getInstruction(4)->name());
    EXPECT_EQ(bits1, ir->getInstruction(4)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(4)->getParameter(0)),
                ERR);

    // CNOT 1,0
    EXPECT_EQ("Ry", ir->getInstruction(5)->name());
    EXPECT_EQ(bits1, ir->getInstruction(5)->bits());
    EXPECT_NEAR(-M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(5)->getParameter(0)),
                ERR);

    EXPECT_EQ("XX", ir->getInstruction(6)->name());
    EXPECT_EQ(bits10, ir->getInstruction(6)->bits());
    EXPECT_NEAR(M_PI/4.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(6)->getParameter(0)),
                ERR);

    EXPECT_EQ("Ry", ir->getInstruction(7)->name());
    EXPECT_EQ(bits1, ir->getInstruction(7)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(7)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rz", ir->getInstruction(8)->name());
    EXPECT_EQ(bits1, ir->getInstruction(8)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(8)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rx", ir->getInstruction(9)->name());
    EXPECT_EQ(bits0, ir->getInstruction(9)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(9)->getParameter(0)),
                ERR);

    // CNOT 0,1
    EXPECT_EQ("Ry", ir->getInstruction(10)->name());
    EXPECT_EQ(bits0, ir->getInstruction(10)->bits());
    EXPECT_NEAR(-M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(10)->getParameter(0)),
                ERR);

    EXPECT_EQ("XX", ir->getInstruction(11)->name());
    EXPECT_EQ(bits01, ir->getInstruction(11)->bits());
    EXPECT_NEAR(M_PI/4.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(11)->getParameter(0)),
                ERR);

    EXPECT_EQ("Ry", ir->getInstruction(12)->name());
    EXPECT_EQ(bits0, ir->getInstruction(12)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(12)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rz", ir->getInstruction(13)->name());
    EXPECT_EQ(bits0, ir->getInstruction(13)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(13)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rx", ir->getInstruction(14)->name());
    EXPECT_EQ(bits1, ir->getInstruction(14)->bits());
    EXPECT_NEAR(M_PI/2.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(14)->getParameter(0)),
                ERR);
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
