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
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include <iomanip>

using namespace xacc;

namespace xacc {
namespace algorithm {

class ParameterShiftGradient : public AlgorithmGradientStrategy {

protected:
  std::shared_ptr<Observable> obs; // Hamiltonian (or any) observable
  std::function<std::shared_ptr<CompositeInstruction>(std::vector<double>)>
      kernel_evaluator;
  double shiftScalar = 0.5;
  std::vector<double> shiftConstants;

public:
  bool initialize(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")) {
      xacc::error("Gradient strategy needs observable");
      return false;
    }
    obs = parameters.get<std::shared_ptr<Observable>>("observable");

    if (parameters.keyExists<std::function<
            std::shared_ptr<CompositeInstruction>(std::vector<double>)>>(
            "kernel-evaluator")) {
      kernel_evaluator =
          parameters.get<std::function<std::shared_ptr<CompositeInstruction>(
              std::vector<double>)>>("kernel-evaluator");
    }
    // Default shiftScalar (this is not clonable, hence need to be
    // reinitialized)
    shiftScalar = 0.5;
    if (parameters.keyExists<double>("shift-scalar")) {
      shiftScalar = parameters.get<double>("shift-scalar");
    }

    return true;
  }

  // Not numerical
  bool isNumerical() const override { return false; }

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

    // get all occurences of a parameter in the circuit
    // and the corresponding coefficients
    std::map<int, std::vector<std::pair<int, double>>> paramIdx;
    auto circInsts = circuit->getInstructions();
    auto vars = circuit->getVariables();
    // parse parameters to circuit before shifts
    auto evaled = circuit->operator()(x);
    auto evaledInsts = evaled->getInstructions();
    for (int i = 0; i < circuit->nInstructions(); i++) {
      auto circInst = circInsts[i];
      if (circInst->isParameterized() &&
          circInst->getParameter(0).isVariable()) {
        auto param = circInst->getParameter(0).as<std::string>();

        // get coefficient
        double coeff = 1.0;
        std::string var = param;
        if (param.find('*') != std::string::npos) {
          coeff = std::stod(param.substr(0, param.find('*') - 1));
          var = param.substr(param.find('*') + 1);
        }

        auto paramValue = evaledInsts[i]->getParameter(0).as<double>() / coeff;
        for (int idx = 0; idx < x.size(); idx++) {
          if (std::fabs(x[idx] - paramValue) < 1.0e-4) {
            paramIdx[idx].push_back({i, coeff});
            break;
          }
        }
      }
    }

    std::vector<std::shared_ptr<CompositeInstruction>> gradientInstructions;
    // Note: for the purpose of parameter-shift gradient calculation, the
    // identity term has no effect, i.e., its contributions to the plus and
    // minus sides will cancel out.
    // loop over parameters
    for (int param = 0; param < x.size(); param++) {

      // get indices for the current parameter
      auto idxs = paramIdx[param];

      // parameter shift sign
      for (double sign : {1.0, -1.0}) {

        // get instructions for shifted parameter
        std::vector<std::shared_ptr<CompositeInstruction>> kernels;
        if (kernel_evaluator) {
          // parameter shift
          // we move this here because only the kernel evaluator will need it
          auto tmpX = x;
          tmpX[param] += sign * xacc::constants::pi * shiftScalar;
          auto evaled_base = kernel_evaluator(tmpX);
          kernels = obs->observe(evaled_base);
          for (auto &f : kernels) {
            if (containMeasureGates(f)) {
              coefficients.push_back(std::real(f->getCoefficient()));
              gradientInstructions.push_back(f);
            }
          }
        } else {

          // clone the circuit after operator()()
          // then loop over the occurences of the parameter and replace
          // the rotation gate by the same gate with the shifted angle
          auto evaledClone =
              std::dynamic_pointer_cast<CompositeInstruction>(evaled->clone());
          for (auto idx : idxs) {
            shiftConstants.push_back(idx.second);
            auto gate = evaledClone->getInstructions()[idx.first]->clone();
            auto shifted = gate->getParameter(0).as<double>() +
                           sign * xacc::constants::pi * shiftScalar;
            gate->setParameter(0, shifted);
            evaledClone->replaceInstruction(idx.first, gate);
          }
          kernels = obs->observe(evaledClone);

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
        // regardless of the parameter sign, so we need only one of this.
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
    // loop over entries in the gradient vector
    for (int gradTerm = 0; gradTerm < dx.size(); gradTerm++) {

      double plusGradElement = 0.0, minusGradElement = 0.0;

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

      // gradient is (<+> - <->) / 2

      dx[gradTerm] =
          (plusGradElement - minusGradElement) * shiftConstants[gradTerm] / 2.0;
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

  std::vector<double>
  compute(const int n,
          std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {

    std::vector<double> dx(n);
    compute(dx, results);
    return dx;
  }

  const std::string name() const override { return "parameter-shift"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif