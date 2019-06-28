#ifndef TOOLS_COMPILER_XACCASTCONSUMER_HPP_
#define TOOLS_COMPILER_XACCASTCONSUMER_HPP_

#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "FuzzyParsingExternalSemaSource.hpp"

namespace clang {
class CompilerInstance;
}
using namespace clang;

namespace xacc {
namespace compiler {
class XACCASTConsumer : public ASTConsumer {
public:
  XACCASTConsumer(CompilerInstance &c, Rewriter &rw);

  bool HandleTopLevelDecl(DeclGroupRef DR) override;

private:
  CompilerInstance &ci;
  std::shared_ptr<FuzzyParsingExternalSemaSource> fuzzyParser;
  Rewriter &rewriter;
};
} // namespace compiler
} // namespace xacc
#endif