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
#include <string>

using namespace xacc;

namespace xacc {
namespace algorithm {
bool VQE::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.pointerLikeExists<Observable>("observable")) {
    std::cout << "Obs was false\n";
    return false;
  } 
  
  if (!parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    std::cout << "Ansatz was false\n";
    return false;
  } 
  
  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  }

  observable = parameters.getPointerLike<Observable>("observable");
  if (parameters.pointerLikeExists<Optimizer>("optimizer")) {
    optimizer = parameters.getPointerLike<Optimizer>("optimizer");
  }
  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");

  // if gradient is provided
  if (parameters.pointerLikeExists<AlgorithmGradientStrategy>(
      "gradient_strategy")){
    gradientStrategy = parameters.getPointerLike<AlgorithmGradientStrategy>(
      "gradient_strategy");
  }

  return true;
}

const std::vector<std::string> VQE::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "ansatz"};
}

void VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  if (!optimizer) {
    xacc::error("VQE Algorithm Error - Optimizer was null. Please provide a "
                "valid Optimizer.");
  }

  auto kernels = observable->observe(xacc::as_shared_ptr(kernel));

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.
  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {
        std::vector<double> coefficients;
        std::vector<std::string> kernelNames;
        std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

        double identityCoeff = 0.0;
        int nInstructionsEnergy = 0, nInstructionsGradient = 0;
        for (auto &f : kernels) {
          kernelNames.push_back(f->name());
          std::complex<double> coeff = f->getCoefficient();

          int nFunctionInstructions;
          if (f->getInstruction(0)->isComposite()) {
            nFunctionInstructions =
                kernel->nInstructions() + f->nInstructions() - 1;
          } else {
            nFunctionInstructions = f->nInstructions();
          }

          if (nFunctionInstructions > kernel->nInstructions()) {
            if (x.empty()) {
              fsToExec.push_back(f);
            } else {
              auto evaled = f->operator()(x);
              fsToExec.push_back(evaled);
            }
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
          }
        }

        // Retrieve instructions for gradient, if a pointer of type 
        // AlgorithmGradientStrategy is given
        if (gradientStrategy){

          auto gradFsToExec = gradientStrategy->getGradientExecutions(xacc::as_shared_ptr(kernel), x);
          // Add gradient instructions to be sent to the qpu
          nInstructionsEnergy = fsToExec.size();
          nInstructionsGradient = gradFsToExec.size();
          for (auto inst: gradFsToExec){
            fsToExec.push_back(inst);
          }
          xacc::info("Number of instructions for energy calculation: " 
                      + std::to_string(nInstructionsEnergy));
          xacc::info("Number of instructions for gradient calculation: "
                      + std::to_string(nInstructionsGradient));

        }

        auto tmpBuffer = xacc::qalloc(buffer->size());
        accelerator->execute(tmpBuffer, fsToExec);
        auto buffers = tmpBuffer->getChildren();

        double energy = identityCoeff;
        auto idBuffer = xacc::qalloc(buffer->size());
        idBuffer->addExtraInfo("coefficient", identityCoeff);
        idBuffer->setName("I");
        idBuffer->addExtraInfo("kernel", "I");
        idBuffer->addExtraInfo("parameters", x);
        idBuffer->addExtraInfo("exp-val-z", 1.0);
        if (accelerator->name() == "ro-error")
          idBuffer->addExtraInfo("ro-fixed-exp-val-z", 1.0);
        buffer->appendChild("I", idBuffer);

        if (buffers[0]->hasExtraInfoKey(
                "purified-energy")) { // FIXME Hack for now...
          energy = buffers[0]->getInformation("purified-energy").as<double>();
          for (auto &b : buffers) {
            b->addExtraInfo("parameters", initial_params);
            buffer->appendChild(b->name(), b);
          }

        } else if (gradientStrategy){ // gradient-based optimization

          for (int i = 0; i < nInstructionsEnergy; i++) {// compute energy
            auto expval = buffers[i]->getExpectationValueZ();
            energy += expval * coefficients[i];
            buffers[i]->addExtraInfo("coefficient", coefficients[i]);
            buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
            buffers[i]->addExtraInfo("exp-val-z", expval);
            buffers[i]->addExtraInfo("parameters", x);
            buffer->appendChild(fsToExec[i]->name(), buffers[i]);
          }

          std::stringstream ss;
          ss << std::setprecision(12) << "Current Energy: " << energy;
          xacc::info(ss.str());
          ss.str(std::string());

          // update gradient vector
          
          gradientStrategy->compute(dx, 
            std::vector<std::shared_ptr<AcceleratorBuffer>>(buffers.begin() + nInstructionsEnergy, buffers.end()));

        } else {// normal VQE run
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
        ss << "E(" << (!x.empty() ? std::to_string(x[0]) : "");
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

std::vector<double>
VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
             const std::vector<double> &x) {

  auto kernels = observable->observe(xacc::as_shared_ptr(kernel));
  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

  double identityCoeff = 0.0;
  for (auto &f : kernels) {
    kernelNames.push_back(f->name());
    std::complex<double> coeff = f->getCoefficient();
            // std::cout << f->name() << "\n" << f->toString() <<"\n";

    int nFunctionInstructions = 0;
    if (f->getInstruction(0)->isComposite()) {
      nFunctionInstructions = kernel->nInstructions() + f->nInstructions() - 1;
    } else {
      nFunctionInstructions = f->nInstructions();
    }

    if (nFunctionInstructions > kernel->nInstructions()) {
      if (x.empty()) {
        fsToExec.push_back(f);
      } else {
        auto evaled = f->operator()(x);
        fsToExec.push_back(evaled);
      }
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();

  double energy = identityCoeff;
  auto idBuffer = xacc::qalloc(buffer->size());
  idBuffer->addExtraInfo("coefficient", identityCoeff);
  idBuffer->setName("I");
  idBuffer->addExtraInfo("kernel", "I");
  idBuffer->addExtraInfo("parameters", x);
  idBuffer->addExtraInfo("exp-val-z", 1.0);
  if (accelerator->name() == "ro-error")
    idBuffer->addExtraInfo("ro-fixed-exp-val-z", 1.0);
  buffer->appendChild("I", idBuffer);

  if (buffers[0]->hasExtraInfoKey("purified-energy")) { // FIXME Hack for now...
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

  ss << "E(" << (!x.empty() ? std::to_string(x[0]) : "");
  for (int i = 1; i < x.size(); i++)
    ss << "," << x[i];
  ss << ") = " << std::setprecision(12) << energy;
  xacc::info(ss.str());
  return {energy};
}

} // namespace algorithm
} // namespace xacc