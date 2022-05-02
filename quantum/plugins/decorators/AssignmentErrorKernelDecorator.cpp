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
 *   Tyler Kharazi - initial implementation
 *   Thien Nguyen - implementation
 *******************************************************************************/
#include "AssignmentErrorKernelDecorator.hpp"
#include "AcceleratorBuffer.hpp"
#include "InstructionIterator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <fstream>
#include <set>
#include <Eigen/Dense>
#include <numeric>

namespace xacc {
namespace quantum {
void AssignmentErrorKernelDecorator::initialize(
    const HeterogeneousMap &params) {
  gen_kernel = true;
  if (params.keyExists<bool>("gen-kernel")) {
    gen_kernel = params.get<bool>("gen-kernel");
    xacc::info("gen_kernel: " + std::to_string(gen_kernel));
  }
  if (params.keyExists<bool>("multiplex")) {
    multiplex = params.get<bool>("multiplex");
    // this is essentially to tell me how to deal with layouts. If multiplex,
    // then we would split layout into two smaller layouts and we would generate
    // two error kernels.
  }
  if (params.keyExists<std::vector<std::size_t>>("layout")) {
    layout = params.get<std::vector<std::size_t>>("layout");
    xacc::info("layout received");
  }
  if (params.keyExists<std::vector<int>>("layout")) {
    auto tmp = params.get<std::vector<int>>("layout");
    std::stringstream ss;
    ss << "Running on physical bits: ";
    for (auto &a : tmp) {
      layout.push_back(a);
      ss << a << " ";
    }
    ss << std::endl;
    ss << "layout recieved" << std::endl;
    xacc::info(ss.str());
  }
} // initialize

void AssignmentErrorKernelDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {
  int num_bits = buffer->size();
  int size = std::pow(2, num_bits);
  if (!layout.empty()) {
    function->mapBits(layout);
  }
  
  // Handle circuits that don't measure all qubits...
  std::vector<int> measured_bits;
  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      if (nextInst->name() == "Measure") {
        measured_bits.emplace_back(nextInst->bits()[0]);
        // Mark to remove, since we'll add measure all
        nextInst->disable();
      }
    }
  }
  function->removeDisabled();
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  for (int q = 0; q < buffer->size(); ++q) {
    function->addInstruction(gateRegistry->createInstruction("Measure", q));
  }
  // get the raw state
  decoratedAccelerator->execute(buffer, function);
  const auto originalCountMap = buffer->getMeasurementCounts();
  const int shots = std::accumulate(
      originalCountMap.begin(), originalCountMap.end(), 0,
      [](const int previous, const auto &p) { return previous + p.second; });
  Eigen::VectorXd init_state(size);
  init_state.setZero();
  for (auto &x : buffer->getMeasurementCounts()) {
    // std::cout << "HELLO: " << x.first << ", " << x.second << std::endl;
    // Convert bitstring to int
    const int bitStrAsInt = stoi(x.first, 0, 2);
    assert(bitStrAsInt < size);
    init_state(bitStrAsInt) = double(x.second);
    // std::cout<<init_state(i)<<std::endl;
  }
  //std::cout << "BEFORE: " << init_state.transpose() << "\n";
  init_state = (double)1 / shots * init_state;
  //std::cout<<"num_shots = "<<shots<<std::endl;
  //std::cout<<"INIT STATE:\n"<<init_state<<std::endl;
  if (gen_kernel) {
    if (decoratedAccelerator) {
      generateKernel(buffer);
    }
  } else {
    // generating the list of permutations is O(2^num_bits), we want to minimize
    // the number of times we have to call it.
    if (permutations.empty()) {
      permutations = generatePermutations(num_bits);
    }
  }

  Eigen::VectorXd EM_state = errorKernel * init_state;
  {
    std::stringstream ss;
    ss << "Error Kernel: \n" << errorKernel << std::endl;
    ss << "Init: " << init_state << std::endl;
    ss << "EM: " << EM_state << std::endl;
    xacc::info(ss.str());
  }

  // checking for negative values and performing a "clip and renorm"
  for (int i = 0; i < EM_state.size(); i++) {
    // Clip
    if (EM_state(i) < 0.0) {
      EM_state(i) = 0.0;
    }
  }
  // Renorm
  const double sumProbs = EM_state.sum();
  EM_state = EM_state * (1.0 / sumProbs);

  {
    std::stringstream ss;
    ss << "Normailized EM_state:" << std::endl << EM_state << std::endl;
    xacc::info(ss.str());
  }

  // update buffer with new counts and save original counts to extra info
  std::map<std::string, double> origCounts;
  int total = 0;
  int i = 0;
  for (auto &x : permutations) {
    if (originalCountMap.find(x) != originalCountMap.end()) {
      origCounts[x] = (double)buffer->getMeasurementCounts()[x];
    }
    int count = floor(shots * EM_state(i) + 0.5);
    // std::cout << "EM_state = " << EM_state(i) << std::endl;
    total += count;
    // std::cout << "saving " << count << " counts in slot: " << x << std::endl;
    buffer->appendMeasurement(x, count);
    i++;
  }

  // Convert mitigated distribution to marginal distribution if we're only
  // measuring a subset of all qubits.
  if (measured_bits.size() < buffer->size()) {
    const auto bitOrder =
        decoratedAccelerator->getBitOrder() == xacc::Accelerator::BitOrder::MSB
            ? xacc::AcceleratorBuffer::BitOrder::MSB
            : xacc::AcceleratorBuffer::BitOrder::LSB;
    const auto marginalCounts =
        buffer->getMarginalCounts(measured_bits, bitOrder);
    buffer->clearMeasurements();
    buffer->setMeasurements(marginalCounts);
  }
  assert(total == shots);
  // std::cout<<origCounts<<std::endl;
  buffer->addExtraInfo("unmitigated-counts", origCounts);

  return;
} // execute

void AssignmentErrorKernelDecorator::execute(
    const std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  for (auto &f : functions) {
    auto tmp_buffer = xacc::qalloc(buffer->size());
    tmp_buffer->setName(f->name());
    execute(tmp_buffer, f);
    buffer->appendChild(f->name(), tmp_buffer);
  }
} // execute (vectorized)
} // namespace quantum
} // namespace xacc
