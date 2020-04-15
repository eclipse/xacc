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
#include "gtest/gtest.h"

#include "xacc.hpp"
#include "xacc_quantum_gate_api.hpp"
#include "xacc_service.hpp"
#include "Utils.hpp"

#include "xacc_internal_compiler.hpp"
#include "qalloc.hpp"

#include "Circuit.hpp"

TEST(XASMCompilerTester, checkTranslate) {
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(R"(__qpu__ void bell_test(qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})");

  auto evaled = (*IR->getComposite("bell_test"))({1.1});
  auto translated = compiler->translate(evaled);
  std::cout << "HELLO:\n" << translated << "\n";
}

TEST(XASMCompilerTester, checkCanParse) {

  auto compiler = xacc::getCompiler("xasm");
  EXPECT_TRUE(compiler->canParse(
      R"(__qpu__ void bell_test_can_parse(qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})"));

  EXPECT_FALSE(compiler->canParse(
      R"(__qpu__ void bell_test_can_parse(qbit q, double t0) {
  H(q[0]);
  CX(q[0], [1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})"));

  EXPECT_FALSE(compiler->canParse(R"(
                      qreg q[2];
                      creg c[2];
                      U(0,0,0) q[0];
                      CX q[0],q[1];
                      rx(3.3) q[0];
                      measure q -> c;
                      )"));
}

TEST(XASMCompilerTester, checkSimple) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR =
      compiler->compile(R"(__qpu__ void bell(qbit q, double t0, double t1) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Ry(q[1], t1);
  U(q[1], pi, t0, t1);
  Measure(q[0]);
  Measure(q[1]);
})");

  auto bell = IR->getComposite("bell");
  EXPECT_EQ(1, IR->getComposites().size());
  EXPECT_EQ(7, bell->nInstructions());
  EXPECT_EQ(2, bell->nVariables());

  std::cout << "KERNEL\n" << bell->toString() << "\n";

  EXPECT_EQ("q", IR->getComposites()[0]->getInstruction(0)->getBufferName(0));

  IR = compiler->compile(R"([&](qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  bell(q);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
}

TEST(XASMCompilerTester, checkMultiRegister) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(
      R"(__qpu__ void bell_multi(qbit q, qbit r, double t0, double t1) {
  H(q[0]);
  CX(q[0], q[1]);
  H(r[0]);
  CX(r[0], r[1]);
  Ry(r[1], t0);
  Rx(q[0], t1);
  Measure(q[0]);
  Measure(q[1]);
  Measure(r[0]);
  Measure(r[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  EXPECT_EQ("q", IR->getComposites()[0]->getInstruction(0)->getBufferName(0));
  EXPECT_EQ("r", IR->getComposites()[0]->getInstruction(2)->getBufferName(0));
}

TEST(XASMCompilerTester, checkVectorArg) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR =
      compiler->compile(R"(__qpu__ void bell22(qbit q, std::vector<double> x) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], x[0]);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
  std::cout << "KERNEL\n"
            << IR->getComposites()[0]->operator()({2.})->toString() << "\n";
}

// TEST(XASMCompilerTester, checkSimpleFor) {

//   auto compiler = xacc::getCompiler("xasm");
//   auto IR =
//       compiler->compile(R"(__qpu__ void testFor(qbit q, std::vector<double> x) {
//   for (int i = 0; i < 5; i++) {
//      H(q[i]);
//   }
//   for (int i = 0; i < 2; i++) {
//       Rz(q[i], x[i]);
//   }
// })");
//   std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
 
//   xacc::internal_compiler::qreg q(5);
//   auto tt = IR->getComposites()[0];
//   tt->updateRuntimeArguments(q, std::vector<double>{1.2, 3.4});
//   std::cout << "EVALED NEW WAY:\n" << tt->toString() << "\n";


//   IR = compiler->compile(
//       R"(__qpu__ void testFor2(qbit q, std::vector<double> x) {
//   for (int i = 0; i < 5; i++) {
//      H(q[i]);
//      Rx(q[i], x[i]);
//      CX(q[0], q[i]);
//   }

//   for (int i = 0; i < 3; i++) {
//       CX(q[i], q[i+1]);
//   }
//   Rz(q[3], 0.22);

//   for (int i = 3; i > 0; i--) {
//       CX(q[i-1],q[i]);
//   }
// })");
//   EXPECT_EQ(1, IR->getComposites().size());
//   std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
//   for (auto ii : IR->getComposites()[0]->getVariables())
//     std::cout << ii << "\n";
//   EXPECT_EQ(22, IR->getComposites()[0]->nInstructions());
// }
// TEST(XASMCompilerTester, checkHWEFor) {

//   auto compiler = xacc::getCompiler("xasm");
//   auto IR = compiler->compile(R"([&](qbit q, std::vector<double> x) {
//     for (int i = 0; i < 2; i++) {
//         Rx(q[i],x[i]);
//         Rz(q[i],x[2+i]);
//     }
//     CX(q[1],q[0]);
//     for (int i = 0; i < 2; i++) {
//         Rx(q[i], x[i+4]);
//         Rz(q[i], x[i+4+2]);
//         Rx(q[i], x[i+4+4]);
//     }
//   })");
//   EXPECT_EQ(1, IR->getComposites().size());
//   std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
//   for (auto ii : IR->getComposites()[0]->getVariables())
//     std::cout << ii << "\n";
//   EXPECT_EQ(11, IR->getComposites()[0]->nInstructions());
// }

TEST(XASMCompilerTester, checkIfStmt) {

  auto q = xacc::qalloc(2);
  q->setName("q");
  xacc::storeBuffer(q);

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(R"([&](qbit q) {
    H(q[0]);
    Measure(q[0]);
    if (q[0]) {
        CX(q[0],q[1]);
    }
  })");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  q->measure(0, 0);

  IR->getComposites()[0]->expand({});
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  q->measure(0, 1);

  IR->getComposites()[0]->expand({});
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  q->reset_single_measurements();
  IR->getComposites()[0]->getInstruction(2)->disable();
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  q->measure(0, 1);

  IR->getComposites()[0]->expand({});
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
}

TEST(XASMCompilerTester, checkApplyAll) {
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(R"([&](qbit q) {
  range(q, {{"gate","H"},{"nq",4}});
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  IR = compiler->compile(R"([&](qbit q, double x) {
  ucc1(q, x);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  IR = compiler->compile(R"([&](qbit q, double x, double y, double z) {
  ucc3(q, x, y, z);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  IR = compiler->compile(R"([&](qbit q, std::vector<double> x) {
  ucc3(q, x[0], x[1], x[2]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

  IR = compiler->compile(R"([&](qbit q, double x) {
  exp_i_theta(q, x, {{"pauli", "X0 Y1 - X1 Y0"}});
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
}

TEST(XASMCompilerTester, checkGateOnAll) {

  auto q = xacc::qalloc(4);
  q->setName("qqq");
  xacc::storeBuffer(q);

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(R"(__qpu__ void on_all(qbit qqq) {
  H(qqq);
  Measure(qqq);
})");

  auto f = IR->getComposite("on_all");

  std::cout << "F:\n" << f->toString() << "\n";
}

TEST(XASMCompilerTester, checkBugBug) {
  auto a = xacc::qalloc(4);
  a->setName("a");
  xacc::storeBuffer(a);

  auto b = xacc::qalloc(4);
  b->setName("b");
  xacc::storeBuffer(b);

  auto c = xacc::qalloc(4);
  c->setName("c");
  xacc::storeBuffer(c);

  auto anc = xacc::qalloc(4);
  anc->setName("anc");
  xacc::storeBuffer(anc);
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(
      R"(__qpu__ void bugbug(qbit a, qbit b, qbit c, qbit anc) {
X(a[0]);
X(a[1]);
X(b[0]);
X(b[2]);
CX(a[0], c[0]);
CX(b[0], c[0]);
H(anc[0]);
CX(b[0], anc[0]);
Tdg(anc[0]);
})");

  std::cout << IR->getComposites()[0]->toString() << "\n";
  std::cout << IR->getComposites()[0]->getInstruction(4)->toString() << "\n";
  for (auto &b : IR->getComposites()[0]->getInstruction(4)->getBufferNames()) {
    std::cout << b << "\n";
  }
}

TEST(XASMCompilerTester, checkCallingPreviousKernel) {
  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(R"(__qpu__ void bell_call(qbit q) {
  H(q[0]);
  CX(q[0], q[1]);
  Measure(q[0]);
  Measure(q[1]);
})");

  auto IR2 = compiler->compile(R"(__qpu__ void call_bell(qbit q) {
  bell_call(q);
})");

  auto bell = IR2->getComposite("call_bell");

  std::cout << bell->toString() << "\n";
}

TEST(XASMCompilerTester, checkIRV3) {
//   auto v = xacc::qalloc(1);
//   v->setName("v");
//   xacc::storeBuffer(v);

//   auto v = xacc::internal_compiler::qalloc(1);
  xacc::internal_compiler::qreg v(1);

  auto H = xacc::quantum::getObservable("pauli", std::string("X0 Y1 + Y0 X1"));

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(
      R"(
   __qpu__ void foo_test (qbit v, double x, double y, double z, std::shared_ptr<Observable> H) {
     Rx(v[0], x);
     U(v[0], x, y, z);
     exp_i_theta(v, x, H);
   }
   )");

  auto foo_test = IR->getComposite("foo_test");

  std::cout << foo_test->toString() << "\n";

  for (auto &val : {2.2, 2.3, 2.4, 2.5}) {
    foo_test->updateRuntimeArguments(v, val, 3.3, 4.4, H);

    std::cout << foo_test->toString() << "\n\n";
  }
}


TEST(XASMCompilerTester, checkIRV3Vector) {
//   auto v = xacc::qalloc(1);
//   v->setName("v");
//   xacc::storeBuffer(v);

//   auto v = xacc::internal_compiler::qalloc(1);
  xacc::internal_compiler::qreg v(1);

  auto H = xacc::quantum::getObservable("pauli", std::string("X0 Y1 + Y0 X1"));

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler->compile(
      R"(
   __qpu__ void foo_test2 (qbit v, std::vector<double> x, std::shared_ptr<Observable> H) {
     Rx(v[0], x[0]);
     U(v[0], x[0], x[1], x[2]);
     exp_i_theta(v, x[1], H);
   }
   )");

  auto foo_test = IR->getComposite("foo_test2");

  std::cout << foo_test->toString() << "\n";

  for (auto &val : {2.2, 2.3, 2.4, 2.5}) {
    foo_test->updateRuntimeArguments(v, std::vector<double>{val, 3.3, 4.4}, H);

    std::cout << foo_test->toString() << "\n\n";
  }

IR = compiler->compile(
      R"(
  __qpu__ void ansatz2(qreg q, std::vector<double> theta) {
  X(q[0]);
  Ry(q[1], theta[0]);
  CX(q[1],q[0]);
}
)");
 
 auto test = IR->getComposites()[0];
 std::cout <<" HELLO: " << test->toString() << "\n";
 test->updateRuntimeArguments(v, std::vector<double>{.48});
  std::cout <<" HELLO: " << test->toString() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
