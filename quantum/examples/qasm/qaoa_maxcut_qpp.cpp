/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "xacc.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  // Use the Qpp simulator as the accelerator
  auto acc = xacc::getAccelerator("qpp");
  const size_t nbNodes = 8;
  const int nbSteps = 1;
  auto buffer = xacc::qalloc(nbNodes);
  auto optimizer = xacc::getOptimizer("nlopt");
  auto qaoa = xacc::getService<xacc::Algorithm>("QAOA");
  auto graph = xacc::getService<xacc::Graph>("boost-digraph")->gen_random_graph(nbNodes, 3.0 / nbNodes); 
  const bool initOk =
      qaoa->initialize({std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("graph", graph),
                        // number of time steps (p) param
                        std::make_pair("steps", nbSteps),
                        // "Standard" or "Extended"
                        std::make_pair("parameter-scheme", "Standard")});
  qaoa->execute(buffer);
  std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
}