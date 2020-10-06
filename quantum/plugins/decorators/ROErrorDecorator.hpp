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
#ifndef XACC_ROERRORDECORATOR_HPP_
#define XACC_ROERRORDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"

namespace xacc {

namespace quantum {

class ROErrorDecorator : public AcceleratorDecorator {
protected:

   std::string roErrorFile = "";

public:

  void initialize(const HeterogeneousMap &params = {}) override {
    if (params.stringExists("file")) {
        roErrorFile = params.getString("file");
    }
  }

  void updateConfiguration(const HeterogeneousMap &config) override {
    decoratedAccelerator->updateConfiguration(config);
    if (config.stringExists("file")) {
        roErrorFile = config.getString("file");
    }
  }
  const std::vector<std::string> configurationKeys() override {
      return {"file"};
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  const std::string name() const override { return "ro-error"; }
  const std::string description() const override { return ""; }

  ~ROErrorDecorator() override {}
};

}
} // namespace xacc
#endif
