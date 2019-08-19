#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "pyxasmLexer.h"
#include "pyxasmParser.h"
#include "pyxasm_compiler.hpp"
#include "pyxasm_errorlistener.hpp"
#include "pyxasm_listener.hpp"

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
  return compile(src,nullptr);
}

const std::string
PyXASMCompiler::translate(std::shared_ptr<xacc::CompositeInstruction> function) {
  xacc::error("translate has not been implemented yet");
  return "";
}
} // namespace xacc
