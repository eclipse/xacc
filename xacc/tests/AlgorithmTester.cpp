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
#include "Algorithm.hpp"

using namespace xacc;
using AlgorithmParameters = HeterogeneousMap;

class TestAlgorithm : public Algorithm {
public:
  bool initialize(const AlgorithmParameters &parameters) override {
    std::cout << std::boolalpha
              << parameters.keyExists<std::shared_ptr<Observable>>("observable")
              << ", " << parameters.keyExists<std::string>("key1") << "\n";
    return parameters.keyExists<std::shared_ptr<Observable>>("observable") &&
           parameters.keyExists<std::string>("key1");
  }
  const std::vector<std::string> requiredParameters() const override {
    return {"observable"};
  }
  DEFINE_ALGORITHM_CLONE(TestAlgorithm)

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override {
    return;
  }
  const std::string name() const override { return "test"; }
  const std::string description() const override { return ""; }
};

class TestObservable : public Observable {
public:
  std::vector<std::shared_ptr<CompositeInstruction>>
  observe(std::shared_ptr<CompositeInstruction> CompositeInstruction) override {
    return {};
  }
  const std::string toString() override { return ""; }
  void fromString(const std::string str) override {}
  const int nBits() override { return 0; }
  const std::string name() const override { return "test"; }
  const std::string description() const override { return ""; }
  void fromOptions(const HeterogeneousMap &options) override {}
  virtual double postProcess(std::shared_ptr<AcceleratorBuffer> buffer,
                      const std::string &postProcessTask,
                      const HeterogeneousMap &extra_data) override {
    return 0.0;
  }
};

TEST(AlgorithmTester, checkSimple) {
  TestAlgorithm alg;
  EXPECT_TRUE(alg.requiredParameters() ==
              std::vector<std::string>{"observable"});
  std::shared_ptr<Observable> obs = std::make_shared<TestObservable>();
  auto buffer = qalloc(2);
  HeterogeneousMap m(std::make_pair("observable", obs),
                     std::make_pair("key1", std::string("val1")));
  EXPECT_TRUE(alg.initialize(m));
  alg.execute(buffer);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
