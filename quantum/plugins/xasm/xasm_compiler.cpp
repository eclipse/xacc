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

#include "xasmLexer.h"
#include "xasmParser.h"
#include "xasm_compiler.hpp"
#include "xasm_errorlistener.hpp"
#include "xasm_listener.hpp"
#include "xasm_visitor.hpp"
#include "InstructionIterator.hpp"

using namespace xasm;
using namespace antlr4;

namespace xacc {

XASMCompiler::XASMCompiler() = default;

std::shared_ptr<IR> XASMCompiler::compile(const std::string &src,
                                          std::shared_ptr<Accelerator> acc) {
  ANTLRInputStream input(src);
  xasmLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  xasmParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new XASMErrorListener());

  auto ir = xacc::getService<IRProvider>("quantum")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  XASMListener listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  auto f = listener.getFunction();
  if (f->name() != "tmp_lambda")
    xacc::appendCompiled(f);

  if (acc) {
    f->set_accelerator_signature(acc->getSignature());
  }
  ir->addComposite(f);
  ir->setRuntimeVariables(listener.runtimeOptions);
  return ir;
}

std::shared_ptr<IR> XASMCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
XASMCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {
  auto visitor = std::make_shared<quantum::XasmVisitor>();
  InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  return "__qpu__ void " + function->name() + "(qbit q) {\n" +
         visitor->getXasmString() + "}";
}
} // namespace xacc
