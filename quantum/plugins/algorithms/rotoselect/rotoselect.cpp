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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include "rotoselect.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>
#include <cassert>
#include <cmath>

using namespace xacc;

namespace {
  enum class PauliType { X = 0, Y = 1, Z = 2 };

  std::vector<double> initializeThetaVec(int in_nbRotationGates)
  {
    std::random_device rndDevice;
    std::mt19937 rndEngine {rndDevice()}; 
    // Theta angle is in (-pi, pi] range 
    std::uniform_real_distribution<double> dist {-M_PI, M_PI};

    auto gen = [&dist, &rndEngine]() {
      return dist(rndEngine);
    };

    std::vector<double> vec(in_nbRotationGates);
    std::generate(vec.begin(), vec.end(), gen);
    return vec;
  }

  std::vector<PauliType> initializePauliTypeVec(int in_nbRotationGates)
  {
    std::random_device rndDevice;
    std::mt19937 rndEngine {rndDevice()}; 
    // Pauli type is X, Y, Z (0->2) 
    std::uniform_int_distribution<int> dist {0, 2};

    auto gen = [&dist, &rndEngine]() -> PauliType {
      const auto randVal = dist(rndEngine);
      return static_cast<PauliType>(randVal);
    };

    std::vector<PauliType> vec(in_nbRotationGates);
    std::generate(vec.begin(), vec.end(), gen);
    return vec;
  }
}


namespace xacc {
namespace algorithm {

bool Rotoselect::initialize(const HeterogeneousMap& in_parameters) 
{
  bool validationResult = true;
  if (!in_parameters.keyExists<std::shared_ptr<Observable>>("observable")) 
  {
    std::cout << "Observable is required. Please provide 'observable' parameter.\n";
    validationResult = false;
  }
  
  if (!in_parameters.keyExists<std::shared_ptr<Accelerator>>("accelerator")) 
  {
    std::cout << "Accelerator is required. Please provide 'accelerator' parameter.\n";
    validationResult = false;
  }


  if (!in_parameters.keyExists<int>("iterations")) 
  {
    std::cout << "Please specify the number of iterations in the 'iterations' parameter.\n";
    validationResult = false;
  }
  else
  {
    const int nbIterations = in_parameters.get<int>("iterations");
    if (nbIterations < 1)
    {
      std::cout << "Invalid 'iterations' parameter. This must be a positive number.\n";
      validationResult = false;
    }
  }  

  if (!in_parameters.keyExists<int>("layers")) 
  {
    std::cout << "Please specify the number of circuit layers in the 'layers' parameter.\n";
    validationResult = false;
  }
  else
  {
    const int nbLayers = in_parameters.get<int>("layers");
    if (nbLayers < 1)
    {
      std::cout << "Invalid 'layers' parameter. This must be a positive number.\n";
      validationResult = false;
    }
  }  

  if (!validationResult)
  {
    xacc::error("Input parameter validation failed.\n");
    return false;
  }

  m_parameters = in_parameters;
  return true;
}

const std::vector<std::string> Rotoselect::requiredParameters() const 
{
  return { "observable", "accelerator", "layers", "iterations" };
}

void Rotoselect::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const 
{
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto accelerator = m_parameters.getPointerLike<Accelerator>("accelerator");

  const auto nbQubits = buffer->size();
  const auto nbLayers = m_parameters.get<int>("layers");
  // D params
  const auto nbRotationGates = nbQubits * nbLayers;
  // Tracking of all rotation gates (angle and type) 
  std::vector<double> thetaVec = initializeThetaVec(nbRotationGates);
  std::vector<PauliType> pauliTypeVec = initializePauliTypeVec(nbRotationGates);
  std::unordered_map<size_t, size_t> pauliGateIdxToCircuitIdx;
  
  auto rotoselectKernel = gateRegistry->createComposite("rotoselectKernel");
  
  const auto createPauliGate = [&gateRegistry](size_t in_qubitIdx, PauliType in_pauliType, double in_angle) {
    switch (in_pauliType)
    {
      case PauliType::X:
      {
        return gateRegistry->createInstruction("Rx", { in_qubitIdx}, { in_angle });
      }         
      case PauliType::Y:
      {
        return gateRegistry->createInstruction("Ry", { in_qubitIdx }, { in_angle });
      }
      case PauliType::Z:
      {
        return gateRegistry->createInstruction("Rz", { in_qubitIdx }, { in_angle });
      }         
    };
  };  
  
  // Step 1: initialize the quantum circuit
  for (size_t i = 0; i < nbLayers; ++i)
  {
    // Pauli rotation layer:
    for (size_t j = 0; j < nbQubits; ++j)
    {
      const auto pauliType = pauliTypeVec[i*nbQubits + j];
      const auto angle = thetaVec[i*nbQubits + j];
      rotoselectKernel->addInstruction(createPauliGate(j, pauliType, angle));
      const auto circuitGateIdx = rotoselectKernel->nInstructions() - 1;
      // Track the mapping between Pauli rotation gate and absolute index within the circuit
      // Note: we have layers of CZ gates in between rotation layers.
      pauliGateIdxToCircuitIdx.emplace(i*nbQubits + j, circuitGateIdx);
    }

    // CZ layer
    for (size_t j = 0; j < nbQubits; j += 2)
    {
      if (j + 1 < nbQubits)
      {
        auto cz = gateRegistry->createInstruction("CZ", { j, j + 1 });
        rotoselectKernel->addInstruction(cz);
      }
    }
  }
  
  if (xacc::verbose)
  {
    // Debug:
    std::cout << "Circuit: \n" <<  rotoselectKernel->toString();
  }
  
  const auto observable = m_parameters.getPointerLike<Observable>("observable");
  // Step 2: Optimization loop
  bool stopCriteriaMet = false;
  // Currently, we only use trial count as the stopping criteria.
  // We can implement other criteria, such as convergence rate, etc.
  const int nbIterations = m_parameters.get<int>("iterations");
  int iterationCount = 0;
  
  // Optimization note:
  // This evaluation function is the main runner of the training loop (7 evaluations per training step/iteration).
  // We can run all evaluations per step in parallel. In order to do so, we need:
  // (1) Clone the rotoselectKernel for each thread, i.e. each thread has an independent copy (because it will swap a gate for evaluation).
  // (2) Clone the accelerator (simulator) which will run the circuit to evaluate the energy for this config.
  // This will give us good speedup in the case that the simulator is not utilize all the CPU resources (i.e. if the simulator is not multithreaded by itself).
  const auto evaluateFn = [&](size_t in_pauliGateIdx, PauliType in_pauliType, double in_angle) -> double {
    // Find the gate:
    auto iter = pauliGateIdxToCircuitIdx.find(in_pauliGateIdx);
    assert(iter != pauliGateIdxToCircuitIdx.end());

    const auto currentGate = rotoselectKernel->getInstruction(iter->second);
    assert(currentGate->nRequiredBits() == 1);
    const auto qubitIdx = currentGate->bits()[0];
    auto newGate = createPauliGate(qubitIdx, in_pauliType, in_angle);
    rotoselectKernel->replaceInstruction(iter->second, newGate);
    // Calculate the observable
    auto kernels = observable->observe(rotoselectKernel);
    double energy = 0.0;

    {
      std::vector<double> coefficients;
      std::vector<std::string> kernelNames;
      std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

      double identityCoeff = 0.0;
      for (auto &f : kernels) 
      {
        kernelNames.push_back(f->name());
        std::complex<double> coeff = f->getCoefficient();
        int nFunctionInstructions = 0;
        if (f->getInstruction(0)->isComposite()) 
        {
          nFunctionInstructions = rotoselectKernel->nInstructions() + f->nInstructions() - 1;
        } 
        else 
        {
          nFunctionInstructions = f->nInstructions();
        }

        if (nFunctionInstructions > rotoselectKernel->nInstructions()) 
        {
          fsToExec.push_back(f);
          coefficients.push_back(std::real(coeff));
        } 
        else 
        {
          identityCoeff += std::real(coeff);
        }
      }

      auto tmpBuffer = xacc::qalloc(buffer->size());
      accelerator->execute(tmpBuffer, fsToExec);
      auto buffers = tmpBuffer->getChildren();

      energy = identityCoeff;
      auto idBuffer = xacc::qalloc(buffer->size());
      idBuffer->addExtraInfo("coefficient", identityCoeff);
      idBuffer->setName("I");
      idBuffer->addExtraInfo("kernel", "I");
      idBuffer->addExtraInfo("exp-val-z", 1.0);
      buffer->appendChild("I", idBuffer);

      for (int i = 0; i < buffers.size(); i++) 
      {
        auto expval = buffers[i]->getExpectationValueZ();
        energy += expval * coefficients[i];
        buffers[i]->addExtraInfo("coefficient", coefficients[i]);
        buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
        buffers[i]->addExtraInfo("exp-val-z", expval);
        buffer->appendChild(fsToExec[i]->name(), buffers[i]);
      } 

    }

    // Return back the original gate:
    rotoselectKernel->replaceInstruction(iter->second, currentGate);
    return energy;
  };
  
  // We keep track of all the min energy value achieved at each iteration.
  // This can be used to determine stopping conditions (in addition to just iteration loop count)
  std::vector<double> minEnergyVec;
  minEnergyVec.reserve(nbIterations);
  // The training result (i.e. the min energy that is encountered so far)
  double trainingResult = std::numeric_limits<double>::max();
  while (!stopCriteriaMet)
  {
    for (size_t i = 0; i < nbLayers; ++i)
    {
      for (size_t j = 0; j < nbQubits; ++j)
      {
        const auto pauliGateIdx =  i*nbQubits + j;
        // We need 7 params to evaluate optimal params for this rotation gate
        const double M_X_PI_2 = evaluateFn(pauliGateIdx, PauliType::X, M_PI_2);
        const double M_X_M_PI_2 = evaluateFn(pauliGateIdx, PauliType::X, -M_PI_2);
        const double M_Y_PI_2 = evaluateFn(pauliGateIdx, PauliType::Y, M_PI_2);
        const double M_Y_M_PI_2 = evaluateFn(pauliGateIdx, PauliType::Y, -M_PI_2);
        const double M_Z_PI_2 = evaluateFn(pauliGateIdx, PauliType::Z, M_PI_2);
        const double M_Z_M_PI_2 = evaluateFn(pauliGateIdx, PauliType::Z, -M_PI_2);
        const double M_ZERO = evaluateFn(pauliGateIdx, PauliType::X, 0.0);

        // Compute the minimum for each Pauli type
        const auto minCalc = [](double in_m0, double in_mPiOver2, double in_mMinusPiOver2){
          double result = -M_PI_2 - atan2(2*in_m0 - in_mPiOver2 - in_mMinusPiOver2, in_mPiOver2 - in_mMinusPiOver2);
          // Normalize to the [-pi,+pi] range
          if (result < -M_PI)
          {
            result += 2*M_PI;
          }  
          assert((result >= -M_PI && result <= M_PI));
          return result;
        };

        const double min_Theta_X = minCalc(M_ZERO, M_X_PI_2, M_X_M_PI_2);
        const double min_Theta_Y = minCalc(M_ZERO, M_Y_PI_2, M_Y_M_PI_2);
        const double min_Theta_Z = minCalc(M_ZERO, M_Z_PI_2, M_Z_M_PI_2);
        
        // Which Pauli give us the minimum?
        const double min_X = evaluateFn(pauliGateIdx, PauliType::X, min_Theta_X);
        const double min_Y = evaluateFn(pauliGateIdx, PauliType::Y, min_Theta_Y);
        const double min_Z = evaluateFn(pauliGateIdx, PauliType::Z, min_Theta_Z);
        double minEnergy = min_X;
        PauliType minPauli = PauliType::X;
        double minAngle = min_Theta_X;
        if (min_Y < minEnergy)
        {
          minEnergy = min_Y;
          minPauli = PauliType::Y;
          minAngle = min_Theta_Y;
        }

        if (min_Z < minEnergy)
        {
          minEnergy = min_Z;
          minPauli = PauliType::Z;
          minAngle = min_Theta_Z;
        }

        // Update the rotation gate:
        ++iterationCount;
        auto iter = pauliGateIdxToCircuitIdx.find(pauliGateIdx);
        assert(iter != pauliGateIdxToCircuitIdx.end());
        const auto currentGate = rotoselectKernel->getInstruction(iter->second);
        assert(currentGate->nRequiredBits() == 1);
        const auto qubitIdx = currentGate->bits()[0];
        auto newGate = createPauliGate(qubitIdx, minPauli, minAngle);
        rotoselectKernel->replaceInstruction(iter->second, newGate);
        
        if (xacc::verbose)
        {
          // Debug:
          std::cout << "Replace " << currentGate->name() << "(" << currentGate->getParameter(0).as<double>() << ")" << " with " << newGate->name() << "(" << minAngle << ")" << " @ q[" << qubitIdx << "] \n";
          std::cout << "Min Energy: " << minEnergy << "\n";
          std::cout << "New circuit: \n" <<  rotoselectKernel->toString();
        }
        
        minEnergyVec.emplace_back(minEnergy);
        if (minEnergy < trainingResult)
        {
          // Update the new result
          trainingResult = minEnergy;
        }
        // Training logging:
        std::cout << "Rotoselect (iteration = " << iterationCount << "): Objective function value = " << trainingResult << "\n";
      }
    }

    // For now, just use loop count as the stopping condition
    // We can implement more intelligent stopping condition based on data in minEnergyVec 
    // (i.e. the history of all min energy values attained in all previous iterations)
    stopCriteriaMet = iterationCount >= nbIterations;
  }  

  // Done, add info to the buffer
  buffer->addExtraInfo("opt-val", trainingResult);
}

} // namespace algorithm
} // namespace xacc