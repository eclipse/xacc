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

using namespace xacc;

namespace xacc {
namespace algorithm {

class ParameterShiftGradient : public AlgorithmGradientStrategy {

protected:

  std::shared_ptr<Observable> H, commutator; // Hamiltonian (or any) observable
  double factor = 0.5;

public:

  // passes the Hamiltonian and current ansatz operators to the gradient class
  bool optionalParameters(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<std::shared_ptr<Observable>>("observable")){
      std::cout << "Parameter shift gradient requires observable.\n"; 
      return false;
    }

    // this is specific to ADAPT-VQE
    if (parameters.keyExists<std::shared_ptr<Observable>>("commutator")){
      commutator = parameters.get<std::shared_ptr<Observable>>("commutator");
    }

    // this is needed because JW already adds a 0.5 factor
    if (parameters.keyExists<bool>("jw")){
      if(parameters.get<bool>("jw")){
        factor = 1.0;
      }
    }

    H = parameters.get<std::shared_ptr<Observable>>("observable");

    return true;

  }

  std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit, const std::vector<double> &x) override {

    std::cout << "Input parameters: \n"; 
    for(auto param : x){
      std::cout << param <<" ";
    }
    std::cout << "\n";

    std::vector<std::shared_ptr<CompositeInstruction>> gradientInstructions;

    // The gradient of the operator added in the current ADAPT-VQE cycle
    // is simply its commutator with the Hamiltonian. This improves convergence
    // over parameter shift
    int start = 0;
    if(commutator){
      auto kernels = commutator->observe(circuit);

      for (auto &f : kernels) {
        auto evaled = f->operator()(x);
        coefficients.push_back(std::real(f->getCoefficient()));
        gradientInstructions.push_back(evaled);
      }
      nInstructionsElement.push_back(kernels.size());
      start = 1;
    }

    for (int op = start; op < x.size(); op++){// loop over the remainder of operators
      for (double sign : {1.0, -1.0}){ // change sign 

        // parameter shift and observe
        auto tmpX = x;
        tmpX[op] += sign * xacc::constants::pi * factor;
        auto kernels = H->observe(circuit);

        // loop over parameter-shifted circuit instructions
        // and gather coefficients/instructions
        for (auto &f : kernels) {
          auto evaled = f->operator()(tmpX);
          coefficients.push_back(std::real(f->getCoefficient()));
          gradientInstructions.push_back(evaled);
        }

        // the number of instructions for a given element of x is the same
        // regardless of the parameter sign, so we need only one of this
        if(sign == 1.0){
          nInstructionsElement.push_back(kernels.size());
        }

      }
    }

    return gradientInstructions;

  }

  void compute(std::vector<double> &dx, std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {

    // if commutator is provided
    int shift = 0, start = 0;
    if(commutator){
      double gradElement = 0.0;
      for (int instElement = 0; instElement < nInstructionsElement[0]; instElement++){
        auto expval = std::real(results[instElement]->getExpectationValueZ());
        gradElement += expval * coefficients[instElement];
      }
      dx[0] = -gradElement / 2.0;

      // shift the indices of the loop below accordingly
      shift = nInstructionsElement[0];
      start = 1;
    }


    // loop over the remaining number of entries in the gradient vector
    for (int gradTerm = start; gradTerm < dx.size(); gradTerm++){ 

      double plusGradElement = 0.0; // <+>
      double minusGradElement = 0.0; // <->

      // loop over instructions for a given term, compute <+> and <->
      for (int instElement = 0; instElement < nInstructionsElement[gradTerm]; instElement++) {

        auto plus_expval = std::real(results[instElement + shift]->getExpectationValueZ());
        auto minus_expval = std::real(results[instElement + nInstructionsElement[gradTerm] + shift]->getExpectationValueZ());
        plusGradElement += plus_expval * coefficients[instElement + shift];
        minusGradElement += minus_expval * coefficients[instElement + nInstructionsElement[gradTerm] + shift];

      }

      // gradient is (<+> - <->)/2
      dx[gradTerm] = -std::real(plusGradElement - minusGradElement) / 2.0;
      shift += 2 * nInstructionsElement[gradTerm];
    }

    coefficients.clear();
    nInstructionsElement.clear();

    std::cout << "Computed gradient: \n"; 
    for(auto param : dx){
      std::cout << param <<" ";
    }
    std::cout << "\n\n";
    return;
  }

  const std::string name() const override { return "parameter-shift-gradient"; }
  const std::string description() const override { return ""; }

};


}
}

#endif