
#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(TriQPlacementTester, checkSimple) {
  auto irt = xacc::getIRTransformation("triQ");
  auto compiler = xacc::getCompiler("xasm");
  auto program = compiler
                     ->compile(R"(__qpu__ void test(qreg q) {
      T(q[0]);
      T(q[0]);
      CNOT(q[0], q[1]);
      X(q[1]);
  })")
                     ->getComposite("test");

  irt->apply(program, nullptr);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
