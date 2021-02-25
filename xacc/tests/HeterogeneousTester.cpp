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
#include "CompositeInstruction.hpp"
#include "heterogeneous.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include "xacc.hpp"
#include "Algorithm.hpp"
#include "xacc_service.hpp"

class print_visitor : public xacc::visitor_base<int, double> {
public:
  template <typename T> void operator()(const std::string &s, const T &t) {
    std::cout << s << ": " << t << "\n";
  }
};

void test_set_ptr(xacc::HeterogeneousMap &map) {
  auto simple_kernel =
      xacc::getCompiler("xasm")
          ->compile(R"#(__qpu__ void foo(qreg q) { H(q[0]); })#")
          ->getComposites()[0];
  map.insert("hello-world", simple_kernel);
}

TEST(HeterogeneousMapTester, checkBetterConstruction) {
  auto provider = xacc::getIRProvider("quantum");
  auto f = provider->createComposite("f", {});
  xacc::HeterogeneousMap m{
      {"key1", 2.2}, {"key2", "hello"}, {"custom_composite", f}};
  std::cout << m.get<double>("key1") << "\n";
  std::cout << m.getString("key2") << "\n";
  EXPECT_EQ(2.2, m.get<double>("key1"));
  EXPECT_EQ("hello", m.getString("key2"));
  EXPECT_EQ("f", m.get<std::shared_ptr<xacc::CompositeInstruction>>("custom_composite")->name());
}

TEST(HeterogeneousMapTester, checkSetValueFromFunctionRef) {
  xacc::HeterogeneousMap m;
  m.insert("hello-world", nullptr);
  test_set_ptr(m);
  std::cout << m.get<std::shared_ptr<xacc::CompositeInstruction>>("hello-world")
                   ->toString()
            << "\n";
}

TEST(HeterogeneousMapTester, checkSimple) {
  xacc::HeterogeneousMap c;
  c.insert("intkey", 1);
  c.insert("intkey2", 2);
  c.insert("doublekey", 2.2);
  c.insert("variable", std::string("t0"));

  print_visitor v;
  c.visit(v);

  EXPECT_EQ(2, c.number_of<int>());
  EXPECT_EQ(2.2, c.get<double>("doublekey"));
  EXPECT_TRUE(c.keyExists<int>("intkey"));
  EXPECT_TRUE(c.stringExists("variable"));
  EXPECT_FALSE(c.keyExists<int>("random"));
  EXPECT_EQ("t0", c.getString("variable"));

  EXPECT_ANY_THROW(c.get_with_throw<bool>("test"));

  auto provider = xacc::getIRProvider("quantum");
  auto f = provider->createComposite("f", {});

  c.insert("function", f);

  EXPECT_EQ(
      "f",
      c.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap m2{std::make_pair("doublekey", 2.0),
                            std::make_pair("intkey", 22),
                            std::make_pair("function", f)};
  EXPECT_EQ(2.0, m2.get<double>("doublekey"));
  EXPECT_EQ(22, m2.get<int>("intkey"));
  EXPECT_EQ(
      "f",
      m2.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap m3(std::make_pair("doublekey", 2.0),
                            std::make_pair("intkey", 22),
                            std::make_pair("function", f));

  EXPECT_EQ(2.0, m3.get<double>("doublekey"));
  EXPECT_EQ(22, m3.get<int>("intkey"));
  EXPECT_EQ(
      "f",
      m3.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap cc;
  EXPECT_ANY_THROW(cc.get_with_throw<int>("intkey"));

  EXPECT_EQ("f",
            m3.getPointerLike<xacc::CompositeInstruction>("function")->name());
  EXPECT_TRUE(m3.pointerLikeExists<xacc::CompositeInstruction>("function"));

  m3.insert("doublekey", 222.222);
  EXPECT_EQ(222.222, m3.get<double>("doublekey"));

  xacc::HeterogeneousMap m4({std::pair<std::string, double>{"doublekey", 2.0},
                             std::pair<std::string, int>{"intkey", 22}});

  m4.print<int, double>(std::cout);
}

TEST(HeterogeneousMapTester, checkVQE) {
  using namespace xacc;
  class TestObservable : public xacc::Observable {
  public:
    std::vector<std::shared_ptr<CompositeInstruction>> observe(
        std::shared_ptr<CompositeInstruction> CompositeInstruction) override {
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

  std::shared_ptr<Observable> observable = std::make_shared<TestObservable>();
  xacc::HeterogeneousMap options;
  auto provider = xacc::getIRProvider("quantum");
  auto function = provider->createComposite("f", {});

  options.insert("observable", observable);
  options.insert("ansatz", function);
  auto vqe = xacc::getService<Algorithm>("vqe");

  vqe->initialize(options);
}

TEST(HeterogeneousMapTester, checkMerge) {
  xacc::HeterogeneousMap c;
  c.insert("intkey", 1);
  c.insert("intkey2", 2);
  c.insert("doublekey", 2.2);
  c.insert("variable", std::string("t0"));
  const auto cSizeBefore = c.size();
  xacc::HeterogeneousMap otherMap;
  // Update a key
  otherMap.insert("intkey", 3);
  // Add some other keys
  otherMap.insert("variable1", std::string("t1"));
  const auto otherSize = otherMap.size();
  // Merge:
  c.merge(otherMap);
  // There is one key updated:
  EXPECT_EQ(c.size(), cSizeBefore + otherSize - 1);
  // Check map content:
  EXPECT_EQ(c.get<int>("intkey"), 3);
  EXPECT_EQ(c.get<int>("intkey2"), 2);
  EXPECT_EQ(c.get<double>("doublekey"), 2.2);
  EXPECT_EQ(c.getString("variable"), "t0");
  EXPECT_EQ(c.getString("variable1"), "t1");
  print_visitor v;
  c.visit(v);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
