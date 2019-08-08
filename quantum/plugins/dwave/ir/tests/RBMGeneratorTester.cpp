#include <gtest/gtest.h>
#include "XACC.hpp"
#include "xacc_service.hpp"
#include "IRGenerator.hpp"
using namespace xacc;

TEST(RBMGeneratorTester, checkRBM){

    xacc::Initialize();
    auto irgen = xacc::getService<IRGenerator>("rbm");//std::make_shared<RBM>();

    auto buffer = std::make_shared<xacc::AcceleratorBuffer>("", 4);

    auto f = irgen->generate({{"visible_units",2},{"hidden_units",2}});

    std::cout << f->toString() << std::endl;
    std::cout << f->nParameters() << std::endl;

    auto params = f->getParameters();
    for (int i = 0; i < params.size(); i++){
        std::cout << params[i].as<std::string>() << std::endl;
    }
    xacc::Finalize();
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}