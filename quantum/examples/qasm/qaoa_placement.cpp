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
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "PauliOperator.hpp"

#include <iomanip>

using namespace xacc::quantum;

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Connectivity looks like
  // o --- o --- o
  //       |
  //       o
  //       |
  //       o
  //
  // QAOA is complete triangle
  //    o --- o
  //     \   /
  //      \ /
  //       o

  // Get the accelerator, provide the custom connectivity
  std::vector<std::pair<int, int>> connectivity{{0, 1}, {1, 0}, {1, 2}, {1, 3},
                                                {2, 1}, {3, 1}, {3, 4}, {4, 3}};
  auto qpu = xacc::getAccelerator("qpp", {{"connectivity", connectivity}});

  // We will run 1 QAOA step
  const int n_steps = 1;

  // Create the Hamiltonian 0.5*(1-ZiZj) for i,j in edges
  PauliOperator H;
  for (auto [vi, vj] :
       std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {2, 0}}) {
    H += 0.5 * (PauliOperator(1.) - PauliOperator({{vi, "Z"}, {vj, "Z"}}));
  }
  xacc::Observable *obs = &H;

  // Create the QAOA ansatz from Cost Hamiltonian H, with 1 step
  auto qaoa_ansatz =
      xacc::createComposite("qaoa", {{"nbQubits", H.nBits()},
                                     {"nbSteps", n_steps},
                                     {"cost-ham", obs},
                                     {"parameter-scheme", "Standard"}});

  // Print the qasm and the depth
  std::cout << "QAOA Circuit:\n" << qaoa_ansatz->toString() << "\n";
  std::cout << "Depth: " << qaoa_ansatz->depth() << "\n\n";

  // Apply the swap-shortest-path placement strategy
  auto placement = xacc::getIRTransformation("swap-shortest-path");
  placement->apply(qaoa_ansatz, qpu);

  // Print the qasm and depth after placement.
  std::cout << "After:\n" << qaoa_ansatz->toString() << "\n";
  std::cout << "Depth: " << qaoa_ansatz->depth() << "\n";

  xacc::Finalize();
}
