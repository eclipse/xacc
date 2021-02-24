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
 ******************************************************************************/
#include <gtest/gtest.h>
#include <memory>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(QCMXTester, checkSimple) {

  auto H = xacc::quantum::getObservable(
      "pauli", std::string("0.2976 + 0.3593 Z0 - 0.4826 Z1 + 0.5818 Z0 Z1 + "
                           "0.0896 X0 X1 + 0.0896 Y0 Y1"));

  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  auto ansatz = provider->createComposite("initial-state");
  ansatz->addInstruction(provider->createInstruction("X", {(size_t)0}));

  auto acc = xacc::getAccelerator("qpp");

  auto qcmx = xacc::getService<xacc::Algorithm>("qcmx");
  qcmx->initialize({{"accelerator", acc},
                    {"observable", H},
                    {"ansatz", ansatz},
                    {"cmx-order", 2}});

  auto buffer = qalloc(2);
  qcmx->execute(buffer);
  auto map =
      buffer->getInformation("energies").as<std::map<std::string, double>>();

  EXPECT_NEAR(-1.14499, map["PDS(2)"], 1e-3);
  EXPECT_NEAR(-1.14517, map["CMX(2)"], 1e-3);
  EXPECT_NEAR(-1.14517, map["Knowles(2)"], 1e-3);
  EXPECT_NEAR(-1.14499, map["Soldatov"], 1e-3);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}