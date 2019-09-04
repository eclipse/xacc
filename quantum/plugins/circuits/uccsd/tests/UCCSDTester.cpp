#include <gtest/gtest.h>
#include "Instruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
using namespace xacc;


TEST(UCCSDTester,checkUCCSD) {

	xacc::Initialize();
 auto tmp = xacc::getService<Instruction>("uccsd");//std::make_shared<QFT>();
  auto uccsd = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
	EXPECT_TRUE(uccsd->expand({std::make_pair("ne", 2), std::make_pair("nq", 4)}));

	std::cout << uccsd->toString() << "\n";
	xacc::Finalize();
}


int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
