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
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"

#include <iomanip>

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);

  // Get reference to the Accelerator
  // specified by --accelerator argument
  auto accelerator = xacc::getAccelerator("tnqvm");

  // Allocate some qubits
  auto buffer = xacc::qalloc(2);

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.circuit deuteron_ansatz
.parameters theta
X 0
Ry(theta) 1
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Observe the Ansatz, creating measured kernels
  auto kernels = H_N_2->observe(ansatz);

  // Create the OptFunction to be optimized
  xacc::OptFunction f(
      [&](const std::vector<double> &x, std::vector<double>& dx) {
        std::vector<double> coefficients;
        std::vector<std::string> kernelNames;
        std::vector<std::shared_ptr<xacc::CompositeInstruction>> fsToExec;

        double identityCoeff = 0.0;
        for (auto &f : kernels) {
          kernelNames.push_back(f->name());
          std::complex<double> coeff = f->getCoefficient();

          int nFunctionInstructions = 0;
          if (f->getInstruction(0)->isComposite()) {
            nFunctionInstructions =
                ansatz->nInstructions() + f->nInstructions() - 1;
          } else {
            nFunctionInstructions = f->nInstructions();
          }

          if (nFunctionInstructions > ansatz->nInstructions()) {
            fsToExec.push_back(f->operator()(x));
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
          }
        }

        auto tmpBuffer = xacc::qalloc(buffer->size());
        accelerator->execute(tmpBuffer, fsToExec);
        auto buffers = tmpBuffer->getChildren();

        double energy = identityCoeff;
        for (int i = 0; i < buffers.size(); i++) {
          auto expval = buffers[i]->getExpectationValueZ();
          energy += expval * coefficients[i];
          buffers[i]->addExtraInfo("coefficient", coefficients[i]);
          buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
          buffers[i]->addExtraInfo("exp-val-z", expval);
          buffers[i]->addExtraInfo("parameters", x);
          buffer->appendChild(fsToExec[i]->name(), buffers[i]);
        }

        std::stringstream ss;
        ss << "E(" << x[0];
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << energy;
        xacc::info(ss.str());
        return energy;
      },
      ansatz->nVariables());

  // Run Optimization routine, default is NLOPT COBYLA
  auto optimizer = xacc::getOptimizer("nlopt");
  auto results = optimizer->optimize(f);

  // Print the result
  std::cout << "Energy: " << results.first << "\n";
}