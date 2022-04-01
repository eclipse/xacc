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
#ifndef XACC_CENTRAL_DIFFERENCE_GRADIENT_HPP_
#define XACC_CENTRAL_DIFFERENCE_GRADIENT_HPP_

#include "CompositeInstruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include <iomanip>
#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {

class CentralDifferenceGradient : public AlgorithmGradientStrategy {

protected:
  std::shared_ptr<Observable> obs; // Hamiltonian (or any) observable
  double step = 1.0e-7;            // step size
  double obsExpValue;              // <H> expectation value of the observable
  std::function<std::shared_ptr<CompositeInstruction>(std::vector<double>)>
      kernel_evaluator;

public:
  // Set this to true to get energy, but see comment on passObsExpValue below
  bool isNumerical() const override { return true; }

  // We won't use it for now, but this may come in handy if we need the Hessian
  void setFunctionValue(const double expValue) override {
    obsExpValue = expValue;
    return;
  }

  bool initialize(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")) {
      xacc::error("Gradient strategy needs observable");
      return false;
    }

    obs = parameters.get<std::shared_ptr<Observable>>("observable");

    // Default value
    step = 1.0e-7; 
    // Change step size if need be
    if (parameters.keyExists<double>("step")) {
      step = parameters.get<double>("step");
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

  // Get the circuit instructions necessary to compute gradients
  // This is very similar to ParameterShift
  std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit,
                        const std::vector<double> &x) override {
    // Check if the composite contains measure gates
    const auto containMeasureGates =
        [](std::shared_ptr<CompositeInstruction> f) -> bool {
      InstructionIterator it(f);
      while (it.hasNext()) {
        auto nextInst = it.next();
        if (nextInst->name() == "Measure") {
          return true;
        }
      }
      return false;
    };
    std::vector<std::shared_ptr<CompositeInstruction>> gradientInstructions;
    for (int op = 0; op < x.size(); op++) { // loop over operators
      for (double sign : {1.0, -1.0}) {     // change sign

        // shift the parameter by step and observe
        auto tmpX = x;
        tmpX[op] += sign * step;

        std::vector<std::shared_ptr<CompositeInstruction>> kernels;
        if (kernel_evaluator) {
          auto evaled_base = kernel_evaluator(tmpX);
          kernels = obs->observe(evaled_base);
          for (auto &f : kernels) {
            if (containMeasureGates(f)) {
              coefficients.push_back(std::real(f->getCoefficient()));
              gradientInstructions.push_back(f);
            }
          }
        } else {
          auto evaled = circuit->operator()(tmpX);
          kernels = obs->observe(evaled);

          // loop over circuit instructions
          // and gather coefficients/instructions
          for (auto &f : kernels) {
            if (containMeasureGates(f)) {
              
              coefficients.push_back(std::real(f->getCoefficient()));
              gradientInstructions.push_back(f);
            }
          }
        }
        // the number of instructions for a given element of x is the same
        // regardless of the parameter sign, so we need only one of this
        if (sign == 1.0) {
          const auto numberGradKernels = std::count_if(
              kernels.begin(), kernels.end(), [&containMeasureGates](auto &f) {
                return containMeasureGates(f);
              });
          nInstructionsElement.push_back(numberGradKernels);
        }
      }
    }

    return gradientInstructions;
  }

  // Compute gradients from executed instructions
  void
  compute(std::vector<double> &dx,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {

    int shift = 0;
    // loop over the terms in the gradient vector
    for (int gradTerm = 0; gradTerm < dx.size(); gradTerm++) {

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

      // gradient is (<+> - <->) / 2 *step
      dx[gradTerm] =
          std::real(plusGradElement - minusGradElement) / (2.0 * step);
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

  const std::string name() const override {
    return "central";
  }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif