
#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "XACCLangLexer.h"
#include "XACCLangParser.h"
#include "XACCLangCompiler.hpp"
#include "XACCLangErrorListener.hpp"
#include "XACCLangListener.hpp"

using namespace xacclang;
using namespace antlr4;

namespace xacc {


XACCLangCompiler::XACCLangCompiler() = default;

std::shared_ptr<IR> XACCLangCompiler::compile(const std::string &src,
                                           std::shared_ptr<Accelerator> acc) {
  accelerator = acc;
  return compile(src);
}

std::shared_ptr<IR> XACCLangCompiler::compile(const std::string &src) {
  ANTLRInputStream input(src);
  XACCLangLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  XACCLangParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new XACCLangErrorListener());

  auto ir = xacc::getService<IRProvider>("gate")->createIR();

  tree::ParseTree *tree = parser.xaccsrc();
  XACCLangListener listener(ir);
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return ir;
}

const std::string
XACCLangCompiler::translate(const std::string &bufferVariable,
                         std::shared_ptr<xacc::Function> function) {
  return "translate has not been implemented yet";
}
} // namespace xacc
