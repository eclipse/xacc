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
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AcceleratorDecorator.hpp"

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
          "gradient_strategy")) {
    gradientStrategy =
        parameters.get<std::shared_ptr<AlgorithmGradientStrategy>>(
            "gradient_strategy");
    // gradientStrategy->initialize({std::make_pair("observable",
    // xacc::as_shared_ptr(observable))});
  }

  if (parameters.stringExists("gradient_strategy") &&
      !parameters.pointerLikeExists<AlgorithmGradientStrategy>(
          "gradient_strategy") &&
      optimizer->isGradientBased()) {
    gradientStrategy = xacc::getService<AlgorithmGradientStrategy>(
        parameters.getString("gradient_strategy"));
    gradientStrategy->initialize(
        {std::make_pair("observable", xacc::as_shared_ptr(observable))});
  }

  if ((parameters.stringExists("gradient_strategy") ||
       parameters.pointerLikeExists<AlgorithmGradientStrategy>(
           "gradient_strategy")) &&
      !optimizer->isGradientBased()) {
    xacc::warning(
        "Chosen optimizer does not support gradients. Using default.");
  }

  if (optimizer->isGradientBased() && gradientStrategy == nullptr) {
    // No gradient strategy was provided, just use autodiff.
    gradientStrategy = xacc::getService<AlgorithmGradientStrategy>("autodiff");
    gradientStrategy->initialize({{"observable", observable}});
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
  // Cache of energy values during iterations.
  std::vector<double> energies;

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
              evaled->setCoefficient(f->getCoefficient());
              fsToExec.push_back(evaled);
            }
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
          }
        }

        // Retrieve instructions for gradient, if a pointer of type
        // AlgorithmGradientStrategy is given
        if (gradientStrategy) {

          auto gradFsToExec = gradientStrategy->getGradientExecutions(
              xacc::as_shared_ptr(kernel), x);
          // Add gradient instructions to be sent to the qpu
          nInstructionsEnergy = fsToExec.size();
          nInstructionsGradient = gradFsToExec.size();
          for (auto inst : gradFsToExec) {
            fsToExec.push_back(inst);
          }
          xacc::info("Number of instructions for energy calculation: " +
                     std::to_string(nInstructionsEnergy));
          xacc::info("Number of instructions for gradient calculation: " +
                     std::to_string(nInstructionsGradient));
        }

        auto tmpBuffer = xacc::qalloc(buffer->size());
        accelerator->execute(tmpBuffer, fsToExec);
        auto buffers = tmpBuffer->getChildren();

        auto tmp_buffer_extra_info = tmpBuffer->getInformation();
        for (auto &[k, v] : tmp_buffer_extra_info) {
          buffer->addExtraInfo(k, v);
        }

        // Compute the Energy. We can do this manually,
        // or we may have a case where a accelerator decorator
        // posts the energy automatically at a given key
        // on the buffer extra info.
        double energy = identityCoeff;
        double variance = 0.0;

        // Create buffer child for the Identity term
        auto idBuffer = xacc::qalloc(buffer->size());
        idBuffer->addExtraInfo("coefficient", identityCoeff);
        idBuffer->setName("I");
        idBuffer->addExtraInfo("kernel", "I");
        idBuffer->addExtraInfo("parameters", x);
        idBuffer->addExtraInfo("exp-val-z", 1.0);
        if (accelerator->name() == "ro-error")
          idBuffer->addExtraInfo("ro-fixed-exp-val-z", 1.0);
        buffer->appendChild("I", idBuffer);

        bool got_aggregate = false;
        std::string aggregate_key = "__internal__decorator_aggregate_vqe__";
        if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
                xacc::as_shared_ptr(accelerator)) &&
            buffer->hasExtraInfoKey(aggregate_key)) {
          // This is a decorator that has an aggregate value
          auto aggregate_value = (*buffer)[aggregate_key].as<double>();
          energy += aggregate_value;
          for (auto &b : buffers) {
            b->addExtraInfo("parameters", initial_params);
            buffer->appendChild(b->name(), b);
          }
          got_aggregate = true;
        }

        if (gradientStrategy) { // gradient-based optimization

          for (int i = 0; i < nInstructionsEnergy; i++) { // compute energy
            auto expval = buffers[i]->getExpectationValueZ();
            if (!got_aggregate) {
              energy += expval * coefficients[i];
              if (!buffers[i]->getMeasurementCounts().empty()) {
                auto paulvar = 1. - expval * expval;
                buffers[i]->addExtraInfo("pauli-variance", paulvar);
                variance += coefficients[i] * coefficients[i] * paulvar;
              }
            }
            buffers[i]->addExtraInfo("coefficient", coefficients[i]);
            buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
            buffers[i]->addExtraInfo("exp-val-z", expval);
            buffers[i]->addExtraInfo("parameters", x);
            if (!buffers[i]->getMeasurementCounts().empty()) {
              int n_shots = 0;
              for (auto [k, v] : buffers[i]->getMeasurementCounts()) {
                n_shots += v;
              }

              buffers[i]->addExtraInfo("energy-standard-deviation",
                                       std::sqrt(variance / n_shots));
            }
            buffer->appendChild(fsToExec[i]->name(), buffers[i]);
          }

          std::stringstream ss;
          ss << std::setprecision(12) << "Current Energy: " << energy;
          xacc::info(ss.str());
          ss.str(std::string());

          // If gradientStrategy is numerical, pass the energy
          // We subtract the identityCoeff from the energy
          // instead of passing the energy because the gradients
          // only take the coefficients of parameterized instructions
          if (gradientStrategy->isNumerical()) {
            gradientStrategy->setFunctionValue(energy - identityCoeff);
          }

          // update gradient vector
          gradientStrategy->compute(
              dx, std::vector<std::shared_ptr<AcceleratorBuffer>>(
                      buffers.begin() + nInstructionsEnergy, buffers.end()));

        } else if (!got_aggregate) {
          for (int i = 0; i < fsToExec.size(); i++) {
            auto expval = buffers[i]->getExpectationValueZ();
            energy += expval * coefficients[i];
            if (!buffers[i]->getMeasurementCounts().empty()) {
              auto paulvar = 1. - expval * expval;
              buffers[i]->addExtraInfo("pauli-variance", paulvar);
              variance += coefficients[i] * coefficients[i] * paulvar;
            }
          }

          for (int i = 0; i < fsToExec.size(); i++) {
            buffers[i]->addExtraInfo("coefficient", coefficients[i]);
            buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
            buffers[i]->addExtraInfo("exp-val-z",
                                     buffers[i]->getExpectationValueZ());
            buffers[i]->addExtraInfo("parameters", x);

            if (!buffers[i]->getMeasurementCounts().empty()) {
              int n_shots = 0;
              for (auto [k, v] : buffers[i]->getMeasurementCounts()) {
                n_shots += v;
              }

              buffers[i]->addExtraInfo("energy-standard-deviation",
                                       std::sqrt(variance / n_shots));
            }
            buffer->appendChild(fsToExec[i]->name(), buffers[i]);
          }
        }

        std::stringstream ss;
        ss << "E(" << (!x.empty() ? std::to_string(x[0]) : "");
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << energy;
        xacc::info(ss.str());
        // Saves the energy value.
        energies.emplace_back(energy);
        return energy;
      },
      kernel->nVariables());

  auto result = optimizer->optimize(f);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  // Adds energies so that users can examine the convergence.
  buffer->addExtraInfo("params-energy", ExtraInfo(energies));
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
        // Need to add this if x != {}
        // so that the HPCVirtDecorator can 
        // access the coefficients to compute the energy
        if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
          xacc::as_shared_ptr(accelerator))) {
          evaled->setCoefficient(coeff);
        }
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
  auto tmp_buffer_extra_info = tmpBuffer->getInformation();
  for (auto &[k, v] : tmp_buffer_extra_info) {
    buffer->addExtraInfo(k, v);
  }

  // Compute the Energy. We can do this manually,
  // or we may have a case where a accelerator decorator
  // posts the energy automatically at a given key
  // on the buffer extra info.
  double energy = identityCoeff;
  double variance = 0.0;

  // Create buffer child for the Identity term
  auto idBuffer = xacc::qalloc(buffer->size());
  idBuffer->addExtraInfo("coefficient", identityCoeff);
  idBuffer->setName("I");
  idBuffer->addExtraInfo("kernel", "I");
  idBuffer->addExtraInfo("parameters", x);
  idBuffer->addExtraInfo("exp-val-z", 1.0);
  if (accelerator->name() == "ro-error")
    idBuffer->addExtraInfo("ro-fixed-exp-val-z", 1.0);
  buffer->appendChild("I", idBuffer);

  std::string aggregate_key = "__internal__decorator_aggregate_vqe__";
  if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
          xacc::as_shared_ptr(accelerator)) &&
      buffer->hasExtraInfoKey(aggregate_key)) {
    // This is a decorator that has an aggregate value
    auto aggregate_value = (*buffer)[aggregate_key].as<double>();
    energy += aggregate_value;
    for (auto &b : buffers) {
      b->addExtraInfo("parameters", initial_params);
      buffer->appendChild(b->name(), b);
    }
  }

  else {
    for (int i = 0; i < fsToExec.size(); i++) { // compute energy
      auto expval = buffers[i]->getExpectationValueZ();
      energy += expval * coefficients[i];
      if (!buffers[i]->getMeasurementCounts().empty()) {
        auto paulvar = 1. - expval * expval;
        buffers[i]->addExtraInfo("pauli-variance", paulvar);
        variance += coefficients[i] * coefficients[i] * paulvar;
      }
    }

    for (int i = 0; i < fsToExec.size(); i++) {
      buffers[i]->addExtraInfo("coefficient", coefficients[i]);
      buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
      buffers[i]->addExtraInfo("exp-val-z", buffers[i]->getExpectationValueZ());
      buffers[i]->addExtraInfo("parameters", x);
      if (!buffers[i]->getMeasurementCounts().empty()) {
        int n_shots = 0;
        for (auto [k, v] : buffers[i]->getMeasurementCounts()) {
          n_shots += v;
        }

        buffers[i]->addExtraInfo("energy-standard-deviation",
                                 std::sqrt(variance / n_shots));
      }
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