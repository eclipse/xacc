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

// Check that we can measure a sub-set of qubits in pulse mode.
TEST(IBMPulseRemoteTester, checkPartialMeasurements) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_bogota", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(2);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  auto h = provider->createInstruction("H", {0});
  auto m0 = provider->createInstruction("Measure", {0});
  auto m1 = provider->createInstruction("Measure", {1});
  f->addInstructions({h, m0, m1});
  acc->execute(buffer, f);
  buffer->print();
  for (const auto &bitString : buffer->getMeasurements()) {
    // Bitstring must have length 2
    EXPECT_EQ(bitString.size(), 2);
  }
}

// Measure all qubits
TEST(IBMPulseRemoteTester, checkMeasureAll) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_bogota", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(5);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  auto h0 = provider->createInstruction("H", {0});
  auto h1 = provider->createInstruction("H", {1});
  auto h2 = provider->createInstruction("H", {2});
  auto h3 = provider->createInstruction("H", {3});
  auto h4 = provider->createInstruction("H", {4});

  // Add all 5 measurements:
  auto m0 = provider->createInstruction("Measure", {0});
  auto m1 = provider->createInstruction("Measure", {1});
  auto m2 = provider->createInstruction("Measure", {2});
  auto m3 = provider->createInstruction("Measure", {3});
  auto m4 = provider->createInstruction("Measure", {4});
  f->addInstructions({ h0, h1, h2, h3, h4, m0, m1, m2, m3, m4 });
  acc->execute(buffer, f);
  buffer->print();
  for (const auto &bitString : buffer->getMeasurements()) {
    // Bitstring must have length 5
    EXPECT_EQ(bitString.size(), 5);
  }
}

// Note: This test is currently failing due to an IBM bug. 
// The bug only affects cross-resonance gates (CNOT).
#if 0
TEST(IBMPulseRemoteTester, checkCnotPulse) {
  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("ibm:ibmq_bogota", {{"mode", "pulse"}});
  auto buffer = xacc::qalloc(5);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto f = provider->createComposite("tmp");
  auto h0 = provider->createInstruction("H", {0});
  auto cx = provider->createInstruction("CNOT", {0, 1});
  auto m0 = provider->createInstruction("Measure", {0});
  auto m1 = provider->createInstruction("Measure", {1});
  
  f->addInstructions({ h0, cx, m0, m1 });
  acc->execute(buffer, f);
  buffer->print();
}
#endif

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}