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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Optimizer.hpp"
#include "Algorithm.hpp"

using namespace xacc;

TEST(mcVqeTester, checkSimple) {
//   if (xacc::hasAccelerator("qpp")) {
    auto acc = xacc::getAccelerator("tnqvm", {std::make_pair("tnqvm-visitor","exatn")});
    //auto acc = xacc::getAccelerator("qpp");
    auto buffer = xacc::qalloc(10);
    auto optimizer = xacc::getOptimizer("nlopt");
    //xacc::setOption("tnqvm-verbose", "1");
    auto mc_vqe = xacc::getService<Algorithm>("mc-vqe");
    EXPECT_TRUE(mc_vqe->initialize({std::make_pair("accelerator",acc),
                                std::make_pair("optimizer",optimizer),
                                std::make_pair("nChromophores", 10)}));
    mc_vqe->execute(buffer);
    //EXPECT_NEAR(-1.13717, mpark::get<double>(buffer->getInformation("opt-val")), 1e-4);
    //EXPECT_NEAR(-1.13717,vqe->execute(buffer, (*buffer)["opt-params"].as<std::vector<double>>())[0], 1e-4);
    // std::cout << "EVALED: " << vqe->execute(buffer, (*buffer)["opt-params"].as<std::vector<double>>()) << "\n";
//   }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
