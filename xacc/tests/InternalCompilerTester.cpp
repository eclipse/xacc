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

  xacc::AcceleratorBuffer * bufs[2] = {q.results(), r.results()};

//   std::vector<xacc::AcceleratorBuffer*> bufs{q.results(),r.results()};
  execute(bufs, 2, circuit);
auto counts = q.counts();
    for (const auto & kv: counts) {
        printf("%s: %i\n", kv.first.c_str(), kv.second);
    }
    counts = r.counts();
    for (const auto & kv: counts) {
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
