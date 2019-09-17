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
#include "vqe.hpp"

#include "Observable.hpp"
#include "xacc.hpp"

#include <memory>
#include <iomanip>

using namespace xacc;

namespace xacc {
namespace algorithm {
bool VQE::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")) {
    std::cout << "Obs was false\n";
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<CompositeInstruction>>(
                 "ansatz")) {
    std::cout << "Ansatz was false\n";
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<Optimizer>>("optimizer")) {
    std::cout << "Opt was false\n";
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<Accelerator>>(
                 "accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  }
  try {
    observable =
        parameters.get_with_throw<std::shared_ptr<Observable>>("observable");
  } catch (std::exception &e) {
    observable = std::shared_ptr<Observable>(
        parameters.get<Observable *>("observable"), [](Observable *) {});
  }
  optimizer = parameters.get<std::shared_ptr<Optimizer>>("optimizer");
  kernel = parameters.get<std::shared_ptr<CompositeInstruction>>("ansatz");
  accelerator = parameters.get<std::shared_ptr<Accelerator>>("accelerator");

  return true;
}

const std::vector<std::string> VQE::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "ansatz"};
}

void VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // void VQE::execute(xacc::Observable &observable, Optimizer &optimizer) {
  auto kernels = observable->observe(kernel);

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.
  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double>& dx) {
        std::vector<double> coefficients;
        std::vector<std::string> kernelNames;
        std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

        double identityCoeff = 0.0;
        for (auto &f : kernels) {
          kernelNames.push_back(f->name());
          std::complex<double> coeff = f->getCoefficient();

          int nFunctionInstructions = 0;
          if (f->getInstruction(0)->isComposite()) {
            nFunctionInstructions =
                kernel->nInstructions() + f->nInstructions() - 1;
          } else {
            nFunctionInstructions = f->nInstructions();
          }

          if (nFunctionInstructions > kernel->nInstructions()) {
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
        if (buffers[0]->hasExtraInfoKey(
                "purified-energy")) { // FIXME Hack for now...
          energy = buffers[0]->getInformation("purified-energy").as<double>();
          for (auto &b : buffers) {
            b->addExtraInfo("parameters", initial_params);
            buffer->appendChild(b->name(), b);
          }
        } else {
          for (int i = 0; i < buffers.size(); i++) {
            auto expval = buffers[i]->getExpectationValueZ();
            energy += expval * coefficients[i];
            buffers[i]->addExtraInfo("coefficient", coefficients[i]);
            buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
            buffers[i]->addExtraInfo("exp-val-z", expval);
            buffers[i]->addExtraInfo("parameters", x);
            buffer->appendChild(fsToExec[i]->name(), buffers[i]);
          }
        }

        std::stringstream ss;
        ss << "E(" << x[0];
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << energy;
        xacc::info(ss.str());
        return energy;
      },
      kernel->nVariables());

  auto result = optimizer->optimize(f);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  return;
}
} // namespace algorithm
} // namespace xacc