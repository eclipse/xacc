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

  auto accelerator = xacc::getAccelerator(
      "aer", {std::make_pair("backend", "ibmq_johannesburg"),
              std::make_pair("readout_error", true)});

  xacc::qasm(R"(
.compiler xasm
.circuit bell
.qbit q
H(q[0]);
CX(q[0],q[1]);
Measure(q[0]);
Measure(q[1]);
)");
  auto bell = xacc::getCompiled("bell");

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, bell);

  buffer->print();

  xacc::external::unload_external_language_plugins();
  xacc::Finalize();
}