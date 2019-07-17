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

#include "XACC.hpp"
#include "Algorithm.hpp"

using namespace xacc;

class TestAlgorithm : public Algorithm {
    public:
  bool initialize(const AlgorithmParameters &parameters) override {
      return parameters.count("observable") && parameters.count("key1");
  }
  const std::vector<std::string> requiredParameters() const override {
      return {"observable"};
  }

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override {
      return;
  }
    const std::string name() const override {return "test";}
    const std::string description() const override {return "";}
};

class TestObservable : public Observable {
public:
  std::vector<std::shared_ptr<Function>>
  observe(std::shared_ptr<Function> function) override {}
  const std::string toString() override { return "";}
  void fromString(const std::string str) override {}
  const int nBits() override {return 0;}
  const std::string name() const override {return "test";}
  const std::string description() const override {return "";}
};

TEST(AlgorithmTester, checkSimple) {
  TestAlgorithm alg;
  EXPECT_TRUE(alg.requiredParameters() == std::vector<std::string>{"observable"});
  auto obs = std::make_shared<TestObservable>();
  auto buffer = qalloc(2);
  EXPECT_TRUE(alg.initialize({{"observable", obs}, {"key1", "val1"}}));
  alg.execute(buffer);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
