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
#ifndef XACC_RICHEXTRAPDECORATOR_HPP_
#define XACC_RICHEXTRAPDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"

namespace xacc {

namespace quantum {

class RichExtrapDecorator : public AcceleratorDecorator {
public:
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<Function> function) override;

  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  const std::string name() const override { return "rich-extrap"; }
  const std::string description() const override { return ""; }

  std::shared_ptr<options_description> getOptions() override {
    auto desc = std::make_shared<options_description>();
    desc->add_options()("rich-extrap-r", value<std::string>(), "");
    return desc;
  }
  ~RichExtrapDecorator() override {}
};

} // namespace quantum
} // namespace xacc
#endif
