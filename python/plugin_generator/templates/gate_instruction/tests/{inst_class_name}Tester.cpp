#include <gtest/gtest.h>
#include "xacc.hpp"
#include "{inst_class_name}.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST({inst_class_name}Tester, emptyTest) {{

    // NOW Test it somehow...
}}

int main(int argc, char** argv) {{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}}