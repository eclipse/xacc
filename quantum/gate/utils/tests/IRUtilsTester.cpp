#include <gtest/gtest.h>
#include "CommonGates.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <random>
#include <iterator>
#include "IRUtils.hpp"
#include "xacc_observable.hpp"

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
  EXPECT_TRUE(result.validate(testCircs));
}

TEST(IRUtilsTester, checkObservable) {
  {
    auto H_N_2 = xacc::quantum::getObservable(
        "pauli", std::string("5.907 - 2.1433 X0X1 "
                             "- 2.1433 Y0Y1"
                             "+ .21829 Z0 - 6.125 Z1"));
    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters theta
        .qbit q
        X(q[0]);
        Ry(q[1], theta);
        CNOT(q[1],q[0]);
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz");
    auto evaled = ansatz->operator()({1.2345});
    const auto fsToExe = H_N_2->observe(evaled);
    auto result = ObservedAnsatz::fromObservedComposites(fsToExe);
    EXPECT_EQ(result.getBase()->toString(), evaled->toString());
    EXPECT_TRUE(result.validate(fsToExe));
  }

  {
    auto H_N_3 = xacc::quantum::getObservable(
        "pauli",
        std::string(
            "5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
            "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

    xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h3
        .parameters t0, t1
        .qbit q
        X(q[0]);
        exp_i_theta(q, t0, {{"pauli", "X0 Y1 - Y0 X1"}});
        exp_i_theta(q, t1, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
    auto ansatz = xacc::getCompiled("deuteron_ansatz_h3");
    auto evaled = ansatz->operator()({1.2345, 6.789});
    const auto fsToExe = H_N_3->observe(evaled);
    auto result = ObservedAnsatz::fromObservedComposites(fsToExe);
    EXPECT_EQ(result.getBase()->toString(), evaled->toString());
    EXPECT_TRUE(result.validate(fsToExe));
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
