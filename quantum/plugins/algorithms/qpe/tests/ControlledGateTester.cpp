#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "CommonGates.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(ControlledGateTester, checkSimple) 
{
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto t = std::make_shared<T>(0);
    auto rz = std::make_shared<Rz>(0, 3.1415);
    auto u1 = std::make_shared<U1>(0, 3.1415);
    auto x = std::make_shared<X>(0);
    const std::vector<size_t> expectedBits { 1, 0};
    {
        std::shared_ptr<xacc::CompositeInstruction> comp = gateRegistry->createComposite("__COMPOSITE__X");
        comp->addInstruction(x->clone());
        auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("C-U"));
        ctrlKernel->expand({ 
            std::make_pair("U",  comp),
            std::make_pair("control-idx",  1)
        });    
        EXPECT_EQ(ctrlKernel->nInstructions(), 1);
        EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CNOT");
        EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CNOT");
        EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
    }
    {
        std::shared_ptr<xacc::CompositeInstruction> comp = gateRegistry->createComposite("__COMPOSITE__T");
        comp->addInstruction(t->clone());
        auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("C-U"));
        ctrlKernel->expand({ 
            std::make_pair("U",  comp),
            std::make_pair("control-idx",  1)
        }); 
        // T -> CPhase(pi/4)   
        EXPECT_EQ(ctrlKernel->nInstructions(), 1);
        EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CPhase");
        EXPECT_NEAR(ctrlKernel->getInstruction(0)->getParameter(0).as<double>(), M_PI_4, 1e-4);
        EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
    }
    {
        std::shared_ptr<xacc::CompositeInstruction> comp = gateRegistry->createComposite("__COMPOSITE__Rz");
        comp->addInstruction(rz->clone());
        auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("C-U"));
        ctrlKernel->expand({ 
            std::make_pair("U",  comp),
            std::make_pair("control-idx",  1)
        }); 
        // Rz -> CRz   
        EXPECT_EQ(ctrlKernel->nInstructions(), 1);
        EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CRZ");
        EXPECT_EQ(ctrlKernel->getInstruction(0)->getParameter(0), rz->getParameter(0));
        EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
    }
    {
        std::shared_ptr<xacc::CompositeInstruction> comp = gateRegistry->createComposite("__COMPOSITE__U1");
        comp->addInstruction(u1->clone());
        auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("C-U"));
        ctrlKernel->expand({ 
            std::make_pair("U",  comp),
            std::make_pair("control-idx",  1)
        }); 
        // U1 -> CPhase (cU1)
        EXPECT_EQ(ctrlKernel->nInstructions(), 1);
        EXPECT_EQ(ctrlKernel->getInstruction(0)->name(), "CPhase");
        EXPECT_EQ(ctrlKernel->getInstruction(0)->getParameter(0), u1->getParameter(0));
        EXPECT_EQ(ctrlKernel->getInstruction(0)->bits(), expectedBits);
    }
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
