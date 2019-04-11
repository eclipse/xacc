/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "AcceleratorBuffer.hpp"
#include "DWQMICompiler.hpp"
#include "DefaultParameterSetter.hpp"
#include "XACC.hpp"
#include <gtest/gtest.h>

using namespace xacc;
using namespace xacc::quantum;

class FakeDWAcc : public xacc::Accelerator {
public:
  virtual std::vector<std::pair<int,int>> getAcceleratorConnectivity() {
    return std::vector<std::pair<int,int>>{{0,4},{0,5},{0,6},{0,7},{1,4},{1,5},{1,6},{1,7},{2,4},{2,5},{2,6},{2,7},{3,4},{3,5},{3,6},{3,7}};
  }

  virtual xacc::Accelerator::AcceleratorType getType() {
    return xacc::Accelerator::AcceleratorType::qpu_aqc;
  }

  virtual bool isValidBufferSize(const int nBits) { return true; }

  virtual std::vector<std::shared_ptr<xacc::IRTransformation>>
  getIRTransformations(){

  };

  virtual std::shared_ptr<xacc::AcceleratorBuffer>
  createBuffer(const std::string &varId) {
    auto b = std::make_shared<AcceleratorBuffer>(varId, 1);
    storeBuffer(varId, b);
  }

  virtual std::vector<std::string> getAllocatedBufferNames() {
    std::vector<std::string> names;
    names.push_back("hello");
    return names;
  }

  virtual std::shared_ptr<xacc::AcceleratorBuffer>
  getBuffer(const std::string &varid) {
    return std::make_shared<AcceleratorBuffer>("hello", 1);
  }

  virtual void initialize() {}
  /**
   * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
   *
   * @param buffer The buffer of bits this Accelerator should operate on.
   * @param function The kernel to execute.
   */
  virtual void execute(std::shared_ptr<xacc::AcceleratorBuffer> buffer,
                       const std::shared_ptr<xacc::Function> function) {}

  virtual std::vector<std::shared_ptr<xacc::AcceleratorBuffer>>
  execute(std::shared_ptr<xacc::AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<xacc::Function>> functions) {}

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string and of the given number of bits.
   * The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId The variable name of the created buffer
   * @param size The number of bits in the created buffer
   * @return buffer The buffer instance created.
   */
  virtual std::shared_ptr<xacc::AcceleratorBuffer>
  createBuffer(const std::string &varId, const int size) {
    auto b = std::make_shared<AcceleratorBuffer>(varId, size);
    storeBuffer(varId, b);
  }

  virtual const std::string name() const { return ""; }

  virtual const std::string description() const { return ""; }
};

class FakeEmbedding : public EmbeddingAlgorithm {
public:
  virtual Embedding embed(std::shared_ptr<xacc::Graph> problem,
                          std::shared_ptr<xacc::Graph> hardware,
                          std::map<std::string, std::string> params =
                              std::map<std::string, std::string>()) override {
    Embedding embedding;
    embedding.insert(std::make_pair(0, std::vector<int>{0, 4}));
    embedding.insert(std::make_pair(1, std::vector<int>{1}));
    embedding.insert(std::make_pair(2, std::vector<int>{5}));
    return embedding;
  }

  virtual const std::string name() const { return "fake-embedding"; }

  virtual const std::string description() const { return ""; }
};

class Factoring15FakeEmbedding : public EmbeddingAlgorithm {
public:
  virtual Embedding embed(std::shared_ptr<xacc::Graph> problem,
                          std::shared_ptr<xacc::Graph> hardware,
                          std::map<std::string, std::string> params =
                              std::map<std::string, std::string>()) override {
    Embedding embedding;
    embedding.insert(std::make_pair(0, std::vector<int>{0}));
    embedding.insert(std::make_pair(1, std::vector<int>{1}));
    embedding.insert(std::make_pair(2, std::vector<int>{2}));
    embedding.insert(std::make_pair(4, std::vector<int>{4}));
    embedding.insert(std::make_pair(5, std::vector<int>{5}));
    embedding.insert(std::make_pair(6, std::vector<int>{6}));
    return embedding;
  }

  virtual const std::string name() const { return "fake-factor15-embedding"; }

  virtual const std::string description() const { return ""; }
};

TEST(DWQMICompilerTester, checkFactoring15OneToOneMapping) {

  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string factoring15QMI =
      "__qpu__ factor15(AcceleratorBuffer ab) {\n"
      "0 0 20.0;\n"
      "1 1 50.0;\n"
      "2 2 60.0;\n"
      "4 4 50.0;\n"
      "5 5 60.0;\n"
      "6 6 -160.0;\n"
      "1 4 -1000.0;\n"
      "2 5 -1000.0;\n"
      "0 4 -14.0;\n"
      "0 5 -12.0;\n"
      "0 6 32.0;\n"
      "1 5 68.0;\n"
      "1 6 -128.0;\n"
      "2 6 -128.0;\n"
      "}";

  xacc::setOption("dwave-embedding", "trivial");

  auto acc = std::make_shared<FakeDWAcc>();

  auto ir = compiler->compile(factoring15QMI, acc);

  auto qmi = ir->getKernel("factor15")->toString("");

  std::cout << "NPARAMS: " << ir->getKernel("factor15")->nParameters() << "\n";
  const std::string expected = R"expected(0 0 20;
1 1 50;
2 2 60;
4 4 50;
5 5 60;
6 6 -160;
1 4 -1000;
2 5 -1000;
0 4 -14;
0 5 -12;
0 6 32;
1 5 68;
1 6 -128;
2 6 -128;
)expected";

  std::cout << "QMI:\n" << qmi << "\n";
  EXPECT_TRUE(expected == qmi);
}

TEST(DWQMICompilerTester, checkVariableWeight) {
  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string testsrc =
      R"testsrc(__qpu__ f(AcceleratorBuffer b, double h0) {
        0 0 h0;
    })testsrc";

  xacc::setOption("dwave-embedding", "trivial");

  auto acc = std::make_shared<FakeDWAcc>();

  auto ir = compiler->compile(testsrc, acc);

  auto qmi = ir->getKernel("f")->toString("");
  std::cout << "QMI:\n" << qmi << "\n";

  auto f = ir->getKernel("f");
  EXPECT_TRUE(f->getInstruction(0)->getParameter(0).isVariable());
  EXPECT_TRUE(mpark::get<std::string>(f->getInstruction(0)->getParameter(0)) ==
              "h0");

  std::vector<double> params{2.2};
  auto evaled = ir->getKernel("f")->operator()(params);

  EXPECT_TRUE(evaled->getInstruction(0)->getParameter(0).isNumeric());
  EXPECT_NEAR(mpark::get<double>(evaled->getInstruction(0)->getParameter(0)),
              2.2, 1e-4);

  std::cout << "HELLO:\n" << evaled->toString("") << "\n";

  params[0] = 3.3;
  evaled = ir->getKernel("f")->operator()(params);

  EXPECT_TRUE(evaled->getInstruction(0)->getParameter(0).isNumeric());
  EXPECT_NEAR(mpark::get<double>(evaled->getInstruction(0)->getParameter(0)),
              3.3, 1e-4);

  std::cout << "HELLO:\n" << evaled->toString("") << "\n";
}
TEST(DWQMICompilerTester, checkMultipleVariableWeights) {

  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string testsrc2 =
      R"testsrc(__qpu__ f(AcceleratorBuffer b, double h0, double h1) {
        0 0 h0;
        1 1 h1;
    })testsrc";
  auto acc = std::make_shared<FakeDWAcc>();

  auto ir = compiler->compile(testsrc2, acc);

  auto qmi = ir->getKernel("f")->toString("");
  std::cout << "QMI:\n" << qmi << "\n";

  auto f = ir->getKernel("f");
  EXPECT_TRUE(f->getInstruction(0)->getParameter(0).isVariable());
  EXPECT_TRUE(f->getInstruction(1)->getParameter(0).isVariable());
  EXPECT_TRUE(mpark::get<std::string>(f->getInstruction(0)->getParameter(0)) ==
              "h0");
  EXPECT_TRUE(mpark::get<std::string>(f->getInstruction(1)->getParameter(0)) ==
              "h1");

  std::vector<double> params2{2.2,3.3};
  std::cout << "Evaluating 2 params\n";
  auto evaled = ir->getKernel("f")->operator()(params2);
  std::cout << "done evaluating 2 params\n";

  EXPECT_TRUE(evaled->getInstruction(0)->getParameter(0).isNumeric());
  EXPECT_TRUE(evaled->getInstruction(1)->getParameter(0).isNumeric());
  EXPECT_NEAR(mpark::get<double>(evaled->getInstruction(0)->getParameter(0)),
              2.2, 1e-4);
  EXPECT_NEAR(mpark::get<double>(evaled->getInstruction(1)->getParameter(0)),
              3.3, 1e-4);

  std::cout << "HELLO:\n" << evaled->toString("") << "\n";
}

TEST(DWQMICompilerTester, checkAnneal) {

  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string testsrc =
      R"testsrc(__qpu__ f(AcceleratorBuffer b, double h0, double h1) {
        anneal 10 10 10;
        0 0 h0;
        1 1 h1;
    })testsrc";
  auto acc = std::make_shared<FakeDWAcc>();

  auto ir = compiler->compile(testsrc, acc);
  auto f = ir->getKernel("f");
  auto qmi = ir->getKernel("f")->toString("");
  std::cout << "QMI:\n" << qmi << "\n";
  EXPECT_EQ(mpark::get<double>(f->getInstruction(0)->getParameter(0)), 10.0);
  EXPECT_EQ(mpark::get<double>(f->getInstruction(0)->getParameter(1)), 10.0);
  EXPECT_EQ(mpark::get<double>(f->getInstruction(0)->getParameter(2)), 10.0);
}

TEST(DWQMICompilerTester, checkVariableAnneal) {

  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string testsrc =
      R"testsrc(__qpu__ f(AcceleratorBuffer b, double h0, double h1, double ts, double tp, double tq) {
        anneal ts tp tq;
        0 0 h0;
        1 1 h1;
    })testsrc";
  auto acc = std::make_shared<FakeDWAcc>();

  auto ir = compiler->compile(testsrc, acc);
  auto f = ir->getKernel("f");
  auto qmi = ir->getKernel("f")->toString("");
  std::cout << "QMI:\n" << qmi << "\n";

  std::vector<double> params{1,2,3,4,5};
  auto evaled = f->operator()(params);
  auto expected = R"expected(anneal ta = 3, tp = 4, tq = 5, forward;
0 0 1;
1 1 2;
)expected";
  std::cout << "HELLO WORKED:\n" << evaled->toString("q") << "\n";
  EXPECT_EQ(expected, evaled->toString(""));

  const std::string testsrc2 =
      R"testsrc(__qpu__ f(AcceleratorBuffer b, double h0, double h1, double ts, double tp, double tq) {
        anneal ts tp tq reverse;
        0 0 h0;
        1 1 h1;
    })testsrc";

  ir = compiler->compile(testsrc2, acc);
  f = ir->getKernel("f");
  qmi = f->toString("");
  std::cout << "QMI2:\n" << qmi << "\n";

  evaled = f->operator()(params);
  expected = R"expected(anneal ta = 3, tp = 4, tq = 5, reverse;
0 0 1;
1 1 2;
)expected";
  std::cout << "HELLO WORKED2:\n" << evaled->toString("") << "\n";

  EXPECT_EQ(expected, evaled->toString(""));
}

TEST(DWQMICompilerTester, checkTranslate) {
  auto compiler = xacc::getService<Compiler>("dwave-qmi");

  const std::string src =
      R"src(__qpu__ f(AcceleratorBuffer b, double h0, double h1) {
   0 0 h0;
   1 1 h1;
})src";

  auto acc = std::make_shared<FakeDWAcc>();
  auto ir = compiler->compile(src, acc);
  std::cout << "COMPILED\n";
  auto qmi = ir->getKernel("f");

  auto translated = compiler->translate("", qmi);

  std::cout << "TRANSLATED:\n" << translated << "\n";
  EXPECT_EQ(src, translated);
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
