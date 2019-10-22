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
#include "xacc_config.hpp"

#include "Optimizer.hpp"
#include "Algorithm.hpp"

using namespace xacc;

TEST(RBMClassificationTester, checkSimpleNist) {
//   auto acc = xacc::getAccelerator("dwave");
  auto buffer = xacc::qalloc();

  auto rbm =
      std::dynamic_pointer_cast<CompositeInstruction>(
          xacc::getService<Instruction>("rbm"));
  EXPECT_TRUE(rbm->expand({std::make_pair("nv", 64), std::make_pair("nh", 64)}));

  auto rbm_classify = xacc::getService<Algorithm>("rbm-classification");
  EXPECT_TRUE(rbm_classify->initialize(
      {std::make_pair("rbm", rbm),
       std::make_pair("model-exp-strategy", "dwave-mcmc"),
       std::make_pair("n-samples", 100),
       std::make_pair(
           "train-file",
           std::string(XACC_RBM_CLASSIFICATION_TESTS_DIR)+std::string("/data/optdigits_csv.csv"))}));
  rbm_classify->execute(buffer);

  EXPECT_TRUE(buffer->hasExtraInfoKey("w"));
  EXPECT_TRUE(buffer->hasExtraInfoKey("bv"));
  EXPECT_TRUE(buffer->hasExtraInfoKey("bh"));
  std::cout << "W:\n" << buffer->getInformation("w").as<std::vector<double>>() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  int ret = 0;
  ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
