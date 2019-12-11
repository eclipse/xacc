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
#ifndef XACC_DWDECORATOR_HPP_
#define XACC_DWDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"
#include "xacc_service.hpp"
#include <unordered_set>

namespace xacc {

namespace quantum {

class DWDecorator : public AcceleratorDecorator {
protected:
public:
  DWDecorator() {
  }

  void initialize(const HeterogeneousMap &params = {}) override {
    decoratedAccelerator = xacc::getService<Accelerator>("dwave-internal");
    decoratedAccelerator->initialize(params);
  }

  void updateConfiguration(const HeterogeneousMap &config) override {
    decoratedAccelerator->updateConfiguration(config);
  }
  const std::vector<std::string> configurationKeys() override { return {}; }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  const std::string name() const override { return "dwave"; }
  const std::string description() const override { return ""; }

  ~DWDecorator() override {}
};

} // namespace quantum
} // namespace xacc
#endif
