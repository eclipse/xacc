/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_AQC_COMPILERS_DWQMICOMPILER_HPP_
#define QUANTUM_AQC_COMPILERS_DWQMICOMPILER_HPP_
#include "DWIR.hpp"
#include "EmbeddingAlgorithm.hpp"
#include "XACC.hpp"
#include "antlr4-runtime.h"
#include "xacc_service.hpp"

namespace xacc {

namespace quantum {

class DWQMIErrorListener : public antlr4::BaseErrorListener {
public:
  void syntaxError(antlr4::Recognizer *recognizer,
                   antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid DQWMI source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::error(output.str());
  }
};

/**
 * The DWQMICompiler is an XACC Compiler that compiles
 * D-Wave quantum machine instructions to produce an
 * appropriate Ising form for execution on the D-Wave QPU.
 *
 * This compilation leverages XACC EmbeddingAlgorithms to
 * compute the minor graph embedding represented by the
 * input source kernel code to output the embedded Ising
 * graph for D-Wave execution.
 */
class DWQMICompiler : public xacc::Compiler {

public:
  /**
   * The Compiler.
   */
  DWQMICompiler() {}

  /**
   * Compile the given kernel code for the
   * given D-Wave Accelerator.
   *
   * @param src The QMI source code
   * @param acc Reference to the D-Wave Accelerator
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc);

  /**
   * This method is not implemented - we must always have
   * D-Wave Accelerator connectivity information for compilation.
   *
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

  /**
   * Return the name of this Compiler
   * @return name Compiler name
   */
  virtual const std::string getName() { return "dwave-qmi"; }

  /**
   * Return the command line options for this compiler
   *
   * @return options Description of command line options.
   */
   OptionPairs getOptions() override {
    OptionPairs desc{{"dwave-embedding",
                        "Provide the name of the Embedding Algorithm to use "
                        "during compilation."},{
        "dwave-parameter-setter",
        "Provide the name of the "
        "ParameterSetter to map logical parameters to physical parameters."},{
        "dwave-load-embedding",
        "Use the embedding in the given file."},{
        "dwave-persist-embedding",
        "Persist the computed embedding to the given file name."},{
        "dwave-list-embedding-algorithms",
        "List all available embedding algorithms."}};
    return desc;
  }

  bool handleOptions(const std::map<std::string, std::string> &map) override {
    if (map.count("dwave-list-embedding-algorithms")) {
      auto ids = xacc::getRegisteredIds<EmbeddingAlgorithm>();
      for (auto i : ids) {
        xacc::info("Registered Embedding Algorithm: " + i);
      }
      return true;
    }
    return false;
  }

  const std::string translate(const std::string &bufferVariable,
                              std::shared_ptr<Function> function) override;

  const std::string name() const override { return "dwave-qmi"; }

  const std::string description() const override {
    return "The D-Wave QMI Compiler takes quantum machine instructions "
           "and performs minor graph embedding and parameter setting.";
  }

  /**
   * The destructor
   */
  virtual ~DWQMICompiler() {}
};

} // namespace quantum

} // namespace xacc

#endif
