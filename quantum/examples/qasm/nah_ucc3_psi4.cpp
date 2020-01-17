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
#include "xacc_observable.hpp"
#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Load Python plugins (psi4 is one of them)
  xacc::external::load_external_language_plugins();
  auto accelerator = xacc::getAccelerator("tnqvm");

  // Create the 4 qubit NaH hamiltonian
  std::string geom = R"(0 1
Na  0.000000   0.0      0.0
H   0.0        0.0  1.914388
symmetry c1)";
  std::vector<int> frozen_orbs {0, 1, 2, 3, 4, 10, 11, 12, 13, 14};
  std::vector<int> active_orbs {5,9,15,19};
  auto NaH_4q = xacc::quantum::getObservable(
      "psi4", {std::make_pair("geometry", geom),
              std::make_pair("basis", "sto-3g"),
              std::make_pair("frozen-spin-orbitals", frozen_orbs),
              std::make_pair("active-spin-orbitals", active_orbs)});

  auto optimizer = xacc::getOptimizer("nlopt");

  // JIT map XASM Ansatz to IR
  xacc::qasm(R"(
.compiler xasm
.circuit ucc3_ansatz
.parameters t0, t1, t2
.qbit q
X(q[0]);
X(q[2]);
ucc3(q, t0, t1, t2);
)");
  auto ansatz = xacc::getCompiled("ucc3_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", NaH_4q),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(4);
  vqe->execute(buffer);

  // Print the result
  std::cout << "Energy: " << (*buffer)["opt-val"].as<double>()
            << "\n";

  xacc::external::unload_external_language_plugins();
  xacc::Finalize();
  return 0;
}


//   if (!xacc::hasService<xacc::Observable>("psi4")) {
//     return 0;
//   }
