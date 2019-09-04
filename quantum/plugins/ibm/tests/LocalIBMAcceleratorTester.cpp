#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "CommonGates.hpp"
#include "Circuit.hpp"

using namespace xacc::quantum;

TEST(LocalIBMAcceleratorTester,checkSimple) {
    xacc::Initialize();

	auto acc = xacc::getAccelerator("local-ibm", {std::make_pair("shots",8192),std::make_pair("cx-p-depol",.15)});
	auto buffer = xacc::qalloc(2);

	auto f = std::make_shared<Circuit>("foo");

	auto h = std::make_shared<Hadamard>(0);
	auto cn1 = std::make_shared<CNOT>(0,1);
	auto m0 = std::make_shared<Measure>(0);
	auto m1 = std::make_shared<Measure>(1);

	f->addInstructions({h,cn1,m0,m1});

	acc->execute(buffer, f);

    buffer->print();
	xacc::Finalize();
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
