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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ACCELERATORDECORATOR_HPP_
#define XACC_ACCELERATORDECORATOR_HPP_

#include "Accelerator.hpp"

namespace xacc {

// The AcceleratorDecorator implements the familiar decorator
// pattern for Accelerators. This enables delegation to a
// concrete Accelerator, with the opportunity to introduce general
// pre- and post-processing of execution input and output.
class AcceleratorDecorator : public Accelerator {
protected:
  std::shared_ptr<Accelerator> decoratedAccelerator;

public:
  AcceleratorDecorator() {}
  AcceleratorDecorator(std::shared_ptr<Accelerator> a)
      : decoratedAccelerator(a) {}
  void setDecorated(std::shared_ptr<Accelerator> a) {
    decoratedAccelerator = a;
  }
  std::shared_ptr<Accelerator> getDecorated() { return decoratedAccelerator; }

  void initialize(const HeterogeneousMap &params = {}) override {
    decoratedAccelerator->initialize(params);
  }
  void updateConfiguration(const HeterogeneousMap &config) override {
    decoratedAccelerator->updateConfiguration(config);
  }

  std::vector<std::pair<int, int>> getConnectivity() override {
    return decoratedAccelerator->getConnectivity();
  }
  HeterogeneousMap getProperties() override {
    return decoratedAccelerator->getProperties();
  }

  const std::string getSignature() override {
    return name() + "," + decoratedAccelerator->getSignature();
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> CompositeInstruction)
      override = 0;

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   CompositeInstructions) override = 0;

  bool isRemote() override { return decoratedAccelerator->isRemote(); }

  virtual ~AcceleratorDecorator() {}
};

} // namespace xacc
#endif
