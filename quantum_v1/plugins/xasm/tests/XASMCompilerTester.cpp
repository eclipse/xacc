#include "gtest/gtest.h"

#include "XACC.hpp"
#include "xacc_service.hpp"

TEST(XASMCompilerTester, checkSimple) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"(__qpu__ void bell(qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";


  IR = compiler -> compile(R"([&](qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  bell(q);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";

}

TEST(XASMCompilerTester, checkApplyAll) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"([&](qbit q) {
  range(q, {{"gate","H"},{"nqubits",4}});
})");
  EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";
}

TEST(XASMCompilerTester, checkUnknownParameter) {
auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"([&](qbit q) {
  uccsd(q, {{"nqubits",4},{"nelectrons",2}});
})");

 EXPECT_EQ(1, IR->getKernels().size());
  std::cout << "KERNEL\n" << IR->getKernels()[0]->toString() << "\n";

  IR = compiler -> compile(R"([&](qbit q) {
  uccsd(q, {{"nqubits",4},{"nelectrons",ne}});
})");

 EXPECT_EQ(1, IR->getKernels().size());
  auto f = IR->getKernels()[0];
  std::cout << "KERNEL2\n" << IR->getKernels()[0]->toString() << "\n";

  f->expandIRGenerators({{"nqubits", 4}, {"nelectrons",2}});
    std::cout << "KERNEL3\n" << f->toString() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
