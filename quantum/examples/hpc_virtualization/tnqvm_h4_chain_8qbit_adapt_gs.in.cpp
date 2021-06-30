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
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_observable.hpp"
#include <fstream>

// run this with
// $ mpirun -n N h4_chain_8qbit_hpc_virt --n-virtual-qpus M --n-hydrogens H
// where N is the number of MPI ranks, M is number of qpus,
// and H is number of hydrogens in the chain

int main(int argc, char **argv) {
  // Initialize and load the python plugins
  xacc::Initialize(argc, argv);
  xacc::external::load_external_language_plugins();
  std::string observable_generator = "pyscf";

  // Process the input arguments
  std::vector<std::string> arguments(argv + 1, argv + argc);
  auto n_virt_qpus = 2;
  auto n_hydrogens = 4;
  for (int i = 0; i < arguments.size(); i++) {
    if (arguments[i] == "--n-virtual-qpus") {
      n_virt_qpus = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--n-hydrogens") {
      n_hydrogens = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--observable-generator") {
      observable_generator = arguments[i + 1];
    }
  }

  // Create the Hydrogen chain geometry
  std::stringstream geom_ss;
  if (observable_generator == "psi4")
    geom_ss << "0 1\n";
  for (int i = 0; i < n_hydrogens; i++) {
    geom_ss << "H 0.0 0.0 " << (double)i << "\n";
  }
  if (observable_generator == "psi4")
    geom_ss << "symmetry c1\n";

  // Create the Observable
  auto H4 = xacc::quantum::getObservable(
      observable_generator, {{"basis", "sto-3g"}, {"geometry", geom_ss.str()}});

  // Get reference to the Accelerator
  auto accelerator =
      xacc::getAccelerator("tnqvm", {{"tnqvm-visitor", "exatn"}});

  // Decorate the accelerator with HPC Virtualization.
  // This decorator assumes the provided number of input QPUs
  // available to a distributed hetereogeneous system. It will
  // divide the work of executing N CompositeInstructions across the
  // available virtual QPUs (here tnqvm instances)
  accelerator = xacc::getAcceleratorDecorator(
      "hpc-virtualization", accelerator, {{"n-virtual-qpus", n_virt_qpus}});

  // Read the ground state circuit source code
  std::ifstream inFile;
  inFile.open("@CMAKE_SOURCE_DIR@/quantum/examples/hpc_virtualization/h4.qasm");
  std::stringstream strStream;
  strStream << inFile.rdbuf();
  const std::string kernelName = "h4_adapt_gs";
  std::string h4SrcStr = strStream.str();
  std::stringstream ss;
  ss << ".compiler staq\n.circuit h4_ansatz\n.qbit q\n" << h4SrcStr;

  // Create some qubits and compile the quantum code
  auto q = xacc::qalloc(8);
  q->setName("q");
  xacc::storeBuffer(q);
  xacc::qasm(ss.str());
  auto ansatz = xacc::getCompiled("h4_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm(
      "vqe",
      {{"ansatz", ansatz}, {"observable", H4}, {"accelerator", accelerator}});

  // Allocate some qubits and execute
  double energy = 0.0;

  xacc::ScopeTimer timer("mpi_timing", false);
  energy = vqe->execute(q, {})[0];
  auto run_time = timer.getDurationMs();

  // Print the result
  if (q->hasExtraInfoKey("rank") ? ((*q)["rank"].as<int>() == 0) : true) {
    std::cout << "Energy: " << energy << " Hartree\n";
    std::cout << "Runtime: " << run_time << " ms.\n";
  }

  // Finalize and unload the python plugins
  xacc::external::unload_external_language_plugins();
  xacc::Finalize();
}