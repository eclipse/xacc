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
 *  Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"

TEST(QLanczosTester, checkSimple) 
{
  auto qLanczos = xacc::getService<xacc::Algorithm>("QLanczos");
  std::shared_ptr<xacc::Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("0.7071067811865475 X0 + 0.7071067811865475 Z0");
  auto acc = xacc::getAccelerator("qpp");
  const int nbSteps = 25;
  const double stepSize = 0.1;

  const bool initOk =  qLanczos->initialize({
    std::make_pair("accelerator", acc),
    std::make_pair("steps", nbSteps),
    std::make_pair("observable", observable),
    std::make_pair("step-size", stepSize)
  });

  EXPECT_TRUE(initOk);
  
  auto buffer = xacc::qalloc(1);
  qLanczos->execute(buffer);
  const double finalEnergy = (*buffer)["opt-val"].as<double>();
  std::cout << "Final Energy: " << finalEnergy << "\n";
  // Fig (2.e) of https://www.nature.com/articles/s41567-019-0704-4
  // Minimal Energy = -1
  EXPECT_NEAR(finalEnergy, -1.0, 1e-3);
  const std::vector<double> energyValues = (*buffer)["exp-vals"].as<std::vector<double>>();
  // Even steps only
  EXPECT_LT(energyValues.size(), (nbSteps + 1) / 2 + 1);
  // Should converge must faster (after 2 Lanczos steps)
  // see Fig. 2(e)
  for (int i = 1; i < energyValues.size(); ++i)
  {
    EXPECT_LT(energyValues[i], 0.9);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
