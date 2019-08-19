
#include <gtest/gtest.h>
#include "XACC.hpp"
#include "QuilVisitor.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "xacc_service.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST(QuilCompilerTester, checkSimple) {
  const std::string src =
      R"src(__qpu__ rotate(qbit qreg) {
RX(3.141592653589793) 0
H 0
CX 1 0
MEASURE 0 [0]
})src";
  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  auto function = ir->getComposite("rotate");

  std::cout << "HELLO\n" << function->toString() << "\n";

  EXPECT_TRUE(ir->getComposites().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 4);

}

TEST(QuilCompilerTester, checkVariableParameter) {

  const std::string src =
      R"src(__qpu__ statePrep2x2(qbit qreg, double theta1) {
RY(theta1) 0
})src";

  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);
  std::cout << "TEST:\n" << ir->getComposites()[0]->toString() << "\n\n";

  std::vector<double> v{2.2};
  auto evaled = ir->getComposites()[0]->operator()(v); //({2.2});
  std::cout << "TEST:\n" << evaled->toString() << "\n\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
