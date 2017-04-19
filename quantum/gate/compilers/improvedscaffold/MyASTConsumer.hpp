/*
 * MyASTConsumer.hpp
 *
 *  Created on: Apr 19, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_COMPILERS_IMPROVEDSCAFFOLD_MYASTCONSUMER_HPP_
#define QUANTUM_GATE_COMPILERS_IMPROVEDSCAFFOLD_MYASTCONSUMER_HPP_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Frontend/DiagnosticOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

using namespace clang;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:

  bool VisitStmt(Stmt *s) {
    // Only care about If statements.
    if (isa<IfStmt>(s)) {
      IfStmt *IfStatement = cast<IfStmt>(s);
      Stmt *Then = IfStatement->getThen();

      IfStatement->dump();
    }

    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *f) {
    // Only function definitions (with bodies), not declarations.
    if (f->hasBody()) {
    }

    return true;
  }

};

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer();

  // Override the method that gets called for each parsed top-level
  // declaration.
  virtual bool HandleTopLevelDecl(DeclGroupRef DR);

  virtual ~MyASTConsumer() {}

private:
  std::shared_ptr<MyASTVisitor> Visitor;
};



#endif /* QUANTUM_GATE_COMPILERS_IMPROVEDSCAFFOLD_MYASTCONSUMER_HPP_ */
