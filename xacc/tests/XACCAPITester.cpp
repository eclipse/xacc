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

#include "XACC.hpp"

using namespace xacc;

TEST(XACCAPITester, checkCacheFunctions) {

  std::string file = "test.json";
  InstructionParameter p("t");
  InstructionParameter q(std::vector<std::pair<double,double>>{{0.0,0.0},{0.5,0.0},{1.0,0.0}});

  xacc::info("Appending cache");
  xacc::appendCache(file, "hello", InstructionParameter(22));
  std::cout << "appended cache\n";
  xacc::appendCache(file, "hello2", p);
  xacc::appendCache(file, "samples", q);

  auto cache = xacc::getCache(file);
  EXPECT_TRUE(cache.count("hello"));
  EXPECT_TRUE(cache.count("hello2"));
  EXPECT_TRUE(cache.count("samples"));

  EXPECT_EQ(22, cache["hello"].as<int>());
  EXPECT_EQ("t", cache["hello2"].as<std::string>());

  auto samples = cache["samples"].as<std::vector<std::pair<double,double>>>();

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
    xacc::qasm(R"(.compiler quil
.function foo
X 0
H 2
CNOT 2 1
CNOT 0 1
Rz(theta) 0
Ry(phi) 1
H 0
MEASURE 0 [0]
MEASURE 1 [1]
.function foo2
foo(theta, phi)
X 0
H 0
MEASURE 0 [0]
)");

  auto function = xacc::getCompiled("foo");
  function = function->operator()({3.1415, 1.57});

  std::cout << function->toString() << "\n";

  auto foo2 = xacc::getCompiled("foo2");
    std::cout << foo2->operator()({3.3,2.2})->toString() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
