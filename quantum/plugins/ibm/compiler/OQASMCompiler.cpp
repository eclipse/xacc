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
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "OpenQasmVisitor.hpp"

#include "OQASM2Lexer.h"
#include "OQASM2Parser.h"
#include "OQASMCompiler.hpp"
#include "OQASMErrorListener.hpp"
#include "OQASMToXACCListener.hpp"

using namespace oqasm;
using namespace antlr4;

namespace xacc {

namespace quantum {

OQASMCompiler::OQASMCompiler() = default;

std::shared_ptr<IR> OQASMCompiler::compile(const std::string &src,
                                           std::shared_ptr<Accelerator> acc) {
  ANTLRInputStream input(src);
  OQASM2Lexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  OQASM2Parser parser(&tokens);
  parser.removeErrorListeners();
  auto e = new OQASMErrorListener;
  parser.addErrorListener(e);

  auto ir = xacc::getService<IRProvider>("quantum")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  OQASMToXACCListener listener(ir);
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  delete e;
  return ir;
  //   accelerator = acc;
  //   return compile(src);
}

std::shared_ptr<IR> OQASMCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
OQASMCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {
  // Get the number of qubits
  int maxBit = 0;
  InstructionIterator temp(function);
  while (temp.hasNext()) {
    auto nextInst = temp.next();
    if (nextInst->isEnabled()) {
      for (auto &b : nextInst->bits()) {
        if (b > maxBit) {
          maxBit = b;
        }
      }
    }
  }
  auto nQubits = maxBit + 1;

  auto visitor = std::make_shared<OpenQasmVisitor>(nQubits);
  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }
  return visitor->getOpenQasmString();
}
} // namespace quantum
} // namespace xacc
