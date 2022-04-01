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
#ifndef XACC_FORWARD_DIFFERENCE_GRADIENT_HPP_
#define XACC_FORWARD_DIFFERENCE_GRADIENT_HPP_

#include "CompositeInstruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include <iomanip>
#include "InstructionIterator.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {

class ForwardDifferenceGradient : public AlgorithmGradientStrategy {

protected:
  std::shared_ptr<Observable> obs; // Hamiltonian (or any) observable
  double step = 1.0e-7;            // step size
  double obsExpValue;              // <H> expectation value of the observable
  std::function<std::shared_ptr<CompositeInstruction>(std::vector<double>)>
      kernel_evaluator;

public:
  // this is a numerical gradient
  bool isNumerical() const override { return true; }

  // Pass the expectation value of the observable
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

    // Default step size
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
    // std::stringstream ss;
    // ss << std::setprecision(5) << "Input parameters: ";
    // for (auto param : x) {
    //   ss << param << " ";
    // }
    // xacc::info(ss.str());
    // ss.str(std::string());

    std::vector<std::shared_ptr<CompositeInstruction>> gradientInstructions;
    for (int op = 0; op < x.size(); op++) { // loop over operators

      // shift the parameter by step and observe
      auto tmpX = x;
      tmpX[op] += step;
      //   auto kernels = obs->observe(circuit);

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

      const auto numberGradKernels = std::count_if(
          kernels.begin(), kernels.end(),
          [&containMeasureGates](auto &f) { return containMeasureGates(f); });
      nInstructionsElement.push_back(numberGradKernels);
    }

    return gradientInstructions;
  }

  // Compute gradients from executed instructions
  void
  compute(std::vector<double> &dx,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {
    int shift = 0;
    // loop over the remaining number of entries in the gradient vector
    for (int gradTerm = 0; gradTerm < dx.size(); gradTerm++) {

      auto identityTerm = obs->getIdentitySubTerm();
      double gradElement =
          identityTerm ? identityTerm->coefficient().real() : 0.0;

      // loop over instructions for a given term, compute <+> and <->
      for (int instElement = 0; instElement < nInstructionsElement[gradTerm];
           instElement++) {

        auto expval =
            std::real(results[instElement + shift]->getExpectationValueZ());
        gradElement += expval * coefficients[instElement + shift];
      }

      // gradient is (<+> - <->)/2
      dx[gradTerm] = std::real(gradElement - obsExpValue) / step;
      shift += nInstructionsElement[gradTerm];
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

  const std::string name() const override { return "forward"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif