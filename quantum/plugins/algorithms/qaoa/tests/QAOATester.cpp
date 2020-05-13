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

using namespace xacc;

TEST(QAOATester, checkSimple) 
{
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(6);
 
  const std::vector<std::string> costHam { "Z0 Z1", "Z3", "Z4 Z5" };
  const std::vector<std::string> refHam { "X0" };
  auto optimizer = xacc::getOptimizer("nlopt");

  auto qaoa = xacc::getService<Algorithm>("QAOA");
  EXPECT_TRUE(
    qaoa->initialize({
                        std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("cost-ham", costHam),
                        std::make_pair("ref-ham", refHam),
                        // number of time steps (p) param
                        std::make_pair("steps", 1)
                      }));
  qaoa->execute(buffer);
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
