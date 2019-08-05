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
#include <gtest/gtest.h>
#include "XACC.hpp"
#include "IRGenerator.hpp"
#include "xacc_service.hpp"

using namespace xacc;

class FakePyAcc : public xacc::Accelerator {
public:
  virtual xacc::Accelerator::AcceleratorType getType() {
    return xacc::Accelerator::AcceleratorType::qpu_gate;
  }

  virtual bool isValidBufferSize(const int nBits) { return true; }

  virtual std::vector<std::shared_ptr<xacc::IRTransformation>>
  getIRTransformations() {
    return {};
  }

  virtual std::shared_ptr<xacc::AcceleratorBuffer>
  createBuffer(const std::string &varId) {
    auto b = std::make_shared<xacc::AcceleratorBuffer>(varId, 1);
    return b;
  }
  virtual std::vector<std::string> getAllocatedBufferNames() {
    std::vector<std::string> names;
    names.push_back("hello");
    return names;
  }

  virtual void initialize(AcceleratorParameters params) {}
  virtual void execute(std::shared_ptr<xacc::AcceleratorBuffer> buffer,
                       const std::shared_ptr<xacc::Function> function) {}
  virtual std::vector<std::shared_ptr<xacc::AcceleratorBuffer>>
  execute(std::shared_ptr<xacc::AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<xacc::Function>> functions) {return {};}

  virtual std::shared_ptr<xacc::AcceleratorBuffer>
  createBuffer(const std::string &varId, const int size) {
    auto b = std::make_shared<xacc::AcceleratorBuffer>(varId, size);
    storeBuffer(varId, b);
    return b;
  }

  virtual const std::string name() const { return ""; }
  virtual const std::string description() const { return ""; }
};

TEST(PyXACCCompilerTester, checkSimple) {

  auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
  const std::string src = R"src(def f(buffer, theta):
       X(0)
       Rx(theta,0)
       CNOT(1,0)
       Measure(0,0)
       )src";

  auto acc = std::make_shared<FakePyAcc>();

  auto ir = compiler->compile(src, acc);
  auto f = ir->getKernel("f");
  EXPECT_EQ("f", f->name());
  EXPECT_EQ(f->nParameters(), 1);
  EXPECT_EQ(f->nInstructions(), 4);

  std::cout << "KERNEL:\n" << ir->getKernel("f")->toString("") << "\n";
}

TEST(PyXACCCompilerTester, checkIRGen) {

  if (xacc::hasService<IRGenerator>("uccsd")) {
    const std::string uccsdSrc = R"uccsdSrc(def foo(buffer, *args):
   uccsd(n_qubits=4, n_electrons=2)
   )uccsdSrc";

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");

    auto acc = std::make_shared<FakePyAcc>();

    auto ir = compiler->compile(R"(def foo(buffer, *args):
   uccsd(n_qubits=4, n_electrons=2)
   )", acc);
   
    auto f = ir->getKernel("foo");
    EXPECT_EQ("foo", f->name());
    EXPECT_EQ(f->nParameters(), 2);
    EXPECT_EQ(f->nInstructions(), 158);

  }
}
TEST(PyXACCCompilerTester, checkDotDotDot) {

  auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
  const std::string src = R"src(def f(q):
       H(0,...,3)
       CNOT(0,1)
       )src";

  auto acc = std::make_shared<FakePyAcc>();
  auto buffer = acc->createBuffer("q", 4);

  auto ir = compiler->compile(src, acc);
  auto f = ir->getKernel("f");

  EXPECT_EQ("f", f->name());
  EXPECT_EQ(f->nParameters(), 0);
  EXPECT_EQ(f->nInstructions(), 5);

  std::cout << "KERNEL:\n" << ir->getKernel("f")->toString("") << "\n";

  const std::string src2 = R"src(def f(q):
       H(1,...,3)
       )src";

  ir = compiler->compile(src2, acc);
  f = ir->getKernel("f");
  std::cout << "KERNEL2:\n" << ir->getKernel("f")->toString("") << "\n";
  EXPECT_EQ("f", f->name());
  EXPECT_EQ(f->nParameters(), 0);
  EXPECT_EQ(f->nInstructions(), 3);
}

TEST(PyXACCCompilerTester, checkDW) {

  if (xacc::hasCompiler("dwave-qmi")) {
    xacc::setOption("dwave-api-key","fake");
    xacc::setOption("dwave-skip-initialization","");

    const std::string dwsrc = R"dwsrc(def f(buffer):
   qmi(0,0, 1.0)
   qmi(1,1, 2.0)
   qmi(0,1, 3.0)
   return
)dwsrc";

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
    auto acc = std::make_shared<FakePyAcc>();

    // auto ir = compiler->compile(dwsrc, acc);
    // auto f = ir->getKernel("f");
    // EXPECT_EQ("f", f->name());
    // EXPECT_EQ(f->nParameters(), 0);
    // EXPECT_EQ(f->nInstructions(), 3);

    // std::cout << "KERNEL:\n" << ir->getKernel("f")->toString("") << "\n";
  }
  // CHECK THAT WE THROW AN ERROR WITH MIXED CODE
  // CHECK WITH ANNEAL
  // CHECK WITH GENERATOR
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  //    xacc::Finalize();
  return ret;
}
