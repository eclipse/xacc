/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "InstructionIterator.hpp"
#include <gtest/gtest.h>
#include "Compiler.hpp"
// #include "CircuitOptimizer.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "CommonGates.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"

using namespace xacc;
using namespace xacc::quantum;
const std::string uccsdSrc = R"uccsdSrc(__qpu__ void foo77(qbit q, double theta0, double theta1) {
   X(q[0]);
   X(q[1]);
   H(q[3]);
   Rx(q[1], pi/2);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta0);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   H(q[3]);
   Rx(q[1], -pi/2);
   Rx(q[3], pi/2);
   H(q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta0);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   Rx(q[3], -pi/2);
   H(q[1]);
   H(q[3]);
   Rx(q[2], pi/2);
   H(q[1]);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   Rx(q[2], -pi/2);
   H(q[1]);
   H(q[0]);
   Rx(q[3], pi/2);
   Rx(q[2], pi/2);
   Rx(q[1], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   Rx(q[2], -pi/2);
   Rx(q[1], -pi/2);
   H(q[0]);
   H(q[3]);
   H(q[2]);
   H(q[1]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   H(q[2]);
   H(q[1]);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   H(q[2]);
   Rx(q[1], pi/2);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   H(q[2]);
   Rx(q[1], -pi/2);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   H(q[2]);
   H(q[1]);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   H(q[2]);
   H(q[1]);
   H(q[0]);
   Rx(q[2], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   Rz(q[2], -theta0);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[2], -pi/2);
   H(q[0]);
   H(q[2]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   Rz(q[2], theta0);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[2]);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   Rx(q[2], pi/2);
   H(q[1]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   Rx(q[2], -pi/2);
   H(q[1]);
   Rx(q[0], -pi/2);
   H(q[3]);
   Rx(q[2], pi/2);
   Rx(q[1], pi/2);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   Rx(q[2], -pi/2);
   Rx(q[1], -pi/2);
   Rx(q[0], -pi/2);
   H(q[3]);
   H(q[2]);
   Rx(q[1], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   H(q[2]);
   Rx(q[1], -pi/2);
   H(q[0]);
   }
   )uccsdSrc";

TEST(CircuitOptimizerTester, checkOptimize) {

    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(uccsdSrc)->getComposites()[0];
    std::vector<double> p{0.0,0.0};
    auto fevaled = (*f)(p);

    // auto provider = xacc::getIRProvider("quantum");

    // auto ir = provider->createIR();
    // ir->addComposite(fevaled);

    CountGatesOfTypeVisitor<CNOT> countCx(fevaled);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");
    opt->apply(fevaled, nullptr);

    // optF = optF->enabledView();//std::dynamic_pointer_cast<Circuit>(optF->enabledView());
    CountGatesOfTypeVisitor<CNOT> countCx2(fevaled);
    CountGatesOfTypeVisitor<Rz> countRz(fevaled);
    EXPECT_EQ(0, countRz.countGates());
    EXPECT_EQ(0, countCx2.countGates());
    EXPECT_EQ(2, fevaled->nInstructions());
    EXPECT_EQ("X", fevaled->getInstruction(0)->name());
    EXPECT_EQ("X", fevaled->getInstruction(1)->name());
    EXPECT_EQ(std::vector<std::size_t>{0}, fevaled->getInstruction(0)->bits());
    EXPECT_EQ(std::vector<std::size_t>{1}, fevaled->getInstruction(1)->bits());

    std::cout << "FINAL CIRCUIT:\n" << fevaled->toString() << "\n";

}

TEST(CircuitOptimizerTester, checkRemove0StrRotation) {
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void test_rot(qbit q, double x) {
        H(q[2]);
        CX(q[0],q[1]);
        Rx(q[1], 0.0 * x);
        CX(q[0],q[1]);
        Rx(q[1], -pi/2);
        H(q[2]);
    })")
                 ->getComposites()[0];

 auto opt = xacc::getService<IRTransformation>("circuit-optimizer");

    opt->apply(f, nullptr);
     std::cout << "HOWDY:\n" << f->toString() << "\n";
  EXPECT_EQ(1, f->nInstructions());
}

TEST(CircuitOptimizerTester, checkAdjRotations) {
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void foo(qbit q) {
        H(q[0]);
        Rx(q[1], pi/2);
        Rx(q[1], -pi/2);
        H(q[2]);
        Rz(q[3], 0.5);
        Rz(q[3], 0.5);
    })")
                 ->getComposites()[0];
     auto provider = xacc::getIRProvider("quantum");

    // auto ir = provider->createIR();

    // ir->addComposite(f);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");

    opt->apply(f, nullptr);
    // auto optF = newir->getComposites()[0];

    // optF = optF->enabledView();
    CountGatesOfTypeVisitor<Rz> countRz(f);
    CountGatesOfTypeVisitor<Rx> countRx(f);

    EXPECT_EQ(3, f->nInstructions());
    EXPECT_EQ(1, countRz.countGates());
    EXPECT_EQ(0, countRx.countGates());
    EXPECT_TRUE(f->getInstruction(2)->getParameter(0).as<double>()==1.0 );
    std::cout << "FINAL CIRCUIT:\n" << f->toString() << "\n";

}

TEST(CircuitOptimizerTester, checkSimple) {

    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void foo1(qbit q) {
        CNOT(q[0],q[1]);
        CNOT(q[0],q[1]);
    })")
                 ->getComposites()[0];

  auto provider = xacc::getIRProvider("quantum");

    auto ir = provider->createIR();
    ir->addComposite(f);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");
    opt->apply(f,nullptr);
    // auto optF = newir->getComposites()[0];

    // optF = optF->enabledView();
    EXPECT_EQ(0, f->nInstructions());

    f = c->compile(R"(__qpu__ void foo2(qbit q) {
        H(q[0]);
        CNOT(q[0],q[1]);
        CNOT(q[0],q[1]);
        H(q[0]);
    })")
            ->getComposites()[0];

    // ir = provider->createIR();
    // ir->addComposite(f);

    opt->apply(f, nullptr);
    // optF = newir->getComposites()[0];
    // optF = optF->enabledView();

    EXPECT_EQ(0, f->nInstructions());

    f = c->compile(R"(__qpu__ void foo3(qbit q) {
        CNOT(q[0],q[1]);
        H(q[1]);
        H(q[1]);
        CNOT(q[0],q[1]);
    })")
            ->getComposites()[0];

    // ir = provider->createIR();
    // ir->addComposite(f);

    opt->apply(f, nullptr);
    // optF = newir->getComposites()[0];

    // optF = f->enabledView();
    EXPECT_EQ(0, f->nInstructions());

}

TEST(CircuitOptimizerTester, checkPermuteAndCancelXGate) {
    // Case 1: back-to-back X gates
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test1(qbit q) {
                X(q[0]);
                X(q[0]);
                X(q[0]);
                X(q[1]);
                X(q[0]);
                X(q[1]);
                X(q[1]);
            })")->getComposites()[0];
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        EXPECT_EQ(1, program->nInstructions());
        // Only remain X(q[1]) instruction
        EXPECT_EQ("X", program->getInstruction(0)->name());
        EXPECT_EQ(1, program->getInstruction(0)->bits()[0]);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 2: Permutation barriers
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test2(qbit q) {
                X(q[0]);
                H(q[0]);
                X(q[0]);
                X(q[1]);
                CX(q[1], q[0]);
                X(q[1]);
                X(q[2]);
                Z(q[2]);
                X(q[2]);
            })")->getComposites()[0];
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        const auto nbInstBefore = program->nInstructions();
        optimizer->apply(program, nullptr);
        // No removal can be done
        EXPECT_EQ(nbInstBefore, program->nInstructions());
    }

    // Case 3: Permutation and cancel
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        // 3 cases of permitted permutation: I gate (same qubit); CNOT (target qubit); arbitrary gates on different qubits.
        auto program = compiler->compile(
            R"(__qpu__ void test3(qbit q) {
                X(q[0]);
                I(q[0]);
                X(q[0]);
                X(q[1]);
                CX(q[0], q[1]);
                X(q[1]);
                X(q[2]);
                Z(q[0]);
                X(q[2]);
            })")->getComposites()[0];
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // We have removed all X gates.
        EXPECT_EQ(3, program->nInstructions());
        for (int i = 0; i < program->nInstructions(); ++i) {
            EXPECT_NE("X", program->getInstruction(i)->name());
        }
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }
}

TEST(CircuitOptimizerTester, checkHadamardGateReduction) {
    // Case 1: H-P-H
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test4(qbit q) {
                H(q[0]);
                Rz(q[0], 1.57079632679489661923);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);

        EXPECT_EQ(3, program->nInstructions());
        // Become P_dag - H - P_dag
        EXPECT_EQ("Rz", program->getInstruction(0)->name());
        EXPECT_EQ("H", program->getInstruction(1)->name());
        EXPECT_EQ("Rz", program->getInstruction(2)->name());
        EXPECT_NEAR(-M_PI_2, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);
        EXPECT_NEAR(-M_PI_2, program->getInstruction(2)->getParameter(0).as<double>(), 1e-12);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 2: H-P_dag-H
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test5(qbit q) {
                H(q[0]);
                Rz(q[0], -1.57079632679489661923);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);

        EXPECT_EQ(3, program->nInstructions());
        // Become P - H - P
        EXPECT_EQ("Rz", program->getInstruction(0)->name());
        EXPECT_EQ("H", program->getInstruction(1)->name());
        EXPECT_EQ("Rz", program->getInstruction(2)->name());
        EXPECT_NEAR(M_PI_2, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);
        EXPECT_NEAR(M_PI_2, program->getInstruction(2)->getParameter(0).as<double>(), 1e-12);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 3: H-H-CNOT-H-H
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test6(qbit q) {
                H(q[0]);
                H(q[1]);
                CX(q[0], q[1]);
                H(q[1]);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);

        EXPECT_EQ(1, program->nInstructions());
        // Become CNOT q[1], q[0] (invert the control/target)
        EXPECT_EQ("CNOT", program->getInstruction(0)->name());
        EXPECT_EQ(1, program->getInstruction(0)->bits()[0]);
        EXPECT_EQ(0, program->getInstruction(0)->bits()[1]);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 4: H-P-CNOT-P_dag-H
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test7(qbit q) {
                H(q[0]);
                Rz(q[0], 1.57079632679489661923);
                CX(q[1], q[0]);
                Rz(q[0], -1.57079632679489661923);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // Reduce two H gates
        EXPECT_EQ(3, program->nInstructions());
        // Become P_dag - CNOT - P
        EXPECT_EQ("Rz", program->getInstruction(0)->name());
        EXPECT_NEAR(-M_PI_2, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);

        // CNOT stays the same
        EXPECT_EQ("CNOT", program->getInstruction(1)->name());
        EXPECT_EQ(1, program->getInstruction(1)->bits()[0]);
        EXPECT_EQ(0, program->getInstruction(1)->bits()[1]);

        EXPECT_EQ("Rz", program->getInstruction(2)->name());
        EXPECT_NEAR(M_PI_2, program->getInstruction(2)->getParameter(0).as<double>(), 1e-12);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 5: H-P_dag-CNOT-P-H
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test8(qbit q) {
                H(q[0]);
                Rz(q[0], -1.57079632679489661923);
                CX(q[1], q[0]);
                Rz(q[0], 1.57079632679489661923);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // Reduce two H gates
        EXPECT_EQ(3, program->nInstructions());
        // Become P - CNOT - P_dag
        EXPECT_EQ("Rz", program->getInstruction(0)->name());
        EXPECT_NEAR(M_PI_2, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);

        // CNOT stays the same
        EXPECT_EQ("CNOT", program->getInstruction(1)->name());
        EXPECT_EQ(1, program->getInstruction(1)->bits()[0]);
        EXPECT_EQ(0, program->getInstruction(1)->bits()[1]);

        EXPECT_EQ("Rz", program->getInstruction(2)->name());
        EXPECT_NEAR(-M_PI_2, program->getInstruction(2)->getParameter(0).as<double>(), 1e-12);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // Case 6: H-P_dag-CNOT^k-P-H : multiple CNOT with the same target qubit
    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        auto program = compiler->compile(
            R"(__qpu__ void test9(qbit q) {
                H(q[0]);
                Rz(q[0], -1.57079632679489661923);
                CX(q[1], q[0]);
                CX(q[2], q[0]);
                CX(q[3], q[0]);
                CX(q[4], q[0]);
                CX(q[5], q[0]);
                CX(q[6], q[0]);
                Rz(q[0], 1.57079632679489661923);
                H(q[0]);
            })")->getComposites()[0];

        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        const auto nbInstructionsBefore = program->nInstructions();
        optimizer->apply(program, nullptr);
        // Reduce two H gates
        EXPECT_EQ(nbInstructionsBefore - 2, program->nInstructions());
        // Become P - CNOT^k - P_dag
        EXPECT_EQ("Rz", program->getInstruction(0)->name());
        EXPECT_NEAR(M_PI_2, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);

        // CNOT's stays the same
        for (int i = 1; i < program->nInstructions() - 1; ++i) {
            EXPECT_EQ("CNOT", program->getInstruction(i)->name());
            EXPECT_EQ(0, program->getInstruction(i)->bits()[1]);
        }

        // Last gate is P_dag
        EXPECT_EQ("Rz", program->getInstruction(program->nInstructions() - 1)->name());
        EXPECT_NEAR(-M_PI_2, program->getInstruction(program->nInstructions() - 1)->getParameter(0).as<double>(), 1e-12);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    // TODO: Add more complex test cases, e.g. combinations of multiple patterns.
}

TEST(CircuitOptimizerTester, checkRotationMergingUsingPhasePolynomials) {
    const auto countRzGates = [](const std::shared_ptr<CompositeInstruction>& in_program){
        int count = 0;
        for (int i = 0; i < in_program->nInstructions(); ++i) {
            const auto& inst = in_program->getInstruction(i);
            if (inst->name() == "Rz") {
                ++count;
            }
        }
        return count;
    };

    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        // Simple test: circuit (7) in page 13 of https://arxiv.org/pdf/1710.07345.pdf
        auto program = compiler->compile(
            R"(__qpu__ void test10(qbit q) {
                H(q[0]);
                Rz(q[1], 1.0);
                CNOT(q[0], q[1]);
                Rz(q[1], 2.0);
                CNOT(q[0], q[1]);
                Rz(q[0], 3.0);
                Rz(q[1], 4.0);
                CNOT(q[1], q[0]);
                X(q[1]);
                H(q[1]);
                H(q[0]);
            })")->getComposites()[0];



        const auto gateCountBefore = program->nInstructions();
        const auto rzCountBefore = countRzGates(program);
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // We remove one Rz gate
        EXPECT_EQ(program->nInstructions(), gateCountBefore -1);
        EXPECT_EQ(countRzGates(program), rzCountBefore - 1);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        // Test: circuit (8) in page 14 of https://arxiv.org/pdf/1710.07345.pdf
        auto program = compiler->compile(
            R"(__qpu__ void test11(qbit q) {
                H(q[0]);
                H(q[1]);
                H(q[2]);
                Rz(q[1], 1.0);
                Rz(q[2], 2.0);
                CNOT(q[1], q[0]);
                CNOT(q[1], q[2]);
                Rz(q[0], 3.0);
                CNOT(q[0], q[1]);
                H(q[2]);
                CNOT(q[1], q[2]);
                CNOT(q[0], q[1]);
                Rz(q[1], 4.0);
                H(q[1]);
                H(q[0]);
            })")->getComposites()[0];
        const auto gateCountBefore = program->nInstructions();
        const auto rzCountBefore = countRzGates(program);
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // We remove one Rz gate (last one in the figure)
        EXPECT_EQ(program->nInstructions(), gateCountBefore -1);
        EXPECT_EQ(countRzGates(program), rzCountBefore - 1);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }

    {
        auto compiler = xacc::getService<xacc::Compiler>("xasm");
        // Test circuit pruning: similar to circuit (8) in page 14 of https://arxiv.org/pdf/1710.07345.pdf,
        // but we swap (ctrl, target) qubits of CNOT#4 => it must correctly identify it as a termination point.
        auto program = compiler->compile(
            R"(__qpu__ void test12(qbit q) {
                H(q[0]);
                H(q[1]);
                H(q[2]);
                Rz(q[1], 1.0);
                Rz(q[2], 2.0);
                CNOT(q[1], q[0]);
                CNOT(q[1], q[2]);
                Rz(q[0], 3.0);
                CNOT(q[0], q[1]);
                H(q[2]);
                CNOT(q[2], q[1]);
                CNOT(q[0], q[1]);
                Rz(q[1], 4.0);
                H(q[1]);
                H(q[0]);
            })")->getComposites()[0];
        const auto gateCountBefore = program->nInstructions();
        const auto rzCountBefore = countRzGates(program);
        auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
        optimizer->apply(program, nullptr);
        // We cannot remove any Rz gates in this case:
        // the last Rz (which can be merged in the previous case) is now outside the subcircuit.
        EXPECT_EQ(program->nInstructions(), gateCountBefore);
        EXPECT_EQ(countRzGates(program), rzCountBefore);
        std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";
    }
}

TEST(CircuitOptimizerTester, checkCCCX) {
  auto compiler = xacc::getService<xacc::Compiler>("staq");
  auto program = compiler
                     ->compile(
                         R"#(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg c[4];
rz(pi/2) q[3];
ry(pi/4) q[3];
cx q[0],q[3];
ry(-pi/4) q[3];
cx q[0],q[3];
rz(pi/2) q[0];
ry(pi/4) q[0];
rz(-pi/2) q[3];
rz(pi/2) q[3];
ry(pi/8) q[3];
cx q[1],q[3];
ry(-pi/8) q[3];
cx q[1],q[3];
cx q[1],q[0];
ry(-pi/4) q[0];
cx q[1],q[0];
rz(-pi/2) q[0];
rz(pi/2) q[0];
ry(pi/4) q[0];
rz(pi/2) q[1];
ry(pi/2) q[1];
rz(-pi/2) q[3];
rz(pi/2) q[3];
ry(pi/8) q[3];
cx q[2],q[3];
ry(-pi/8) q[3];
cx q[2],q[3];
cx q[2],q[0];
ry(-pi/4) q[0];
cx q[2],q[0];
rz(-pi/2) q[0];
rz(pi/2) q[0];
cx q[2],q[1];
ry(-pi/2) q[1];
cx q[2],q[1];
rz(-pi/2) q[1];
rz(-pi/2) q[3];
rz(pi/2) q[3];
cx q[1],q[3];
ry(pi/8) q[3];
cx q[1],q[3];
cx q[1],q[0];
ry(pi/4) q[0];
cx q[1],q[0];
ry(-pi/4) q[0];
rz(-pi/2) q[0];
ry(-pi/8) q[3];
rz(-pi/2) q[3];
rz(pi/2) q[3];
cx q[0],q[3];
ry(pi/4) q[3];
cx q[0],q[3];
rz(pi/2) q[0];
ry(pi/4) q[0];
cx q[1],q[0];
ry(-pi/4) q[0];
cx q[1],q[0];
rz(-pi/2) q[0];
rz(pi/2) q[0];
rz(pi/2) q[1];
cx q[2],q[1];
ry(pi/2) q[1];
cx q[2],q[1];
ry(-pi/2) q[1];
rz(-pi/2) q[1];
cx q[2],q[0];
ry(pi/4) q[0];
cx q[2],q[0];
ry(-pi/4) q[0];
rz(-pi/2) q[0];
rz(pi/2) q[0];
cx q[1],q[0];
ry(pi/4) q[0];
cx q[1],q[0];
ry(-pi/4) q[0];
rz(-pi/2) q[0];
ry(-pi/4) q[3];
rz(-pi/2) q[3];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];
measure q[3] -> c[3];
)#")
                     ->getComposites()[0];
  std::cout << "HOWDY CIRCUIT:\n" << program->toString() << "\n";

  // Evaluate all possible inputs.
  const auto getTruthTable = [](auto program) {
    std::vector<std::string> results;
    // 4 bits: run all test cases
    for (int dec_val = 0; dec_val < (1 << 4); ++dec_val) {
      auto provider = xacc::getService<IRProvider>("quantum");
      auto test_program =
          provider->createComposite("test" + std::to_string(dec_val));

      for (size_t bitIdx = 0; bitIdx < 4; ++bitIdx) {
        if ((dec_val & (1 << bitIdx)) == (1 << bitIdx)) {
          test_program->addInstruction(
              provider->createInstruction("X", bitIdx));
        }
      }

      test_program->addInstructions(program->getInstructions());

      auto accelerator = xacc::getAccelerator("qpp", {{"shots", 1024}});
      auto buffer = xacc::qalloc(4);
      accelerator->execute(buffer, test_program);
      std::cout << "Test case: " << dec_val << "\n";
      buffer->print();
      EXPECT_EQ(buffer->getMeasurements().size(), 1);
      results.emplace_back(buffer->getMeasurements()[0]);
    }
    return results;
  };

  const auto truthTableBefore = getTruthTable(program);
  EXPECT_EQ(truthTableBefore.size(), 16);

  // Apply optimization
  auto optimizer = xacc::getService<IRTransformation>("circuit-optimizer");
  const auto nBefore = program->nInstructions();
  optimizer->apply(program, nullptr);
  const auto nAfter = program->nInstructions();
  // Reduce instructions
  EXPECT_TRUE(nAfter < nBefore);
  std::cout << "FINAL CIRCUIT:\n" << program->toString() << "\n";

  const auto truthTableAfter = getTruthTable(program);
  EXPECT_EQ(truthTableAfter.size(), 16);
  EXPECT_EQ(truthTableBefore, truthTableAfter);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
