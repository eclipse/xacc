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

  // Uncomment these lines to generate hamiltonian from psi4/pyscf
  /*
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
  */

  std::string hamiltonian = R"((0.111499,0) Z0 Z6 + (0.0696375,0) Z0 Z1 + (0.089825,0) Z5 Z7 + (-0.0376584,0) X0 X1 X6 X7 + (0.130738,0) Z3 Z4 + (0.0776204,0) Z1 Z2 + (0.0845405,0) Z0 Z2 + (-0.0102909,0) Z4 X5 Z6 X7 + (-0.0102909,0) Z4 Y5 Z6 Y7 + (0.130738,0) Z0 Z7 + (0.111941,0) Z1 Z6 + (0.113407,0) Z1 Z5 + (0.115986,0) Z3 Z5 + (0.124321,0) Z0 Z4 + (-0.00101412,0) Z1 Y5 Z6 Y7 + (-0.00101412,0) Z1 X5 Z6 X7 + (0.111941,0) Z2 Z5 + (-0.00245129,0) X0 Z1 X2 Z5 + (0.108983,0) Z0 Z5 + (-0.0246282,0) X0 Z1 X2 X5 Z6 X7 + (-0.0248416,0) X1 X2 X6 X7 + (-0.0248416,0) Y1 Y2 X6 X7 + (0.0393455,0) Y0 Y1 X4 X5 + (0.115986,0) Z1 Z7 + (0.010771,0) X0 X1 X4 Z5 Z6 X7 + (0.0203913,0) Z0 X4 Z5 X6 + (-0.0102424,0) X0 Z1 Z2 X3 X6 X7 + (0.0242674,0) X0 Z1 Z2 X3 Y4 Z5 Z6 Y7 + (-0.000703626,0) Z2 X5 Z6 X7 + (0.0261613,0) Y1 Z2 Y3 Y5 Z6 Y7 + (-0.0102424,0) Y0 Z1 Z2 Y3 X6 X7 + (-0.0102424,0) Y0 Z1 Z2 Y3 Y6 Y7 + (0.0245002,0) Y0 Y1 Y5 Y6 + (0.026958,0) Y0 Z1 Y2 X4 Z5 X6 + (0.026958,0) Y0 Z1 Y2 Y4 Z5 Y6 + (0.026958,0) X0 Z1 X2 X4 Z5 X6 + (-0.0376584,0) X2 X3 Y4 Y5 + (0.0203913,0) Y0 Z1 Y2 Z4 + (0.0112345,0) X4 Z5 X6 Z7 + (-0.0130301,0) Y0 Y1 X2 X3 + (0.00171566,0) Z2 X4 Z5 X6 + (0.00556213,0) Y4 Z5 Y6 + (-0.0244182,0) Y4 Y5 Y6 Y7 + (0.0245002,0) Y0 Y1 X5 X6 + (-0.0132401,0) Y1 Y2 X4 Z5 Z6 X7 + (-0.334612,0) Z3 + (0.010771,0) X0 Z1 Z2 X3 Y4 Y5 + (-0.0376584,0) Y2 Y3 X4 X5 + (0.0242674,0) Y0 Z1 Z2 Y3 Y4 Z5 Z6 Y7 + (0.0214768,0) X0 Z1 X2 Z7 + (0.0776204,0) Z5 Z6 + (-0.0248416,0) Y1 Y2 Y6 Y7 + (-0.0233845,0) Z3 Y5 Z6 Y7 + (-0.0376584,0) X0 X1 Y6 Y7 + (0.0242674,0) Y0 Z1 Z2 Y3 X4 Z5 Z6 X7 + (-0.0246282,0) X1 Z2 X3 Y4 Z5 Y6 + (-0.0130301,0) X4 X5 Y6 Y7 + (0.0242674,0) X0 Z1 Z2 X3 X4 Z5 Z6 X7 + (-0.0246282,0) X0 Z1 X2 Y5 Z6 Y7 + (0.0393455,0) X0 X1 X4 X5 + (-0.0102424,0) X0 Z1 Z2 X3 Y6 Y7 + (0.0696375,0) Z4 Z5 + (0.00171566,0) X0 Z1 X2 Z6 + (-0.0102424,0) Y2 Y3 Y4 Z5 Z6 Y7 + (0.010771,0) X0 Z1 Z2 X3 X4 X5 + (-0.0246282,0) X1 Z2 X3 X4 Z5 X6 + (0.0393455,0) Y0 Y1 Y4 Y5 + (-0.0376584,0) X2 X3 X4 X5 + (-0.0113881,0) Y0 X1 X2 Y3 + (0.026958,0) X0 Z1 X2 Y4 Z5 Y6 + (0.00556213,0) X4 Z5 X6 + (0.0172666,0) Y5 Z6 Y7 + (0.00556213,0) X0 Z1 X2 + (0.0172666,0) X1 Z2 X3 + (0.0214768,0) Z3 Y4 Z5 Y6 + (0.010771,0) Y0 Y1 X4 Z5 Z6 X7 + (0.0879264,0) Z5 + (0.0245002,0) X1 X2 X4 X5 + (-0.0102909,0) Z0 Y1 Z2 Y3 + (-0.0233845,0) Z3 X5 Z6 X7 + (-0.0132401,0) X1 X2 Y4 Z5 Z6 Y7 + (-0.000703626,0) Y1 Z2 Y3 Z6 + (0.00556213,0) Y0 Z1 Y2 + (-0.00101412,0) Y1 Z2 Y3 Z5 + (0.0794385,0) Z6 Z7 + (-0.0102424,0) Y2 Y3 X4 Z5 Z6 X7 + (-0.0102424,0) X2 X3 Y4 Z5 Z6 Y7 + (0.0214768,0) Y0 Z1 Y2 Z7 + (-0.0102424,0) X2 X3 X4 Z5 Z6 X7 + (-0.0246282,0) Y1 Z2 Y3 Y4 Z5 Y6 + (-0.334612,0) Z7 + (0.0172666,0) X5 Z6 X7 + (-0.331478,0) + (-0.0790441,0) Z6 + (-0.0132401,0) X1 X2 X4 Z5 Z6 X7 + (0.181365,0) Z4 + (-0.0210619,0) Y1 Z2 Y3 Z4 + (-0.0376584,0) Y0 Y1 Y6 Y7 + (-0.0132401,0) Y1 Y2 Y4 Z5 Z6 Y7 + (-0.0210619,0) Z0 Y5 Z6 Y7 + (0.010771,0) Y0 Z1 Z2 Y3 X4 X5 + (-0.000703626,0) X1 Z2 X3 Z6 + (-0.0233845,0) Y1 Z2 Y3 Z7 + (0.0245002,0) Y1 Y2 Y4 Y5 + (0.0245002,0) X0 X1 X5 X6 + (-0.00245129,0) Z1 Y4 Z5 Y6 + (-0.0132401,0) Y0 Z1 Z2 Y3 X5 X6 + (-0.0248416,0) Y2 Y3 Y5 Y6 + (-0.00245129,0) Z1 X4 Z5 X6 + (0.00171566,0) Y0 Z1 Y2 Z6 + (-0.0255452,0) Y5 Y7 + (0.0261613,0) X1 Z2 X3 X5 Z6 X7 + (-0.0790441,0) Z2 + (0.010771,0) X0 X1 Y4 Z5 Z6 Y7 + (0.0245002,0) X0 X1 Y5 Y6 + (-0.0130301,0) X0 X1 Y2 Y3 + (0.0245002,0) X1 X2 Y4 Y5 + (-0.0244182,0) X0 X1 X2 X3 + (0.0203913,0) Z0 Y4 Z5 Y6 + (0.0845405,0) Z4 Z6 + (0.0245002,0) Y1 Y2 X4 X5 + (-0.0248416,0) X2 X3 X5 X6 + (-0.00101412,0) X1 Z2 X3 Z5 + (0.111499,0) Z2 Z4 + (0.0261613,0) X1 Z2 X3 Y5 Z6 Y7 + (0.010771,0) Y0 Z1 Z2 Y3 Y4 Y5 + (0.0879264,0) Z1 + (0.00171566,0) Z2 Y4 Z5 Y6 + (0.0172666,0) Y1 Z2 Y3 + (-0.00245129,0) Y0 Z1 Y2 Z5 + (-0.0255452,0) X5 X7 + (-0.0132401,0) X0 Z1 Z2 X3 X5 X6 + (-0.0102909,0) Z0 X1 Z2 X3 + (-0.0376584,0) Y0 Y1 X6 X7 + (0.116851,0) Z2 Z6 + (-0.000703626,0) Z2 Y5 Z6 Y7 + (0.0261613,0) Y1 Z2 Y3 X5 Z6 X7 + (-0.0132401,0) X0 Z1 Z2 X3 Y5 Y6 + (-0.0132401,0) Y0 Z1 Z2 Y3 Y5 Y6 + (-0.0244182,0) X4 X5 X6 X7 + (0.0393455,0) X0 X1 Y4 Y5 + (-0.0113881,0) X4 Y5 Y6 X7 + (0.010771,0) Y0 Y1 Y4 Z5 Z6 Y7 + (-0.0246282,0) Y0 Z1 Y2 X5 Z6 X7 + (-0.0210619,0) X1 Z2 X3 Z4 + (0.120055,0) Z3 Z6 + (0.181365,0) Z0 + (-0.0130301,0) Y4 Y5 X6 X7 + (0.089825,0) Z1 Z3 + (-0.0248416,0) X1 X2 Y6 Y7 + (0.0112345,0) Y4 Z5 Y6 Z7 + (-0.0233845,0) X1 Z2 X3 Z7 + (0.0343207,0) Y1 Y2 Y5 Y6 + (0.145261,0) Z3 Z7 + (-0.0244182,0) Y0 Y1 Y2 Y3 + (-0.0113881,0) Y4 X5 X6 Y7 + (0.108983,0) Z1 Z4 + (0.0406161,0) Y2 Y3 Y6 Y7 + (0.106471,0) Z4 Z7 + (0.0406161,0) Y2 Y3 X6 X7 + (0.0406161,0) X2 X3 X6 X7 + (-0.0248416,0) Y2 Y3 X5 X6 + (-0.0248416,0) X2 X3 Y5 Y6 + (-0.0113881,0) X0 Y1 Y2 X3 + (0.120055,0) Z2 Z7 + (0.022049,0) X0 X2 + (0.022049,0) Y0 Y2 + (0.0203913,0) X0 Z1 X2 Z4 + (0.0406161,0) X2 X3 Y6 Y7 + (0.022049,0) X4 X6 + (-0.0210619,0) Z0 X5 Z6 X7 + (0.0214768,0) Z3 X4 Z5 X6 + (0.0794385,0) Z2 Z3 + (-0.0255452,0) X1 X3 + (-0.0376584,0) Y2 Y3 Y4 Y5 + (-0.0255452,0) Y1 Y3 + (0.0343207,0) X1 X2 X5 X6 + (0.106471,0) Z0 Z3 + (0.022049,0) Y4 Y6 + (0.0343207,0) X1 X2 Y5 Y6 + (0.0343207,0) Y1 Y2 X5 X6 + (0.0112345,0) Y0 Z1 Y2 Z3 + (0.0112345,0) X0 Z1 X2 Z3 + (-0.0246282,0) Y0 Z1 Y2 Y5 Z6 Y7 + (-0.0246282,0) Y1 Z2 Y3 X4 Z5 X6)";

  auto H4 = xacc::quantum::getObservable("pauli", hamiltonian);
  std::cout << H4->toString() << "\n";

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