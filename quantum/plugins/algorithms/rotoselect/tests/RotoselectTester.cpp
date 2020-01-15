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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"

using namespace xacc;

TEST(RotoselectTester, checkSimple) 
{
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);
 
  std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString("0.01128 Z0 Z1 + 0.180931 X0 X1 + 0.397936 Z0 + 0.397936 Z1");

  auto rotoselect = xacc::getService<Algorithm>("rotoselect");
  EXPECT_TRUE(rotoselect->initialize({
                                      std::make_pair("accelerator", acc),
                                      std::make_pair("observable", observable),
                                      std::make_pair("layers", 4),
                                      std::make_pair("iterations", 100),
                                    }));
  rotoselect->execute(buffer);
  const auto result = buffer->getInformation("opt-val").as<double>();
  // The expected result is somewhere around -0.8 (see FIG. 4 of https://arxiv.org/abs/1905.09692)
  // Note: the graph resolution is not very high hence could not get a very accurate reading.
  // For now, allow a high tolerance (0.2) to prevent false failure.
  EXPECT_NEAR(result, -0.8, 0.2);
  std::cout << "Optimal value: " << buffer->getInformation("opt-val").as<double>() << "\n";
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
