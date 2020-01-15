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

// This example demonstrates XACC implementation of the Rotoselect quantum circuit structure learning.
// Ref: https://arxiv.org/abs/1905.09692
// The algorithm will *train* the circuit to prepare a quantum state that minimizes an objective function (aka Hamiltonian observable).
// Inputs to the Rotoselect algorithm are:
//  - accelerator: reference to a quantum processor accelerator (e.g. a simulator) which will evaluate circuit to get the energy.
//  - observable: the objective function to minimize in the form of a XACC Observable object
//  - layers: depth of the training circuit; each layer contains a rotation gate (Rx, Ry, Rz) on each qubit and CZ gates between neighboring qubits.
//  - iterations: the number of iterations that we want to train the circuit. Each rotation gate update is considered as an iteration.
// Note: the number of qubits is inferred automatically from the input acceleration buffer.
// The optimal energy achieved after the training is embedded in the input acceleration buffer as "opt-val".
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  // Use the Qpp simulator as the accelerator
  auto acc = xacc::getAccelerator("qpp");
  
  // In this example, we try to find the ground state energy of the 5-qubit Heisenberg model on a 
  // 1D lattice with periodic boundary conditions.
  auto buffer = xacc::qalloc(5);
  // The corresponding Hamiltonian is:
  // Note: we set all scalar params to 1.0 for simplicity.
  auto observable = xacc::quantum::getObservable(
      "pauli",
      std::string("X0 X1 + X1 X2 + X2 X3 + X3 X4 + X4 X0 + "
                  "Y0 Y1 + Y1 Y2 + Y2 Y3 + Y3 Y4 + Y4 Y0 +"
                  "Z0 Z1 + Z1 Z2 + Z2 Z3 + Z3 Z4 + Z4 Z0 + "
                  "Z0 + Z1 + Z2 + Z3 + Z4"));

  auto rotoselect = xacc::getService<xacc::Algorithm>("rotoselect");
  // We train the circuit that has 6 layers for 1000 iterations.
  // Note: we can repeat this training multiple times to collect the mean and standard deviation
  // as reported in https://arxiv.org/abs/1905.09692
  rotoselect->initialize({
                          std::make_pair("accelerator", acc),
                          std::make_pair("observable", observable),
                          std::make_pair("layers", 6),
                          std::make_pair("iterations", 1000),
                        });
  
  rotoselect->execute(buffer);
  // Expected value ~ -8.4 -> -8.5
  // Ref: FIG. 2 of https://arxiv.org/abs/1905.09692
  std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
}