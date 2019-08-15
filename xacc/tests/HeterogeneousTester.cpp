#include "heterogeneous.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include "XACC.hpp"

class print_visitor : public xacc::visitor_base<int,double> {
public:
  template<typename T>
  void operator()(const std::string& s, const T& t) {
      std::cout << s<< ": "<< t << "\n";
  }
};

TEST(HeterogeneousMapTester, checkSimple) {
  xacc::HeterogeneousMap c;
  c.insert("intkey", 1);
  c.insert("intkey2", 2);
  c.insert("doublekey", 2.2);
  c.insert("variable", std::string{"t0"});

  print_visitor v;
  c.visit(v);

  EXPECT_EQ(2, c.number_of<int>());
  EXPECT_EQ(2.2, c.get<double>("doublekey"));
  EXPECT_TRUE(c.keyExists<int>("intkey"));
  EXPECT_TRUE(c.keyExists<std::string>("variable"));
  EXPECT_FALSE(c.keyExists<int>("random"));
  EXPECT_EQ("t0", c.get<std::string>("variable"));

  EXPECT_ANY_THROW(c.get_with_throw<bool>("test"));

//   auto provider = xacc::getIRProvider("gate");
//   auto f = provider->createCompositeInstruction("f", {});

//   c.insert("function", f);

//   EXPECT_EQ(
//       "f",
//       c.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap m2{std::make_pair("doublekey", 2.0),
                            std::make_pair("intkey", 22)};
                            // std::make_pair("function", f)};
  EXPECT_EQ(2.0, m2.get<double>("doublekey"));
  EXPECT_EQ(22, m2.get<int>("intkey"));
//   EXPECT_EQ(
//       "f",
//       m2.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap m3(std::make_pair("doublekey", 2.0),
                            std::make_pair("intkey", 22));
                            // std::make_pair("function", f));

  EXPECT_EQ(2.0, m3.get<double>("doublekey"));
  EXPECT_EQ(22, m3.get<int>("intkey"));
//   EXPECT_EQ(
//       "f",
//       m3.get<std::shared_ptr<xacc::CompositeInstruction>>("function")->name());

  xacc::HeterogeneousMap cc;
  EXPECT_ANY_THROW(cc.get_with_throw<int>("intkey"));

  xacc::HeterogeneousMap m4({std::pair<std::string, double>{"doublekey", 2.0},
                             std::pair<std::string, int>{"intkey", 22}});
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
