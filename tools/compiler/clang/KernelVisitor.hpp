#ifndef TOOLS_COMPILER_KERNELASTVISITOR_HPP_
#define TOOLS_COMPILER_KERNELASTVISITOR_HPP_

#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include "XACCASTConsumer.hpp"

#include "IRGenerator.hpp"
#include "XACC.hpp"

using namespace xacc;
using namespace clang;

namespace xacc {
class IRProvider;
}
namespace xacc {
namespace compiler {

class KernelVisitor : public RecursiveASTVisitor<KernelVisitor> {
protected:
  class CppToXACCIRVisitor : public RecursiveASTVisitor<CppToXACCIRVisitor> {
  protected:
    std::shared_ptr<Function> function;
    std::shared_ptr<xacc::IRProvider> provider;
    std::vector<std::string> irGeneratorNames;

  public:
    CppToXACCIRVisitor(const std::string name);
    bool VisitCallExpr(CallExpr *expr);
    std::shared_ptr<Function> getFunction();
  };

  class CallExprToXACCInstructionVisitor
      : public RecursiveASTVisitor<CallExprToXACCInstructionVisitor> {
  protected:
    std::vector<int> bits;
    std::vector<InstructionParameter> parameters;
    std::string name;
    std::shared_ptr<IRProvider> provider;
    bool addMinus = false;

  public:
    CallExprToXACCInstructionVisitor(const std::string n,
                                     std::shared_ptr<IRProvider> p)
        : name(n), provider(p) {}
    std::shared_ptr<Instruction> getInstruction();
    bool VisitIntegerLiteral(IntegerLiteral *il);
    bool VisitUnaryOperator(UnaryOperator *op);
    bool VisitFloatingLiteral(FloatingLiteral *fl);
    bool VisitDeclRefExpr(DeclRefExpr *expr);
  };

  class CallExprToIRGenerator
      : public RecursiveASTVisitor<CallExprToIRGenerator> {
  protected:
    std::shared_ptr<IRProvider> provider;
    std::string name;
    std::map<std::string, InstructionParameter> options;
    bool haveSeenFirstDeclRef = false;
    bool haveSeenFirstInit = false;
    bool keepSearching = true;
    std::vector<Stmt *> immediate_children;

  public:
    CallExprToIRGenerator(const std::string n, std::shared_ptr<IRProvider> p)
        : name(n), provider(p) {}
    // bool VisitDeclRefExpr(DeclRefExpr* expr);
    bool VisitInitListExpr(InitListExpr *expr);
    bool VisitDeclRefExpr(DeclRefExpr *expr);
    std::shared_ptr<IRGenerator> getIRGenerator();
  };
  class ScanInitListExpr : public RecursiveASTVisitor<ScanInitListExpr> {
  protected:
    bool isFirstStringLiteral = true;
    bool isVectorValue;
    bool hasSeenFirstIL = false;
    bool skipSubInits = false;

  public:
    std::vector<int> intsFound;
    std::vector<double> realsFound;
    std::vector<std::string> stringsFound;

    std::string key;
    InstructionParameter value;
    ScanInitListExpr(bool isVecValued = false) : isVectorValue(isVecValued) {}
    bool VisitDeclRefExpr(DeclRefExpr *expr);
    bool VisitStringLiteral(StringLiteral *literal);
    bool VisitFloatingLiteral(FloatingLiteral *literal);
    bool VisitIntegerLiteral(IntegerLiteral *literal);
    bool VisitInitListExpr(InitListExpr *initList);
  };
  class HasSubInitListExpr : public RecursiveASTVisitor<HasSubInitListExpr> {
  public:
    bool hasSubInitLists = false;
    bool VisitInitListExpr(InitListExpr *initList) {
      hasSubInitLists = true;
      return true;
    }
  };
  class GetPairVisitor : public RecursiveASTVisitor<GetPairVisitor> {
  public:
    std::vector<int> intsFound;
    std::vector<double> realsFound;
    bool VisitFloatingLiteral(FloatingLiteral *literal);
    bool VisitIntegerLiteral(IntegerLiteral *literal);
  };

public:
  KernelVisitor(CompilerInstance &c, Rewriter &rw);

  bool VisitFunctionDecl(FunctionDecl *F);
  bool VisitCallExpr(CallExpr *expr);

private:
  CompilerInstance &ci;
  Rewriter &rewriter;
};
} // namespace compiler
} // namespace xacc
#endif
