#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include <gtest/gtest.h>
#include <memory>

// This test requires remote access
// TEST(MidCircuitMeasurementTester, checkSimple) {
//   xacc::set_verbose(true);
//   auto acc = xacc::getAccelerator("ibm:ibmq_manhattan");
//   auto props = acc->getProperties();
//   EXPECT_TRUE(props.get<bool>("multi_meas_enabled"));
//   auto buffer = xacc::qalloc(2);
//   auto provider = xacc::getService<xacc::IRProvider>("quantum");
//   auto f = provider->createComposite("tmp");
//   f->addInstruction(provider->createInstruction("X", 0));
//   f->addInstruction(provider->createInstruction("Measure", 0));
//   f->addInstruction(provider->createInstruction("Measure", 1));
//   f->addInstruction(provider->createInstruction("X", 1));
//   f->addInstruction(provider->createInstruction("Measure", 0));
//   f->addInstruction(provider->createInstruction("Measure", 1));
//   acc->execute(buffer, f);
//   buffer->print();
// }

// Check the QObj with mid-circuit measurements and 
// if statements.
TEST(MidCircuitMeasurementTester, checkQObjSim) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void iqpe(qbit q) {
H(q[0]);
X(q[1]);
// Prepare the state:
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
H(q[0]);
// Measure and reset
Measure(q[0], cReg[0]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
// Conditional rotation
if(cReg[0]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[1]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
if(cReg[0]) {
  Rz(q[0], -pi/4);
}
if(cReg[1]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[2]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
if(cReg[0]) {
  Rz(q[0], -pi/8);
}
if(cReg[1]) {
  Rz(q[0], -pi/4);
}
if(cReg[2]) {
  Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], cReg[3]);
})");
  auto accelerator = xacc::getAccelerator("aer", {{"shots", 1024}});
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, ir->getComposites()[0]);
  buffer->print();
  // Expect: 1011 = 11 (decimal)
  EXPECT_EQ(buffer->getMeasurementCounts()["1011"], 1024);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}