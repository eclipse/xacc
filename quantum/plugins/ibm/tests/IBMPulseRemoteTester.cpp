#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "Pulse.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(IBMPulseRemoteTester, checkPulseSimple) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_armonk", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(1);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  auto m = provider->createInstruction("Measure", {0});
  f->addInstruction(m);
  acc->execute(buffer, f);
  buffer->print();
  // A simple measure pulse
  EXPECT_TRUE(buffer->computeMeasurementProbability("1") > 0.01);
}

TEST(IBMPulseRemoteTester, checkPulseLowering) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_armonk", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(1);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  auto m = provider->createInstruction("Measure", {0});
  auto h = provider->createInstruction("H", {0});
  // Verify the gate-pulse lowering in pulse mode
  f->addInstruction(h);
  f->addInstruction(m);
  acc->execute(buffer, f);
  buffer->print();
  EXPECT_NEAR(buffer->computeMeasurementProbability("0"), 0.5, 0.15);
  EXPECT_NEAR(buffer->computeMeasurementProbability("1"), 0.5, 0.15);
}

// Check raw pulse
TEST(IBMPulseRemoteTester, checkRawPulse) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_armonk", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(1);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");

  const size_t nbSamples = 128;
  const std::vector<std::vector<double>> samples(nbSamples, {1.0, 0.0});
  auto pulse = std::make_shared<xacc::quantum::Pulse>("square", "d0");
  pulse->setSamples(samples);
  xacc::contributeService("square", pulse);
  auto f = provider->createComposite("tmp");
  auto m = provider->createInstruction("Measure", {0});
  auto pulseInst = provider->createInstruction("square", {0});
  // Add the raw pulse then measure
  f->addInstruction(pulseInst);
  f->addInstruction(m);
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