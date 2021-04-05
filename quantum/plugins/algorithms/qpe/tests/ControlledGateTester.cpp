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

TEST(ControlledGateTester, checkMultipleControl) 
{
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto x = std::make_shared<X>(0);
    std::shared_ptr<xacc::CompositeInstruction> comp =
        gateRegistry->createComposite("__COMPOSITE__X");
    comp->addInstruction(x);
    auto ccx = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("C-U"));
    const std::vector<int> ctrl_idxs{1, 2};
    ccx->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
    std::cout << "HOWDY: \n" << ccx->toString() << "\n";

    // Test truth table
    auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", 8192)});
    auto xGate0 = gateRegistry->createInstruction("X", { 0 });
    auto xGate1 = gateRegistry->createInstruction("X", { 1 });
    auto xGate2 = gateRegistry->createInstruction("X", { 2 });
    auto measureGate0 = gateRegistry->createInstruction("Measure", { 0 });
    auto measureGate1 = gateRegistry->createInstruction("Measure", { 1 });
    auto measureGate2 = gateRegistry->createInstruction("Measure", { 2 });

    const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2) {
        static int counter = 0;
        auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" + std::to_string(counter));
        counter++;
        // State prep:
        if (in_bit0)
        {
            composite->addInstruction(xGate0);
        }
        if (in_bit1)
        {
            composite->addInstruction(xGate1);
        }
        if (in_bit2)
        {
            composite->addInstruction(xGate2);
        }

        std::string inputBitString;
        inputBitString.append(in_bit0 ? "1" : "0");
        inputBitString.append(in_bit1 ? "1" : "0");
        inputBitString.append(in_bit2 ? "1" : "0");

        // Add ccx
        composite->addInstructions(ccx->getInstructions());
        // Mesurement:
        composite->addInstructions({ measureGate0, measureGate1, measureGate2 });

        auto buffer = xacc::qalloc(3);
        acc->execute(buffer, composite);
        std::cout << "Input bitstring: " << inputBitString << "\n";
        buffer->print();
        // CCNOT gate:
        const auto expectedBitString = [&inputBitString]() -> std::string {
            // If both control bits are 1's
            // q0q1q2
            if (inputBitString == "011")
            {
                return "111";
            }
            if (inputBitString == "111")
            {
                return "011";
            }
            // Otherwise, no changes
            return inputBitString;
        }();
        // Check bit string
        EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString),  1.0,  0.1);
    };  

    // 3 bits: run all test cases (8)
    for (int i = 0; i < (1 << 3); ++i)
    {
        runTestCase(i & 0x001, i & 0x002, i & 0x004);
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
