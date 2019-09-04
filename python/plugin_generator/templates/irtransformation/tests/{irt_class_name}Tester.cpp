#include <gtest/gtest.h>
#include "xacc.hpp"
#include "{irt_class_name}.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST({irt_class_name}Tester, emptyTest) {{

    // CREATE IR
    // auto ir = ...

    // Create Transformation and execute
    // {irt_class_name} transform;
    // auto newIr = transform.transform(ir);

    // NOW Test it somehow...
}}

int main(int argc, char** argv) {{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}}