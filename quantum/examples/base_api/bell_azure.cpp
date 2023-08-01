/*******************************************************************************
 * Copyright (c) 2023 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino
 *******************************************************************************/
#include "xacc.hpp"
//#include "xacc_service.hpp"

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);

  xacc::external::load_external_language_plugins();

  xacc::qasm(R"(
.compiler xasm
.circuit ansatz
.qbit q
H(q[0]);
CX(q[0],q[1]);
Measure(q[0]);
Measure(q[1]);
)");
  auto ansatz = xacc::getCompiled("ansatz");

  auto buffer = xacc::qalloc(2);
  auto qpu = xacc::getAccelerator("azure", {{"shots", 1024}, {"backend", "microsoft.estimator"}, {"visitor", "pyqir"}});
  qpu->execute(buffer, ansatz);
  std::cout << "Running the resource estimator\n\n" << (*buffer)["estimate-data"].as<std::string>() << std::endl;

  buffer->resetBuffer();
  qpu->updateConfiguration({{"shots", 1024}, {"backend", "ionq.simulator"}, {"visitor", "qiskit"}});
  qpu->execute(buffer, ansatz);
  std::cout << "Printing counts from a Bell state run on the IonQ simulator\n" << std::endl;
  for (auto & m :  buffer->getMeasurementCounts())
    std::cout << m.first << "\t" << m.second << std::endl;

  buffer->resetBuffer();
  qpu->updateConfiguration({{"shots", 1024}, {"backend", "quantinuum.qpu.h1-2"}, {"visitor", "qiskit"}});
  qpu->execute(buffer, ansatz);
  std::cout << "Cost estimate for a Bell state on H1-2 with 1024 shots: " << (*buffer)["cost-estimate"].as<std::string>() << std::endl;

  xacc::external::unload_external_language_plugins();
  xacc::Finalize();
  return 0;
}