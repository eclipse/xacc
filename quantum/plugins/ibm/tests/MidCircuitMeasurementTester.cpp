#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(MidCircuitMeasurementTester, checkSimple) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_manhattan");
  auto props = acc->getProperties();
  EXPECT_TRUE(props.get<bool>("multi_meas_enabled"));
  auto buffer = xacc::qalloc(2);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  f->addInstruction(provider->createInstruction("X", 0));
  f->addInstruction(provider->createInstruction("Measure", 0));
  f->addInstruction(provider->createInstruction("Measure", 1));
  f->addInstruction(provider->createInstruction("X", 1));
  f->addInstruction(provider->createInstruction("Measure", 0));
  f->addInstruction(provider->createInstruction("Measure", 1));
  acc->execute(buffer, f);
  buffer->print();
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}