#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "GateFusion.hpp"
namespace {
    const std::complex<double> I(0.0, 1.0);
}

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

    // Two qubit gate with non-continuous index
    // CNOT b/w q[0] and q[2] 
    {
        auto program = compiler->compile(
            R"(__qpu__ void test3(qbit q) {
                CNOT(q[0], q[2]);
            })")->getComposites()[0];
        auto fuser = xacc::getService<GateFuser>("default");
        fuser->initialize(program);
        auto result = fuser->calcFusedGate(3);
        // Expected location (row, column) that has an element 1.0
        // Q0 is the control, hence it is a map 4(100)->5(101), 6->7 and vice versa
        std::vector<std::pair<size_t, size_t>> expectedLoc {
            {0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 5}, { 5, 4}, {6, 7}, {7, 6}
        };
        // std::cout << result << "\n";
        for (const auto& loc : expectedLoc)
        {
            // These entries are 1.0
            EXPECT_NEAR(result(loc.first, loc.second).real(), 1.0, 1e-12);
        }
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
        // Expected matrix
        Eigen::MatrixXcd QftMat{ Eigen::MatrixXcd::Zero(4, 4) }; 
        QftMat <<   1, 1, 1, 1,
                    1, I, -1, -I,
                    1, -1, 1, -1,
                    1, -I, -1, I;
        QftMat *= 0.5;
        std::cout << "Expected QFT Matrix: \n" << result << "\n";
        // Compare the result matrix vs. expected.
        const auto diff = (result - QftMat).norm();
        EXPECT_NEAR(diff, 0.0, 1e-6);
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