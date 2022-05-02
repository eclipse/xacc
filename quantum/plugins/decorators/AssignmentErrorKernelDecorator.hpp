/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Tyler Kharazi - initial implementation
 *******************************************************************************/
#ifndef XACC_ASSIGNMENTERRORKERNELDECORATOR_HPP_
#define XACC_ASSIGNMENTERRORKERNELDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"
#include <Eigen/Dense>
#include <sstream>
#include "xacc.hpp"

namespace xacc {

namespace quantum {

class AssignmentErrorKernelDecorator : public AcceleratorDecorator {
protected:
  bool gen_kernel;
  Eigen::MatrixXd errorKernel;
  std::vector<std::string> permutations = {""};
  bool multiplex;
  std::vector<std::size_t> layout;

  std::vector<std::string> generatePermutations(int num_bits) {
    int pow_bits = std::pow(2, num_bits);
    std::vector<std::string> bitstring(pow_bits);
    std::string str = "";
    std::string curr;
    int counter = 0;
    int j = num_bits;
    while (j--) {
      str.push_back('0');
    }
    for (int k = 0; k <= num_bits; k++) {
      str[num_bits - k] = '1';
      curr = str;
      do {
        bitstring[counter] = curr;
        counter++;
      } while (next_permutation(curr.begin(), curr.end()));
    }
    return bitstring;
  }


  void generateKernel(std::shared_ptr<AcceleratorBuffer> buffer) {
    // pow(2, num_bits) gets used a lot, so I figured I should just define it
    // here

    int num_bits = buffer->size();
    int pow_bits = std::pow(2, num_bits);
    this->permutations = generatePermutations(num_bits);

    // permutations contains all possible permutationss to generate circuits,
    // there is direct mapping from permutations to circuit as follows:
    // permutations: 10 => X gate on zeroth qubit and nothing on first qubit and
    // measure both permutations: 11 => X gate on both qubits and measure both
    // etc...

    std::shared_ptr<AcceleratorBuffer> tmpBuffer = xacc::qalloc(buffer->size());
    // generating list of circuits to evaluate
    std::vector<std::shared_ptr<CompositeInstruction>> circuits;
    auto provider = xacc::getIRProvider("quantum");
    for (int i = 0; i < pow_bits; i++) {
      auto circuit = provider->createComposite(permutations[i]);
      int j = num_bits-1;
      for (char c : permutations[i]) {
        if (c == '1') {
          // std::cout<<"1 found at position: "<<j<<std::endl;
          //std::cout<<"j = "<<j<<std::endl;
          auto x = provider->createInstruction("X", j);
          circuit->addInstruction(x);
        }
        j = (j-1)%num_bits;
      }
      for(int i = 0; i < num_bits; i++){
        circuit->addInstruction(provider->createInstruction("Measure", i));
      }
      if (!layout.empty()){
        circuit->mapBits(layout);
      }
      //std::cout<<circuit->toString()<<std::endl;
      //decoratedAccelerator->execute(tmpBuffer, circuit);
      circuits.push_back(circuit);
    }
    //std::cout<<tmpBuffer->size()<<std::endl;
    decoratedAccelerator->execute(tmpBuffer, circuits);
    auto buffers = tmpBuffer->getChildren();
    int shots = 0;
    // compute num shots;
    //std::cout<<tmpBuffer->nChildren()<<std::endl;
    for (auto &x : buffers[0]->getMeasurementCounts()) {
      shots += x.second;
    }

    //std::cout<<"num_shots = " << shots <<std::endl;
    // initializing vector of vector of counts to size 2^num_bits x 2^num_bits
    std::vector<std::vector<int>> counts(std::pow(2, num_bits),
                                         std::vector<int>(pow_bits));

    int row = 0;
    Eigen::MatrixXd K(pow_bits, pow_bits);
    K.setZero();
    for (auto &b : buffers) {
      //b->print();
      int col = 0;
      for (auto &x : permutations) {
        auto temp = b->computeMeasurementProbability(x);
        K(row, col) = temp;
        col++;
      }
      row++;
    }
    {
      std::stringstream ss;
      ss << "MATRIX:\n" << K << "\n";
      xacc::info(ss.str());
    }
    errorKernel = K.inverse();
    {
      std::stringstream ss;
      ss << "INVERSE:\n" << errorKernel << "\n";
      xacc::info(ss.str());
    }
    std::vector<double> vec(errorKernel.data(), errorKernel.data() + errorKernel.rows()*errorKernel.cols());
    buffer->addExtraInfo("error-kernel", vec);
    // No need to generate again
    gen_kernel = false;
  }

public:
  AssignmentErrorKernelDecorator() = default;
  
  const std::vector<std::string> configurationKeys() override {
    return {"gen-kernel"};
  }

  void initialize(const HeterogeneousMap &params = {}) override;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  const std::string name() const override { return "assignment-error-kernel"; }
  const std::string description() const override { return ""; }

  ~AssignmentErrorKernelDecorator() override {}
};

} // namespace quantum
} // namespace xacc
#endif
