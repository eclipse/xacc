/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_STAQCOMPILER_HPP
#define XACC_STAQCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

class StaqCompiler : public xacc::Compiler {

protected:
  bool run_staq_optimize = true;
public:
  StaqCompiler();

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                    std::shared_ptr<Accelerator> acc) override;

  std::shared_ptr<xacc::IR> compile(const std::string &src) override;
  void setExtraOptions(const HeterogeneousMap options) override {
    if (options.keyExists<bool>("no-optimize") && options.get<bool>("no-optimize")) {
        run_staq_optimize = false;
    }
  }
  bool canParse(const std::string &src) override;

  const std::string
  translate(std::shared_ptr<CompositeInstruction> function) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> program,
                              HeterogeneousMap &options) override;

  const std::string name() const override { return "staq"; }

  const std::string description() const override { return ""; }

  virtual ~StaqCompiler() {}
};

} // namespace xacc
#endif
