#include "gtest/gtest.h"

#include "XACC.hpp"

TEST(XACCLangCompilerTester, checkSimple) {
  auto src = R"src(

__qpu__ void bell(qbit &q) {
  H(0);
  CX(0, 1);
  Measure(0);
  Measure(1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Allocate a register of 2 qubits
  qbit q = xacc::qalloc(2);

  // Execute the bell state computation
  bell(q);

  // Print the results
  q->print();

  xacc::Finalize();

  return 0;
})src";

  auto compiler = xacc::getCompiler("xacclang");
//   xacc::Compiler::XACCLangCompiler compiler = new xacc::Compiler::XACCLangCompiler();
  auto IR = compiler -> compile(src);
  EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
