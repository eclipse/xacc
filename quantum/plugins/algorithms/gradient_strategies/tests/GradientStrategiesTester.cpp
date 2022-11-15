/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include <string>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "PauliOperator.hpp"
#include "Optimizer.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(GradientStrategiesTester, checkParameterShift) {
  auto accelerator = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(1);

  std::shared_ptr<Observable> observable =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("X0");

  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("testCircuit");
  ansatz->addVariable("x0");
  ansatz->addInstruction(provider->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto parameterShift =
      xacc::getService<AlgorithmGradientStrategy>("parameter-shift");
  parameterShift->initialize({std::make_pair("observable", observable)});
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, {0.0});
  accelerator->execute(buffer, gradientInstructions);

  std::vector<double> dx(1);
  parameterShift->compute(dx, buffer->getChildren());
  EXPECT_NEAR(dx[0], 1, 1e-10);
}

TEST(GradientStrategiesTester, checkParameterShiftXanadu) {
  auto accelerator = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);

  std::shared_ptr<Observable> observable =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("Y0 Z2");

  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("testCircuit");
  std::vector<std::string> varNames = {"x0", "x1", "x2", "x3", "x4", "x5"};
  ansatz->addVariables(varNames);
  ansatz->addInstruction(provider->createInstruction("Rx", {0}, {"x0"}));
  ansatz->addInstruction(provider->createInstruction("Ry", {1}, {"x1"}));
  ansatz->addInstruction(provider->createInstruction("Rz", {2}, {"x2"}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {0, 1}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {1, 2}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {2, 0}));
  ansatz->addInstruction(provider->createInstruction("Rx", {0}, {"x3"}));
  ansatz->addInstruction(provider->createInstruction("Ry", {1}, {"x4"}));
  ansatz->addInstruction(provider->createInstruction("Rz", {2}, {"x5"}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {0, 1}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {1, 2}));
  ansatz->addInstruction(provider->createInstruction("CNOT", {2, 0}));

  std::vector<double> params{0.37454012, 0.95071431, 0.73199394, 0.59865848, 0.15601864, 0.15599452};
  auto opt = xacc::getOptimizer("nlopt", {{"nlopt-optimizer", "l-bfgs"}, {"initial-parameters", params}, {"maxeval", 1}});
  auto vqe = xacc::getAlgorithm("vqe", {{"ansatz", ansatz}, {"accelerator", accelerator}, {"observable", observable}, {"optimizer", opt}, {"gradient_strategy", "parameter-shift"}});
  vqe->execute(buffer);
  EXPECT_NEAR(buffer->getInformation("opt-val").as<double>(), -0.1197136570687156, 1e-5);

  auto parameterShift = xacc::getGradient("parameter-shift", {{"observable", observable}, {"shift-scalar", 0.5}});
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, params);
      auto tmpbuffer = xacc::qalloc(3);
      accelerator->execute(tmpbuffer, gradientInstructions);
   std::vector<double> dx(6);

  parameterShift->compute(dx, tmpbuffer->getChildren());

  // check gradient
 std::vector<double> expectedGradient = {-0.0651888, -0.0272892, 0, -0.0933935, -0.761068, 0};
 for(int i = 0; i < 6; i++) {
   EXPECT_NEAR(dx[i], expectedGradient[i], 1e-5);
 }
}

TEST(GradientStrategiesTester, checkCentralDifference) {
  auto accelerator2 = xacc::getAccelerator("qpp");
  auto buffer2 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable2 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable2->fromString("X0");

  auto provider2 = xacc::getIRProvider("quantum");
  auto ansatz2 = provider2->createComposite("testCircuit");
  ansatz2->addVariable("x0");
  ansatz2->addInstruction(provider2->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto centralDifference =
      xacc::getService<AlgorithmGradientStrategy>("central");
  centralDifference->initialize({std::make_pair("observable", observable2)});
  auto gradientInstructions =
      centralDifference->getGradientExecutions(ansatz2, {0.0});
  accelerator2->execute(buffer2, gradientInstructions);

  std::vector<double> dx(1);
  centralDifference->compute(dx, buffer2->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 1e-4);
}

TEST(GradientStrategiesTester, checkForwardDifference) {
  auto accelerator3 = xacc::getAccelerator("qpp");
  auto buffer3 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable3 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable3->fromString("X0");

  auto provider3 = xacc::getIRProvider("quantum");
  auto ansatz3 = provider3->createComposite("testCircuit");
  ansatz3->addVariable("x0");
  ansatz3->addInstruction(provider3->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto forwardDifference =
      xacc::getService<AlgorithmGradientStrategy>("forward");
  forwardDifference->initialize({std::make_pair("observable", observable3)});
  auto gradientInstructions =
      forwardDifference->getGradientExecutions(ansatz3, {0.0});
  accelerator3->execute(buffer3, gradientInstructions);

  std::vector<double> dx(1);
  forwardDifference->compute(dx, buffer3->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 1e-4);
}

TEST(GradientStrategiesTester, checkBackwardDifference) {
  auto accelerator4 = xacc::getAccelerator("qpp");
  auto buffer4 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable4 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable4->fromString("X0");

  auto provider4 = xacc::getIRProvider("quantum");
  auto ansatz4 = provider4->createComposite("testCircuit");
  ansatz4->addVariable("x0");
  ansatz4->addInstruction(provider4->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto backwardDifference =
      xacc::getService<AlgorithmGradientStrategy>("backward");
  backwardDifference->initialize({std::make_pair("observable", observable4)});
  auto gradientInstructions =
      backwardDifference->getGradientExecutions(ansatz4, {0.0});
  accelerator4->execute(buffer4, gradientInstructions);

  std::vector<double> dx(1);
  backwardDifference->compute(dx, buffer4->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 1e-4);
}

TEST(GradientStrategiesTester, checkYanPSproblem) {

  int nLayer = 7;
  double param = 2.0 / nLayer, shiftScalar = 1.0 / 8.0;
  auto H = xacc::quantum::getObservable("pauli", std::string("Z0"));
  auto acc = xacc::getAccelerator("qpp");

  std::vector<double> init = {param};
  auto opt = xacc::getOptimizer(
      "nlopt", {{"algorithm", "l-bfgs"}, {"initial-parameters", init}});

  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto ansatz = provider->createComposite("initial-state");

  ansatz->addVariable("x0");
  for (int i = 0; i < nLayer; i++) {
    ansatz->addInstruction(provider->createInstruction("Rx", {0}, {"x0"}));
  }

  auto objFxn = xacc::getService<xacc::Algorithm>("vqe");
  objFxn->initialize({{"accelerator", acc},
                      {"observable", H},
                      {"ansatz", ansatz},
                      {"optimizer", opt},
                      {"shift-scalar", shiftScalar},
                      {"gradient_strategy", "parameter-shift"}});
  auto q = xacc::qalloc(1);
  objFxn->execute(q);

  EXPECT_NEAR((*q)["opt-val"].as<double>(), -1.0, 1e-4);
}

// Check running shots:
TEST(GradientStrategiesTester, checkParameterShiftShots) {
  auto accelerator = xacc::getAccelerator("aer", {{"shots", 65536}});
  auto buffer = xacc::qalloc(1);

  std::shared_ptr<Observable> observable =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("X0");

  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("testCircuit");
  ansatz->addVariable("x0");
  ansatz->addInstruction(provider->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto parameterShift =
      xacc::getService<AlgorithmGradientStrategy>("parameter-shift");
  parameterShift->initialize({std::make_pair("observable", observable)});
  auto gradientInstructions =
      parameterShift->getGradientExecutions(ansatz, {0.0});
  accelerator->execute(buffer, gradientInstructions);
  std::vector<double> dx(1);
  parameterShift->compute(dx, buffer->getChildren());
  EXPECT_NEAR(dx[0], 1, 0.1);
}

TEST(GradientStrategiesTester, checkCentralDifferenceShots) {
  auto accelerator2 = xacc::getAccelerator("aer", {{"shots", 65536}});
  // auto accelerator2 = xacc::getAccelerator("qpp");
  auto buffer2 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable2 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable2->fromString("X0");

  auto provider2 = xacc::getIRProvider("quantum");
  auto ansatz2 = provider2->createComposite("testCircuit");
  ansatz2->addVariable("x0");
  ansatz2->addInstruction(provider2->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto centralDifference =
      xacc::getService<AlgorithmGradientStrategy>("central");
  centralDifference->initialize({{"observable", observable2}, {"step", 0.1}});
  auto gradientInstructions =
      centralDifference->getGradientExecutions(ansatz2, {0.0});
  accelerator2->execute(buffer2, gradientInstructions);

  std::vector<double> dx(1);
  centralDifference->compute(dx, buffer2->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 0.1);
}

TEST(GradientStrategiesTester, checkForwardDifferenceShots) {
  auto accelerator3 = xacc::getAccelerator("aer", {{"shots", 65536}});
  auto buffer3 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable3 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable3->fromString("X0");

  auto provider3 = xacc::getIRProvider("quantum");
  auto ansatz3 = provider3->createComposite("testCircuit");
  ansatz3->addVariable("x0");
  ansatz3->addInstruction(provider3->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto forwardDifference =
      xacc::getService<AlgorithmGradientStrategy>("forward");
  forwardDifference->initialize({{"observable", observable3}, {"step", 0.1}});
  auto gradientInstructions =
      forwardDifference->getGradientExecutions(ansatz3, {0.0});
  accelerator3->execute(buffer3, gradientInstructions);

  std::vector<double> dx(1);
  forwardDifference->compute(dx, buffer3->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 0.1);
}

TEST(GradientStrategiesTester, checkBackwardDifferenceShots) {
  auto accelerator4 = xacc::getAccelerator("aer", {{"shots", 65536}});
  auto buffer4 = xacc::qalloc(1);

  std::shared_ptr<Observable> observable4 =
      std::make_shared<xacc::quantum::PauliOperator>();
  observable4->fromString("X0");

  auto provider4 = xacc::getIRProvider("quantum");
  auto ansatz4 = provider4->createComposite("testCircuit");
  ansatz4->addVariable("x0");
  ansatz4->addInstruction(provider4->createInstruction(
      "Ry", std::vector<std::size_t>{0}, {InstructionParameter("x0")}));

  auto backwardDifference =
      xacc::getService<AlgorithmGradientStrategy>("backward");
  backwardDifference->initialize({{"observable", observable4}, {"step", 0.1}});
  auto gradientInstructions =
      backwardDifference->getGradientExecutions(ansatz4, {0.0});
  accelerator4->execute(buffer4, gradientInstructions);

  std::vector<double> dx(1);
  backwardDifference->compute(dx, buffer4->getChildren());
  EXPECT_NEAR(dx[0], 1.0, 0.1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}