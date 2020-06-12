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
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"
#include "AlgorithmGradientStrategy.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(AdaptVQETester, checkSimple) {

  xacc::set_verbose(true);
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(4);
  auto optimizer = xacc::getOptimizer("nlopt", {std::make_pair("nlopt-optimizer","l-bfgs")});
  auto adapt_vqe = xacc::getService<Algorithm>("adapt-vqe");

  auto str = std::string("(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
                          "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
                          "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
                          "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
                          "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
                          "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
                          "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
                          "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
                          "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
                          "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
                          "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
                          "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
                          "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
                          "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
                          "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
                          "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
                          "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
                          "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
                          "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
                          "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
                          "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
                          "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)");


  auto H = xacc::quantum::getObservable("fermion", str);

  EXPECT_TRUE(adapt_vqe->initialize({std::make_pair("accelerator",acc),
                                std::make_pair("observable", H),
                                std::make_pair("optimizer", optimizer),
                                std::make_pair("pool", "singlet-adapted-uccsd"),
                                std::make_pair("gradient-strategy", "parameter-shift-gradient"),
                                std::make_pair("nElectrons", 2)}));

  adapt_vqe->execute(buffer);
  EXPECT_NEAR(-1.13717, buffer->getInformation("opt-val").as<double>(), 1e-4);

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}