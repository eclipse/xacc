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
#include "xacc_service.hpp"
#include "Circuit.hpp"

// run this with
// $ mpirun -n N h4_chain_8qbit_hpc_virt --n-virtual-qpus M --n-hydrogens H
// where N is the number of MPI ranks, M is number of qpus,
// and H is number of hydrogens in the chain

int main(int argc, char **argv) {
  // Initialize 
  xacc::set_verbose(true);
  xacc::logToFile(true);
  xacc::setLoggingLevel(2);
  xacc::Initialize(argc, argv);

  // Process the input arguments
  std::vector<std::string> arguments(argv + 1, argv + argc);
  int n_virt_qpus, n_hydrogen;
  std::string acc;
  for (int i = 0; i < arguments.size(); i++) {
    if (arguments[i] == "--n-virtual-qpus") {
      n_virt_qpus = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--accelerator") {
      acc = arguments[i + 1];
    }
    if (arguments[i] == "--n-hydrogens") {
      n_hydrogen = std::stoi(arguments[i + 1]);
    }
  }

  // Read in hamiltonian
  std::ifstream hfile("@CMAKE_SOURCE_DIR@/quantum/examples/hpc_virtualization/h" + std::to_string(n_hydrogen) + ".txt");
  std::stringstream sss;
  sss << hfile.rdbuf();
  auto H = xacc::quantum::getObservable("pauli", sss.str());
  
  // Get reference to the Accelerator
  std::shared_ptr<xacc::Accelerator> accelerator;
  if (acc == "tnqvm") {
    accelerator = xacc::getAccelerator("tnqvm", {{"tnqvm-visitor", "exatn"}});
  } else if (acc == "qpp") {
    accelerator = xacc::getAccelerator("qpp");
  } else if (acc == "aer") {
    accelerator = xacc::getAccelerator("aer");
  }

  // Decorate the accelerator with HPC Virtualization.
  // This decorator assumes the provided number of input QPUs
  // available to a distributed hetereogeneous system. It will
  // divide the work of executing N CompositeInstructions across the
  // available virtual QPUs (here tnqvm instances)
  accelerator = xacc::getAcceleratorDecorator(
      "hpc-virtualization", accelerator, {{"n-virtual-qpus", n_virt_qpus}});

  auto ansatz = std::dynamic_pointer_cast<xacc::CompositeInstruction>(xacc::getService<xacc::Instruction>("uccsd"));
  ansatz->expand({{"ne", n_hydrogen}, {"nq", 2 * n_hydrogen}});
  std::vector<double> zeros(ansatz->nVariables(), 0.0);
  auto evaled = ansatz->operator()(zeros);

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm(
      "vqe",
      {{"ansatz", evaled}, {"observable", H}, {"accelerator", accelerator}});

  double energy = 0.0;

  for(int i = 0; i < 2; i++){
    auto q = xacc::qalloc(2 * n_hydrogen);
    xacc::ScopeTimer timer("mpi_timing", false);
    energy = vqe->execute(q, {})[0];
    auto run_time = timer.getDurationMs();

    // Print the result
    if (q->hasExtraInfoKey("rank") ? ((*q)["rank"].as<int>() == 0) : true) {
      std::cout << "Energy: " << energy << " Hartree\n";
      std::cout << "Runtime: " << run_time << " ms.\n";
    }
  }

  // Finalize
  xacc::Finalize();
}