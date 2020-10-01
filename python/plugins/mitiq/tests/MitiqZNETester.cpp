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
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"

const std::string noiseModelJson =
    R"#({"errors": [{"type": "qerror", "operations": ["u1"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}, {"type": "qerror", "operations": ["u2"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}, {"type": "qerror", "operations": ["u3"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}], "x90_gates": []})#";

TEST(MitiqZNETester, checkSimple) {

  auto qpu = xacc::getAccelerator(
      "aer", {{"noise-model", noiseModelJson}, {"shots", 4096}});
  qpu = xacc::getAcceleratorDecorator("mitiq-zne", qpu);

  auto q = xacc::qalloc(1);

  xacc::qasm(R"#(.compiler xasm
.circuit foo
.parameters x
.qbit q
for (int i = 0; i < 50; i++) {
    X(q[0]);
}
Measure(q[0]);
)#");
  auto program = xacc::getCompiled("foo");

  qpu->execute(q, program);

  q->print(std::cout);

  xacc::qasm(R"#(.compiler xasm
.circuit foo2
.parameters x
.qbit q
for (int i = 0; i < 50; i++) {
    H(q[0]);
}
Measure(q[0]);
)#");

  auto program2 = xacc::getCompiled("foo2");
  auto r = xacc::qalloc(1);
  qpu->execute(r, program2);

  r->print(std::cout);

  auto v = xacc::qalloc(1);

  qpu->execute(v, {program, program2});
  v->print(std::cout);
}

TEST(QppAcceleratorTester, testDeuteron) {
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", noiseModelJson}, {"shots", 4096}});
  auto mitiq_accelerator = xacc::getAcceleratorDecorator("mitiq-zne", accelerator);
  
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      X(q[0]);
      Ry(q[1], t);
      CX(q[1], q[0]);
      H(q[0]);
      H(q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                  accelerator);

  auto program = ir->getComposite("ansatz");
  // Expected results from deuteron_2qbit_xasm_X0X1
  const std::vector<double> expectedResults{
      0.0,       -0.324699, -0.614213, -0.837166, -0.9694,
      -0.996584, -0.915773, -0.735724, -0.475947, -0.164595,
      0.164595,  0.475947,  0.735724,  0.915773,  0.996584,
      0.9694,    0.837166,  0.614213,  0.324699,  0.0};

  const auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto q = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    mitiq_accelerator->execute(buffer, evaled);
    accelerator->execute(q, evaled);
    std::cout << "Error: " << std::fabs(expectedResults[i] - q->getExpectationValueZ()) << "\n";
    std::cout << "Mitigated Error: " << std::fabs(expectedResults[i] - buffer->getExpectationValueZ()) << "\n\n";
  }
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
