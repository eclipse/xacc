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
#include "mc-vqe.hpp"

#include "Observable.hpp"
#include "xacc.hpp"

#include <memory>
#include <iomanip>
#include <vector>

using namespace xacc;

namespace xacc {
namespace algorithm {
bool MC_VQE::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.pointerLikeExists<Observable>("observable")) {
    std::cout << "Obs was false\n";
    return false;
  }  else if (!parameters.pointerLikeExists<Accelerator>(
                 "accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  } else if(!parameters.stringExists("nChromophores")){
    std::cout << "Missing number of chromophores\n";
    return false;
  } else if(!parameters.stringExists("angles")){
    //else if(!parameters.pointerLikeExists<std::vector<std::vector<double>>>("angles")){
    std::cout << "Missing gate angles for CIS state preparation\n";
  }

  observable = parameters.getPointerLike<Observable>("observable");
  optimizer = parameters.getPointerLike<Optimizer>("optimizer");
  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  angles = parameters.get< std::vector< std::vector<double> > >("angles");
  nChromophores = parameters.get<int>("nChromophores");

  // This is to include the last entangler if the system is cyclic
  if (parameters.stringExists("cyclic")){
    isCyclic = parameters.get<bool>("cyclic");
  } else {
    isCyclic = false;
  }

  return true;
}

const std::vector<std::string> MC_VQE::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "angles", "nChromophores"};
}

void MC_VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.

  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {


        double averageEnergy = 0.0;
        for (int state = 0; state < nChromophores + 1; state++){

          std::vector<double> coefficients;
          std::vector<std::string> kernelNames;
          std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

          auto kernel = circuit(nChromophores, angles[state]);
          auto kernels = observable->observe(kernel);

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
              auto evaled = f->operator()(x);
              fsToExec.push_back(evaled);
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

            for (int i = 0; i < buffers.size(); i++) {
              auto expval = buffers[i]->getExpectationValueZ();
              energy += expval * coefficients[i];
              buffers[i]->addExtraInfo("coefficient", coefficients[i]);
              buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
              buffers[i]->addExtraInfo("exp-val-z", expval);
              buffers[i]->addExtraInfo("parameters", x);
              buffer->appendChild(fsToExec[i]->name(), buffers[i]);
            }
          
          averageEnergy += energy;
        }

        averageEnergy /= nChromophores;
        std::stringstream ss;
        ss << "E(" << ( !x.empty() ? std::to_string(x[0]) : "");
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << averageEnergy;
        xacc::info(ss.str());
        return averageEnergy;


      },
      6*nChromophores);

  auto result = optimizer->optimize(f);

  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  return;
}

std::vector<double>
MC_VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
             const std::vector<double> &x) {

  double averageEnergy = 0.0;
  for (int state = 0; state < nChromophores + 1; state++){

    std::vector<double> coefficients;
    std::vector<std::string> kernelNames;
    std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

    auto kernel = circuit(nChromophores, angles[state]);
    auto kernels = observable->observe(kernel);

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
        auto evaled = f->operator()(x);
        fsToExec.push_back(evaled);
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

    for (int i = 0; i < buffers.size(); i++) {
      auto expval = buffers[i]->getExpectationValueZ();
      energy += expval * coefficients[i];
      buffers[i]->addExtraInfo("coefficient", coefficients[i]);
      buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
      buffers[i]->addExtraInfo("exp-val-z", expval);
      buffers[i]->addExtraInfo("parameters", x);
      buffer->appendChild(fsToExec[i]->name(), buffers[i]);
    }

    averageEnergy += energy;

  }

  averageEnergy /= nChromophores;  

  std::stringstream ss;

  ss << "E(" << ( !x.empty() ? std::to_string(x[0]) : "");
  for (int i = 1; i < x.size(); i++)
    ss << "," << x[i];
  ss << ") = " << std::setprecision(12) << averageEnergy;
  xacc::info(ss.str());
  return {averageEnergy};
}


std::shared_ptr<CompositeInstruction>
MC_VQE::circuit(const int &N, const std::vector<double> &angles) const {

  auto mcvqeRegistry = xacc::getIRProvider("quantum");
  auto mcvqeInstructions = mcvqeRegistry->createComposite("mcvqeCircuit");

  // Beginning of CIS state preparation

  // Ry "pump"
  auto ry = mcvqeRegistry->createInstruction("Ry", std::vector<std::size_t>{0}, {angles[0]});
  mcvqeInstructions->addInstruction(ry);

  // Fy gates = Ry(-theta/2)-CZ-Ry(theta/2)
  // |A>-----------------------o-----------------------
  //                           |            
  // |B>--[Ry(-angles[i]/2)]--[Z]--[Ry(+angles[i]/2)]--
  //
  for (int i = 1; i < N; i++){

    std::size_t control = i - 1, target = i;
    // Ry(-theta/2)
    auto ry_minus = mcvqeRegistry->createInstruction("Ry", {target}, {-angles[i]/2});
    mcvqeInstructions->addInstruction(ry_minus);

    //CNOT
    auto cz = mcvqeRegistry->createInstruction("Z", {control, target});
    mcvqeInstructions->addInstruction(cz);

    // Ry(+theta/2)
    auto ry_plus = mcvqeRegistry->createInstruction("Ry", {target}, {angles[i]/2});
    mcvqeInstructions->addInstruction(ry_plus);
  }

  // Wall of CNOTs
  for (int i = N - 2; i >= 0; i--){
    for(int j = N - 1; j > i; j--){

      std::size_t control = i, target = j;
      auto cnot = mcvqeRegistry->createInstruction("CNOT", {control, target});
      mcvqeInstructions->addInstruction(cnot);

    }
  }
  // end of CIS state preparation
  
  // Beginning of entangler
  //
  // |A>--[Ry(x0)]--o--[Ry(x2)]--o--[Ry(x4)]-
  //                |            |
  // |B>--[Ry(x1)]--x--[Ry(x3)]--x--[Ry(x5)]-
  //
  // first layer
  int paramCounter = 0;
  std::string paramLetter = "x";
  for (int i = 0; i <= N ; i += 2){

    std::size_t control = i, target = i + 1;

    auto ry1 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter))});
    mcvqeInstructions->addInstruction(ry1);

    auto ry2 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 1))});
    mcvqeInstructions->addInstruction(ry2);
    
    auto cnot1 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot1);

    auto ry3 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 2))});
    mcvqeInstructions->addInstruction(ry3);

    auto ry4 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 3))});
    mcvqeInstructions->addInstruction(ry4); 

    auto cnot2 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot2);

    auto ry5 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 4))});
    mcvqeInstructions->addInstruction(ry5);

    auto ry6 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 5))});
    mcvqeInstructions->addInstruction(ry6); 

    paramCounter += 6;

  }

  // second layer
  for (int i = 1; i <= N ; i += 2){

    std::size_t control = i, target = i + 1;

    auto ry1 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter))});
    mcvqeInstructions->addInstruction(ry1);

    auto ry2 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 1))});
    mcvqeInstructions->addInstruction(ry2);
    
    auto cnot1 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot1);

    auto ry3 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 2))});
    mcvqeInstructions->addInstruction(ry3);

    auto ry4 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 3))});
    mcvqeInstructions->addInstruction(ry4); 

    auto cnot2 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot2);

    auto ry5 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 4))});
    mcvqeInstructions->addInstruction(ry5);

    auto ry6 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 5))});
    mcvqeInstructions->addInstruction(ry6); 

    paramCounter += 6;

  }

  // if the molecular system is cyclic, need to add this last entangler
  if(isCyclic){

    std::size_t control = N - 1, target = 0;

    auto ry1 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter))});
    mcvqeInstructions->addInstruction(ry1);

    auto ry2 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 1))});
    mcvqeInstructions->addInstruction(ry2);
    
    auto cnot1 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot1);

    auto ry3 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 2))});
    mcvqeInstructions->addInstruction(ry3);

    auto ry4 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 3))});
    mcvqeInstructions->addInstruction(ry4); 

    auto cnot2 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot2);

    auto ry5 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 4))});
    mcvqeInstructions->addInstruction(ry5);

    auto ry6 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 5))});
    mcvqeInstructions->addInstruction(ry6); 

  }

}

} // namespace algorithm
} // namespace xacc