/**********************************************************************
 * Copyright (c)
**********************************************************************/

#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"

using namespace xacc;
//using namespace xacc::quantum;

TEST(QPlacementTester, checkSimpleConnection) {
  std::cout << "TEST: test accelerator has 2 qubits. \n";
  int e1 = 0;
  int e2 = 0;

  xacc::setAccelerator("dummy");
  auto acc = xacc::getAccelerator("dummy");
  auto hw = acc->getConnectivity();
  std::cout << "\tConnectivity is " << hw << "\n";
  for (auto &edge : hw) {
    std::cout << "first edge: " << edge.first << "\n\t second: " << edge.second << std::endl;
   e1 = edge.first;
   e2 = edge.second;
  }

  EXPECT_EQ(e1, 3);
  EXPECT_EQ(e2, 4);
}


TEST(QPlacementTester, checkBellSimple) {
  // use the dummy accelerator
  xacc::setAccelerator("dummy");
  auto acc = xacc::getAccelerator("dummy");

  auto compiler = xacc::getCompiler("xasm");
  auto bell_IR = compiler->compile(R"(__qpu__ void bell(qbit q) {
    H(q[0]);
    CX(q[0], q[1]);
    Measure(q[0]);
    Measure(q[1]);
  })");
  auto bell_composite = bell_IR->getComposites()[0];

  auto A = xacc::getIRTransformation("minor-graph-embedding-placement");
  HeterogeneousMap m;
  A->apply(bell_composite, //std::shared_ptr<CompositeInstruction> function,
                         acc, //const std::shared_ptr<Accelerator> acc,
                         m);

  // check bell_composite holds new mapped value
  std::cout << "new map: " << bell_composite->toString() << std::endl;
  EXPECT_EQ(4, bell_composite->nInstructions());

  EXPECT_TRUE(std::vector<long unsigned int>{4} == bell_composite->getInstruction(0)->bits() || std::vector<long unsigned int>{3} == bell_composite->getInstruction(0)->bits());
  EXPECT_TRUE((std::vector<long unsigned int>{4,3}) ==  bell_composite->getInstruction(1)->bits() || (std::vector<long unsigned int>{3,4}) == bell_composite->getInstruction(1)->bits());
//  EXPECT_EQ(std::vector<int>{3}, bell_composite->getInstruction(2)->bits());
//  EXPECT_EQ(std::vector<int>{4}, bell_composite->getInstruction(3)->bits());
}


// subclass Accelerator since can't run real in a test: no http calls
class DummyAccelerator : public xacc::Accelerator {
public:
  const std::string name() const override { return "dummy"; }
  const std::string description() const override { return ""; }
  void initialize(const HeterogeneousMap &params = {}) override { return; }
  void execute(std::shared_ptr<xacc::AcceleratorBuffer> buf,
               std::shared_ptr<xacc::CompositeInstruction> f) override {}
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override {}
  void updateConfiguration(const HeterogeneousMap &config) override {}
  const std::vector<std::string> configurationKeys() override { return {}; }

  std::vector<std::pair<int,int>> getConnectivity() override {
    std::vector<std::pair<int,int>> graph;
    // add some dummy connections
    graph.push_back({3,4});
    return graph;
  }
};

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  std::shared_ptr<Accelerator> acc = std::make_shared<DummyAccelerator>();
  xacc::contributeService("dummy", acc);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
