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
#include "xacc_observable.hpp"

// run this with
// $ mpirun -n N tnqvm_hpc_virtualization
// where N is the number of MPI ranks

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  auto accelerator =
      xacc::getAccelerator("tnqvm", {{"tnqvm-visitor", "exatn"}});

  // Decorate the accelerator with HPC Virtualization.
  // This decorator assumes the provided number of input QPUs
  // available to a distributed hetereogeneous system. It will
  // divide the work of executing N CompositeInstructions across the
  // available virtual QPUs (here tnqvm instances)
  accelerator = xacc::getAcceleratorDecorator("hpc-virtualization", accelerator,
                                              {{"n-virtual-qpus", 3}});

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto optimizer = xacc::getOptimizer("nlopt");

  // JIT map XASM Ansatz to IR
  xacc::qasm(R"(
.compiler xasm
.circuit deuteron_ansatz
.parameters theta
.qbit q
X(q[0]);
Ry(q[1], theta);
CNOT(q[1],q[0]);
)");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe", {{"ansatz", ansatz},
                                        {"observable", H_N_2},
                                        {"accelerator", accelerator},
                                        {"optimizer", optimizer}});
  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  xacc::ScopeTimer timer("mpi_timing", false);
  vqe->execute(buffer);
  auto run_time = timer.getDurationMs();

  // Print the result
  auto print_predicate = buffer->hasExtraInfoKey("rank")
                             ? ((*buffer)["rank"].as<int>() == 0)
                             : true;
  if (print_predicate) {
    std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
    std::cout << "Runtime: " << run_time << " ms.\n";
  }

  xacc::Finalize();
}