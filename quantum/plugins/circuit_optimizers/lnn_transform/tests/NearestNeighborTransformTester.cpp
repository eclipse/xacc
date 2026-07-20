#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace {
int countSwap(const std::shared_ptr<xacc::CompositeInstruction> in_program) {
  int result = 0;

  for (int i = 0; i < in_program->nInstructions(); ++i) {
    auto inst = in_program->getInstruction(i);
    if (inst->name() == "Swap") {
      result++;
    }
  }

  return result;
}
} // namespace

TEST(NearestNeighborTransformTester, checkSimple) {
  auto c = xacc::getService<xacc::Compiler>("xasm");
  auto f = c->compile(R"(__qpu__ void test1(qbit q) {
        X(q[0]);
        CNOT(q[1], q[3]);
        H(q[7]);
        CNOT(q[6], q[7]);
        CNOT(q[7], q[3]);
        X(q[6]);
        Swap(q[4], q[9]);
        S(q[5]);
    })")
               ->getComposites()[0];

  auto provider = xacc::getIRProvider("quantum");

  auto ir = provider->createIR();
  ir->addComposite(f);
  auto opt = xacc::getService<xacc::IRTransformation>("nnizer");
  const auto nInstBefore = f->nInstructions();
  const auto nbSwapGateBefore = countSwap(f);

  // No option: default max distance = 1
  opt->apply(f, nullptr);
  std::cout << "After LNN transform: \n" << f->toString() << "\n";

  const auto nbSwapGateAfter = countSwap(f);
  const auto nInstAfter = f->nInstructions();
  const auto newSwapGateCount = nbSwapGateAfter - nbSwapGateBefore;

  EXPECT_EQ(nInstAfter, nInstBefore + newSwapGateCount);
  // Check that no 2-qubit gates have distance > 1
  for (int i = 0; i < f->nInstructions(); ++i) {
    auto inst = f->getInstruction(i);
    if (inst->bits().size() == 2) {
      const int distance = inst->bits()[0] - inst->bits()[1];
      EXPECT_EQ(std::abs(distance), 1);
    }
  }
}

TEST(NearestNeighborTransformTester, checkDistanceConfig) {
  auto c = xacc::getService<xacc::Compiler>("xasm");
  auto f = c->compile(R"(__qpu__ void test2(qbit q) {
        X(q[0]);
        CNOT(q[1], q[9]);
        H(q[7]);
        CNOT(q[3], q[1]);
    })")
               ->getComposites()[0];

  auto provider = xacc::getIRProvider("quantum");

  auto ir = provider->createIR();
  ir->addComposite(f);
  auto opt = xacc::getService<xacc::IRTransformation>("nnizer");
  const auto nInstBefore = f->nInstructions();
  const auto nbSwapGateBefore = countSwap(f);
  // max distance = 2
  opt->apply(f, nullptr, {std::make_pair("max-distance", 2)});
  std::cout << "After LNN transform: \n" << f->toString() << "\n";

  const auto nbSwapGateAfter = countSwap(f);
  const auto nInstAfter = f->nInstructions();
  const auto newSwapGateCount = nbSwapGateAfter - nbSwapGateBefore;

  EXPECT_EQ(nInstAfter, nInstBefore + newSwapGateCount);
  auto lastInst = f->getInstruction(nInstAfter - 1);
  // The last instruction is CNOT(q[3], q[1])
  // (distance = 2, within the limit, hence no conversion)
  EXPECT_STREQ(lastInst->name().c_str(), "CNOT");
  EXPECT_EQ(lastInst->bits()[0], 3);
  EXPECT_EQ(lastInst->bits()[1], 1);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
