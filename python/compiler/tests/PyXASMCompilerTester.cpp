#include "gtest/gtest.h"

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "Circuit.hpp"

TEST(PyXASMCompilerTester, checkSimple) {

  auto compiler = xacc::getCompiler("pyxasm");
  auto IR = compiler -> compile(R"(def bell(q, t0):
  H(q[0])
  CX(q[0], q[1])
  Ry(q[0], t0)
  Measure(q[0])
  Measure(q[1])
)");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

}

TEST(PyXASMCompilerTester, checkVectorArg) {

  auto compiler = xacc::getCompiler("pyxasm");
  auto IR = compiler -> compile(R"(def bell22(q, t):
  H(q[0])
  CX(q[0], q[1])
  Ry(q[0], t[0])
  Measure(q[0])
  Measure(q[1])
)");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
  std::cout << "KERNEL\n" << IR->getComposites()[0]->operator()({2.})->toString() << "\n";

}
TEST(PyXASMCompilerTester, checkApplyAll) {
class custom_range : public xacc::quantum::Circuit {
  public:
    custom_range() : Circuit("range") {}
    bool expand(const xacc::HeterogeneousMap &runtimeOptions) override {
      if (!runtimeOptions.keyExists<int>("nq") &&
          !runtimeOptions.keyExists<std::string>("gate")) {
        return false;
      }

      auto val1 = runtimeOptions.get<int>("nq");
      auto val2 = runtimeOptions.get<std::string>("gate");

      auto provider = xacc::getIRProvider("quantum");
      auto f = provider->createComposite("range" + val2);
      for (std::size_t i = 0; i < val1; i++) {
         auto g = provider->createInstruction(val2, std::vector<std::size_t>{i});
         addInstruction(g);
      }
      return true;
    }
    const std::vector<std::string> requiredKeys() override {
      return {"nq", "gate"};
    }
    DEFINE_CLONE(custom_range);
  };
  std::shared_ptr<xacc::Instruction> service = std::make_shared<custom_range>();
  xacc::contributeService("custom_range", service);

  auto compiler = xacc::getCompiler("pyxasm");
  auto IR = compiler -> compile(R"(def foo(q):
  custom_range(q, {"gate":"H","nq":4})
)");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
}

TEST(PyXASMCompilerTester, checkUCCSD) {
auto compiler = xacc::getCompiler("pyxasm");
  auto IR = compiler -> compile(R"(def ansatz_vqe(buffer, args):
    uccsd(q, {"ne":2,"nq":4})
    )");

  std::cout << IR->getComposites()[0]->toString() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
