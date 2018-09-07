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
#ifndef PYTHON_PYXACCCOMPILER_HPP_
#define PYTHON_PYXACCCOMPILER_HPP_
#include "XACC.hpp"
#include "antlr4-runtime.h"

namespace xacc {

namespace quantum {

/**
 * An Antlr error listener for handling parsing errors.
 */
class PyXACCErrorListener : public antlr4::BaseErrorListener {
public:
  void syntaxError(antlr4::Recognizer *recognizer,
                   antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid PyXACC source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::error(output.str());
  }
};

/**
 * The PyXACCCompiler is an XACC Compiler that compiles
 * python-like gate instruction source code to produce a
 * XACC IR.
 */
class PyXACCCompiler : public xacc::Compiler {

public:
  /**
   * The Compiler.
   */
  PyXACCCompiler() {}

  /**
   * Compile the given kernel code for the
   * given Accelerator.
   *
   * @param src The source code
   * @param acc Reference to the D-Wave Accelerator
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc);

  /**
   * Compile the given kernel code.
   *
   * @param src The source code
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

  /**
   * Return the command line options for this compiler
   *
   * @return options Description of command line options.
   */
  virtual std::shared_ptr<options_description> getOptions() {
    auto desc =
        std::make_shared<options_description>("PyXACC Compiler Options");
    return desc;
  }

  virtual bool handleOptions(variables_map &map) { return false; }

  /**
   * We don't allow translations for the PyXACC Compiler.
   * @param bufferVariable
   * @param function
   * @return
   */
  virtual const std::string translate(const std::string &bufferVariable,
                                      std::shared_ptr<Function> function);

  virtual const std::string name() const { return "xacc-py"; }

  virtual const std::string description() const { return ""; }

  /**
   * The destructor
   */
  virtual ~PyXACCCompiler() {}
};

} // namespace quantum

} // namespace xacc

#endif
