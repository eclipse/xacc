#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"

TEST(ResetInstTester,checkResetCompile) {
auto src = R"#(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[1];
creg c[1];
h q[0];
reset q[0];
reset q[0];
x q[0];
measure q[0] -> c[0];)#";
  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
}


int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}