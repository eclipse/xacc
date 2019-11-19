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

  auto accelerator = xacc::getAccelerator("ibm:ibmq_poughkeepsie");
  auto buffer = xacc::qalloc(2);

  xacc::qasm(R"(
.compiler xasm
.circuit bell
.qbit q
.parameters a, b, c, d
pulse::u3(q[0], pi/2, 0, pi);
pulse::cx(q[0],q[1], c, d);
pulse::meas(q[0]);
pulse::meas(q[1]);
)");

 auto bell = xacc::getCompiled("bell");
 accelerator->execute(buffer, bell);

}