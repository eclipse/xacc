#include "IRProvider.hpp"
#include "XACC.hpp"
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
