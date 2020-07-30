
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"
#include "GateFusion.hpp"

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

TEST(GateMergingTester, checkTwoQubitSimple) 
{
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void test3(qbit q) {
        H(q[0]);
        H(q[1]);
        Rz(q[0], 1.234);
        Y(q[1]);
        T(q[0]);
        Z(q[1]);
        H(q[0]);
        H(q[1]);
        S(q[0]);
        H(q[1]);
        X(q[0]);
        Z(q[1]);
        CNOT(q[1], q[0]);
        H(q[0]);
        H(q[1]);
        CPhase(q[0], q[1], 1.123);
        H(q[0]);
        T(q[1]);
        X(q[0]);
        CNOT(q[0], q[1]);
        T(q[0]);
        //H(q[2]);
        Rx(q[0], 1.234);
        Ry(q[1], -2.456);
        H(q[1]);
        Rz(q[0], 3.124);
        H(q[0]);
        H(q[1]);
        Y(q[0]);
        H(q[0]);
        H(q[1]);
        T(q[0]);
        X(q[1]);
    })")->getComposites()[0];
   
    auto opt = xacc::getService<xacc::IRTransformation>("two-qubit-block-merging");
    const auto nbInstBefore = f->nInstructions();
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto circuitCopy = gateRegistry->createComposite("__COPY__");
    for (size_t i = 0; i < f->nInstructions(); ++i)
    {
        circuitCopy->addInstruction(f->getInstruction(i)->clone());
    }
    opt->apply(f, nullptr);
    std::cout << "HOWDY:\n" << f->toString() << "\n";
    const auto nbInstAfter = f->nInstructions();
    EXPECT_TRUE(nbInstAfter < nbInstBefore); 
    // Validate using gate fusion:
    auto fuser = xacc::getService<xacc::quantum::GateFuser>("default");
    fuser->initialize(circuitCopy);
    const Eigen::MatrixXcd uMatOriginal = fuser->calcFusedGate(2);
    fuser->initialize(f);
    const Eigen::MatrixXcd uMatAfter = fuser->calcFusedGate(2);
    // Compensate any global phase differences.
    // Find index of the largest element:
    size_t colIdx = 0;
    size_t rowIdx = 0;
    double maxVal = std::abs(uMatAfter(0,0));
    for (size_t i = 0; i < uMatAfter.rows(); ++i)
    {
        for (size_t j = 0; j < uMatAfter.cols(); ++j)
        {
            if (std::abs(uMatAfter(i,j)) > maxVal)
            {
                maxVal = std::abs(uMatAfter(i,j));
                colIdx = j;
                rowIdx = i;
            }
        }
    }

    const std::complex<double> globalFactor = uMatOriginal(rowIdx, colIdx) / uMatAfter(rowIdx, colIdx);
    auto uMatAfterFixedPhase = globalFactor * uMatAfter;
    for (size_t i = 0; i < uMatAfter.rows(); ++i)
    {
        for (size_t j = 0; j < uMatAfter.cols(); ++j)
        {
            EXPECT_NEAR(uMatAfterFixedPhase(i, j).real(), uMatOriginal(i, j).real(), 1e-9);
            EXPECT_NEAR(uMatAfterFixedPhase(i, j).imag(), uMatOriginal(i, j).imag(), 1e-9);
        }
    }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}