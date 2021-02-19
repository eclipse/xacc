#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"

TEST(ResetInstTester, checkResetCompileStaq) {
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
  EXPECT_EQ(hello->nInstructions(), 5);
  EXPECT_EQ(hello->getInstruction(1)->name(), "Reset");
  EXPECT_EQ(hello->getInstruction(2)->name(), "Reset");
}

TEST(ResetInstTester, checkResetCompileXasm) {
  auto src = R"#(__qpu__ void reset_test(qbit q) {
  H(q[0]);
  Reset(q[0]);
  Reset(q[0]);
  X(q[0]);
  Measure(q[0]);
})#";
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  EXPECT_EQ(hello->nInstructions(), 5);
  EXPECT_EQ(hello->getInstruction(1)->name(), "Reset");
  EXPECT_EQ(hello->getInstruction(2)->name(), "Reset");
}

TEST(ResetInstTester, checkResetSim) {
  auto acc = xacc::getAccelerator("qpp", {{"shots", 1024}});
  auto src = R"#(__qpu__ void reset_run(qbit q) {
  H(q[0]);
  Reset(q[0]);
  Reset(q[0]);
  X(q[0]);
  Measure(q[0]);
})#";
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  xacc::set_verbose(true);
  auto buffer = xacc::qalloc(1);
  acc->execute(buffer, hello);
  xacc::set_verbose(false);
  buffer->print();
  // Due to reset => back to 0 => become 1 after X...
  EXPECT_NEAR(buffer->computeMeasurementProbability("1"), 1.0, 1e-9);
}

// Check that Aer can also handle the QObj that
// has reset instructions.
TEST(ResetInstTester, checkResetAerSim) {
  auto acc = xacc::getAccelerator("aer");
  auto src = R"#(__qpu__ void reset_run_aer(qbit q) {
  H(q[0]);
  Reset(q[0]);
  Reset(q[0]);
  X(q[0]);
  Measure(q[0]);
})#";
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  xacc::set_verbose(true);
  auto buffer = xacc::qalloc(1);
  acc->execute(buffer, hello);
  xacc::set_verbose(false);
  buffer->print();
  // Due to reset => back to 0 => become 1 after X...
  EXPECT_NEAR(buffer->computeMeasurementProbability("1"), 1.0, 1e-9);
}

// TEST(ResetInstTester, checkQobjRun) {
//   auto acc = xacc::getAccelerator("ibm:ibmq_manhattan");
//   auto src = R"#(__qpu__ void reset_run(qbit q) {
//   H(q[0]);
//   Reset(q[0]);
//   Reset(q[0]);
//   X(q[0]);
//   Measure(q[0]);
// })#";
//   auto compiler = xacc::getCompiler("xasm");
//   auto IR = compiler->compile(src);
//   auto hello = IR->getComposites()[0];
//   std::cout << "HELLO:\n" << hello->toString() << "\n";
//   xacc::set_verbose(true);
//   auto buffer = xacc::qalloc(1);
//   acc->execute(buffer, hello);
//   xacc::set_verbose(false);
//   buffer->print();
//   // Due to reset => back to 0 => become 1 after X...
//   EXPECT_TRUE(buffer->computeMeasurementProbability("1") > 0.8);
// }

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}