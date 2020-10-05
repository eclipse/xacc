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

// Running pulse-level simulation.
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  // Using Aer simulator with the ibmq_bogota backend.
  auto accelerator =
      xacc::getAccelerator("aer:ibmq_bogota", {{"sim-type", "pulse"}});
  auto buffer = xacc::qalloc(5);
  auto provider = xacc::getService<xacc::IRProvider>("quantum");
  // Helper to compute a Gaussian pulse.
  // In this example, we just want to test a random Gaussian pulse.
  const auto gaussianCalc = [](double in_time, double in_amp, double in_center, double in_sigma){
    return in_amp*std::exp(-std::pow(in_time - in_center, 2) / 2.0 / std::pow(in_sigma, 2.0));
  };

  const size_t nbSamples = 160;
  std::vector<double> samples;
  for (size_t i = 0; i < nbSamples; ++i)
  {
    samples.emplace_back(gaussianCalc(i, 0.1, nbSamples/2, nbSamples/4));
  }
  // Create pulse instructions to send to all D channels. 
  auto pulseInst0 = provider->createInstruction(
      "gaussian", {0}, {}, {{"channel", "d0"}, {"samples", samples}});
  auto pulseInst1 = provider->createInstruction(
      "gaussian", {1}, {}, {{"channel", "d1"}});
  auto pulseInst2 = provider->createInstruction(
      "gaussian", {2}, {}, {{"channel", "d2"}});
  auto pulseInst3 = provider->createInstruction(
      "gaussian", {3}, {}, {{"channel", "d3"}});
  auto pulseInst4 = provider->createInstruction(
      "gaussian", {4}, {}, {{"channel", "d4"}});
  auto f = provider->createComposite("tmp");
  // Add the raw pulses 
  f->addInstructions({pulseInst0, pulseInst1, pulseInst2, pulseInst3, pulseInst4});
  
  auto m0 = provider->createInstruction("Measure", {0});
  auto m1 = provider->createInstruction("Measure", {1});
  auto m2 = provider->createInstruction("Measure", {2});
  auto m3 = provider->createInstruction("Measure", {3});
  auto m4 = provider->createInstruction("Measure", {4});
  // Measure alls
  f->addInstructions({m0, m1, m2, m3, m4});
  
  // Simulate:
  accelerator->execute(buffer, f);
  std::cout << "First experiment:\n";
  buffer->print();

  // Run gate-pulse simulation.
  // Also, this will test Python interpreter lifetime.
  auto x0 = provider->createInstruction("X", {0});
  auto f2 = provider->createComposite("tmp2");
  f2->addInstructions({x0, m0, m1, m2, m3, m4});
  auto buffer2 = xacc::qalloc(5);
  accelerator->execute(buffer2, f2);
  std::cout << "Second experiment:\n";
  buffer2->print();
  xacc::Finalize();
}