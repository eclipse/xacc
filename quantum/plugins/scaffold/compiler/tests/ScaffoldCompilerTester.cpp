#include "ScaffCompiler.hpp"
#include "gtest/gtest.h"
using namespace scaffold;

TEST(ScaffoldCompilerTester, checkSimple) {
  auto src = R"src(int main() {
    qbit reg[2];
    cbit out[2];
    H(reg[0]);
    CNOT(reg[0], reg[1]);
    out[0] = MeasZ(reg[0]);
    out[1] = MeasZ(reg[1]);
    return 0;
})src";

  ScaffCompiler compiler;
  auto IR = compiler.compile(src);
  EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
