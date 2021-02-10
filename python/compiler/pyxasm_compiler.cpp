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

#include "pyxasmLexer.h"
#include "pyxasmParser.h"
#include "pyxasm_compiler.hpp"
#include "pyxasm_errorlistener.hpp"
#include "pyxasm_listener.hpp"

#include "InstructionIterator.hpp"
#include "pyxasm_visitor.hpp"

using namespace pyxasm;
using namespace antlr4;

namespace xacc {

PyXASMCompiler::PyXASMCompiler() = default;

std::shared_ptr<IR> PyXASMCompiler::compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) {
  ANTLRInputStream input(src);
  pyxasmLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  pyxasmParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new PyXASMErrorListener());

  auto ir = xacc::getService<IRProvider>("quantum")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  PyXASMListener listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  auto f = listener.getFunction();
  xacc::appendCompiled(f);
  ir->addComposite(f);
  return ir;
}

std::shared_ptr<IR> PyXASMCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string
PyXASMCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> program,
                          HeterogeneousMap &options) {

  std::string qreg_name = "__translate_qrg__";
  std::string tab_prepend = "";
  if (options.stringExists("qreg_name")) {
    qreg_name = options.getString("qreg_name");
  }
  if (options.stringExists("tab_prepend")) {
    tab_prepend = options.getString("tab_prepend");
  }

  auto visitor = std::make_shared<quantum::PyXACCVisitor>(qreg_name, tab_prepend);
  InstructionIterator it(program);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled() && !nextInst->isComposite()) {
      nextInst->accept(visitor);
    }
  }
  return visitor->getPyXACCString();
}
} // namespace xacc
