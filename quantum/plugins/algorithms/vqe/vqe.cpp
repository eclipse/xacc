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
    gradientStrategy->initialize(parameters);
  }

  if ((parameters.stringExists("gradient_strategy") ||
       parameters.pointerLikeExists<AlgorithmGradientStrategy>(
           "gradient_strategy")) &&
      !optimizer->isGradientBased()) {
    xacc::warning(
        "Chosen optimizer does not support gradients. Using default.");
  }

  if (optimizer && optimizer->isGradientBased() &&
      gradientStrategy == nullptr) {
    // No gradient strategy was provided, just use autodiff.
    gradientStrategy = xacc::getService<AlgorithmGradientStrategy>("autodiff");
    gradientStrategy->initialize(parameters);
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

  std::vector<std::shared_ptr<AcceleratorBuffer>> min_child_buffers;

  // auto kernels = observable->observe(xacc::as_shared_ptr(kernel));
  // Cache of energy values during iterations.
  std::vector<double> energies;
  double last_energy = std::numeric_limits<double>::max();

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.
  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {
        std::vector<double> coefficients;
        std::vector<std::string> kernelNames;
        std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

        // call CompositeInstruction::operator()()
        auto tmp_x = x;
        std::reverse(tmp_x.begin(), tmp_x.end());
        auto evaled = kernel->operator()(tmp_x);
        // observe
        auto kernels = observable->observe(evaled);

        double identityCoeff = 0.0;
        int nInstructionsEnergy = kernels.size(), nInstructionsGradient = 0;
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
            fsToExec.push_back(f);
            coefficients.push_back(std::real(coeff));
          } else {
            identityCoeff += std::real(coeff);
            coefficients.push_back(std::real(coeff));
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

        // Tag any gradient buffers;
        for (int i = nInstructionsEnergy; i < buffers.size(); i++) {
          buffers[i]->addExtraInfo("is-gradient-calc", true);
        }

        auto tmp_buffer_extra_info = tmpBuffer->getInformation();
        for (auto &[k, v] : tmp_buffer_extra_info) {
          buffer->addExtraInfo(k, v);
        }

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

        // Add information about the variational parameters to the child
        // buffers.
        // Other energy (observable-related) information will be populated by
        // the Observable::postProcess().
        for (auto &childBuffer : buffers) {
          childBuffer->addExtraInfo("parameters", x);
        }

        // Special key to indicate that the buffer was processed by a
        // HPC virtualization decorator.
        const std::string aggregate_key =
            "__internal__decorator_aggregate_vqe__";
        const double energy = [&]() {
          // Compute the Energy. We can do this manually,
          // or we may have a case where a accelerator decorator
          // posts the energy automatically at a given key
          // on the buffer extra info.
          if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
                  xacc::as_shared_ptr(accelerator)) &&
              buffer->hasExtraInfoKey(aggregate_key)) {
            // Handles VQE that was executed on a virtualized Accelerator,
            // i.e. the energy has been aggregated by the Decorator.
            double resultEnergy = identityCoeff;
            // This is a decorator that has an aggregate value
            auto aggregate_value = (*buffer)[aggregate_key].as<double>();
            resultEnergy += aggregate_value;
            return resultEnergy;
          } else {
            // Normal VQE: post-proces the result with the Observable.
            // This will also populate meta-data to the child-buffer of
            // the main buffer.
            return observable->postProcess(tmpBuffer);
          }
        }();

        // Compute the variance as well as populate any variance-related
        // information to the child buffers
        const double variance = [&]() {
          if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
                  xacc::as_shared_ptr(accelerator)) &&
              buffer->hasExtraInfoKey(aggregate_key)) {
            // HPC decorator doesn't support variance...
            return 0.0;

          } else {
            // This will also populate information about variance to each child
            // buffer.
            return observable->postProcess(
                tmpBuffer, Observable::PostProcessingTask::VARIANCE_CALC);
          }
        }();

        if (gradientStrategy) {
          // gradient-based optimization
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
        }

        // Append the child buffers from the temp. buffer
        // to the main buffer.
        // These child buffers have extra-information populate in the above
        // post-process steps.
        for (auto &b : buffers) {
          buffer->appendChild(b->name(), b);
        }
        std::stringstream ss;
        ss << "E(" << (!x.empty() ? std::to_string(x[0]) : "");
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << energy;
        xacc::info(ss.str());
        // Saves the energy value.
        energies.emplace_back(energy);

        if (energy < last_energy) {
          min_child_buffers.clear();
          min_child_buffers.push_back(idBuffer);
          for (auto b : buffers) {
            min_child_buffers.push_back(b);
          } 
          last_energy = energy;
        }

        return energy;
      },
      kernel->nVariables());

  auto result = optimizer->optimize(f);

  // Get the children at the opt-params
  std::vector<double> opt_exp_vals, children_coeffs;
  std::vector<std::string> children_names;
  for (auto &child : min_child_buffers) {
    if (!child->hasExtraInfoKey("is-gradient-calc")) {
      opt_exp_vals.push_back(child->getInformation("exp-val-z").as<double>());

      // will not have
      children_coeffs.push_back(
          child->getInformation("coefficient").as<double>());
      children_names.push_back(child->name());
    } 
  }

  buffer->addExtraInfo("opt-exp-vals", opt_exp_vals);
  buffer->addExtraInfo("coefficients", children_coeffs);
  buffer->addExtraInfo("kernel-names", children_names);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  // Adds energies so that users can examine the convergence.
  buffer->addExtraInfo("params-energy", ExtraInfo(energies));
  return;
}

std::vector<double>
VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
             const std::vector<double> &x) {

  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

  double identityCoeff = 0.0;
  auto tmp_x = x;
  std::reverse(tmp_x.begin(), tmp_x.end());
  auto evaled = xacc::as_shared_ptr(kernel)->operator()(tmp_x);
  auto kernels = observable->observe(evaled);
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
      fsToExec.push_back(f);
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();
  for (auto &b : buffers) {
    b->addExtraInfo("parameters", x);
  }
  auto tmp_buffer_extra_info = tmpBuffer->getInformation();
  for (auto &[k, v] : tmp_buffer_extra_info) {
    buffer->addExtraInfo(k, v);
  }

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
  const std::string aggregate_key = "__internal__decorator_aggregate_vqe__";
  const double energy = [&]() {
    // Compute the Energy. We can do this manually,
    // or we may have a case where a accelerator decorator
    // posts the energy automatically at a given key
    // on the buffer extra info.
    if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
            xacc::as_shared_ptr(accelerator)) &&
        buffer->hasExtraInfoKey(aggregate_key)) {
      // Handles VQE that was executed on a virtualized Accelerator,
      // i.e. the energy has been aggregated by the Decorator.
      double resultEnergy = identityCoeff;
      // This is a decorator that has an aggregate value
      auto aggregate_value = (*buffer)[aggregate_key].as<double>();
      resultEnergy += aggregate_value;
      return resultEnergy;
    } else {
      // Normal VQE: post-proces the result with the Observable.
      // This will also populate meta-data to the child-buffer of
      // the main buffer.
      return observable->postProcess(tmpBuffer);
    }
  }();

  // Compute the variance as well as populate any variance-related information
  // to the child buffers
  const double variance = [&]() {
    if (std::dynamic_pointer_cast<xacc::AcceleratorDecorator>(
            xacc::as_shared_ptr(accelerator)) &&
        buffer->hasExtraInfoKey(aggregate_key)) {
      // HPC decorator doesn't support variance...
      return 0.0;

    } else {
      // This will also populate information about variance to each child
      // buffer.
      return observable->postProcess(
          tmpBuffer, Observable::PostProcessingTask::VARIANCE_CALC);
    }
  }();
  // Append the child buffers from the temp. buffer
  // to the main buffer.
  // These child buffers have extra-information populate in the above
  // post-process steps.
  for (auto &b : buffers) {
    buffer->appendChild(b->name(), b);
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