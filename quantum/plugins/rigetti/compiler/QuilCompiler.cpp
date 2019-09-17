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
#include "QuilCompiler.hpp"
#include "QuilVisitor.hpp"
#include "QuilLexer.h"
#include "QuilParser.h"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "QuilToXACCListener.hpp"
#include "QuilErrorListener.hpp"
#include "xacc_service.hpp"

using namespace quil;
using namespace antlr4;

namespace xacc {

namespace quantum {

QuilCompiler::QuilCompiler() = default;

std::shared_ptr<IR> QuilCompiler::compile(const std::string &src,
                                          std::shared_ptr<Accelerator> acc) {

  ANTLRInputStream input(src);
  QuilLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  QuilParser parser(&tokens);
  parser.removeErrorListeners();
  auto e = new QuilErrorListener();
  parser.addErrorListener(e);

  auto ir = xacc::getService<IRProvider>("quantum")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  QuilToXACCListener listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  delete e;
  auto f = listener.getFunction();
  xacc::appendCompiled(f);

  ir->addComposite(f);
  return ir;
}
std::shared_ptr<IR> QuilCompiler::compile(const std::string &src) {
  return compile(src,nullptr);
}

const std::string QuilCompiler::translate(std::shared_ptr<CompositeInstruction> function) {
  auto visitor = std::make_shared<QuilVisitor>();
  InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  return visitor->getQuilString();
}

} // namespace quantum

} // namespace xacc
