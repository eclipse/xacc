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
#include "PauliOperator.hpp"
#include "xacc.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include "xacc_service.hpp"

#include <memory>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace algorithm {
bool MC_VQE::initialize(const HeterogeneousMap &parameters) {
  /** Checks for the required parameters and other optional keywords
   * @param[in] HeterogeneousMap A map of strings to keys
   */

  start = std::chrono::high_resolution_clock::now();
  if (!parameters.pointerLikeExists<Accelerator>(
                 "accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  } else if(!parameters.pointerLikeExists<Optimizer>("optimizer")){
    std::cout << "Opt was false\n";
    return false;
  } else if(!parameters.keyExists<int>("nChromophores")){
    std::cout << "Missing number of chromophores\n";
    return false;
  } else if(!parameters.keyExists<std::string>("data-path")){
    std::cout << "Missing data file\n";
    return false; 
  }

  optimizer = parameters.getPointerLike<Optimizer>("optimizer");
  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  nChromophores = parameters.get<int>("nChromophores");
  dataPath = parameters.get<std::string>("data-path");
  
  // This is to include the last entangler if the system is cyclic
  if (parameters.keyExists<bool>("cyclic")){
    isCyclic = parameters.get<bool>("cyclic");
  } else {
    isCyclic = false;
  }

  nStates = nChromophores + 1;
  CISGateAngles.resize(nChromophores, nStates);
  preProcessing();

  auto endPrep = std::chrono::high_resolution_clock::now();
  auto prepTime = std::chrono::duration_cast<std::chrono::duration<double>>(endPrep - start).count();
  std::cout << std::setprecision(5) << "AIEM Hamiltonian and state preparation parameters computed in " <<
    prepTime << " s.\n";
  return true;
}

const std::vector<std::string> MC_VQE::requiredParameters() const {
  return {"optimizer", "accelerator", "nChromophores", "data-path"};
}

void MC_VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.

  // entangledHamiltonian stores the Hamiltonian matrix elements in the basis of MC states
  Eigen::MatrixXd entangledHamiltonian(nStates, nStates);
  entangledHamiltonian.setZero();

  auto entangler = entanglerCircuit(); // all CIS states share the same entangler gates
  auto nOptParams = entangler->nVariables(); // number of parameters to be optimized

  std::cout << "Starting the MC-VQE optimization...\n\n";

  // f is the objective function
  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {

        double averageEnergy = 0.0; // MC-VQE minimizes the average energy over all MC states
        auto startIter = std::chrono::high_resolution_clock::now();
        for (int state = 0; state < nStates; state++){
          std::vector<double> coefficients;
          std::vector<std::string> kernelNames;
          std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

          auto kernel = statePreparationCircuit(CISGateAngles.col(state)); // prepare CIS state
          kernel->addVariables(entangler->getVariables()); // add entangler variables
          for (auto& inst : entangler->getInstructions()){
            kernel->addInstruction(inst); // append entangler gates
          }
          
          std::cout << kernel->toString() << "\n";
          auto kernels = observable->observe(kernel); // observe AIEM Hamiltonian in the circuit above

          double identityCoeff = 0.0;
          for (auto &f : kernels) {// loop over CompositeInstructions from observe
            kernelNames.push_back(f->name());
            std::complex<double> coeff = f->getCoefficient();

            int nFunctionInstructions = 0;
            if (f->getInstruction(0)->isComposite()) {
            // checks whether f is Composite or single Instruction and retrieve number of instructions
              nFunctionInstructions =
                  kernel->nInstructions() + f->nInstructions() - 1;
            } else {
              nFunctionInstructions = f->nInstructions();
            }

            if (nFunctionInstructions > kernel->nInstructions()) {
              // checks if f has parameterized instructions
              auto evaled = f->operator()(x);
              fsToExec.push_back(evaled);
              coefficients.push_back(std::real(coeff));
            } else {
              identityCoeff += std::real(coeff);
            }
          }

          auto tmpBuffer = xacc::qalloc(buffer->size()); // temp instance of AcceleratorBuffer
          accelerator->execute(tmpBuffer, fsToExec); // execute parametrized instructions with tmpBuffer
          auto buffers = tmpBuffer->getChildren(); // children buffers one for each executed function

          // add info to the buffer holding the I term
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

          // handles the info from the remaining children buffers
          for (int i = 0; i < buffers.size(); i++) {
            auto expval = buffers[i]->getExpectationValueZ();
            energy += expval * coefficients[i];
            buffers[i]->addExtraInfo("coefficient", coefficients[i]);
            buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
            buffers[i]->addExtraInfo("exp-val-z", expval);
            buffers[i]->addExtraInfo("parameters", x);
            buffer->appendChild(fsToExec[i]->name(), buffers[i]);
          }
          
          entangledHamiltonian(state, state) = energy; // state energy goes to the diagonal of entangledHamiltonian
          averageEnergy += energy;
          std::cout << "State # " << state << " energy " << energy << "\n"; 
        }

        auto endIter = std::chrono::high_resolution_clock::now();
        auto iterTime = std::chrono::duration_cast<std::chrono::duration<double>>(endIter - startIter).count();
        std::cout << std::setprecision(5) << "Optimization iteration finished in " << iterTime << " s.\n";
        std::cout << std::setprecision(5) << "Average iteration time " << iterTime / nStates << " s.\n";

        // compute average energy and store current info
        averageEnergy /= nStates;
        std::cout << averageEnergy <<"\n";
        std::stringstream ss;
        ss << "E(" << ( !x.empty() ? std::to_string(x[0]) : "");
        for (int i = 1; i < x.size(); i++)
          ss << "," << x[i];
        ss << ") = " << std::setprecision(12) << averageEnergy;
        xacc::info(ss.str());
        
        return averageEnergy;

      },
      nOptParams);

  // run optimization
  auto result = optimizer->optimize(f);
  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  std::cout << "MC-VQE optimization complete.\n\n";

// Leaving out for now to test accelerator performance
/*
  auto startMC = std::chrono::high_resolution_clock::now();
  // now construct interference states and observe Hamiltonian
  auto optimizedEntangler = result.second;
  std::cout << "Computing Hamiltonian matrix elements in the interference state basis...\n\n";
  for (int stateA = 0; stateA < nStates - 1; stateA++){
    for (int stateB = stateA + 1; stateB < nStates; stateB++){

       // |+> = (|A> + |B>)/sqrt(2)
      auto plusInterferenceCircuit = 
        statePreparationCircuit((CISGateAngles.col(stateA) + CISGateAngles.col(stateB))/std::sqrt(2));
      plusInterferenceCircuit->addVariables(entangler->getVariables());
      for (auto& inst : entangler->getInstructions()){
        plusInterferenceCircuit->addInstruction(inst);
      }

      // vqe call to compute the AEIM in the plusInterferenceCircuit with optimal entangler parameters
      auto plus_vqe = xacc::getAlgorithm(
          "vqe", {std::make_pair("observable", observable),
                  std::make_pair("optimizer", optimizer),
                  std::make_pair("accelerator", accelerator),
                  std::make_pair("ansatz", plusInterferenceCircuit)});
      auto plusTerm = plus_vqe->execute(buffer, optimizedEntangler)[0];

       // |-> = (|A> - |B>)/sqrt(2)
      auto minusInterferenceCircuit = 
        statePreparationCircuit((CISGateAngles.col(stateA) - CISGateAngles.col(stateB))/std::sqrt(2));
      minusInterferenceCircuit->addVariables(entangler->getVariables());
      for (auto& inst : entangler->getInstructions()){
        minusInterferenceCircuit->addInstruction(inst);
      }

      // vqe call to compute the AEIM in the minusInterferenceCircuit with optimal entangler parameters
      auto minus_vqe = xacc::getAlgorithm(
          "vqe", {std::make_pair("observable", observable),
                  std::make_pair("optimizer", optimizer),
                  std::make_pair("accelerator", accelerator),
                  std::make_pair("ansatz", minusInterferenceCircuit)});
      auto minusTerm = minus_vqe->execute(buffer, optimizedEntangler)[0];

      // add the new matrix elements to entangledHamiltonian
      entangledHamiltonian(stateA, stateB) = (plusTerm - minusTerm)/std::sqrt(2);
      entangledHamiltonian(stateB, stateA) = entangledHamiltonian(stateA, stateB);
    }

  }

  auto endMC = std::chrono::high_resolution_clock::now();
  auto MCTime = std::chrono::duration_cast<std::chrono::duration<double>>(endMC - startMC).count();
  std::cout << "Interference basis Hamiltonian matrix elements computed in" << MCTime << " s.\n\n";

  std::cout << "Diagonalizing entangled Hamiltonian...\n\n";
  // Diagonalizing the entangledHamiltonian gives the energy spectrum
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> EigenSolver(entangledHamiltonian);
  auto MC_VQE_Energies = EigenSolver.eigenvalues();
  auto MC_VQE_States = EigenSolver.eigenvectors();

  //buffer->addExtraInfo("mc-vqe-energies", ExtraInfo(MC_VQE_Energies));
  //buffer->addExtraInfo("opt-params", ExtraInfo(MC_VQE_States));
  std::cout << "MC-VQE energy spectrum.\n";
  std::cout << std::setprecision(9) << MC_VQE_Energies;

  auto end = std::chrono::high_resolution_clock::now();
  auto totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
  std::cout << "MC-VQE simulation finished in " << totalTime << " s. Exiting...\n\n";
  */

  return;
}

std::shared_ptr<CompositeInstruction>
MC_VQE::statePreparationCircuit(const Eigen::VectorXd &angles) const {
/** Constructs the circuit that prepares a CIS state
 * @param[in] angles Angles to parameterize CIS state
 */

  auto mcvqeRegistry = xacc::getIRProvider("quantum"); // Provider to create IR for CompositeInstruction, aka circuit
  auto statePreparationInstructions = mcvqeRegistry->createComposite("mcvqeCircuit"); // allocate memory for circuit

  // Beginning of CIS state preparation
  //
  // Ry "pump"
  auto ry = mcvqeRegistry->createInstruction("Ry", std::vector<std::size_t>{0}, {angles(0)});
  statePreparationInstructions->addInstruction(ry);

  // Fy gates = Ry(-theta/2)-CZ-Ry(theta/2) = Ry(-theta/2)-H-CNOT-H-Ry(theta/2)
  //
  // |A>------------------------o------------------------
  //                            |            
  // |B>--[Ry(-angles(i)/2)]-H-[X]-H-[Ry(+angles(i)/2)]--
  //
  for (int i = 1; i < nChromophores; i++){

    std::size_t control = i - 1, target = i;
    // Ry(-theta/2)
    auto ry_minus = mcvqeRegistry->createInstruction("Ry", {target}, {-angles(i)/2});
    statePreparationInstructions->addInstruction(ry_minus);

    auto hadamard1 = mcvqeRegistry->createInstruction("H", {target});
    statePreparationInstructions->addInstruction(hadamard1);

    auto cnot = mcvqeRegistry->createInstruction("CNOT", {control, target});
    statePreparationInstructions->addInstruction(cnot);

    auto hadamard2 = mcvqeRegistry->createInstruction("H", {target});
    statePreparationInstructions->addInstruction(hadamard2);

    // Ry(+theta/2)
    auto ry_plus = mcvqeRegistry->createInstruction("Ry", {target}, {angles(i)/2});
    statePreparationInstructions->addInstruction(ry_plus);
  }

  // Wall of CNOTs
  for (int i = nChromophores - 2; i >= 0; i--){
    for(int j = nChromophores - 1; j > i; j--){

      std::size_t control = j, target = i;
      auto cnot = mcvqeRegistry->createInstruction("CNOT", {control, target});
      statePreparationInstructions->addInstruction(cnot);

    }
  }
  // end of CIS state preparation
  return statePreparationInstructions;
}

std::shared_ptr<CompositeInstruction> MC_VQE::entanglerCircuit() const {
  /** Constructs the entangler part of the circuit
   */

  std::string paramLetter = "x";
  auto mcvqeRegistry = xacc::getIRProvider("quantum"); // Provider to create IR for CompositeInstruction, aka circuit
  auto entanglerInstructions = mcvqeRegistry->createComposite("mcvqeCircuit"); // allocate memory for circuit

  // structure of entangler
  // does not implement the first two Ry to remove redundancies in the circuit
  // and to reduce the number of variational parameters
  //
  // |A>--[Ry(x0)]--o--[Ry(x2)]--o--[Ry(x4)]-
  //                |            |
  // |B>--[Ry(x1)]--x--[Ry(x3)]--x--[Ry(x5)]-
  //
  auto entanglerGate = [&](const std::size_t control, const std::size_t target, int paramCounter){
    /** Lambda function to construct the entangler gates
     * Interleaves CNOT(control, target) and Ry rotations
     * 
     * @param[in] control Index of the control/source qubit
     * @param[in] target Index of the target qubit
     * @param[in] paramCounter Variable that keeps track of and indexes the circuit parameters
     */

    auto cnot1 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    entanglerInstructions->addInstruction(cnot1);

    auto ry1 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter))});
    entanglerInstructions->addVariable(paramLetter + std::to_string(paramCounter));
    entanglerInstructions->addInstruction(ry1);

    auto ry2 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 1))});
    entanglerInstructions->addVariable(paramLetter + std::to_string(paramCounter + 1));
    entanglerInstructions->addInstruction(ry2);
    
    auto cnot2 = mcvqeRegistry->createInstruction("CNOT", {control, target});
    entanglerInstructions->addInstruction(cnot2);

    auto ry3 = mcvqeRegistry->createInstruction("Ry", {control}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 2))});
    entanglerInstructions->addVariable(paramLetter + std::to_string(paramCounter + 2));
    entanglerInstructions->addInstruction(ry3);

    auto ry4 = mcvqeRegistry->createInstruction("Ry", {target}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter + 3))});
    entanglerInstructions->addVariable(paramLetter + std::to_string(paramCounter + 3));
    entanglerInstructions->addInstruction(ry4); 

  }; 

  // placing the first Ry's in the circuit
  int paramCounter = 0;
  for (int i = 0; i < nChromophores; i++){
    std::size_t q = i;
    auto ry = mcvqeRegistry->createInstruction("Ry", {q}, 
      {InstructionParameter(paramLetter + std::to_string(paramCounter))});
    entanglerInstructions->addVariable(paramLetter + std::to_string(paramCounter));
    entanglerInstructions->addInstruction(ry);
    
    paramCounter++;
  }

  // placing the entanglerGates in the circuit
  for (int layer : {0, 1}){
    for (int i = layer; i < nChromophores - layer; i += 2){
      std::size_t control = i, target = i + 1;
      entanglerGate(control, target, paramCounter);
      paramCounter += nParamsEntangler;
    }
  }

  // if the molecular system is cyclic, we need to add this last entangler
  if(isCyclic){
    std::size_t control = nChromophores - 1, target = 0;
    entanglerGate(control, target, paramCounter);
  }

  // end of entangler circuit construction
  return entanglerInstructions;
}

void MC_VQE::preProcessing() {
  /** Function to process the quantum chemistry data into CIS state preparation angles and the AIEM Hamiltonian.
   * 
   * Excited state refers to the first excited state.
   * Ref1 = PRL 122, 230401 (2019)
   * Ref2 = Supplemental Material for Ref2
   * Ref3 = arXiv:1906.08728v1
   */

  // allocate memory for the quantum chemistry input
  // energiesGS = ground state energies
  // energiesES = excited state energies
  // dipoleGS = ground state dipole moment vectors
  // dipoleES = excited state dipole moment vectors
  // dipoleT = transition (between ground and excited states) dipole moment
  // com = center of mass of each chromophore
  Eigen::VectorXd energiesGS(nChromophores), energiesES(nChromophores);
  Eigen::MatrixXd dipoleGS(nChromophores, 3), dipoleES(nChromophores, 3), 
                  dipoleT(nChromophores, 3), com(nChromophores, 3);

  energiesES.setZero();
  energiesGS.setZero();
  dipoleGS.setZero();
  dipoleES.setZero();
  dipoleT.setZero();
  com.setZero();

  std::ifstream file(dataPath);
  std::string line, tmp, comp;
  int xyz, start;
  for (int A = 0; A < nChromophores; A++){
    // scans output file and retrieve data

    std::getline(file, line);// this is just the number label of the chromophore
    std::getline(file, line);
    energiesGS(A) =  std::stod(line.substr(line.find(":") + 1));

    std::getline(file, line);
    energiesES(A) =  std::stod(line.substr(line.find(":") + 1));

    std::getline(file, line);
    tmp = line.substr(line.find(":") + 1);
    std::stringstream comStream(tmp);
    xyz = 0;
    while(std::getline(comStream, comp, ',')) {
      com(A, xyz) = std::stod(comp);
      xyz++;
    }

    std::getline(file, line);
    tmp = line.substr(line.find(":") + 1);
    std::stringstream gsDipoleStream(tmp);
    xyz = 0;
    while(std::getline(gsDipoleStream, comp, ',')) {
      dipoleGS(A, xyz) = std::stod(comp);
      xyz++;
    }

    std::getline(file, line);
    tmp = line.substr(line.find(":") + 1);
    std::stringstream esDipoleStream(tmp);
    xyz = 0;
    while(std::getline(esDipoleStream, comp, ',')) {
      dipoleES(A, xyz) = std::stod(comp);
      xyz++;
    }

    std::getline(file, line);
    tmp = line.substr(line.find(":") + 1);
    std::stringstream tDipoleStream(tmp);
    xyz = 0;
    while(std::getline(tDipoleStream, comp, ',')) {
      dipoleT(A, xyz) = std::stod(comp);
      xyz++;
    }
    std::getline(file, line);

  }
  file.close();

  com *= ANGSTROM2BOHR; // angstrom to bohr
  dipoleGS *= DEBYE2AU; // D to a.u.
  dipoleES *= DEBYE2AU;

  auto twoBodyH = [&](const Eigen::VectorXd mu_A, const Eigen::VectorXd mu_B, const Eigen::VectorXd r_AB){
    /** Lambda function to compute the two-body AIEM Hamiltonian matrix elements (Ref.2 Eq. 67)
     * @param[in] mu_A Some dipole moment vector (gs/es/t) from chromophore A
     * @param[in] mu_B Some dipole moment vector (gs/es/t) from chromophore B
     * @param[in] r_AB Vector between A and B
     */
    auto d_AB = r_AB.norm(); // Distance between A and B
    auto n_AB = r_AB / d_AB; // Normal vector along r_AB
    return (mu_A.dot(mu_B) - 3.0 * mu_A.dot(n_AB) * mu_B.dot(n_AB))/std::pow(d_AB, 3.0); // return matrix element
  }; 

  // hamiltonian = AIEM Hamiltonian
  // term = creates individual Pauli terms
  PauliOperator hamiltonian, term;

  // stores the indices of the valid chromophore pairs
  std::vector<std::vector<int>> pairs(nChromophores);
  for (int A = 0; A < nChromophores; A++){
    if(A == 0){
      pairs[A] = {A + 1, nChromophores - 1};
    } else if (A == nChromophores - 1){
      pairs[A] = {A - 1, 0};
    } else {
      pairs[A] = {A - 1, A + 1};
    }
  }

  auto S_A = (energiesGS + energiesES) / 2.0; // Ref2 Eq. 20
  auto D_A = (energiesGS - energiesES) / 2.0; // Ref2 Eq. 21
  auto dipoleSum = (dipoleGS + dipoleES) / 2.0; // sum of dipole moments, coordinate-wise
  auto dipoleDiff = (dipoleGS - dipoleES) / 2.0; // difference of dipole moments, coordinate-wise
  double E = 0.0; // = S_A.sum(), but this only shifts the eigenvalues

  Eigen::VectorXd Z_A(nChromophores), X_A(nChromophores);
  Z_A = D_A;
  X_A.setZero();
  Eigen::MatrixXd XX_AB(nChromophores, nChromophores), XZ_AB(nChromophores, nChromophores),
                  ZX_AB(nChromophores, nChromophores), ZZ_AB(nChromophores, nChromophores);
  XX_AB.setZero();
  XZ_AB.setZero();
  ZX_AB.setZero();
  ZZ_AB.setZero();
  
  // Compute the AIEM Hamiltonian
  for (int A = 0; A < nChromophores; A++){

    for (int B : pairs[A]) {

      E += 0.5 * twoBodyH(dipoleSum.row(A), dipoleSum.row(B), com.row(A) - com.row(B));
      E += 0.5 * twoBodyH(dipoleSum.row(B), dipoleSum.row(A), com.row(B) - com.row(A));

      Z_A(A) += 0.5 * twoBodyH(dipoleSum.row(A), dipoleDiff.row(B), com.row(A) - com.row(B));
      Z_A(A) += 0.5 * twoBodyH(dipoleDiff.row(B), dipoleSum.row(A), com.row(B) - com.row(A));

      X_A(A) += 0.5 * twoBodyH(dipoleT.row(A), dipoleSum.row(B), com.row(A) - com.row(B));
      X_A(A) += 0.5 * twoBodyH(dipoleSum.row(B), dipoleT.row(A), com.row(B) - com.row(A));     

      XX_AB(A, B) = twoBodyH(dipoleT.row(A), dipoleT.row(B), com.row(A) - com.row(B));
      XZ_AB(A, B) = twoBodyH(dipoleT.row(A), dipoleDiff.row(B), com.row(A) - com.row(B));
      ZX_AB(A, B) = twoBodyH(dipoleDiff.row(A), dipoleT.row(B), com.row(A) - com.row(B));
      ZZ_AB(A, B) = twoBodyH(dipoleDiff.row(A), dipoleDiff.row(B), com.row(A) - com.row(B));

      term.fromString(std::to_string(XX_AB(A, B)) + " X" + std::to_string(A) + " X" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(XZ_AB(A, B)) + " X" + std::to_string(A) + " Z" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(ZX_AB(A, B)) + " Z" + std::to_string(A) + " X" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(ZZ_AB(A, B)) + " Z" + std::to_string(A) + " Z" + std::to_string(B)); 
      hamiltonian += term;
    }

    term.fromString(std::to_string(Z_A(A)) + " Z" + std::to_string(A));
    hamiltonian += term;

    term.fromString(std::to_string(X_A(A)) + " X" + std::to_string(A));
    hamiltonian += term;

  }

  term.fromString(std::to_string(E));
  hamiltonian += term;

  // Done with the AIEM Hamiltonian, just need a pointer for it and upcast to Observable
  auto hamiltonianPtr = std::make_shared<PauliOperator>(hamiltonian);
  observable = std::dynamic_pointer_cast<Observable>(hamiltonianPtr);

  // CISMatrix stores the CIS matrix elements in the nChromophore two-state basis
  Eigen::MatrixXd CISMatrix(nStates , nStates);
  CISMatrix.setZero();

  // E_ref
  auto E_ref = E + Z_A.sum() + 0.5 * ZZ_AB.sum();
  CISMatrix(0, 0) = E_ref;

  // diagonal singles-singles
  for (int A = 0; A < nChromophores; A++){
    CISMatrix(A + 1, A + 1) = E_ref - 2.0 * Z_A(A);
    for (int B : pairs[A]){
      CISMatrix(A + 1, A + 1) -= ZZ_AB(A, B) + ZZ_AB(B, A);
    }
  }

  // reference-singles off diagonal
  for (int A = 0; A < nChromophores; A++){
    CISMatrix(A + 1, 0) = X_A(A);
    for (int B : pairs[A]){
      CISMatrix(A + 1, 0) += 0.5 * (XZ_AB(A, B) + ZX_AB(B, A));
    }
    CISMatrix(0, A + 1) = CISMatrix(A + 1, 0);
  }

  // singles-singles off-diagonal 
  for (int A = 0; A < nChromophores; A++){
    for (int B : pairs[A]){
      CISMatrix(A + 1, B + 1) = XX_AB(A, B);
    }
  } 

  // Diagonalizing the CISMatrix
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> EigenSolver(CISMatrix);
  auto CISEnergies = EigenSolver.eigenvalues();
  auto CISStates = EigenSolver.eigenvectors();

  // Computing the CIS state preparation angles (Ref3 Eqs. 60-61)
  for (int state = 0; state <= nChromophores; state++){
    for (int angle = 0; angle < nChromophores; angle++){

      double partialCoeffNorm = CISStates.col(state).segment(angle, nChromophores - angle + 1).norm();
      CISGateAngles(angle, state) = std::acos(CISStates(angle, state)/partialCoeffNorm);
    }

    if(CISStates(Eigen::last, state) < 0.0) {
      CISGateAngles(nStates - 1, state) *= -1.0;
    }

  }

  // end of preProcessing
  return;
}

} // namespace algorithm
} // namespace xacc