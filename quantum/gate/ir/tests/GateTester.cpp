#include <gtest/gtest.h>
#include <stdexcept>
#include "CommonGates.hpp"
#include "Circuit.hpp"
#include "XACC.hpp"
using namespace xacc::quantum;

TEST(GateTester, checkBasicGatesAndCircuits) {

  auto h = std::make_shared<Hadamard>(0);
  auto cx = std::make_shared<CNOT>(0, 1);
  auto m0 = std::make_shared<Measure>(0);
  auto ry = std::make_shared<Ry>(1, "theta");

  Circuit circuit("foo");
  EXPECT_EQ("foo", circuit.name());
  circuit.addInstruction(h);
  circuit.addInstruction(cx);
  circuit.addInstruction(m0);

  EXPECT_ANY_THROW(circuit.addInstruction(ry));
  EXPECT_EQ(3, circuit.nInstructions());

  circuit.addVariable("theta");
  circuit.addInstruction(ry);

  EXPECT_EQ(4, circuit.nInstructions());
  EXPECT_FALSE(circuit.isAnalog());
  EXPECT_TRUE(circuit.isComposite());
  EXPECT_EQ("H", circuit.getInstruction(0)->name());
  EXPECT_EQ("Ry", circuit.getInstruction(3)->name());

  circuit.removeInstruction(2);
  EXPECT_EQ(3, circuit.nInstructions());
  EXPECT_EQ("H", circuit.getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit.getInstruction(1)->name());
  EXPECT_EQ("Ry", circuit.getInstruction(2)->name());

  circuit.replaceInstruction(2, m0);
  EXPECT_EQ(3, circuit.nInstructions());
  EXPECT_EQ("H", circuit.getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit.getInstruction(1)->name());
  EXPECT_EQ("Measure", circuit.getInstruction(2)->name());

  circuit.replaceInstruction(2, ry);
  circuit.addInstruction(m0);

  EXPECT_EQ(4, circuit.nInstructions());
  EXPECT_EQ("H", circuit.getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit.getInstruction(1)->name());
  EXPECT_EQ("Ry", circuit.getInstruction(2)->name());
  EXPECT_EQ("Measure", circuit.getInstruction(3)->name());

  EXPECT_EQ("theta",
            circuit.getInstruction(2)->getParameter(0).as<std::string>());
  auto evaled = circuit({2.2});
  EXPECT_EQ(2.2, evaled->getInstruction(2)->getParameter(0).as<double>());

  auto ry0 = std::make_shared<Ry>(0, "t0");
  auto ry1 = std::make_shared<Ry>(1, "t1");
  auto ry2 = std::make_shared<Ry>(2, "t2");
  auto ry3 = std::make_shared<Ry>(3, "t3");
  auto ry4 = std::make_shared<Ry>(4, "t4");
  auto ry5 = std::make_shared<Ry>(5, "-pi + 3*pi");
  auto ry6 = std::make_shared<Ry>(5, "-pi + 3*t0 + 2*t1");
  auto ry7 = std::make_shared<Ry>(5, "-pi + 3*t0 + 2*t1 + 33*s0");

  Circuit bar("bar", {"t0", "t1", "t2", "t3", "t4"});
  bar.addInstruction(ry0);
  bar.addInstruction(ry1);
  bar.addInstruction(ry2);
  bar.addInstruction(ry3);
  bar.addInstruction(ry4);
  bar.addInstruction(ry5);
  bar.addInstruction(ry6);

  std::cout << bar.toString() << "\n";

  EXPECT_ANY_THROW(bar.addInstruction(ry7));

  bar.addVariable("s0");
  bar.addInstruction(ry7);
  evaled = bar({.22, .33, .44, .55, .66, .77});
  std::cout << "\n" << evaled->toString() << "\n";

  EXPECT_NEAR(.22, evaled->getInstruction(0)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.33, evaled->getInstruction(1)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.44, evaled->getInstruction(2)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.55, evaled->getInstruction(3)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.66, evaled->getInstruction(4)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(6.28319, evaled->getInstruction(5)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(-1.82159, evaled->getInstruction(6)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(23.5884, evaled->getInstruction(7)->getParameter(0).as<double>(),
              1e-4);

  bar.getInstruction(0)->disable();
  EXPECT_EQ(8, bar.nInstructions());
  auto enabled = bar.enabledView();
  EXPECT_EQ(7, enabled->nInstructions());
}

TEST(GateTester, checkTerminatingNode) {
  class sub_circuit : public Circuit {
  public:
    sub_circuit(const std::string name) : Circuit(name) {}
    bool expand(const xacc::HeterogeneousMap &runtimeOptions) override {
      if (!runtimeOptions.keyExists<int>("key1") &&
          !runtimeOptions.keyExists<int>("key2")) {
        return false;
      }

      auto val1 = runtimeOptions.get<int>("key1");
      auto val2 = runtimeOptions.get<int>("key2");

      for (int i = 0; i < val1; i++) {
        auto h = std::make_shared<Hadamard>(i);
        addInstruction(h);
      }
      for (int i = 0; i < val2; i++) {
        addVariable(std::string("t") + std::to_string(i));
        auto rz = std::make_shared<Rz>(i, std::string("t") + std::to_string(i));
        addInstruction(rz);
      }

      return true;
    }
    const std::vector<std::string> requiredKeys() override {
      return {"key1", "key2"};
    }
  };

  auto sub = std::make_shared<sub_circuit>("sub");
  auto h = std::make_shared<Hadamard>(0);
  auto cx = std::make_shared<CNOT>(0, 1);

  auto circuit = std::make_shared<Circuit>("foo");
  EXPECT_EQ("foo", circuit->name());
  circuit->addInstruction(h);
  circuit->addInstruction(cx);
  circuit->addInstruction(sub);

  std::cout << "Circuit:\n" << circuit->toString() << "\n";

  xacc::HeterogeneousMap m(std::make_pair("key1", 3),
                           std::make_pair("key2", 4));
  std::cout << "HELLO\n";
  circuit->expand(m);
  std::cout << "Circuit2:\n" << circuit->toString() << "\n";

}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
