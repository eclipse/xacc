/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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

using namespace xacc;

TEST(XACCAPITester, checkCacheFunctions) {

    using complextype = std::vector<std::pair<double,double>>;
    std::string file = "test.json";
    std::string p {"t"};
    complextype
    q{{0.0,0.0},{0.5,0.0},{1.0,0.0}};

    xacc::info("Appending cache");
    xacc::appendCache(file, "hello", 22);
    std::cout << "appended cache\n";
    xacc::appendCache(file, "hello2", p);
    xacc::appendCache(file, "samples", q);

    auto cache = xacc::getCache(file);
    EXPECT_TRUE(cache.keyExists<int>("hello"));
    EXPECT_TRUE(cache.keyExists<std::string>("hello2"));
    EXPECT_TRUE(cache.keyExists<complextype>("samples"));

    EXPECT_EQ(22, cache.get<int>("hello"));
    EXPECT_EQ("t", cache.get<std::string>("hello2"));

    auto samples =
    cache.get<complextype>("samples");//.as<std::vector<std::pair<double,double>>>();

    EXPECT_EQ(0.0, samples[0].first);
    EXPECT_EQ(0.0, samples[0].second);
    EXPECT_EQ(0.5, samples[1].first);
    EXPECT_EQ(0.0, samples[1].second);
    EXPECT_EQ(1.0, samples[2].first);
    EXPECT_EQ(0.0, samples[2].second);

    std::string toRemove = xacc::getRootDirectory() + "/" + file;
    std::remove(toRemove.c_str());
}

TEST(XACCAPITester, checkQasm) {
  if (xacc::hasCompiler("quil")) {
    xacc::qasm(R"(.compiler quil
.circuit foo
.parameters theta, phi
X 0
H 2
CNOT 2 1
CNOT 0 1
Rz(theta) 0
Ry(phi) 1
H 0
MEASURE 0 [0]
MEASURE 1 [1]
.circuit foo2
foo(theta, phi)
X 0
H 0
MEASURE 0 [0]
)");

    auto function = xacc::getCompiled("foo");
    function = function->operator()({3.1415, 1.57});

    std::cout << function->toString() << "\n";

    auto foo2 = xacc::getCompiled("foo2");
    std::cout << foo2->operator()({3.3, 2.2})->toString() << "\n";
  }
}

TEST(XACCAPITester, checkXasm) {
    xacc::qasm(R"(.compiler xasm
.circuit ansatz22
.parameters t0, t1
.qbit q
X(q[0]);
Ry(q[1],t0);
Rx(q[0], t1);
CX(q[1],q[0]);
)");

    auto function = xacc::getCompiled("ansatz22");
    function = function->operator()({3.1415, 1.56});

    std::cout << function->toString() << "\n";

     xacc::qasm(R"(.compiler xasm
.circuit ansatz2
.parameters t
.qbit q
X(q[0]);
Ry(q[1],t[0]);
Rx(q[0], t[1]);
CX(q[1],q[0]);
)");

    auto function2 = xacc::getCompiled("ansatz2");
    function2 = function2->operator()({3.1415, 1.56});

    std::cout << function2->toString() << "\n";
}

TEST(XACCAPITester, checkXasmBug) {
      xacc::qasm(R"(.compiler xasm
.circuit ansatz3
.parameters t, s
.qbit q
X(q[0]);
exp_i_theta(q, t, {{"pauli", "X0 Y1 - Y0 X1"}});
exp_i_theta(q, s, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
)");

  auto function3 = xacc::getCompiled("ansatz3");
  std::cout << function3->getVariables() << "\n";
  std::cout << function3->toString() << "\n";
    function3 = function3->operator()({3.1415, 1.56});

    std::cout << function3->toString() << "\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
