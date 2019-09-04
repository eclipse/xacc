#include <gtest/gtest.h>
#include "JW.hpp"
#include "xacc.hpp"
#include <memory>
#include <regex>
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"

using namespace xacc::quantum;

TEST(JordanWignerTransformationTester, checkEasyTransform) {

  const std::string code =
      R"code(2^ 0
	   - 0^ 2
       + 3^ 1
       - 1^ 3
	    + 2^ 0 2^ 0
	   - 0^ 2 0^ 2
	    + 2^ 0 3^ 1
	   - 1^ 3 0^ 2
	    + 3^ 1 2^ 0
	   - 0^ 2 1^ 3
	    + 3^ 1 3^ 1
	   - 1^ 3 1^ 3)code";

  auto fermion = std::make_shared<FermionOperator>(code);
  JW t;
  auto result = t.transform(fermion);
  auto resultsStr = result->toString();
  std::cout << "JW:\n" << resultsStr << "\n";
  resultsStr = std::regex_replace(resultsStr, std::regex("\\+"), "+\n");
  // boost::replace_all(resultsStr, "+", "+\n");
  std::cout << "Transformed Fermion to Spin:\nBEGIN\n"
            << resultsStr << "\nEND\n\n";

  std::string expected =
      R"expected((0,-0.5) X0 Z1 Y2 + (0,0.5) Y0 Z1 X2 + (0,-0.25) X0 X1 Y2 X3 + (0,-0.5) X1 Z2 Y3 + (0,0.25) Y0 X1 X2 X3 + (0,-0.25) X0 Y1 Y2 Y3 + (0,0.25) X0 Y1 X2 X3 + (0,-0.25) X0 X1 X2 Y3 + (0,-0.25) Y0 X1 Y2 Y3 + (0,0.5) Y1 Z2 X3 + (0,0.25) Y0 Y1 Y2 X3 + (0,0.25) Y0 Y1 X2 Y3)expected";
  PauliOperator op(expected);
  EXPECT_TRUE(std::dynamic_pointer_cast<PauliOperator>(result)->operator==(op));
}

// TEST(JordanWignerTransformationTester,checkH2Transform) {

// 	const std::string code =
// 			R"code(__qpu__ kernel() {
//    0.7137758743754461
//    -1.252477303982147 0 1 0 0
//    0.337246551663004 0 1 1 1 1 0 0 0
//    0.0906437679061661 0 1 1 1 3 0 2 0
//    0.0906437679061661 0 1 2 1 0 0 2 0
//    0.3317360224302783 0 1 2 1 2 0 0 0
//    0.0906437679061661 0 1 3 1 1 0 2 0
//    0.3317360224302783 0 1 3 1 3 0 0 0
//    0.337246551663004 1 1 0 1 0 0 1 0
//    0.0906437679061661 1 1 0 1 2 0 3 0
//    -1.252477303982147 1 1 1 0
//    0.0906437679061661 1 1 2 1 0 0 3 0
//    0.3317360224302783 1 1 2 1 2 0 1 0
//    0.0906437679061661 1 1 3 1 1 0 3 0
//    0.3317360224302783 1 1 3 1 3 0 1 0
//    0.3317360224302783 2 1 0 1 0 0 2 0
//    0.0906437679061661 2 1 0 1 2 0 0 0
//    0.3317360224302783 2 1 1 1 1 0 2 0
//    0.0906437679061661 2 1 1 1 3 0 0 0
//    -0.4759344611440753 2 1 2 0
//    0.0906437679061661 2 1 3 1 1 0 0 0
//    0.3486989747346679 2 1 3 1 3 0 2 0
//    0.3317360224302783 3 1 0 1 0 0 3 0
//    0.0906437679061661 3 1 0 1 2 0 1 0
//    0.3317360224302783 3 1 1 1 1 0 3 0
//    0.0906437679061661 3 1 1 1 3 0 1 0
//    0.0906437679061661 3 1 2 1 0 0 1 0
//    0.3486989747346679 3 1 2 1 2 0 3 0
//    -0.4759344611440753 3 1 3 0
// })code";

// 	auto fermionKernel = compileKernel(code);

// 	JordanWignerIRTransformation t;
// 	auto result = t.transform(*fermionKernel);

// 	auto resultsStr = result.toString();
//     resultsStr = std::regex_replace(resultsStr, std::regex("\\+"), "+\n");
// 	// boost::replace_all(resultsStr, "+", "+\n");
// 	std::cout << "Transformed Fermion to Spin:\nBEGIN\n" << resultsStr <<
// "\nEND\n\n";

// 	PauliOperator expected({{2,"Z"}, {3,"Z"}}, .174349);
// 	expected += PauliOperator({{1,"Z"}, {3,"Z"}}, .120546);
// 	expected += PauliOperator({{3,"Z"}}, -.222796);
// 	expected += PauliOperator({{0,"Z"}, {3,"Z"}}, .165868);
// 	expected += PauliOperator({{0,"Z"}, {2,"Z"}}, .120546);
// 	expected += PauliOperator({{1,"Z"}, {2,"Z"}}, .165868);
// 	expected += PauliOperator({{0,"Z"}, {1,"Z"}}, .168623);
// 	expected += PauliOperator({{1,"Z"}}, .171201);
// 	expected += PauliOperator({{2,"Z"}}, -.222796);
// 	expected += PauliOperator({{0,"Z"}}, .171201);
// 	expected += PauliOperator({}, -.0988349);
// 	expected += PauliOperator({{0,"X"}, {1,"X"}, {2,"Y"}, {3,"Y"}},
// -0.0453219); 	expected += PauliOperator({{0,"Y"}, {1,"X"}, {2,"X"}, {3,"Y"}},
// 0.0453219); 	expected += PauliOperator({{0,"Y"}, {1,"Y"}, {2,"X"}, {3,"X"}},
// -0.0453219); 	expected += PauliOperator({{0,"X"}, {1,"Y"}, {2,"Y"}, {3,"X"}},
// 0.0453219);

//     resultsStr = expected.toString();
//     resultsStr = std::regex_replace(resultsStr, std::regex("\\+"), "+\n");
// 	// boost::replace_all(resultsStr, "+", "+\n");
// 	std::cout << "EXPECTED Fermion to Spin:\nBEGIN\n" << resultsStr <<
// "\nEND\n\n";

// 	EXPECT_TRUE(expected == result);
// }

// TEST(JordanWignerTransformationTester,checkJWTransform) {

// 	auto Instruction = std::make_shared<FermionInstruction>(
// 			std::vector<std::pair<int, int>> { { 2, 1 }, { 0, 0 }
// }, 3.17); 	auto Instruction2 = std::make_shared<FermionInstruction>(
// 			std::vector<std::pair<int, int>> { { 0, 1 }, { 2, 0 }
// }, 3.17);

// 	auto kernel = std::make_shared<FermionKernel>("foo");
// 	kernel->addInstruction(Instruction);
// 	kernel->addInstruction(Instruction2);

// 	JordanWignerIRTransformation jwTransform;

// 	auto newIr = jwTransform.transform(*kernel);

// 	PauliOperator expected({{0,"X"}, {1,"Z"}, {2,"X"}}, 1.585);
// 	expected += PauliOperator({{0,"Y"}, {1,"Z"}, {2,"Y"}}, 1.585);

// 	std::cout << "TRANSFORMED: " << newIr.toString() << "\n";
// 	EXPECT_TRUE(expected == newIr);

// }

// TEST(JordanWignerTransformationTester,checkEfficientJW) {
// 	const std::string k = R"k(__qpu__ kernel() {
//    0.7137758743754461
//    -1.252477303982147 0 1 0 0
//    0.337246551663004 0 1 1 1 1 0 0 0
//    0.0906437679061661 0 1 1 1 3 0 2 0
//    0.0906437679061661 0 1 2 1 0 0 2 0
//    0.3317360224302783 0 1 2 1 2 0 0 0
//    0.0906437679061661 0 1 3 1 1 0 2 0
//    0.3317360224302783 0 1 3 1 3 0 0 0
//    0.337246551663004 1 1 0 1 0 0 1 0
//    0.0906437679061661 1 1 0 1 2 0 3 0
//    -1.252477303982147 1 1 1 0
//    0.0906437679061661 1 1 2 1 0 0 3 0
//    0.3317360224302783 1 1 2 1 2 0 1 0
//    0.0906437679061661 1 1 3 1 1 0 3 0
//    0.3317360224302783 1 1 3 1 3 0 1 0
//    0.3317360224302783 2 1 0 1 0 0 2 0
//    0.0906437679061661 2 1 0 1 2 0 0 0
//    0.3317360224302783 2 1 1 1 1 0 2 0
//    0.0906437679061661 2 1 1 1 3 0 0 0
//    -0.4759344611440753 2 1 2 0
//    0.0906437679061661 2 1 3 1 1 0 0 0
//    0.3486989747346679 2 1 3 1 3 0 2 0
//    0.3317360224302783 3 1 0 1 0 0 3 0
//    0.0906437679061661 3 1 0 1 2 0 1 0
//    0.3317360224302783 3 1 1 1 1 0 3 0
//    0.0906437679061661 3 1 1 1 3 0 1 0
//    0.0906437679061661 3 1 2 1 0 0 1 0
//    0.3486989747346679 3 1 2 1 2 0 3 0
//    -0.4759344611440753 3 1 3 0
// })k";

// 	auto fermionKernel = compileKernel(k);
// 	auto fermionir = std::make_shared<FermionIR>();
// 	fermionir->addKernel(fermionKernel);

// 	EfficientJW t;
// 	auto ir = t.transform(fermionir);
// 	auto result = t.getResult();

// }

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
