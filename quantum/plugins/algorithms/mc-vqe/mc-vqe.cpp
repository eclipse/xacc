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
  if (!parameters.pointerLikeExists<Accelerator>(
                 "accelerator")) {
    std::cout << "Acc was false\n";
    return false;
  } else if(!parameters.keyExists<int>("nChromophores")){
    std::cout << "Missing number of chromophores\n";
    return false;
  }

  optimizer = parameters.getPointerLike<Optimizer>("optimizer");
  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  nChromophores = parameters.get<int>("nChromophores");

  // This is to include the last entangler if the system is cyclic
  if (parameters.keyExists<bool>("cyclic")){
    isCyclic = parameters.get<bool>("cyclic");
  } else {
    isCyclic = false;
  }

  CISGateAngles.resize(nChromophores, nStates);
  preProcessing();

  return {"optimizer", "accelerator", "nChromophores"};
}

void MC_VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // Here we just need to make a lambda kernel
  // to optimize that makes calls to the targeted QPU.

  Eigen::MatrixXd entangledHamiltonian(nStates, nStates);
  entangledHamiltonian.setZero();
  int nOptParams = nParamsEntangler * nChromophores;
  if(!isCyclic) {
    nOptParams -= nParamsEntangler;
  }

  std::cout << "Starting the MC-VQE optimization...\n\n";

  OptFunction f(
      [&, this](const std::vector<double> &x, std::vector<double> &dx) {

        double averageEnergy = 0.0;
        for (int state = 0; state < nStates; state++){
          std::vector<double> coefficients;
          std::vector<std::string> kernelNames;
          std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

          auto kernel = circuit(CISGateAngles.col(state));
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
          
          entangledHamiltonian(state, state) = energy;
          averageEnergy += energy;
        }

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

  auto result = optimizer->optimize(f);
  buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
  buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
  std::cout << "MC-VQE optimization complete.\n\n";

  // now construct interference states and observe Hamiltonian
  auto optimizedEntangler = result.second;

  std::cout << "Computing Hamiltonian matrix elements in the interference state basis...\n\n";
  for (int stateA = 0; stateA < nStates - 1; stateA++){
    for (int stateB = stateA + 1; stateB < nStates; stateB++){

       // |+> = (|A> + |B>)/sqrt(2)
      auto plusInterferenceCircuit = circuit((CISGateAngles.col(stateA) + CISGateAngles.col(stateB))/std::sqrt(2));
      auto plus_vqe = xacc::getAlgorithm(
          "vqe", {std::make_pair("observable", observable),
                  std::make_pair("optimizer", optimizer),
                  std::make_pair("accelerator", accelerator),
                  std::make_pair("ansatz", plusInterferenceCircuit)});
      auto plusTerm = plus_vqe->execute(buffer, optimizedEntangler)[0];

       // |-> = (|A> - |B>)/sqrt(2)
      auto minusInterferenceCircuit = circuit((CISGateAngles.col(stateA) - CISGateAngles.col(stateB))/std::sqrt(2));
      auto minus_vqe = xacc::getAlgorithm(
          "vqe", {std::make_pair("observable", observable),
                  std::make_pair("optimizer", optimizer),
                  std::make_pair("accelerator", accelerator),
                  std::make_pair("ansatz", minusInterferenceCircuit)});
      auto minusTerm = minus_vqe->execute(buffer, optimizedEntangler)[0];

      entangledHamiltonian(stateA, stateB) = (plusTerm - minusTerm)/std::sqrt(2);
      entangledHamiltonian(stateB, stateA) = entangledHamiltonian(stateA, stateB);
    }

  }

  std::cout << "Diagonalizing entangled Hamiltonian...\n\n";
  // MC_VQE_Energies gives the energy spectrum
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> EigenSolver(entangledHamiltonian);
  auto MC_VQE_Energies = EigenSolver.eigenvalues();
  auto MC_VQE_States = EigenSolver.eigenvectors();

  //buffer->addExtraInfo("mc-vqe-energies", ExtraInfo(MC_VQE_Energies));
  //buffer->addExtraInfo("opt-params", ExtraInfo(MC_VQE_States));
  std::cout << "MC-VQE energy spectrum. Exiting...\n\n";
  std::cout << std::setprecision(9) << MC_VQE_Energies;
  return;
}

// I'm going to leave this here for now, but I don't see any use for it
/*
std::vector<double>
MC_VQE::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
             const std::vector<double> &x) {

  double averageEnergy = 0.0;
  for (int state = 0; state < nStates; state++){

    std::vector<double> coefficients;
    std::vector<std::string> kernelNames;
    std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

    auto kernel = circuit(CISGateAngles.col(state));
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

  averageEnergy /= nStates;  

  std::stringstream ss;

  ss << "E(" << ( !x.empty() ? std::to_string(x[0]) : "");
  for (int i = 1; i < x.size(); i++)
    ss << "," << x[i];
  ss << ") = " << std::setprecision(12) << averageEnergy;
  xacc::info(ss.str());
  return {averageEnergy};
}*/


std::shared_ptr<CompositeInstruction>
MC_VQE::circuit(const Eigen::VectorXd &angles) const {


  std::string paramLetter = "x";
  std::vector<std::string> vars;
  if(isCyclic){
    for (int i = 0; i < nParamsEntangler * nChromophores; i++){
      vars.push_back(paramLetter + std::to_string(i));
    } 
  } else {
    for (int i = 0; i < nParamsEntangler * (nChromophores - 1); i++){
      vars.push_back(paramLetter + std::to_string(i));
    }
  }
  auto mcvqeRegistry = xacc::getIRProvider("quantum");
  auto mcvqeInstructions = mcvqeRegistry->createComposite("mcvqeCircuit", vars);

  // Beginning of CIS state preparation

  // Ry "pump"
  auto ry = mcvqeRegistry->createInstruction("Ry", std::vector<std::size_t>{0}, {angles(0)});
  mcvqeInstructions->addInstruction(ry);

  // Fy gates = Ry(-theta/2)-CZ-Ry(theta/2)
  // |A>------------------------o------------------------
  //                            |            
  // |B>--[Ry(-angles[i]/2)]-H-[X]-H-[Ry(+angles[i]/2)]--
  //
  for (int i = 1; i < nChromophores; i++){

    std::size_t control = i - 1, target = i;
    // Ry(-theta/2)
    auto ry_minus = mcvqeRegistry->createInstruction("Ry", {target}, {-angles(i)/2});
    mcvqeInstructions->addInstruction(ry_minus);

    auto hadamard1 = mcvqeRegistry->createInstruction("H", {target});
    mcvqeInstructions->addInstruction(hadamard1);

    auto cnot = mcvqeRegistry->createInstruction("CNOT", {control, target});
    mcvqeInstructions->addInstruction(cnot);

    auto hadamard2 = mcvqeRegistry->createInstruction("H", {target});
    mcvqeInstructions->addInstruction(hadamard2);

    // Ry(+theta/2)
    auto ry_plus = mcvqeRegistry->createInstruction("Ry", {target}, {angles(i)/2});
    mcvqeInstructions->addInstruction(ry_plus);
  }

  // Wall of CNOTs
  for (int i = nChromophores - 2; i >= 0; i--){
    for(int j = nChromophores - 1; j > i; j--){

      std::size_t control = j, target = i;
      auto cnot = mcvqeRegistry->createInstruction("CNOT", {control, target});
      mcvqeInstructions->addInstruction(cnot);

    }
  }
  // end of CIS state preparation
  
  // structure of entangler
  //
  // |A>--[Ry(x0)]--o--[Ry(x2)]--o--[Ry(x4)]-
  //                |            |
  // |B>--[Ry(x1)]--x--[Ry(x3)]--x--[Ry(x5)]-
  //
  auto entanglerGate = [&](const std::size_t control, const std::size_t target, int paramCounter){
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
  }; 

  // placing the entanglers in the circuit
  int paramCounter = 0;
  for (int layer : {0, 1}){
    for (int i = layer; i < nChromophores - layer; i += 2){
      std::size_t control = i, target = i + 1;
      entanglerGate(control, target, paramCounter);
      paramCounter += nParamsEntangler;
    }
  }

  // if the molecular system is cyclic, need to add this last entangler
  if(isCyclic){
    std::size_t control = nChromophores - 1, target = 0;
    entanglerGate(control, target, paramCounter);
  }
  
  return mcvqeInstructions;
}

void MC_VQE::preProcessing() {//Eigen::MatrixXd &CISGateAngles, std::shared_ptr<Observable> observable) {

  Eigen::VectorXd es_energies(nChromophores), gs_energies(nChromophores);
  Eigen::MatrixXd es_dipole(nChromophores, 3), gs_dipole(nChromophores, 3), 
                  t_dipole(nChromophores, 3), com(nChromophores, 3);

  es_energies.setZero();
  gs_energies.setZero();
  gs_dipole.setZero();
  es_dipole.setZero();
  t_dipole.setZero();
  com.setZero();

  std::string line, data, comp;
  std::stringstream dataStream;
  for (int out = 0; out < nChromophores; out++){
    std::string filePath = std::string("/workspace/data/") + std::to_string(out + 1) + ".out";
    std::ifstream file(filePath);
    if(file.bad()){
      xacc::error("Cannot find output file.");
    }

    unsigned int del1, del2;
    while(!file.eof()){
      std::getline(file, line);

      if(line.find("FINAL ENERGY") != std::string::npos) {
        del1 = line.find(":");
        del2 = line.find("a.u.");
        data = line.substr(del1 + 1, del2 - del1 - 1);
        gs_energies(out) = std::stod(data);
      }

      if(line.find("CENTER OF MASS") != std::string::npos) {
        del1 = line.find("{");
        del2 = line.find("}");
        data = line.substr(del1 + 1, del2 - del1 - 1);
        std::stringstream dataStream(data);
        int xyz = 0;
        while (std::getline(dataStream, comp, ',')) {
          com(out, xyz) = std::stod(comp);
          xyz++;
        } 
      }

      if(line.find("DIPOLE MOMENT") != std::string::npos) {
        del1 = line.find("{");
        del2 = line.find("}");
        data = line.substr(del1 + 1, del2 - del1 - 1);
        std::stringstream dataStream(data);
        int xyz = 0;
        while (std::getline(dataStream, comp, ',')) {
          gs_dipole(out, xyz) = std::stod(comp);
          xyz++;
        }
      }

      if(line.find("Ex. Energy") != std::string::npos) {
        std::getline(file, line);
        std::getline(file, line);
        data = line.substr(9, 20);
        es_energies(out) = std::stod(data);
      }

      if(line.find("Dx") != std::string::npos) {
        std::getline(file, line);
        std::getline(file, line);
        data = line.substr(9, 30);
        std::stringstream dataStream(data);
        int xyz = 0;
        do {
            dataStream >> comp;
            es_dipole(out, xyz) = std::stod(comp);
            xyz++;
        } while (dataStream && xyz < 3);
      }
      
      if(line.find("Tx") != std::string::npos) {
        std::getline(file, line);
        std::getline(file, line);
        data = line.substr(9, 30);
        std::stringstream dataStream(data);
        int xyz = 0;
        do {
            dataStream >> comp;
            t_dipole(out, xyz) = std::stod(comp);
            xyz++;
        } while (dataStream && xyz < 3);
        break; // if we get to this point, we've got everything we need
      }
    }
  }

  com *= angstrom2Bohr; // angstrom to bohr
  gs_dipole *= debye2Au; // D to a.u.
  es_dipole *= debye2Au;

  // computes the two-body AIEM Hamiltonian matrix elements
  auto twoBodyH = [&](const Eigen::VectorXd mu_A, const Eigen::VectorXd mu_B, const Eigen::VectorXd r_AB){
    auto d_AB = r_AB.norm();
    auto n_AB = r_AB / d_AB;
    return (mu_A.dot(mu_B) - 3.0 * mu_A.dot(n_AB) * mu_B.dot(n_AB))/std::pow(d_AB, 3.0);
  }; 

  // AIEM Hamiltonian
  PauliOperator hamiltonian, term;

  auto S_A = (gs_energies + es_energies)/2;
  auto D_A = (gs_energies - es_energies)/2;

  auto E = S_A.sum();
  for (int A = 0; A < nChromophores - 1; A++){
    auto mu_A = (gs_dipole.row(A) + es_dipole.row(A))/2;
    for (int B = A + 1; B < nChromophores; B++){
      auto mu_B = (gs_dipole.row(B) + es_dipole.row(B))/2;
      E += twoBodyH(mu_A, mu_B, com.row(A) - com.row(B));
    }
  }

  hamiltonian.fromString(std::to_string(E) + " I");

  Eigen::VectorXd Z_A(nChromophores);
  Z_A.setZero();
  for (int A = 0; A < nChromophores; A++){
    auto mu_A = (gs_dipole.row(A) - es_dipole.row(A))/2;
    Z_A(A) = D_A(A);
    for (int B = 0; B < nChromophores; B++){
      if (B != A){
        auto mu_B = (gs_dipole.row(B) + es_dipole.row(B))/2;
        Z_A(A) += twoBodyH(mu_A, mu_B, com.row(A) - com.row(B));
      }
    }
    term.fromString(std::to_string(Z_A(A)) + " Z" + std::to_string(A));
    hamiltonian += term;
  }

  Eigen::VectorXd X_A(nChromophores);
  X_A.setZero();
  for (int A = 0; A < nChromophores; A++){
    auto mu_A = t_dipole.row(A);
    for (int B = 0; B < nChromophores; B++){
      if (B != A){
        auto mu_B = (gs_dipole.row(B) + es_dipole.row(B))/2;
        X_A(A) += twoBodyH(mu_A, mu_B, com.row(A) - com.row(B));
      }
    }
    term.fromString(std::to_string(Z_A(A)) + " Z" + std::to_string(A));
    hamiltonian += term;
  }

  Eigen::MatrixXd XX_AB(nChromophores, nChromophores), XZ_AB(nChromophores, nChromophores),
                  ZX_AB(nChromophores, nChromophores), ZZ_AB(nChromophores, nChromophores);

  XX_AB.setZero();
  XZ_AB.setZero();
  ZX_AB.setZero();
  ZZ_AB.setZero();

  for (int A = 0; A < nChromophores - 1; A++){
    for (int B = A + 1 ; B < nChromophores; B++){

      XX_AB(A, B) = twoBodyH(t_dipole.row(A), t_dipole.row(B), com.row(A) - com.row(B));
      XZ_AB(A, B) = twoBodyH(t_dipole.row(A), (gs_dipole.row(B) - es_dipole.row(B))/2, com.row(A) - com.row(B));
      ZX_AB(A, B) = twoBodyH((gs_dipole.row(A) - es_dipole.row(A))/2, t_dipole.row(B), com.row(A) - com.row(B));
      ZZ_AB(A, B) = twoBodyH((gs_dipole.row(A) - es_dipole.row(A))/2, (gs_dipole.row(B) - es_dipole.row(B))/2, com.row(A) - com.row(B));
      XX_AB(B, A) = XX_AB(A, B);
      XZ_AB(B, A) = XZ_AB(A, B);
      ZX_AB(B, A) = ZX_AB(A, B);
      ZZ_AB(B, A) = ZZ_AB(A, B);

      term.fromString(std::to_string(XX_AB(A, B)) + " X" + std::to_string(A) + " X" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(XZ_AB(A, B)) + " X" + std::to_string(A) + " Z" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(ZX_AB(A, B)) + " Z" + std::to_string(A) + " X" + std::to_string(B));
      hamiltonian += term;
      term.fromString(std::to_string(ZZ_AB(A, B)) + " Z" + std::to_string(A) + " Z" + std::to_string(B)); 
      hamiltonian += term;

    }
  }
  // We're done with the AIEM Hamiltonian, just need a pointer for it
  auto hamiltonianPtr = std::make_shared<PauliOperator>(hamiltonian);
  observable = std::dynamic_pointer_cast<Observable>(hamiltonianPtr);

  Eigen::MatrixXd CISMatrix(nStates , nStates);
  CISMatrix.setZero();
  auto E_ref = E + Z_A.sum();

  for (int A = 0; A < nChromophores; A++){

    CISMatrix(A + 1, A + 1) = -2.0 * Z_A(A);
    CISMatrix(A + 1, 0) = X_A(A);

    for (int B = 0; B < nChromophores; B++){

      if(B != A){

        E_ref += 0.5 * ZZ_AB(A, B);
        CISMatrix(A + 1, A + 1) -= (ZZ_AB(A, B) + ZZ_AB(B, A));
        CISMatrix(A + 1, 0) += 0.5 * (XZ_AB(A, B) + ZX_AB(B, A));
        CISMatrix(A + 1, B + 1) = XX_AB(A, B);
        CISMatrix(B + 1, A + 1) = CISMatrix(A + 1, B + 1);

      }
    }
    CISMatrix(0, A + 1) = CISMatrix(A + 1, 0);
  }


  for (int A = 0; A <= nChromophores; A++){
    CISMatrix(A, A) += E_ref; 
  }

  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> EigenSolver(CISMatrix);
  auto CISEnergies = EigenSolver.eigenvalues();
  auto CISStates = EigenSolver.eigenvectors();
  for (int state = 0; state <= nChromophores; state++){
    for (int angle = 0; angle < nChromophores; angle++){

      double partialCoeffNorm = CISStates.col(state).segment(angle, nChromophores - angle + 1).norm();
      CISGateAngles(angle, state) = std::acos(CISStates(angle, state)/partialCoeffNorm);
    }
    
    if(CISStates(nStates, state) < 0.0) {
      CISGateAngles(nStates, state) *= -1.0;
    }
  }
  return;
}

} // namespace algorithm
} // namespace xacc