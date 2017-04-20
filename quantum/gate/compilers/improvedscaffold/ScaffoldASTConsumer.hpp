/*
 * MyASTConsumer.hpp
 *
 *  Created on: Apr 19, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_GATE_COMPILERS_IMPROVEDSCAFFOLD_SCAFFOLDASTCONSUMER_HPP_
#define QUANTUM_GATE_COMPILERS_IMPROVEDSCAFFOLD_SCAFFOLDASTCONSUMER_HPP_

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Type.h"

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

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

//#include "GateQIR.hpp"
//
#include "GateFunction.hpp"

using namespace clang;

namespace scaffold {

class ScaffoldASTConsumer: public clang::ASTConsumer,
		public clang::RecursiveASTVisitor<ScaffoldASTConsumer> {
public:

	// Override the method that gets called for each parsed top-level
	// declaration.
	virtual bool HandleTopLevelDecl(DeclGroupRef DR);

	bool VisitStmt(clang::Stmt *s);
	bool VisitDecl(clang::Decl *d);
	bool VisitCallExpr(CallExpr * c);
	bool VisitBinaryOperator(BinaryOperator *b);
	auto getFunction() {
		return function;
	}

	virtual ~ScaffoldASTConsumer() {
	}

protected:

	std::string cbitVarName;
	std::string qbitVarName;

	int currentInstId = 0;
	int currentInstLayer = 0;
	std::shared_ptr<xacc::quantum::GateFunction> function;

	int nCallExprToSkip = 0;
};

}

#endif
