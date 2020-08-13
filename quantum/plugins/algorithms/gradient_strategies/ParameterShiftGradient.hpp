/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#ifndef XACC_PARAMETER_SHIFT_GRADIENT_HPP_
#define XACC_PARAMETER_SHIFT_GRADIENT_HPP_

#include "CompositeInstruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include <iomanip>

using namespace xacc;

namespace xacc {
namespace algorithm {

class ParameterShiftGradient : public AlgorithmGradientStrategy {

protected:
  std::shared_ptr<Observable> obs,
      commutator; // Hamiltonian (or any) observable
  std::function<std::shared_ptr<CompositeInstruction>(std::vector<double>)>
      kernel_evaluator;

public:
  bool initialize(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")) {
      xacc::error("Gradient strategy needs observable");
      return false;
    }

    obs = parameters.get<std::shared_ptr<Observable>>("observable");

    // this is specific to ADAPT-VQE
    if (parameters.keyExists<std::shared_ptr<Observable>>("commutator")) {
      commutator = parameters.get<std::shared_ptr<Observable>>("commutator");
    }

    if (parameters.keyExists<std::function<
            std::shared_ptr<CompositeInstruction>(std::vector<double>)>>(
            "kernel-evaluator")) {
      kernel_evaluator =
          parameters.get<std::function<std::shared_ptr<CompositeInstruction>(
              std::vector<double>)>>("kernel-evaluator");
    }
    return true;
  }

  // Not numerical
  bool isNumerical() const override { return false; }

  // Get the circuit instructions necessary to compute gradients
  std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit,
                        const std::vector<double> &x) override {

    // std::stringstream ss;
    // ss << std::setprecision(5) << "Input parameters: ";
    // for (auto param : x) {
    //   ss << param << " ";
    // }
    // xacc::info(ss.str());
    // ss.str(std::string());

    std::vector<std::shared_ptr<CompositeInstruction>> gradientInstructions;

    // The gradient of the operator added in the current ADAPT-VQE cycle
    // is simply its commutator with the Hamiltonian. This improves convergence
    // over parameter shift

    int start = 0;
    if (commutator) {
      std::vector<std::shared_ptr<CompositeInstruction>> kernels;
      if (kernel_evaluator) {
        auto evaled_base = kernel_evaluator(x);
        kernels = obs->observe(evaled_base);
        for (auto &f : kernels) {
          coefficients.push_back(std::real(f->getCoefficient()));
          gradientInstructions.push_back(f);
        }
      } else {
        kernels = obs->observe(circuit);

        // loop over circuit instructions
        // and gather coefficients/instructions
        for (auto &f : kernels) {
          auto evaled = f->operator()(x);
          coefficients.push_back(std::real(f->getCoefficient()));
          gradientInstructions.push_back(evaled);
        }
      }

      nInstructionsElement.push_back(kernels.size());
      start = 1;
    }

    for (int op = start; op < x.size();
         op++) {                        // loop over the remainder of operators
      for (double sign : {1.0, -1.0}) { // change sign

        // parameter shift and observe
        auto tmpX = x;
        tmpX[op] += sign * xacc::constants::pi / 2.0;

        std::vector<std::shared_ptr<CompositeInstruction>> kernels;
        if (kernel_evaluator) {
          auto evaled_base = kernel_evaluator(tmpX);
          kernels = obs->observe(evaled_base);
          for (auto &f : kernels) {
            coefficients.push_back(std::real(f->getCoefficient()));
            gradientInstructions.push_back(f);
          }
        } else {
          kernels = obs->observe(circuit);

          // loop over circuit instructions
          // and gather coefficients/instructions
          for (auto &f : kernels) {
            auto evaled = f->operator()(tmpX);
            coefficients.push_back(std::real(f->getCoefficient()));
            gradientInstructions.push_back(evaled);
          }
        }

        // the number of instructions for a given element of x is the same
        // regardless of the parameter sign, so we need only one of this
        if (sign == 1.0) {
          nInstructionsElement.push_back(kernels.size());
        }
      }
    }

    return gradientInstructions;
  }

  // Compute gradients from executed instructions
  void
  compute(std::vector<double> &dx,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {

    // if commutator is provided
    int shift = 0, start = 0;
    if (commutator) {
      double gradElement = 0.0;
      for (int instElement = 0; instElement < nInstructionsElement[0];
           instElement++) {
        auto expval = std::real(results[instElement]->getExpectationValueZ());
        gradElement += expval * coefficients[instElement];
      }
      dx[0] = -gradElement / 2.0;

      // shift the indices of the loop below accordingly
      shift = nInstructionsElement[0];
      start = 1;
    }

    // loop over the remaining number of entries in the gradient vector
    for (int gradTerm = start; gradTerm < dx.size(); gradTerm++) {

      double plusGradElement = 0.0;  // <+>
      double minusGradElement = 0.0; // <->

      // loop over instructions for a given term, compute <+> and <->
      for (int instElement = 0; instElement < nInstructionsElement[gradTerm];
           instElement++) {

        auto plus_expval =
            std::real(results[instElement + shift]->getExpectationValueZ());
        auto minus_expval = std::real(
            results[instElement + nInstructionsElement[gradTerm] + shift]
                ->getExpectationValueZ());
        plusGradElement += plus_expval * coefficients[instElement + shift];
        minusGradElement +=
            minus_expval *
            coefficients[instElement + nInstructionsElement[gradTerm] + shift];
      }

      // gradient is (<+> - <->)/2
      dx[gradTerm] = std::real(plusGradElement - minusGradElement) / 2.0;
      shift += 2 * nInstructionsElement[gradTerm];
    }

    coefficients.clear();
    nInstructionsElement.clear();
    std::stringstream ss;
    ss << std::setprecision(5) << "Computed gradient: ";
    for (auto param : dx) {
      ss << param << " ";
    }
    xacc::info(ss.str());
    ss.str(std::string());

    return;
  }

  const std::string name() const override { return "parameter-shift"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif