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

bool XASMCompiler::canParse(const std::string &src) {

  class XASMThrowExceptionErrorListener : public BaseErrorListener {
  public:
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol,
                     size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
      std::stringstream ss;
      ss << "XASM Cannot parse this source: " << msg << "\n";
      ss << line << ": " << charPositionInLine << ", offending symbol = " << offendingSymbol->getText() << "\n";
    //   xacc::info(ss.str());
      throw std::runtime_error("Cannot parse this XASM source string.");
    }
  };

  ANTLRInputStream input(src);
  xasmLexer lexer(&input);
  lexer.removeErrorListeners();
  CommonTokenStream tokens(&lexer);
  xasmParser parser(&tokens);
  parser.removeErrorListeners();
  XASMThrowExceptionErrorListener el;
  parser.addErrorListener(&el);

  try {
    tree::ParseTree *tree = parser.xaccsrc();
    return true;
  } catch (std::exception &e) {
    return false;
  }
}

std::shared_ptr<IR> XASMCompiler::compile(const std::string &src,
                                          std::shared_ptr<Accelerator> acc) {
  ANTLRInputStream input(src);
  xasmLexer lexer(&input);
  lexer.removeErrorListeners();
  CommonTokenStream tokens(&lexer);
  xasmParser parser(&tokens);
  parser.removeErrorListeners();
  XASMErrorListener el;
  parser.addErrorListener(&el);

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

std::vector<std::string>
XASMCompiler::getKernelBufferNames(const std::string &src) {
  ANTLRInputStream input(src);
  xasmLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  xasmParser parser(&tokens);
  parser.removeErrorListeners();
  XASMErrorListener el;
  parser.addErrorListener(&el);

  auto ir = xacc::getService<IRProvider>("quantum")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  XASMListener listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return listener.getBufferNames();
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
