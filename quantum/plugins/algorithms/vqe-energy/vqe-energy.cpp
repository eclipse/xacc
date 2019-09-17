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
#include "vqe-energy.hpp"

#include "Observable.hpp"
#include "xacc.hpp"

#include <memory>
using namespace xacc;

namespace xacc {
namespace algorithm {
bool VQEEnergy::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")) {
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<CompositeInstruction>>(
                 "ansatz")) {
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<Accelerator>>(
                 "accelerator")) {
    return false;
  }
  try {
    observable =
        parameters.get_with_throw<std::shared_ptr<Observable>>("observable");
  } catch (std::exception &e) {
    observable = std::shared_ptr<Observable>(
        parameters.get<Observable *>("observable"), [](Observable *) {});
  }
  kernel = parameters.get<std::shared_ptr<CompositeInstruction>>("ansatz");
  accelerator = parameters.get<std::shared_ptr<Accelerator>>("accelerator");

  if (parameters.keyExists<std::vector<double>>("parameters")) {
    initial_params = parameters.get<std::vector<double>>("parameters");
  } else {
    xacc::error(
        "Cannot run Energy computation without providing 'parameters' list.");
  }
  return true;
}

const std::vector<std::string> VQEEnergy::requiredParameters() const {
  return {"observable", "accelerator", "ansatz"};
}

void VQEEnergy::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // void VQE::execute(xacc::Observable &observable, Optimizer &optimizer) {
  auto kernels = observable->observe(kernel);
  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

  double identityCoeff = 0.0;
  for (auto &f : kernels) {
    kernelNames.push_back(f->name());
    std::complex<double> coeff = f->getCoefficient();

    int nFunctionInstructions = 0;
    if (f->getInstruction(0)->isComposite()) {
      nFunctionInstructions = kernel->nInstructions() + f->nInstructions() - 1;
    } else {
      nFunctionInstructions = f->nInstructions();
    }

    if (nFunctionInstructions > kernel->nInstructions()) {
      fsToExec.push_back(f->operator()(initial_params));
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();

  double energy = identityCoeff;
  if (buffers[0]->hasExtraInfoKey("purified-energy")) { // FIXME Hack for now...
    energy = buffers[0]->getInformation("purified-energy").as<double>();
    for (auto& b : buffers) {
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
      buffers[i]->addExtraInfo("parameters", initial_params);
      buffer->appendChild(fsToExec[i]->name(), buffers[i]);
    }
  }
  std::stringstream ss;
  ss << "E(" << initial_params[0];
  for (int i = 1; i < initial_params.size(); i++)
    ss << "," << initial_params[i];
  ss << ") = " << energy;
  xacc::info(ss.str());

  buffer->addExtraInfo("opt-val", ExtraInfo(energy));
  buffer->addExtraInfo("opt-params", ExtraInfo(initial_params));
  return;
}
} // namespace algorithm
} // namespace xacc