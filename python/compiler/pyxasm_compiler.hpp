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
#ifndef XACC_PYXASMCOMPILER_HPP
#define XACC_PYXASMCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

class PyXASMCompiler : public xacc::Compiler {
public:
  PyXASMCompiler();

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) override;

  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override {
    HeterogeneousMap m;
    return translate(function, m);
  }
  const std::string
  translate(std::shared_ptr<CompositeInstruction> program, HeterogeneousMap& options) override;
  const std::string name() const override { return "pyxasm"; }

  const std::string description() const override {
    return "The PyXASM Compiler compiles kernels written in the Pythonic XACC assembly language.";
  }

  /**
   * The destructor
   */
  virtual ~PyXASMCompiler() {}
};


} // namespace xacc
#endif
