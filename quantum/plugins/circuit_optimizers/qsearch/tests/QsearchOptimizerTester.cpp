
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"

using namespace xacc::quantum;

TEST(QsearchOptimizerTester, checkSimpleQITE) {

  auto qite = xacc::getService<xacc::Algorithm>("qite");
  std::shared_ptr<xacc::Observable> observable =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString(
      "5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1");
  auto acc = xacc::getAccelerator("qpp");
  const int nbSteps = 5;
  const double stepSize = 0.1;
  auto compiler = xacc::getCompiler("xasm");
  auto ir =
      compiler->compile(R"(__qpu__ void f(qbit q) { X(q[0]); })", nullptr);
  auto x = ir->getComposite("f");

  auto opt = xacc::getIRTransformation("qsearch");
  const bool initOk = qite->initialize({{"accelerator", acc},
                                        {"steps", nbSteps},
                                        {"observable", observable},
                                        {"step-size", stepSize},
                                        {"ansatz", x},
                                        {"circuit-optimizer", opt},
                                        {"verbosity", 1},
                                        {"write_to_stdout", true},
                                        {"compiler_class", "LeapCompiler"}});

  EXPECT_TRUE(initOk);

  xacc::set_verbose(true);
  auto buffer = xacc::qalloc(2);
  qite->execute(buffer);
  const double finalEnergy = (*buffer)["opt-val"].as<double>();
  std::cout << "Final Energy: " << finalEnergy << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}