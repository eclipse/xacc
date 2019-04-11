/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - H. Charles Zhao
 *
 **********************************************************************************/
#ifndef IMPLS_IBM_OQASMCOMPILER_HPP
#define IMPLS_IBM_OQASMCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

namespace quantum {

class OQASMCompiler : public xacc::Compiler {
public:
  OQASMCompiler();

  /**
   * Translate OpenQASM to the XACC intermediate representation.
   *
   * @return ir XACC intermediate representation
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc);

  /**
   *
   * @param src
   * @return ir
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

  /**
   * This produces an OpenQASM source code representation of the given IR
   * Function.
   *
   * @param function The XACC IR Function to translate
   * @return src The source code as a string
   */
  virtual const std::string translate(const std::string &bufferVariable,
                                      std::shared_ptr<Function> function);

  virtual const std::string name() const { return "openqasm"; }

  virtual const std::string description() const {
    return "The OpenQASM Compiler compiles kernels written in the OpenQASM "
           "intermediate language.";
  }

  /**
   * The destructor
   */
  virtual ~OQASMCompiler() {}
};

} // namespace quantum

} // namespace xacc
#endif
