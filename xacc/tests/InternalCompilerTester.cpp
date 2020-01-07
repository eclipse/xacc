/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "xacc_internal_compiler.hpp"
#include <qalloc>

using namespace xacc::internal_compiler;

TEST(InternalCompilerTester, checkMultipleBuffers) {

  auto q = qalloc(2);
  q.setName("q");

  auto r = qalloc(2);
  r.setName("r");

  auto circuit = compile("xasm", R"(__qpu__ void bell_x_2(qreg q, qreg r) {
  // First bell state on qbits 0,1
  H(q[0]);
  CX(q[0],q[1]);

  // Second bell state on qbis 2,3
  H(r[0]);
  CX(r[0],r[1]);

  // Measure them all
  Measure(q[0]);
  Measure(q[1]);

  Measure(r[0]);
  Measure(r[1]);
  // should see 0000, 0011, 1100, 1111
})");

  xacc::AcceleratorBuffer *bufs[2] = {q.results(), r.results()};

  //   std::vector<xacc::AcceleratorBuffer*> bufs{q.results(),r.results()};
  execute(bufs, 2, circuit);
  auto counts = q.counts();
  for (const auto &kv : counts) {
    printf("%s: %i\n", kv.first.c_str(), kv.second);
  }
  counts = r.counts();
  for (const auto &kv : counts) {
    printf("%s: %i\n", kv.first.c_str(), kv.second);
  }
}

TEST(InternalCompilerTester, checkStaqAdd) {

  auto a = qalloc(4);
  a.setName("a");
  a.store();

  auto b = qalloc(4);
  b.setName("b");
  b.store();

  auto c = qalloc(4);
  c.setName("c");
  c.store();

   auto anc = qalloc(3);
  anc.setName("anc");
  anc.store();

  auto src = R"(__qpu__ void add(qreg a, qreg b, qreg c) {
OPENQASM 2.0;
include "qelib1.inc";

oracle adder a0,a1,a2,a3,b0,b1,b2,b3,c0,c1,c2,c3 { "adder_4.v" }

creg result[4];

// a = 3
x a[0];
x a[1];

// b = 5
x b[0];
x b[2];

adder a[0],a[1],a[2],a[3],b[0],b[1],b[2],b[3],c[0],c[1],c[2],c[3];

// measure
measure c -> result;
})";
  auto circuit = compile("staq", src);
  std::cout << circuit->toString() << "\n";
  xacc::AcceleratorBuffer *bufs[4] = {a.results(), b.results(), c.results(), anc.results()};

  //   std::vector<xacc::AcceleratorBuffer*> bufs{q.results(),r.results()};
  execute(bufs, 4, circuit);
  auto counts = a.counts();
  for (const auto &kv : counts) {
    printf("%s: %i\n", kv.first.c_str(), kv.second);
  }
  counts = b.counts();
  for (const auto &kv : counts) {
    printf("%s: %i\n", kv.first.c_str(), kv.second);
  }

  counts = c.counts();
  for (const auto &kv : counts) {
    printf("%s: %i\n", kv.first.c_str(), kv.second);
  }
}

int main(int argc, char **argv) {
  compiler_InitializeXACC("local-ibm");
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  //   xacc::Finalize();
  return ret;
}
