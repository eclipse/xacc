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
const std::string src =
    R"rucc(__qpu__ void f(qbit q, double t0, double t1, double t2) {
    Rx(q[0], t0);
    Ry(q[0], t1);
    Rx(q[0], t2);
})rucc";

TEST(DDCLTester, checkJSSimpleGradientFree) {
  if (xacc::hasAccelerator("local-ibm")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(1);

    auto simple = xacc::getCompiled("f");

    // get cobyla optimizer
    auto optimizer = xacc::getOptimizer(
        "nlopt", HeterogeneousMap{std::make_pair("nlopt-maxeval", 20)});

    std::vector<double> target{.5, .5};

    auto ddcl = xacc::getService<Algorithm>("ddcl");
    EXPECT_TRUE(ddcl->initialize(
        {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
         std::make_pair("target_dist", target), std::make_pair("loss", "js"),
         std::make_pair("optimizer", optimizer)}));
    ddcl->execute(buffer);

    std::cout << buffer->getInformation("opt-params").as<std::vector<double>>()
              << "\n";
    auto loss = buffer->getInformation("opt-val").as<double>();
    EXPECT_NEAR(loss, 0.0, 1e-3);
  }
}

TEST(DDCLTester, checkMMDSimpleGradientFree) {
  if (xacc::hasAccelerator("local-ibm")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(1);

    auto simple = xacc::getCompiled("f");

    // get cobyla optimizer
    auto optimizer = xacc::getOptimizer(
                                        "nlopt", HeterogeneousMap{std::make_pair("nlopt-maxeval", 20)});

    std::vector<double> target{.5, .5};

    auto ddcl = xacc::getService<Algorithm>("ddcl");
    EXPECT_TRUE(ddcl->initialize(
                                 {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
                                  std::make_pair("target_dist", target), std::make_pair("loss", "mmd"),
                                  std::make_pair("optimizer", optimizer)}));
    ddcl->execute(buffer);

    std::cout << buffer->getInformation("opt-params").as<std::vector<double>>()
              << "\n";
    auto loss = buffer->getInformation("opt-val").as<double>();
    EXPECT_NEAR(loss, 0.0, 1e-3);
  }
}

TEST(DDCLTester, checkJSSimpleWithGradient) {
  if (xacc::hasAccelerator("local-ibm")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(1);

    auto simple = xacc::getCompiled("f");

    // get cobyla optimizer
    auto optimizer = xacc::getOptimizer(
        "nlopt", HeterogeneousMap{std::make_pair("nlopt-maxeval", 20), std::make_pair("nlopt-ftol", 1e-4), std::make_pair("initial-parameters", std::vector<double>{1.5, 0, 1.5}),
                                  std::make_pair("nlopt-optimizer", "l-bfgs")});

    std::vector<double> target{.5, .5};

    auto ddcl = xacc::getService<Algorithm>("ddcl");
    EXPECT_TRUE(ddcl->initialize(
        {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
         std::make_pair("target_dist", target), std::make_pair("loss", "js"),
         std::make_pair("gradient", "js-parameter-shift"),
         std::make_pair("optimizer", optimizer)}));
    ddcl->execute(buffer);

    std::cout << buffer->getInformation("opt-params").as<std::vector<double>>()
              << "\n";

    auto loss = buffer->getInformation("opt-val").as<double>();
    EXPECT_NEAR(loss, 0.0, 1e-3);
  }
}

TEST(DDCLTester, checkMMDSimpleWithGradient) {
  if (xacc::hasAccelerator("local-ibm")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(1);

    auto simple = xacc::getCompiled("f");

    // get cobyla optimizer
    auto optimizer = xacc::getOptimizer(
        "nlopt", HeterogeneousMap{std::make_pair("nlopt-maxeval", 20), std::make_pair("nlopt-ftol", 1e-4), std::make_pair("initial-parameters", std::vector<double>{1.5, 0, 1.5}),
                                  std::make_pair("nlopt-optimizer", "l-bfgs")});

    std::vector<double> target{.5, .5};

    auto ddcl = xacc::getService<Algorithm>("ddcl");
    EXPECT_TRUE(ddcl->initialize(
        {std::make_pair("ansatz", simple), std::make_pair("accelerator", acc),
         std::make_pair("target_dist", target), std::make_pair("loss", "mmd"),
         std::make_pair("gradient", "mmd-parameter-shift"),
         std::make_pair("optimizer", optimizer)}));
    ddcl->execute(buffer);

    std::cout << buffer->getInformation("opt-params").as<std::vector<double>>()
              << "\n";

    auto loss = buffer->getInformation("opt-val").as<double>();
    EXPECT_NEAR(loss, 0.0, 1e-3);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  int ret = 0;
  if (xacc::hasAccelerator("local-ibm")) {
    auto acc = xacc::getAccelerator("aer");
    auto compiler = xacc::getCompiler("xasm");
    auto ir = compiler->compile(src, acc);
    ::testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();
  }
  xacc::Finalize();
  return ret;
}
