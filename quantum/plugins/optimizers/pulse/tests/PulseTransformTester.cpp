#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "GateFusion.hpp"

TEST(PulseTransformTester, checkSimple) 
{
    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    // Single gate
    {
        auto program = compiler->compile(
            R"(__qpu__ void test0(qbit q) {
                H(q[0]);
            })")->getComposites()[0];
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(program);
        auto result = fuser->calcFusedGate(1);
        EXPECT_NEAR(result(0,0).real(), 1.0 / std::sqrt(2.), 1e-12);
        EXPECT_NEAR(result(0,1).real(), 1.0 / std::sqrt(2.), 1e-12);
        EXPECT_NEAR(result(1,0).real(), 1.0 / std::sqrt(2.), 1e-12);
        EXPECT_NEAR(result(1,1).real(), -1.0 / std::sqrt(2.), 1e-12);
    }
    
    // Cascading gates
    {
        // This should be equivalent to an X gate
        auto program = compiler->compile(
            R"(__qpu__ void test1(qbit q) {
                H(q[0]);
                Z(q[0]);
                H(q[0]);
            })")->getComposites()[0];
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(program);
        auto result = fuser->calcFusedGate(1);
        EXPECT_NEAR(result(0,0).real(), 0.0, 1e-12);
        EXPECT_NEAR(result(0,1).real(), 1.0, 1e-12);
        EXPECT_NEAR(result(1,0).real(), 1.0, 1e-12);
        EXPECT_NEAR(result(1,1).real(), 0.0, 1e-12);
    }

    // Circuit with two-qubit gate
    {
        // This should be equivalent to an X gate
        auto program = compiler->compile(
            R"(__qpu__ void test2(qbit q) {
                H(q[0]);
                CNOT(q[0], q[1]);
            })")->getComposites()[0];
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(program);
        auto result = fuser->calcFusedGate(2);
        std::cout << result << "\n";
        // TODO: add checks
    }
    // QFT circuit
    {
        auto tmp = xacc::getService<Instruction>("qft");
        auto qft = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
        qft->expand({std::make_pair("nq", 2)});
        std::cout << "QFT Circuit: \n" << qft->toString() << "\n";
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(qft);
        auto result = fuser->calcFusedGate(2);
        std::cout << "QFT Matrix: \n" << result << "\n";
        // TODO: add checks
    }
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}