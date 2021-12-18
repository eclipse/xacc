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
#include "IonTrapOneQubitPass.hpp"
#include "IonTrapTwoQubitPass.hpp"

namespace {
// Give us some nice breathing room
const double ERR = 1e-4;
const double THRESHOLD = 1e-3;
}

//
// Single-qubit pass tests
//

TEST(IonTrapOneQubitPassTester, oneQubitPassExactX)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void my_x(qbit q) {
        X(q[0]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", true},
                                      {"keep-leading-rz", true}});

    std::vector<std::size_t> bits0({0});

    EXPECT_EQ(2, ir->nInstructions());

    EXPECT_EQ("Rphi", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());
    EXPECT_NEAR(0.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rphi", ir->getInstruction(1)->name());
    EXPECT_EQ(bits0, ir->getInstruction(1)->bits());
    EXPECT_NEAR(0.0,
                xacc::InstructionParameterToDouble(ir->getInstruction(1)->getParameter(0)),
                ERR);
}

TEST(IonTrapOneQubitPassTester, oneQubitPassCommuteXAroundXX)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void xx_x_test(qbit q) {
        X(q[0]);
        X(q[1]);
        XX(q[0], q[1], pi/4);
        X(q[0]);
        X(q[1]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", false},
                                      {"keep-leading-rz", true}});

    std::vector<std::size_t> bits01({0, 1});

    EXPECT_EQ(1, ir->nInstructions());

    EXPECT_EQ("XX", ir->getInstruction(0)->name());
    EXPECT_EQ(bits01, ir->getInstruction(0)->bits());
    EXPECT_NEAR(M_PI/4,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);
}

TEST(IonTrapOneQubitPassTester, oneQubitPassUpToZ)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void upToZ(qbit q) {
        H(q[0]);
        Measure(q[0]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", false},
                                      {"keep-rx-before-xx", true},
                                      {"keep-leading-rz", true}});

    std::vector<std::size_t> bits0({0});

    EXPECT_EQ(2, ir->nInstructions());

    EXPECT_EQ("Rphi", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());
    EXPECT_NEAR(-M_PI/2,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);

    EXPECT_EQ("Measure", ir->getInstruction(1)->name());
    EXPECT_EQ(bits0, ir->getInstruction(1)->bits());
}

TEST(IonTrapOneQubitPassTester, oneQubitPassFromZ)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void fromZ(qbit q) {
        H(q[0]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", true},
                                      {"keep-leading-rz", false}});

    std::vector<std::size_t> bits0({0});

    EXPECT_EQ(1, ir->nInstructions());

    EXPECT_EQ("Rphi", ir->getInstruction(0)->name());
    EXPECT_EQ(bits0, ir->getInstruction(0)->bits());
    EXPECT_NEAR(M_PI/2,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);
}

TEST(IonTrapOneQubitPassTester, oneQubitPassFromZtoX)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void fromZtoX(qbit q) {
        Y(q[0]);
        XX(q[0], q[1], pi/4);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", false},
                                      {"keep-leading-rz", false}});

    std::vector<std::size_t> bits0({0});
    std::vector<std::size_t> bits01({0,1});

    EXPECT_EQ(3, ir->nInstructions());

    EXPECT_EQ("XX", ir->getInstruction(0)->name());
    EXPECT_EQ(bits01, ir->getInstruction(0)->bits());
    EXPECT_NEAR(M_PI/4,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rphi", ir->getInstruction(1)->name());
    EXPECT_EQ(bits0, ir->getInstruction(1)->bits());
    EXPECT_NEAR(0,
                xacc::InstructionParameterToDouble(ir->getInstruction(1)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rphi", ir->getInstruction(2)->name());
    EXPECT_EQ(bits0, ir->getInstruction(2)->bits());
    EXPECT_NEAR(0,
                xacc::InstructionParameterToDouble(ir->getInstruction(2)->getParameter(0)),
                ERR);
}

TEST(IonTrapOneQubitPassTester, oneQubitPassFromZtoZ)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void fromZtoZ(qbit q) {
        Z(q[0]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", false},
                                      {"keep-rx-before-xx", true},
                                      {"keep-leading-rz", false}});

    std::vector<std::size_t> bits0({0});

    EXPECT_EQ(0, ir->nInstructions());
}

TEST(IonTrapOneQubitPassTester, oneQubitPassTrailingGates)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void fromZtoZ(qbit q) {
        XX(q[0], q[1], pi/4);
        X(q[1]);
        Y(q[0]);
        Measure(q[0]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", false},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", true},
                                      {"keep-leading-rz", true}});

    std::vector<std::size_t> bits0({0});
    std::vector<std::size_t> bits01({0,1});

    EXPECT_EQ(4, ir->nInstructions());

    EXPECT_EQ("XX", ir->getInstruction(0)->name());
    EXPECT_EQ(bits01, ir->getInstruction(0)->bits());
    EXPECT_NEAR(M_PI/4,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rphi", ir->getInstruction(1)->name());
    EXPECT_EQ(bits0, ir->getInstruction(1)->bits());
    EXPECT_NEAR(M_PI/2,
                xacc::InstructionParameterToDouble(ir->getInstruction(1)->getParameter(0)),
                ERR);

    EXPECT_EQ("Rphi", ir->getInstruction(2)->name());
    EXPECT_EQ(bits0, ir->getInstruction(2)->bits());
    EXPECT_NEAR(M_PI/2,
                xacc::InstructionParameterToDouble(ir->getInstruction(2)->getParameter(0)),
                ERR);

    EXPECT_EQ("Measure", ir->getInstruction(3)->name());
    EXPECT_EQ(bits0, ir->getInstruction(3)->bits());
}

TEST(IonTrapOneQubitPassTester, oneQubitPassIdentity)
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto ir = c->compile(R"(__qpu__ void identityTest(qbit q) {
        X(q[0]);
        XX(q[0], q[1], pi/4);
        X(q[0]);
        H(q[1]);
        H(q[1]);
    })")->getComposites()[0];
    auto oneQubitPass = xacc::getService<xacc::IRTransformation>("iontrap-1q-pass");

    oneQubitPass->apply(ir, nullptr, {{"threshold", THRESHOLD},
                                      {"keep-trailing-gates", true},
                                      {"keep-rz-before-meas", true},
                                      {"keep-rx-before-xx", false},
                                      {"keep-leading-rz", true}});

    std::vector<std::size_t> bits01({0,1});

    EXPECT_EQ(1, ir->nInstructions());

    EXPECT_EQ("XX", ir->getInstruction(0)->name());
    EXPECT_EQ(bits01, ir->getInstruction(0)->bits());
    EXPECT_NEAR(M_PI/4,
                xacc::InstructionParameterToDouble(ir->getInstruction(0)->getParameter(0)),
                ERR);
}


int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
