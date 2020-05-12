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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef IMPLS_RIGETTI_QUILC_HPP_
#define IMPLS_RIGETTI_QUILC_HPP_

#include "Compiler.hpp"
#include "Utils.hpp"

namespace xacc {
namespace quilc {


class Quilc : public xacc::Compiler {
public:
  Quilc();

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) override;
  
  bool canParse(const std::string& src) override;

  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override {return "";}

  const std::string name() const override { return "quilc"; }

  const std::string description() const override {
    return "";
  }

  virtual ~Quilc() {}
};

} // namespace quantum

} // namespace xacc

#endif
