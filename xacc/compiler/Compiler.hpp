/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef XACC_COMPILER_HPP_
#define XACC_COMPILER_HPP_

#include <memory>
#include "IR.hpp"
#include "Accelerator.hpp"

namespace xacc {

/**
 * The Compiler class provides an extensible interface
 * for injecting custom compilation mechanisms into the
 * XACC framework. Implementations provide a compile method
 * that takes the kernel source code string, performs
 * compiler-specific compilation mechanism, and returns a valid
 * XACC IR instance modeling the result of the compilation.
 */
class Compiler : public OptionsProvider, public Identifiable {

public:
  /**
   * This method is to be implemented by derived Compilers
   * and is in charge of executing the compilation mechanism
   * on the provided source string. Implementations also are
   * given access to the Accelerator that this source code is
   * intended for.
   *
   * @param src The kernel source string.
   * @param acc The Accelerator this code will be executed on
   * @return ir Intermediate representation for provided source kernel code.
   */
  virtual std::shared_ptr<IR> compile(const std::string &src,
                                      std::shared_ptr<Accelerator> acc) = 0;

  /**
   * This method is to be implemented by derived Compilers
   * and is in charge of executing the compilation mechanism
   * on the provided source string.
   * @param src
   * @return
   */
  virtual std::shared_ptr<IR> compile(const std::string &src) = 0;

  /**
   * This method is to be implemented by derived Compilers and
   * is in charge of taking the provided CompositeInstruction IR and converting
   * it to source code in this Compiler's language.
   *
   * @param CompositeInstruction The XACC IR CompositeInstruction to translate
   * @return src The source code as a string
   */
  virtual const std::string translate(const std::string &bufferVariable,
                                      std::shared_ptr<CompositeInstruction> CompositeInstruction) = 0;

  virtual const std::shared_ptr<CompositeInstruction>
  compile(std::shared_ptr<CompositeInstruction> f, std::shared_ptr<Accelerator> acc) {
    XACCLogger::instance()->info("Compiler::compile(CompositeInstruction, accelerator) not "
                                 "implemented. Returning given CompositeInstruction.");
    return f;
  }

  /**
   * Return an empty options_description, this is for
   * subclasses to implement.
   */
  virtual OptionPairs getOptions() {
    return OptionPairs{};
  }

  virtual bool handleOptions(const std::map<std::string,std::string> &arg_map) { return false; }

  /**
   * The destructor
   */
  virtual ~Compiler() {}

protected:
  /**
   * Reference to the provided kernel source code string
   */
  std::string kernelSource;

  /**
   * Reference to the Accelerator that this compiler is
   * targeting.
   */
  std::shared_ptr<Accelerator> accelerator;
};

} // namespace xacc
#endif
