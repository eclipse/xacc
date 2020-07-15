
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"


TEST(GateMergingTester, checkSingleQubitSimple) 
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void test1(qbit q) {
        Z(q[0]);
        H(q[0]);
        X(q[0]);
        H(q[0]);
    })")->getComposites()[0];

    auto opt = xacc::getService<xacc::IRTransformation>("single-qubit-gate-merging");
    opt->apply(f, nullptr);
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
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}