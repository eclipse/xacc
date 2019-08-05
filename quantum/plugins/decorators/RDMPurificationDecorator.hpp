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
#ifndef XACC_RDMPURIFICATIONDECORATOR_HPP_
#define XACC_RDMPURIFICATIONDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"

namespace xacc {

namespace vqe {

class RDMPurificationDecorator : public AcceleratorDecorator {
protected:
  std::shared_ptr<Observable> fermionObservable;

public:
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<Function> function) override;

  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  void initialize(AcceleratorParameters params = {}) override;

  const std::string name() const override { return "rdm-purification"; }
  const std::string description() const override { return ""; }

  OptionPairs getOptions() override {
    OptionPairs desc{{"rdm-source", ""}, {"rdm-qubit-map", ""}};
    return desc;
  }
  ~RDMPurificationDecorator() override {}
};

} // namespace vqe
} // namespace xacc
#endif
