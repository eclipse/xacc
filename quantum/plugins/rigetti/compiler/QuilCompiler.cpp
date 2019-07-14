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
 *   ANTLR compiler implementation - H. Charles Zhao
 *
 **********************************************************************************/
#include "QuilCompiler.hpp"
#include "QuilVisitor.hpp"
#include "QuilLexer.h"
#include "QuilParser.h"
#include "XACC.hpp"
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
  accelerator = acc;
  return compile(src);
}

std::shared_ptr<IR> QuilCompiler::compile(const std::string &src) {
  ANTLRInputStream input(src);
  QuilLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  QuilParser parser(&tokens);
  parser.removeErrorListeners();
  auto e = new QuilErrorListener();
  parser.addErrorListener(e);

  auto ir = xacc::getService<IRProvider>("gate")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  QuilToXACCListener listener(ir);
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  delete e;
  return ir;
}

const std::string QuilCompiler::translate(const std::string &bufferVariable,
                                          std::shared_ptr<Function> function) {
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
