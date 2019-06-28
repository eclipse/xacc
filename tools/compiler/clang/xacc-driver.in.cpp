#include "clang/Frontend/FrontendAction.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

#include "clang/Tooling/Tooling.h"
#include <fstream>
#include <string>

#include "FuzzyParsingExternalSemaSource.hpp"

#include "XACCASTConsumer.hpp"
#include "XACC.hpp"
using namespace clang;

class XACCFrontendAction : public clang::ASTFrontendAction {

public:
  XACCFrontendAction(Rewriter &rw, const std::string file)
      : rewriter(rw), fileName(file) {}

protected:
  Rewriter &rewriter;
  std::string fileName;
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef /* dummy */) override {
    return llvm::make_unique<xacc::compiler::XACCASTConsumer>(Compiler,
                                                              rewriter);
  }

  void ExecuteAction() override {
    CompilerInstance &CI = getCompilerInstance();
    CI.createSema(getTranslationUnitKind(), nullptr);
    rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());

    xacc::compiler::FuzzyParsingExternalSemaSource source(CI.getASTContext());
    CI.getSema().addExternalSource(&source);

    ParseAST(CI.getSema());

    CI.getDiagnosticClient().EndSourceFile();

    std::string outName(fileName);
    size_t ext = outName.rfind(".");
    if (ext == std::string::npos)
      ext = outName.length();
    outName.insert(ext, "_out");
    outName = "." + outName;

    // llvm::errs() << "Output to: " << outName << "\n";
    std::error_code OutErrorInfo;
    std::error_code ok;
    llvm::raw_fd_ostream outFile(llvm::StringRef(outName), OutErrorInfo,
                                 llvm::sys::fs::F_None);

    if (OutErrorInfo == ok) {

      const RewriteBuffer *RewriteBuf =
          rewriter.getRewriteBufferFor(CI.getSourceManager().getMainFileID());
      outFile << std::string(RewriteBuf->begin(), RewriteBuf->end());
    } else {
      llvm::errs() << "Cannot open " << outName << " for writing\n";
    }

    outFile.close();
  }
};

int main(int argc, char **argv) {

  xacc::Initialize(argc, argv);

  // Get filename
  std::string fileName(argv[argc - 1]);
  if (!xacc::fileExists(fileName)) {
    xacc::error("File " + fileName + " does not exist.");
  }

  std::ifstream t(fileName);
  std::string src((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());

  // Initialize rewriter
  Rewriter Rewrite;

  auto action = new XACCFrontendAction(Rewrite, fileName);
  std::vector<std::string> args{
      "-std=c++11", "-I@CMAKE_INSTALL_PREFIX@/include/xacc",
      "-I@CMAKE_INSTALL_PREFIX@/include/cppmicroservices4"};

  if (!tooling::runToolOnCodeWithArgs(action, src, args)) {
    xacc::error("Error running qcor compiler.");
  }

  return 0;
}