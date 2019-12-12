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
#include "xacc.hpp"

namespace xacc {

namespace quantum {

class AssignmentErrorKernelDecorator : public AcceleratorDecorator {
protected:
  bool gen_kernel;
  Eigen::MatrixXd errorKernel;
  std::vector<std::string> permutations = {""};
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
    this->permutations =
        generatePermutations(num_bits);

    // permutations contains all possible permutationss to generate circuits,
    // there is direct mapping from permutations to circuit as follows:
    // permutations: 10 => X gate on zeroth qubit and nothing on first qubit and
    // measure both permutations: 11 => X gate on both qubits and measure both
    // etc...
    std::shared_ptr<AcceleratorBuffer> tmpBuffer = xacc::qalloc(buffer->size());
    // list of circuits to evaluate
    std::vector<std::shared_ptr<CompositeInstruction>> circuits;
    auto provider = xacc::getIRProvider("quantum");
    for (int i = 0; i < pow_bits; i++) {
      auto circuit = provider->createComposite(permutations[i]);
      int j = num_bits - 1;
      for (char c : permutations[i]) {
        if (c == '1') {
          // std::cout<<"1 found at position: "<<j<<std::endl;
          auto x = provider->createInstruction("X", j);
          circuit->addInstruction(x);
        }
        j--;
      }
      auto m0 = provider->createInstruction("Measure", 0);
      auto m1 = provider->createInstruction("Measure", 1);
      circuit->addInstruction(m0);
      circuit->addInstruction(m1);
      circuits.push_back(circuit);
    }
    decoratedAccelerator->execute(tmpBuffer, circuits);
    // std::cout<<"here"<<std::endl;
    auto buffers = tmpBuffer->getChildren();

    int shots = 0;
    // compute num shots;
    for (auto &x : buffers[0]->getMeasurementCounts()) {
      shots += x.second;
    }
    // std::cout<<"num_shots = " << shots <<std::endl;
    // initializing vector of vector of counts to size 2^num_bits x 2^num_bits
    std::vector<std::vector<int>> counts(std::pow(2, num_bits),
                                         std::vector<int>(pow_bits));

    int row = 0;
    Eigen::MatrixXd K(pow_bits, pow_bits);
    for (auto &b : buffers) {
      int col = 0;
      for (auto &x : permutations) {
        auto temp = b->computeMeasurementProbability(x);
        K(row, col) = temp;
        col++;
      }
      row++;
    }
    AssignmentErrorKernelDecorator::errorKernel = K.inverse();
    // std::cout<<"error Kernel:"<<std::endl;
    // std::cout<<std::endl<<errorKernel<<std::endl;
    gen_kernel = false;
  }

public:
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
