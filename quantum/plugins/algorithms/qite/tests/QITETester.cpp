/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *  Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Optimizer.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"

TEST(QITETester, checkSimple) 
{
  auto qite = xacc::getService<xacc::Algorithm>("qite");
  std::shared_ptr<xacc::Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("0.7071067811865475 X0 + 0.7071067811865475 Z0");
  auto acc = xacc::getAccelerator("qpp");
  const int nbSteps = 25;
  const double stepSize = 0.1;

  const bool initOk =  qite->initialize({
    std::make_pair("accelerator", acc),
    std::make_pair("steps", nbSteps),
    std::make_pair("observable", observable),
    std::make_pair("step-size", stepSize)
  });

  EXPECT_TRUE(initOk);
  
  auto buffer = xacc::qalloc(1);
  qite->execute(buffer);
  const double finalEnergy = (*buffer)["opt-val"].as<double>();
  std::cout << "Final Energy: " << finalEnergy << "\n";
  // Fig (2.e) of https://www.nature.com/articles/s41567-019-0704-4
  // Minimal Energy = -1
  EXPECT_NEAR(finalEnergy, -1.0, 1e-3);
  const std::vector<double> energyValues = (*buffer)["exp-vals"].as<std::vector<double>>();
  EXPECT_EQ(energyValues.size(), nbSteps + 1);
}

TEST(QITETester, checkDeuteuronH2) 
{
  auto qite = xacc::getService<xacc::Algorithm>("qite");
  std::shared_ptr<xacc::Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1");
  auto acc = xacc::getAccelerator("qpp");
  const int nbSteps = 10;
  const double stepSize = 0.1;

  const bool initOk =  qite->initialize({
    std::make_pair("accelerator", acc),
    std::make_pair("steps", nbSteps),
    std::make_pair("observable", observable),
    std::make_pair("step-size", stepSize)
  });

  EXPECT_TRUE(initOk);
  
  auto buffer = xacc::qalloc(2);
  qite->execute(buffer);
  const double finalEnergy = (*buffer)["opt-val"].as<double>();
  std::cout << "Final Energy: " << finalEnergy << "\n";
  EXPECT_NEAR(finalEnergy, -1.74886, 1e-3);
  buffer->print();
}

TEST(QITETester, checkDeuteuronH3) 
{
  auto qite = xacc::getService<xacc::Algorithm>("qite");
  std::shared_ptr<xacc::Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + 9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2");
  auto acc = xacc::getAccelerator("qpp");
  const int nbSteps = 10;
  const double stepSize = 0.1;

  const bool initOk =  qite->initialize({
    std::make_pair("accelerator", acc),
    std::make_pair("steps", nbSteps),
    std::make_pair("observable", observable),
    std::make_pair("step-size", stepSize)
  });

  EXPECT_TRUE(initOk);
  
  auto buffer = xacc::qalloc(3);
  qite->execute(buffer);
  const double finalEnergy = (*buffer)["opt-val"].as<double>();
  std::cout << "Final Energy: " << finalEnergy << "\n";
  EXPECT_NEAR(finalEnergy, -2.04482, 1e-3);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
