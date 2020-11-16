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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "qaoa.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "CompositeInstruction.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <cassert>
#include <iomanip>

namespace xacc {
namespace algorithm {
Observable *QAOA::constructMaxCutHam(xacc::Graph *in_graph) const {
  if (xacc::verbose) {
    std::cout << "Graph:\n";
    in_graph->write(std::cout);
  }
  std::stringstream pauliStr;
  // Construct the MAX-CUT Hamiltonian based on graph edges
  for (int i = 0; i < in_graph->order(); ++i) {
    auto neighbors = in_graph->getNeighborList(i);
    for (const auto &neighborId : neighbors) {
      pauliStr << "1.0 Z" << i << "Z" << neighborId << " + ";
    }
  }

  std::string hamStr = pauliStr.str();
  if (!hamStr.empty()) {
    // Remove the trailing + sign
    hamStr.resize(hamStr.size() - 3);
  }

  xacc::info("Graph Hamiltonian: " + hamStr);
  static auto graphHam = xacc::quantum::getObservable("pauli", hamStr);
  // DEBUG: Print the graph Hamiltonian matrix
  // auto els = graphHam->to_sparse_matrix();
  // for (auto el : els) {
  //     std::cout << el.row() << ", " << el.col() << ", " << el.coeff() << "\n";
  // }
  return graphHam.get();
}

bool QAOA::initialize(const HeterogeneousMap &parameters) {
  bool initializeOk = true;
  // Hyper-parameters for QAOA:
  // (1) Accelerator (QPU)
  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    std::cout << "'accelerator' is required.\n";
    // We check all required params; hence don't early return on failure.
    initializeOk = false;
  }

  // (2) Classical optimizer
  if (!parameters.pointerLikeExists<Optimizer>("optimizer")) {
    std::cout << "'optimizer' is required.\n";
    initializeOk = false;
  }

  // (3) Number of mixing and cost function steps to use (default = 1)
  m_nbSteps = 1;
  if (parameters.keyExists<int>("steps")) {
    m_nbSteps = parameters.get<int>("steps");
  }

  // (4) Cost Hamiltonian or a graph to construct the max-cut cost Hamiltonian
  // from.
  bool graphInput = false;
  m_maxcutProblem = false;
  if (!parameters.pointerLikeExists<Observable>("observable")) {
    if (parameters.pointerLikeExists<Graph>("graph")) {
      graphInput = true;
      m_maxcutProblem = true;
    } else {
      std::cout << "'observable' or 'graph' is required.\n";
      initializeOk = false;
    }
  }
  // Default is Extended ParameterizedMode (less steps, more params)
  m_parameterizedMode = "Extended";
  if (parameters.stringExists("parameter-scheme")) {
    m_parameterizedMode = parameters.getString("parameter-scheme");
  }

  if (initializeOk) {
    m_costHamObs =
        graphInput
            ? constructMaxCutHam(parameters.getPointerLike<Graph>("graph"))
            : parameters.getPointerLike<Observable>("observable");
    m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
    m_optimizer = parameters.getPointerLike<Optimizer>("optimizer");
    // Optional ref-hamiltonian
    m_refHamObs = nullptr;
    if (parameters.pointerLikeExists<Observable>("ref-ham")) {
      m_refHamObs = parameters.getPointerLike<Observable>("ref-ham");
    }
  }

  // we need this for ADAPT-QAOA (Daniel)
  if (parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    externalAnsatz =
        parameters.get<std::shared_ptr<CompositeInstruction>>("ansatz");
  }

  if (parameters.pointerLikeExists<AlgorithmGradientStrategy>(
          "gradient_strategy")) {
    gradientStrategy =
        parameters.get<std::shared_ptr<AlgorithmGradientStrategy>>(
            "gradient_strategy");
  }

  if (parameters.stringExists("gradient_strategy") &&
      !parameters.pointerLikeExists<AlgorithmGradientStrategy>(
          "gradient_strategy") &&
      m_optimizer->isGradientBased()) {
    gradientStrategy = xacc::getService<AlgorithmGradientStrategy>(
        parameters.getString("gradient_strategy"));
    gradientStrategy->initialize(
        {std::make_pair("observable", xacc::as_shared_ptr(m_costHamObs))});
  }

  if ((parameters.stringExists("gradient_strategy") ||
       parameters.pointerLikeExists<AlgorithmGradientStrategy>(
           "gradient_strategy")) &&
      !m_optimizer->isGradientBased()) {
    xacc::warning(
        "Chosen optimizer does not support gradients. Using default.");
  }

  if (parameters.keyExists<bool>("maximize")) {
      m_maximize = parameters.get<bool>("maximize");
  }

  if (m_optimizer->isGradientBased() && gradientStrategy == nullptr) {
    // No gradient strategy was provided, just use autodiff.
    gradientStrategy = xacc::getService<AlgorithmGradientStrategy>("autodiff");
    gradientStrategy->initialize(
        {{"observable", xacc::as_shared_ptr(m_costHamObs)}});
  }

  return initializeOk;
}

const std::vector<std::string> QAOA::requiredParameters() const {
  return {"accelerator", "optimizer", "observable"};
}

void QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {
  const int nbQubits = buffer->size();

  // we need this for ADAPT-QAOA (Daniel)
  std::shared_ptr<CompositeInstruction> kernel;
  if (externalAnsatz) {
    kernel = externalAnsatz;
  } else {
    kernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("qaoa"));
    kernel->expand({std::make_pair("nbQubits", nbQubits),
                    std::make_pair("nbSteps", m_nbSteps),
                    std::make_pair("cost-ham", m_costHamObs),
                    std::make_pair("ref-ham", m_refHamObs),
                    std::make_pair("parameter-scheme", m_parameterizedMode)});
  }

  // Observe the cost Hamiltonian:
  auto kernels = m_costHamObs->observe(kernel);

  int iterCount = 0;
  // Construct the optimizer/minimizer:
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

        // enables gradients (Daniel)
        if (gradientStrategy) {

          auto gradFsToExec =
              gradientStrategy->getGradientExecutions(kernel, x);
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
        m_qpu->execute(tmpBuffer, fsToExec);
        auto buffers = tmpBuffer->getChildren();

        double energy = identityCoeff;
        auto idBuffer = xacc::qalloc(buffer->size());
        idBuffer->addExtraInfo("coefficient", identityCoeff);
        idBuffer->setName("I");
        idBuffer->addExtraInfo("kernel", "I");
        idBuffer->addExtraInfo("parameters", x);
        idBuffer->addExtraInfo("exp-val-z", 1.0);
        buffer->appendChild("I", idBuffer);

        if (gradientStrategy) { // gradient-based optimization

          for (int i = 0; i < nInstructionsEnergy; i++) { // compute energy
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

        } else { // normal QAOA run

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
        iterCount++;
        ss << "Iter " << iterCount << ": E("
           << (!x.empty() ? std::to_string(x[0]) : "");
        for (int i = 1; i < x.size(); i++) {
          ss << "," << std::setprecision(3) << x[i];
          if (i > 4) {
            // Don't print too many params
            ss << ", ...";
            break;
          }
        }
        ss << ") = " << std::setprecision(12) << energy;
        xacc::info(ss.str());
        
        if (m_maximize) energy *= -1.0;
        return energy;
      },
      kernel->nVariables());

  auto result = m_optimizer->optimize(f);
  // Reports the final cost:
  // If the input is a graph (MAXCUT problem), shift and scale the value to
  // match the common convention.
  double finalCost =
      m_maxcutProblem ? (-0.5 * result.first +
                         0.5 * (m_costHamObs->getNonIdentitySubTerms().size()))
                      : result.first;
  if (m_maximize) finalCost *= -1.0;

  buffer->addExtraInfo("opt-val", ExtraInfo(finalCost));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
}

std::vector<double>
QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
              const std::vector<double> &x) {
  const int nbQubits = buffer->size();
  std::shared_ptr<CompositeInstruction> kernel;
  if (externalAnsatz) {
    kernel = externalAnsatz;
  } else if (m_single_exec_kernel) {
    kernel = m_single_exec_kernel;
  } else {
    kernel = std::dynamic_pointer_cast<CompositeInstruction>(
        xacc::getService<Instruction>("qaoa"));
    kernel->expand({std::make_pair("nbQubits", nbQubits),
                    std::make_pair("nbSteps", m_nbSteps),
                    std::make_pair("cost-ham", m_costHamObs),
                    std::make_pair("ref-ham", m_refHamObs),
                    std::make_pair("parameter-scheme", m_parameterizedMode)});
    // save this kernel for future calls to execute
    m_single_exec_kernel = kernel;
  }

  // Observe the cost Hamiltonian:
  auto kernels = m_costHamObs->observe(kernel);
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
      auto evaled = f->operator()(x);
      fsToExec.push_back(evaled);
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }

  auto tmpBuffer = xacc::qalloc(buffer->size());
  m_qpu->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();

  double energy = identityCoeff;
  auto idBuffer = xacc::qalloc(buffer->size());
  idBuffer->addExtraInfo("coefficient", identityCoeff);
  idBuffer->setName("I");
  idBuffer->addExtraInfo("kernel", "I");
  idBuffer->addExtraInfo("parameters", x);
  idBuffer->addExtraInfo("exp-val-z", 1.0);
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

  const double finalCost =
      m_maxcutProblem ? (-0.5 * energy +
                         0.5 * (m_costHamObs->getNonIdentitySubTerms().size()))
                      : energy;
  return {finalCost};
}

} // namespace algorithm
} // namespace xacc