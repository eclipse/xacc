#include <gtest/gtest.h>
#include "xacc.hpp"
#include "{compiler_class_name}.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST({compiler_class_name}Tester, emptyTest) {{

    // const std::string src("__qpu__ example(AcceleratorBuffer qreg) {{\n"
    //         "}}\n");

    // auto compiler = std::make_shared<{compiler_class_name}>();
    // auto ir = compiler->compile(src);

    // auto function = ir->getKernel("example");
    // std::cout << "HELLO\n" << function->toString("qreg") << "\n";

    // EXPECT_TRUE(ir->getKernels().size() == 1);
    // EXPECT_TRUE(function->nInstructions() == 0);
}}

int main(int argc, char** argv) {{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}}