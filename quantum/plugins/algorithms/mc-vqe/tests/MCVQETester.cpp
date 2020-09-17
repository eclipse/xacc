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

TEST(mcVqeTester, check4qubitExaTN) {

    std::string path = "/home/cades/dev/xacc/quantum/plugins/algorithms/mc-vqe/tests/datafile.txt";
    auto acc = xacc::getAccelerator("tnqvm", {std::make_pair("tnqvm-visitor","exatn")});
    auto buffer = xacc::qalloc(4);
    auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-maxeval", 1)});
    auto mc_vqe = xacc::getService<Algorithm>("mc-vqe");
    EXPECT_TRUE(mc_vqe->initialize({std::make_pair("accelerator",acc),
                                std::make_pair("optimizer",optimizer),
                                std::make_pair("data-path", path),
                                std::make_pair("nChromophores", 4)}));
    mc_vqe->execute(buffer);

}

TEST(mcVqeTester, check4qubitITensorMPS) {

    std::string path = "/home/cades/dev/xacc/quantum/plugins/algorithms/mc-vqe/tests/datafile.txt";
    auto acc = xacc::getAccelerator("tnqvm", {std::make_pair("tnqvm-visitor","itensor-mps")});
    auto buffer = xacc::qalloc(4);
    auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-maxeval", 1)});
    auto mc_vqe = xacc::getService<Algorithm>("mc-vqe");
    EXPECT_TRUE(mc_vqe->initialize({std::make_pair("accelerator",acc),
                                std::make_pair("optimizer",optimizer),
                                std::make_pair("data-path", path),
                                std::make_pair("nChromophores", 4)}));
    mc_vqe->execute(buffer);

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
