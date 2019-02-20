#include <gtest/gtest.h>
#include "XACC.hpp"
#include "{irgenerator_class_name}.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST({irgenerator_class_name}Tester, emptyTest) {{

    
    //auto irgen = std::make_shared{irgenerator_class_name}>();

    //auto buffer = std::make_shared<xacc::AcceleratorBuffer>("", 4);

    // Generate the IR function somehow

    //std::cout << "HELLO\n" << f->toString('qreg') << "\n" std::endl;
    
    // Test it somehow.. 
    
    //EXPECT_TRUE(f->nInstructions() == 0);
    //EXPECT_TRUE(f->nParameters() == 2);
    
}}

int main(int argc, char** argv) {{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}}