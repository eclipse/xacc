#ifndef TOOLS_COMPILER_FUZZYPARSINGEXTERNALSEMASOURCE_HPP_
#define TOOLS_COMPILER_FUZZYPARSINGEXTERNALSEMASOURCE_HPP_

#include "clang/AST/ASTContext.h"
#include "clang/Sema/ExternalSemaSource.h"
#include "clang/Sema/Lookup.h"

using namespace clang;

namespace xacc {
namespace compiler {
class FuzzyParsingExternalSemaSource : public ExternalSemaSource {
private:
  ASTContext &m_Context;
  std::vector<std::string> validInstructions;

public:
  FuzzyParsingExternalSemaSource(ASTContext &context);
  bool LookupUnqualified(clang::LookupResult &R, clang::Scope *S) override;
};
} // namespace compiler
} // namespace xacc
#endif