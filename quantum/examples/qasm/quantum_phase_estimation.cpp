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
#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  // Accelerator:
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 4096)});
  
  // In this example: we want to estimate the *phase* of an arbitrary 'oracle'
  // i.e. Oracle(|State>) = exp(i*Phase)*|State>
  // and we need to estimate that Phase.

  // Oracle: CPhase(theta) or CU1(theta) which is defined as
  // 1 0 0 0
  // 0 1 0 0
  // 0 0 1 0
  // 0 0 0 e^(i*theta)
  // The eigenstate is |11>; i.e. CPhase(theta)|11> = e^(i*theta)|11>

  // Since this oracle operates on 2 qubits, we need to add more qubits to the buffer.
  // The more qubits we have, the more accurate the estimate.
  // Resolution := 2^(number qubits in the calculation register).
  // 5-bit precision => 7 qubits in total
  auto buffer = xacc::qalloc(7);
  auto qpe = xacc::getService<xacc::Algorithm>("QPE");
  auto compiler = xacc::getCompiler("xasm");
  
  // Create oracle: CPhase gate with theta = 2pi/3
  // i.e. the phase value to estimate is 1/3 ~ 0.33333.
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto oracle = gateRegistry->createComposite("oracle");
  oracle->addInstruction(gateRegistry->createInstruction("CPhase", { 0, 1 }, { 2.0 * M_PI/ 3.0 }));

  // Eigenstate preparation = |11> state
  auto statePrep = compiler->compile(R"(__qpu__ void prep1(qbit q) {
    X(q[0]); 
    X(q[1]); 
  })", nullptr)->getComposite("prep1");  
  
  // Initialize the Quantum Phase Estimation:
  qpe->initialize({
                    std::make_pair("accelerator", acc),
                    std::make_pair("oracle", oracle),
                    std::make_pair("state-preparation", statePrep)
                  });
  
  // Run the algorithm
  qpe->execute(buffer);
  // Expected result: 
  // The factor here is 2^5 (precision) = 32
  // we expect the two most-likely bitstring is 10 and 11
  // i.e. the true result is between 10/32 = 0.3125 and 11/32 = 0.34375  
  std::cout << "Probability of the two most-likely bitstrings 10 (theta = 0.3125) and 11 (theta = 0.34375 ): \n";
  std::cout << "Probability of |11010> (11) = " << buffer->computeMeasurementProbability("11010") << "\n";
  std::cout << "Probability of |01010> (10) = " << buffer->computeMeasurementProbability("01010") << "\n";

  xacc::Finalize();
}