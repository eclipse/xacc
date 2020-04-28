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
#ifndef IMPLS_RIGETTI_QUILCOMPILER_HPP_
#define IMPLS_RIGETTI_QUILCOMPILER_HPP_

#include "Compiler.hpp"
#include "Utils.hpp"

namespace xacc {

namespace quantum {

/**
 */
class QuilCompiler : public xacc::Compiler {

public:
  QuilCompiler();

  /**
   * Translate Quil to the
   * XACC intermediate representation.
   *
   * @return ir XACC intermediate representation
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc);
  bool canParse(const std::string& src) override;

  /**
   *
   * @param src
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

  /**
   * This produces a Quil source code representation of the
   * given IR Function
   *
   * @param function The XACC IR Function to translate
   * @return src The source code as a string
   */
  virtual const std::string translate(std::shared_ptr<CompositeInstruction> function);

  virtual const std::string name() const { return "quil"; }

  virtual const std::string description() const {
    return "The Quil Compiler compiles kernels written in the Quil "
           "intermediate language.";
  }

  /**
   * The destructor
   */
  virtual ~QuilCompiler() {}
};

} // namespace quantum

} // namespace xacc

#endif
