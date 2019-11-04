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

  xacc::external::load_external_language_plugins();
  xacc::set_verbose(true);

  // Get reference to the Accelerator
  // specified by --accelerator argument
  auto accelerator = xacc::getAccelerator("aer");

  auto optimizer = xacc::getOptimizer("mlpack");
  xacc::qasm(R"(
.compiler xasm
.circuit qubit2_depth1
.parameters x
.qbit q
U(q[0], x[0], -pi/2, pi/2 );
U(q[0], 0, 0, x[1]);
U(q[1], x[2], -pi/2, pi/2);
U(q[1], 0, 0, x[3]);
CNOT(q[0], q[1]);
U(q[0], 0, 0, x[4]);
U(q[0], x[5], -pi/2, pi/2);
U(q[1], 0, 0, x[6]);
U(q[1], x[7], -pi/2, pi/2);
)");
  auto ansatz = xacc::getCompiled("qubit2_depth1");

  std::vector<double> target_distribution {.5, .5, .5, .5};

  auto ddcl = xacc::getAlgorithm("ddcl");
  ddcl->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("target_dist", target_distribution),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("loss", "js"),
                   std::make_pair("gradient", "js-parameter-shift"),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  ddcl->execute(buffer);

  // Print the result
  std::cout << "Loss: " << buffer["opt-val"].as<double>()
            << "\n";

  xacc::external::unload_external_language_plugins();
  xacc::Finalize();

}