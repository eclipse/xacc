
#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "xasmLexer.h"
#include "xasmParser.h"
#include "xasm_compiler.hpp"
#include "xasm_errorlistener.hpp"
#include "xasm_listener.hpp"

using namespace xasm;
using namespace antlr4;

namespace xacc {


XASMCompiler::XASMCompiler() = default;

std::shared_ptr<IR> XASMCompiler::compile(const std::string &src,
                                           std::shared_ptr<Accelerator> acc) {
  accelerator = acc;
  return compile(src);
}

std::shared_ptr<IR> XASMCompiler::compile(const std::string &src) {
  ANTLRInputStream input(src);
  xasmLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  xasmParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new XASMErrorListener());

  auto ir = xacc::getService<IRProvider>("gate")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  XASMListener listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  auto f = listener.getFunction();
  if (f->name() != "tmp_lambda") xacc::appendCompiled(f);
  ir->addKernel(f);
  return ir;
}

const std::string
XASMCompiler::translate(const std::string &bufferVariable,
                         std::shared_ptr<xacc::Function> function) {
  xacc::error("translate has not been implemented yet");
  return "";
}
} // namespace xacc
