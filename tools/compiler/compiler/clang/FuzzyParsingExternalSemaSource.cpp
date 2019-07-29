#include "FuzzyParsingExternalSemaSource.hpp"

#include "IRProvider.hpp"
#include "XACC.hpp"
#include "IRGenerator.hpp"
#include "xacc_service.hpp"

using namespace clang;

namespace xacc {
namespace compiler {

FuzzyParsingExternalSemaSource::FuzzyParsingExternalSemaSource(
    ASTContext &context)
    : m_Context(context) {
  auto irProvider = xacc::getService<xacc::IRProvider>("quantum");
  validInstructions = irProvider->getInstructions();
  validInstructions.push_back("CX");
  auto irgens = xacc::getRegisteredIds<xacc::IRGenerator>();
  for (auto &irg : irgens) {
    validInstructions.push_back(irg);
  }
}

bool FuzzyParsingExternalSemaSource::LookupUnqualified(clang::LookupResult &R,
                                                       clang::Scope *S) {
  DeclarationName Name = R.getLookupName();
  std::string unknownName = Name.getAsString();

  // If this is a valid quantum instruction, tell Clang its
  // all gonna be ok, we got this...
  if (std::find(validInstructions.begin(),
                validInstructions.end(),                   // not template scope
                unknownName) != validInstructions.end()) { //} &&
    //   S->getFlags() != 128 && S->getBlockParent() != nullptr) {

    // std::cout << "HELLO FP: " << unknownName << ", " << S->getFlags() <<
    // "\n"; S->dump(); S->getBlockParent()->dump();
    IdentifierInfo *II = Name.getAsIdentifierInfo();
    SourceLocation Loc = R.getNameLoc();
    auto fdecl = FunctionDecl::Create(
        m_Context, R.getSema().getFunctionLevelDeclContext(), Loc, Loc, Name,
        m_Context.DependentTy, 0, SC_None);

    Stmt *S = new (m_Context) NullStmt(Stmt::EmptyShell());

    fdecl->setBody(S);

    R.addDecl(fdecl);
    return true;
  }
  return false;
}
} // namespace compiler
} // namespace xacc