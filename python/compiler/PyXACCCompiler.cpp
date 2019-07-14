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
#include "PyXACCCompiler.hpp"
#include "PyXACCListener.hpp"
#include "PyXACCIRLexer.h"
#include "GateIR.hpp"
#include "InstructionIterator.hpp"
#include "PyXACCVisitor.hpp"

using namespace antlr4;
using namespace pyxacc;

namespace xacc {

namespace quantum {

std::shared_ptr<IR> PyXACCCompiler::compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) {

  auto e1 = new PyXACCErrorListener();
  auto e2 = new PyXACCErrorListener();

  // Setup the Antlr Parser
  ANTLRInputStream input(src);
  PyXACCIRLexer lexer(&input);
  lexer.removeErrorListeners();
  lexer.addErrorListener(e1);

  CommonTokenStream tokens(&lexer);
  PyXACCIRParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(e2);

  // Walk the Abstract Syntax Tree
  tree::ParseTree *tree = parser.xaccsrc();

  DWorGateListener l;
  tree::ParseTreeWalker::DEFAULT.walk(&l, tree);

  PyXACCListener listener(l.isDW, acc);
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  // Create and return the IR
  auto ir = std::make_shared<GateIR>();
  ir->addKernel(listener.getKernel());

  delete e1;
  delete e2;
  
  return ir;
}

std::shared_ptr<IR> PyXACCCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
PyXACCCompiler::translate(const std::string &bufferVariable,
                          std::shared_ptr<Function> function) {
  auto visitor = std::make_shared<PyXACCVisitor>(function->getParameters());
  xacc::InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }
  return visitor->getPyXACCString();
}
} // namespace quantum

} // namespace xacc
