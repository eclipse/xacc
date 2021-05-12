#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "GateFusion.hpp"
#include <unsupported/Eigen/KroneckerProduct>

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
        std::cout << "Result:\n" << result << "\n";
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
        Eigen::Matrix2cd IdMat = Eigen::Matrix2cd::Identity();
        const auto H = []() {
          Eigen::Matrix2cd result;
          result << 1.0 / std::sqrt(2), 1.0 / std::sqrt(2), 1.0 / std::sqrt(2),
              -1.0 / std::sqrt(2);
          return result;
        };
        Eigen::Matrix4cd matHkronI = Eigen::kroneckerProduct(H(), IdMat);       
        Eigen::Matrix4cd cnotMat;
        cnotMat <<  1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1,
                    0, 0, 1, 0;
       
        const auto flipKronOrder = [](const Eigen::Matrix4cd &in_mat) {
          Eigen::Matrix4cd result = Eigen::Matrix4cd::Zero();
          const std::vector<size_t> order{0, 2, 1, 3};
          for (size_t i = 0; i < in_mat.rows(); ++i) {
            for (size_t j = 0; j < in_mat.cols(); ++j) {
              result(order[i], order[j]) = in_mat(i, j);
            }
          }
          return result;
        };

        // Note: gate application and matrix multiplication is in reverse order
        // i.e. cnot after h means cnot * H
        const auto expectResult =  flipKronOrder(cnotMat * matHkronI);
        std::cout << "Expected result: \n " << expectResult << "\n";
        // Compare the result matrix vs. expected.
        const auto diff = (result - expectResult).norm();
        EXPECT_NEAR(diff, 0.0, 1e-6);
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
        // Q0 is the control, hence it is a map 1 (001)->5(101), 3 (011) -> 7 (111) and vice versa
        std::vector<std::pair<size_t, size_t>> expectedLoc {
            {0, 0}, {4, 4}, {2, 2}, {6, 6}, {1, 5}, { 5, 1}, {3, 7}, {7, 3}
        };
        std::cout << result << "\n";
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