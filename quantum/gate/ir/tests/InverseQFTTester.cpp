#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE QFTTester

#include <boost/test/included/unit_test.hpp>
#include "InverseQFT.hpp"
#include "JsonVisitor.hpp"
#include "GateQIR.hpp"

using namespace xacc;
using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreation) {

	auto iqft = std::make_shared<InverseQFT>();

	auto iqftKernel = iqft->generateAlgorithm(std::vector<int>{0,1,2});
	auto ir = std::make_shared<GateQIR>();
	ir->addKernel(iqftKernel);

	JsonVisitor v(iqftKernel);
	std::cout << v.write() << "\n";

//	auto h1 = std::make_shared<Hadamard>(2);
//	auto cphase1 = std::make_shared<CPhase>(1, 2, 1.5707963);
//	auto h2 = std::make_shared<Hadamard>(2);
//	auto cphase2 = std::make_shared<CPhase>(0, 2, 0.78539815);
//	auto cphase3 = std::make_shared<CPhase>(0, 1, 1.5707963);
//	auto h3 = std::make_shared<Hadamard>(0);
//	auto swap = std::make_shared<Swap>(0, 2);
//
//	auto expectedF = std::make_shared<GateFunction>("qft");
//	expectedF->addInstruction(h1);
//	expectedF->addInstruction(cphase1);
//	expectedF->addInstruction(h2);
//	expectedF->addInstruction(cphase2);
//	expectedF->addInstruction(cphase3);
//	expectedF->addInstruction(h3);
//	expectedF->addInstruction(swap);
//
//
//	auto expectedIR = std::make_shared<GateQIR>();
//	expectedIR->addKernel(expectedF);

}
