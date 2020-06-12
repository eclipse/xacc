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

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "PauliOperator.hpp"
#include "Optimizer.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(GradientStrategiesTester, checkParameterShift) 
{
  auto accelerator = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("X0");

  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("testCircuit");
  ansatz->addVariable("x0");
  ansatz->addInstruction(provider->createInstruction("Ry", 
                        std::vector<std::size_t>{0}, 
                        {InstructionParameter("x0")}));

  auto parameterShift = xacc::getService<AlgorithmGradientStrategy>("parameter-shift-gradient");
  parameterShift->optionalParameters({std::make_pair("observable", observable)});
  auto gradientInstructions = parameterShift->getGradientExecutions(ansatz, {0.0});
  accelerator->execute(buffer, gradientInstructions);

  std::vector<double> dx(1);
  parameterShift->compute(dx, buffer->getChildren());
  EXPECT_NEAR(dx[0], -1.0, 1e-4);
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}