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
#include <utility>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Optimizer.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"
#include "AlgorithmGradientStrategy.hpp"

using namespace xacc;
using namespace xacc::quantum;


TEST(AdaptTesterQAOA, checkAdaptQAOA) {

  xacc::set_verbose(true);
  auto acc_qaoa = xacc::getAccelerator("qpp");
  auto buffer_qaoa = xacc::qalloc(4);
  auto optimizer_qaoa = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-optimizer", "l-bfgs")});
  auto adapt_qaoa = xacc::getService<xacc::Algorithm>("adapt");
  auto pool_qaoa = "multi-qubit-qaoa";
  auto subAlgo_qaoa = "QAOA"; 

  auto H_qaoa = xacc::quantum::getObservable(
      "pauli", std::string("-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3"));

  EXPECT_TRUE(adapt_qaoa->initialize({std::make_pair("accelerator",acc_qaoa),
                                std::make_pair("observable", H_qaoa),
                                std::make_pair("optimizer", optimizer_qaoa),
                                std::make_pair("pool", pool_qaoa),
                                std::make_pair("maxiter", 2), // this is the number of layers
                                std::make_pair("sub-algorithm", subAlgo_qaoa)
                                }));

  adapt_qaoa->execute(buffer_qaoa);
  EXPECT_NEAR(-9.4999, buffer_qaoa->getInformation("opt-val").as<double>(), 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}