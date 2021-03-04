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
#ifndef IMPLS_IBM_QOBJECTCOMPILER_HPP
#define IMPLS_IBM_QOBJECTCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

namespace quantum {

class QObjectCompiler : public xacc::Compiler {
public:
  QObjectCompiler() = default;

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                    std::shared_ptr<Accelerator> acc) override;
  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override;
  const std::string translate(std::shared_ptr<CompositeInstruction> function,
                              HeterogeneousMap &options) override;
  const std::string name() const override { return "qobj"; }
  const std::string description() const override {
    return "The QObject Compiler compiles an IBM QObject (represented as a "
           "JSON string) and maps it to the XACC IR.";
  }

  /**
   * The destructor
   */
  ~QObjectCompiler() override {}
};

} // namespace quantum

} // namespace xacc
#endif
