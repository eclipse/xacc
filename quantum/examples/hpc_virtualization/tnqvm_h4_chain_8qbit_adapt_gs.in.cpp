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
#include "PauliOperator.hpp"

// run this with
// <MPI variables> srun -n X -N Y ./path/to/chain_hpc_virt --n-virtual-qpus <n_qpus> --n-hydrogens <n_h> --n-terms <n_terms> --accelerator <acc>

int main(int argc, char **argv) {
  // Initialize 
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  xacc::logToFile(true);
  xacc::setLoggingLevel(2);

  // Process the input arguments
  std::vector<std::string> arguments(argv + 1, argv + argc);
  int n_virt_qpus, n_hydrogen, n_terms = 0, n_cycles = 2;
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
    if (arguments[i] == "--n-terms") {
      n_terms = std::stoi(arguments[i + 1]);
    }
    if (arguments[i] == "--n-cycles") {
      n_cycles = std::stoi(arguments[i + 1]);
    }
  }

  // Read in hamiltonian
  std::ifstream hfile("@CMAKE_SOURCE_DIR@/quantum/examples/hpc_virtualization/h" + std::to_string(n_hydrogen) + ".txt");
  std::stringstream sss;
  sss << hfile.rdbuf();
  auto H = xacc::quantum::getObservable("pauli", sss.str());
  
  if (n_terms > H->getSubTerms().size()) {
    xacc::error("--n-terms needs to be less than " + std::to_string(H->getSubTerms().size()));
  } else if (n_terms != 0) {
    auto terms = H->getSubTerms();
    xacc::quantum::PauliOperator reducedH;
    for (int i = 0; i < n_terms; i++) {
      reducedH += *std::dynamic_pointer_cast<xacc::quantum::PauliOperator>(terms[i]);
    }
    H->fromString(reducedH.toString());
    //H = xacc::quantum::getObservable("pauli", reducedH.toString());
  }

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

  for(int i = 0; i < n_cycles; i++){
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
