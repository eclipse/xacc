#include <gtest/gtest.h>
#include "CommonGates.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <random>
#include <iterator>
#include "IRUtils.hpp"

using namespace xacc::quantum;

namespace {

template <typename Iter, typename RandomGenerator>
Iter selectRandomly(Iter start, Iter end, RandomGenerator &g) {
  std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
  std::advance(start, dis(g));
  return start;
}

template <typename Iter> Iter selectRandomly(Iter start, Iter end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return selectRandomly(start, end, gen);
}
} // namespace
TEST(IRUtilsTester, checkSimple) {
  auto base = std::make_shared<Circuit>("base");
  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto rz = std::make_shared<Rz>(1, 3.1415);
  auto z = std::make_shared<Z>(2);
  base->addInstruction(x);
  base->addInstruction(h);
  base->addInstruction(cn1);
  base->addInstruction(rz);
  base->addInstruction(z);
  std::vector<std::shared_ptr<xacc::quantum::Gate>> InstructionSet{h, x, z, rz};
  auto measure = std::make_shared<Measure>(1);

  const int NB_CIRCUITS = 10;

  std::vector<std::shared_ptr<xacc::CompositeInstruction>> testCircs;
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  for (int i = 0; i < NB_CIRCUITS; ++i) {
    std::shared_ptr<xacc::CompositeInstruction> newComp =
        gateRegistry->createComposite("__COMPOSITE__" + std::to_string(i));
    for (int idx = 0; idx < base->nInstructions(); ++idx) {
      newComp->addInstruction(base->getInstruction(idx)->clone());
    }
    // Add a random gate to the base circuit
    auto randomGate =
        selectRandomly(InstructionSet.begin(), InstructionSet.end());
    newComp->addInstruction((*randomGate)->clone());
    newComp->addInstruction(measure->clone());
    testCircs.emplace_back(newComp);
  }

  for (const auto &circ : testCircs) {
    std::cout << "HOWDY: \n" << circ->toString() << "\n";
  }
  auto result = ObservedAnsatz::fromObservedComposites(testCircs);
  
  EXPECT_EQ(result.getBase()->nInstructions(), base->nInstructions());
  EXPECT_EQ(result.getBase()->toString(), base->toString());
  for (int i = 0; i < result.getObservedSubCircuits().size(); ++i) {
    auto circ = result.getObservedSubCircuits()[i];
    // All circuits have one extra gate and a Measure
    EXPECT_EQ(circ->nInstructions(), 2);
    std::shared_ptr<xacc::CompositeInstruction> newComp =
        gateRegistry->createComposite("__RECOVERED__" + std::to_string(i));
    for (int idx = 0; idx < result.getBase()->nInstructions(); ++idx) {
      newComp->addInstruction(result.getBase()->getInstruction(idx)->clone());
    }
    for (int idx = 0; idx < circ->nInstructions(); ++idx) {
      newComp->addInstruction(circ->getInstruction(idx)->clone());
    }
    auto originalCirc = testCircs[i];
    // The recovered: base + obs sub-circuit matches the original 
    EXPECT_EQ(newComp->toString(), originalCirc->toString());
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
