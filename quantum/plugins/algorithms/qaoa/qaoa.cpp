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
 *   Milos Prokop - variable assignment mode
 *******************************************************************************/

#include "qaoa.hpp"

#include "xacc.hpp"
#include "Circuit.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"
#include "CompositeInstruction.hpp"
#include "AlgorithmGradientStrategy.hpp"

#include <cassert>
#include <iomanip>

namespace xacc {
namespace algorithm {

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

  // (4) Cost Hamiltonian to construct the max-cut cost Hamiltonian from.
  if (!parameters.pointerLikeExists<Observable>("observable")) {
        std::cout << "'observable' is required.\n";
        initializeOk = false;
    }

  // Default is Extended ParameterizedMode (less steps, more params)
  m_parameterizedMode = "Extended";
  if (parameters.stringExists("parameter-scheme")) {
    m_parameterizedMode = parameters.getString("parameter-scheme");
  }

  // Determine the optimal variable assignment of the QUBO problem
  // If false, only optimal value is returned
  m_varAssignmentMode = false;
  if (parameters.keyExists<bool>("calc-var-assignment")) {
	  m_varAssignmentMode = parameters.get<bool>("calc-var-assignment");
  }

  // Number of samples generated to find the optimal problem solution
  if(m_varAssignmentMode){
	nbSamples = 1024;
	if(parameters.keyExists<int>("nbSamples"))
		nbSamples = parameters.get<int>("nbSamples");
  }

  if (initializeOk) {
    m_costHamObs = parameters.getPointerLike<Observable>("observable");
    m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
    m_optimizer = parameters.getPointerLike<Optimizer>("optimizer");
    // Optional ref-hamiltonian
    m_refHamObs = nullptr;
    if (parameters.pointerLikeExists<Observable>("ref-ham")) {
      m_refHamObs = parameters.getPointerLike<Observable>("ref-ham");
    }
  }
     
  // Check if an initial composite instruction set has been provided
  if (parameters.pointerLikeExists<CompositeInstruction>("initial-state")) {
        m_initial_state = parameters.getPointerLike<CompositeInstruction>("initial-state");
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
    gradientStrategy->initialize(parameters);
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

  if (m_optimizer && m_optimizer->isGradientBased() &&
      gradientStrategy == nullptr) {
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

//evaluate the energy of measurement
double QAOA::evaluate_assignment(xacc::Observable* const observable, std::string measurement) const{

	double result =	observable->getIdentitySubTerm()->coefficient().real();
	for(auto &term: observable->getNonIdentitySubTerms()){

		//get the real part, imag expected to be 0
		double coeff = term->coefficient().real();

		//parse to get hamiltonian terms
		std::string term_str = term->toString();
		std::vector<int> qubit_indices;

		char z_coeff[6]; // Number of ciphers in qubit index are expected to fit here.
		int z_index = 0;

		int term_i = 0;

		for(size_t i = term_str.length()-1; i > 0; --i){
			char c = term_str[i];
			if(isdigit(c))
				z_coeff[5-z_index++] = c;
			else if(c == 'Z'){
				int val = 0;
				for(; z_index>0; --z_index){
					val += pow(10, z_index-1) * (z_coeff[5-z_index+1] - '0');
				}
				qubit_indices.push_back(val);
				z_index = 0;
			}
			else if(c == ' '){
				if(++term_i == 2)
					break;
			}
			else{
				break;
			}
		}
		int multiplier = 1;
		for(auto &qubit_index: qubit_indices)
			multiplier *= (measurement[qubit_index] == '0') ? 1 : -1;
		result += multiplier * coeff;
	}
	return result;
}


void QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {
  const int nbQubits = buffer->size();
  // we need this for ADAPT-QAOA (Daniel)
  std::shared_ptr<CompositeInstruction> kernel;
  if (externalAnsatz) {
    kernel = externalAnsatz;
  } else {
      HeterogeneousMap m;
      kernel = std::dynamic_pointer_cast<CompositeInstruction>(
          xacc::getService<Instruction>("qaoa"));
      m.insert("nbQubits", nbQubits);
      m.insert("nbSteps", m_nbSteps);
      m.insert("ref-ham", m_refHamObs);
      m.insert("cost-ham", m_costHamObs);
      m.insert("parameter-scheme", m_parameterizedMode);
      if (m_initial_state){
          m.insert("initial-state", m_initial_state);
      }
      kernel->expand(m);
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
  double finalCost = result.first;
  if (m_maximize) finalCost *= -1.0;

  if(m_varAssignmentMode){

 	  typedef std::pair<int, double> meas_freq_eval;
 	  typedef std::pair<std::string, meas_freq_eval> measurement;

 	  auto provider = xacc::getIRProvider("quantum");
 	  auto evaled = kernel->operator()(result.second);
 	  m_qpu->updateShotsNumber(nbSamples);

 	  //comment below and uncomment commented to use multiple measurement gates
 	  for(size_t i=0; i < nbQubits; ++i){
 	  	evaled->addInstructions({provider->createInstruction("Measure", i)});
 	  }

 	  //std::vector<size_t> indices;
 	  //for(size_t i=0; i < nbQubits; ++i){
 	  //   indices.push_back(i);
 	  //}
 	  //auto meas = provider->createInstruction("Measure", indices);
 	  //evaled->addInstructions({meas});

 	  m_qpu->execute(buffer, evaled);
 	  std::vector<measurement> measurements;

 	  for(std::pair<std::string, int> meas : buffer->getMeasurementCounts()){

 		  bool found = false;
 		  for(auto &instance : measurements)
 			  if(instance.first == meas.first){
 				  found = true;
 				  break;
 			  }

 		  if(!found){
 			  measurements.push_back(measurement(meas.first, // bit string
 					  	  	  	  	  meas_freq_eval(meas.second, //number of occurences
 					  	  	  	  			  evaluate_assignment(m_costHamObs, meas.first))));
 		  }
 	  }

 	 //sort by number of occurences
 	  if(m_maximize){
 		  std::sort(measurements.begin(), measurements.end(),
 			  [](const std::pair<std::string, std::pair<int, double>>& a, const std::pair<std::string, std::pair<int,int>>& b) {
 				  return a.second.second > b.second.second;
 	      });
 	  } else {
 		  std::sort(measurements.begin(), measurements.end(),
 			  [](const std::pair<std::string, std::pair<int, double>>& a, const std::pair<std::string, std::pair<int,int>>& b) {
 		  	  	  return a.second.second < b.second.second;
 		  });
 	  }

 	  double optimal_value = measurements[0].second.second;
 	  std::string opt_config = measurements[0].first;

 	  int i = 0;
 	  int hits;
 	  while(i < measurements.size() && abs(measurements[i++].second.second - optimal_value)<10e-1){
 		  hits += measurements[i-1].second.first;
 	  }

 	  double hit_rate = hits / double(nbSamples);

 	  buffer->addExtraInfo("opt-val", ExtraInfo(optimal_value));
 	  buffer->addExtraInfo("opt-config", opt_config);
 	  buffer->addExtraInfo("hit_rate: ", ExtraInfo(hit_rate));
 	  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));

   }else{
 	  buffer->addExtraInfo("opt-val", ExtraInfo(finalCost));
 	  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
   }
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
    HeterogeneousMap m;
    kernel = std::dynamic_pointer_cast<CompositeInstruction>(
          xacc::getService<Instruction>("qaoa"));
    m.insert("nbQubits", nbQubits);
    m.insert("nbSteps", m_nbSteps);
    m.insert("ref-ham", m_refHamObs);
    m.insert("cost-ham", m_costHamObs);
    m.insert("parameter-scheme", m_parameterizedMode);
    if (m_initial_state){
        m.insert("initial-state", m_initial_state);
    }
    kernel->expand(m);
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
  
  // WARNING: Removing the parameter shifting here. Remember for later
  // in case of any tests that fail. 
  const double finalCost = energy;
    //   m_maxcutProblem ? (-0.5 * energy +
    //                      0.5 * (m_costHamObs->getNonIdentitySubTerms().size()))
    //                   : energy;
  return {finalCost};
}

} // namespace algorithm
} // namespace xacc
