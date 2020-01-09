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

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator = xacc::getAccelerator("tnqvm");

  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("ansatz", {"t"});
  auto x = provider->createInstruction("X", std::vector<std::size_t>{0});
  auto ry = provider->createInstruction("Ry", std::vector<std::size_t>{1}, {"t"});
  auto cx = provider->createInstruction("CNOT", std::vector<std::size_t>{1,0});
  auto h0 = provider->createInstruction("H", std::vector<std::size_t>{0});
  auto h1 = provider->createInstruction("H", std::vector<std::size_t>{1});
  auto m0 = provider->createInstruction("Measure", std::vector<std::size_t>{0});
  auto m1 = provider->createInstruction("Measure", std::vector<std::size_t>{1});
  program->addInstructions({x,ry,cx,h0,h1,m0,m1});

  std::cout << "P:\n" << program->toString() << "\n";
  auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (auto &a : angles) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({a});
    accelerator->execute(buffer, evaled);
    std::cout << "<X0X1>(theta) = " << buffer->getExpectationValueZ() << "\n";
  }

  xacc::Finalize();

  return 0;
}