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
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"

namespace {
  const std::string testXGateReductionSrc = 
    R"(__qpu__ void test(qbit q) {
      X(q[1]);
      CX(q[0], q[1]);
      H(q[0]);
      I(q[1]);
      X(q[1]);
      X(q[0]);
      H(q[0]);
      X(q[0]);
      X(q[1]);
      X(q[0]);
      Measure(q[1]);
    })";

  const std::string testHadamardGateReductionSrc = 
    R"(__qpu__ void test(qbit q) {
      X(q[0]);
      H(q[0]);
      Rz(q[0], 1.57079632679489661923);
      H(q[0]);
      Z(q[0]);
      H(q[0]);
      Z(q[0]);
      X(q[1]);
      H(q[2]);
      CX(q[1], q[2]);
      H(q[0]);
      Rz(q[0], 1.57079632679489661923);
      CX(q[1], q[0]);
      CX(q[2], q[0]);
      Rz(q[0], -1.57079632679489661923);
      H(q[0]);
      X(q[0]);
      Z(q[2]);
      H(q[0]);
      X(q[1]);
      CX(q[1], q[2]);
      H(q[2]);
      CX(q[0], q[2]);
      H(q[1]);
      H(q[0]);
      H(q[2]);
      CX(q[0], q[2]);
      CX(q[0], q[2]);
    })";
} // end anon namespace

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
 
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto optimizer = xacc::getService<xacc::IRTransformation>("circuit-optimizer");

  auto ir = xasmCompiler->compile(testHadamardGateReductionSrc);

  auto circuitComposite = ir->getComposites()[0];

  std::cout << "Number of instructions before optimization is: " << circuitComposite->nInstructions() << "\n";
  // Run the IRTransformation optimizer
  optimizer->apply(circuitComposite, nullptr);
  std::cout << "Number of instructions after optimization is: " << circuitComposite->nInstructions() << "\n";

  xacc::Finalize();

  return 0;
}