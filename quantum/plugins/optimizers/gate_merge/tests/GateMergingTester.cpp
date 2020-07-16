
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"


TEST(GateMergingTester, checkSingleQubitSimple) 
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    // This is identity: H-X-H is equal to Z => Z-Z = I
    auto f = c->compile(R"(__qpu__ void test1(qbit q) {
        Z(q[0]);
        H(q[0]);
        X(q[0]);
        H(q[0]);
    })")->getComposites()[0];

    auto opt = xacc::getService<xacc::IRTransformation>("single-qubit-gate-merging");
    opt->apply(f, nullptr);
    // No instruction after optimization
    EXPECT_EQ(f->nInstructions(), 0);
}

TEST(GateMergingTester, checkSingleQubitStopAtTwoQubitGate) 
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void test2(qbit q) {
        H(q[0]);
        X(q[0]);
        H(q[0]);
        CNOT(q[0], q[1]);
        H(q[0]);
    })")->getComposites()[0];

    auto opt = xacc::getService<xacc::IRTransformation>("single-qubit-gate-merging");
    opt->apply(f, nullptr);
    // Becomes: Z (Rz(pi)) - CNOT - H
    EXPECT_EQ(f->nInstructions(), 3);
    EXPECT_EQ(f->getInstruction(0)->name(), "Rz");
    EXPECT_NEAR(f->getInstruction(0)->getParameter(0).as<double>(), M_PI, 1e-6);
    EXPECT_EQ(f->getInstruction(1)->name(), "CNOT");
    EXPECT_EQ(f->getInstruction(2)->name(), "H");
}

TEST(GateMergingTester, checkMixing) 
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void test2(qbit q) {
        H(q[1]);
        CNOT(q[1], q[0]);
        H(q[0]);
        H(q[1]);
        X(q[0]);
        // Not involved
        CNOT(q[2], q[3]);
        X(q[1]);
        H(q[0]);
        H(q[1]);
        CNOT(q[0], q[2]);
        Z(q[1]);
    })")->getComposites()[0];

    auto opt = xacc::getService<xacc::IRTransformation>("single-qubit-gate-merging");
    opt->apply(f, nullptr);
    std::cout << "HOWDY:\n" << f->toString() << "\n";
    EXPECT_EQ(f->nInstructions(), 5);
}


int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}