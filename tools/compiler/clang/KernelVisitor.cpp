#include "KernelVisitor.hpp"
#include "IRGenerator.hpp"
#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/Sema.h"
#include "clang/Tooling/Tooling.h"
#include <memory>

using namespace clang;
using namespace xacc;

namespace xacc {
namespace compiler {

KernelVisitor::KernelVisitor(CompilerInstance &c, Rewriter &rw)
    : ci(c), rewriter(rw) {}

bool KernelVisitor::VisitCallExpr(CallExpr *expr) {
  auto F = expr->getDirectCallee();
  if (F != nullptr && F->hasAttr<AnnotateAttr>() &&
      F->getAttr<AnnotateAttr>()->getAnnotation().str() == "__qpu__") {

    auto fdeclName = expr->getDirectCallee()->getNameAsString();
     if (F->getAttrs().size() > 1 &&
        dyn_cast<AnnotateAttr>(F->getAttrs()[1])
                ->getAnnotation()
                .str()
                .find("observe_") != std::string::npos) {
      auto obsName =
          dyn_cast<AnnotateAttr>(F->getAttrs()[1])->getAnnotation().str();
      obsName = xacc::split(obsName, '_')[1];
      obsName = obsName.substr(1, obsName.length() - 2);

      auto endl = expr->getEndLoc();
      rewriter.ReplaceText(endl, 1,
                           ","+obsName+")");
    }
  }
  return true;
}

bool KernelVisitor::VisitFunctionDecl(FunctionDecl *F) {

  if (F->hasAttr<AnnotateAttr>() &&
      F->getAttr<AnnotateAttr>()->getAnnotation().str() == "__qpu__") {

    F->dump();

    CppToXACCIRVisitor visitor(F->getNameAsString());
    visitor.TraverseDecl(F);

    auto function = visitor.getFunction();
    // std::cout << "XACCIR:\n" << function->toString() << "\n";

    if (!xacc::optionExists("accelerator")) {
      if (xacc::hasAccelerator("tnqvm")) {
        xacc::setOption("accelerator", "tnqvm");
      } else if (xacc::hasAccelerator("local-ibm")) {
        xacc::setOption("accelerator", "local-ibm");
      } else {
        xacc::error("No Accelerator specified for compilation. Compile with "
                    "--accelerator and ensure you have the desired Accelerator "
                    "installed.");
      }
    }

    auto acceleratorName = xacc::getAccelerator()->name();

    // Here we need to do Quantum Compilation
    // ... get Accelerator IRTransformations
    // ... run hardware independent transforms too
    //

    function->setOption("compiled_for", acceleratorName);

    std::stringstream ss;
    function->persist(ss);
    auto irStr = ss.str();

    auto bufferName = F->getParamDecl(0)->getNameAsString();
    auto sr = F->getBody()->getSourceRange();
    std::string replacement = "{\n";
    replacement += bufferName + "->resetBuffer();\n";
    replacement += "auto irstr = R\"irstr(" + irStr + ")irstr\";\n";
    replacement += "auto function = xacc::loadFromIR(irstr);\n";

    if (F->getNumParams() > 1) {
      replacement +=
          "std::vector<double> params{" + F->getParamDecl(1)->getNameAsString();
      for (int i = 2; i < F->getNumParams(); i++) {
        replacement += "," + F->getParamDecl(i)->getNameAsString();
      }
      replacement += "};\n";
    }
    replacement +=
        "auto acc = xacc::getAccelerator(\"" + acceleratorName + "\");\n";
    if (F->getAttrs().size() > 1 &&
        dyn_cast<AnnotateAttr>(F->getAttrs()[1])
                ->getAnnotation()
                .str()
                .find("observe_") != std::string::npos) {
      auto obsName =
          dyn_cast<AnnotateAttr>(F->getAttrs()[1])->getAnnotation().str();
      obsName = xacc::split(obsName, '_')[1];
      obsName = obsName.substr(1, obsName.length() - 2);

      replacement +=
          "xacc::observe(" + bufferName + ", params, acc, function, obs);\n";
      auto endl = F->getParamDecl(F->getNumParams() - 1)->getEndLoc();
      rewriter.ReplaceText(endl.getLocWithOffset(2), 1,
                           ", std::shared_ptr<xacc::Observable> obs)");

    } else {
      replacement += "function = function->operator()(params);\n";
      replacement += "acc->execute(" + bufferName + ",function);\n";
    }
    replacement += "}\n";

    rewriter.ReplaceText(sr, replacement);

    xacc::appendCompiled(function);
  }

  return true;
}

KernelVisitor::CppToXACCIRVisitor::CppToXACCIRVisitor(const std::string name) {
  provider = xacc::getService<IRProvider>("quantum");
  function = provider->createFunction(name, {}, {InstructionParameter("gate")});
  auto irgens = xacc::getRegisteredIds<xacc::IRGenerator>();
  for (auto &irg : irgens) {
    irGeneratorNames.push_back(irg);
  }
}

bool KernelVisitor::CppToXACCIRVisitor::VisitCallExpr(CallExpr *expr) {
  std::string gate_name = "";
  auto firstchild = *(expr->child_begin());
  if (dyn_cast<DeclRefExpr>(firstchild)) {
    gate_name = dyn_cast<DeclRefExpr>(*(expr->child_begin()))
                    ->getNameInfo()
                    .getAsString();
  } else if (dyn_cast<ImplicitCastExpr>(firstchild)) {
    // For this case, we have a reference to a previous __qpu__ function
    auto ice = dyn_cast<ImplicitCastExpr>(firstchild);
    auto irFunctionName =
        dyn_cast<DeclRefExpr>(ice->getSubExpr())->getNameInfo().getAsString();
    auto irFunction = xacc::getCompiled(irFunctionName);
    function->addInstruction(irFunction);
    return true;
  }

  if (std::find(irGeneratorNames.begin(), irGeneratorNames.end(), gate_name) !=
      irGeneratorNames.end()) {

    // This is an IRGenerator
    // Map this CallExpr to an IRGenerator
    CallExprToIRGenerator visitor(gate_name, provider);
    visitor.TraverseStmt(expr);
    auto irg = visitor.getIRGenerator();
    if (irg->validateOptions()) {
      auto generated =
          irg->generate(std::map<std::string, InstructionParameter>{});
      for (auto inst : generated->getInstructions()) {
        function->addInstruction(inst);
      }
    } else {
      function->addInstruction(irg);
    }
  } else {
    // This is a regular gate
    // Map this Call Expr to a Instruction
    if (gate_name == "CX") {
      gate_name = "CNOT";
    }
    CallExprToXACCInstructionVisitor visitor(gate_name, provider);
    visitor.TraverseStmt(expr);
    auto inst = visitor.getInstruction();
    function->addInstruction(inst);
  }

  return true;
}

bool KernelVisitor::CallExprToXACCInstructionVisitor::VisitIntegerLiteral(
    IntegerLiteral *il) {
  if (name == "anneal") {
    int i = il->getValue().getLimitedValue();
    InstructionParameter p(i);
    parameters.push_back(p);
  } else {
    // std::cout << "INT LITERAL\n";
    bits.push_back(il->getValue().getLimitedValue());
    if (name == "Measure") {
      InstructionParameter p(bits[0]);
      parameters.push_back(p);
    }
  }
  return true;
}

bool KernelVisitor::CallExprToXACCInstructionVisitor::VisitUnaryOperator(
    UnaryOperator *op) {
  if (op->getOpcode() == UnaryOperator::Opcode::UO_Minus) {
    addMinus = true;
  }
  return true;
}

bool KernelVisitor::CallExprToXACCInstructionVisitor::VisitFloatingLiteral(
    FloatingLiteral *literal) {
  double value = literal->getValue().convertToDouble();
  InstructionParameter p(addMinus ? -1.0 * value : value);
  addMinus = false;
  parameters.push_back(p);
  return true;
}

bool KernelVisitor::CallExprToXACCInstructionVisitor::VisitDeclRefExpr(
    DeclRefExpr *decl) {
  auto declName = decl->getNameInfo().getAsString();
  if (addMinus) {
    declName = "-" + declName;
  }

  if (dyn_cast<ParmVarDecl>(decl->getDecl())) {
    // std::cout << "PARMVARDECL\n";
    parameters.push_back(InstructionParameter(declName));
  } else if (dyn_cast<VarDecl>(decl->getDecl())) {
    // std::cout << "THIS IS A VARDECL: " << declName << "\n";
    parameters.push_back(InstructionParameter(declName));
  }

  return true;
}

std::shared_ptr<Instruction>
KernelVisitor::CallExprToXACCInstructionVisitor::getInstruction() {
  return provider->createInstruction(name, bits, parameters);
}

bool KernelVisitor::CallExprToIRGenerator::VisitInitListExpr(
    InitListExpr *expr) {
  if (haveSeenFirstInit && keepSearching) {

    for (auto child : immediate_children) {
      ScanInitListExpr visitor;
      visitor.TraverseStmt(child);
      options.insert({visitor.key, visitor.value});
      //   std::cout << "Inserting " << visitor.key << ", "
      //             << visitor.value.toString() << "\n";
    }

    keepSearching = false;

  } else {
    haveSeenFirstInit = true;
    auto children = expr->children();
    for (auto it = children.begin(); it != children.end(); ++it) {
      immediate_children.push_back(*it);
    }
  }
  return true;
}

bool KernelVisitor::CallExprToIRGenerator::VisitDeclRefExpr(DeclRefExpr *decl) {

  if (dyn_cast<ParmVarDecl>(decl->getDecl())) {
    auto declName = decl->getNameInfo().getAsString();
    options.insert({"param-id", declName});
  }
  return true;
}
std::shared_ptr<IRGenerator>
KernelVisitor::CallExprToIRGenerator::getIRGenerator() {
  auto irg = xacc::getService<IRGenerator>(name);
  for (auto &kv : options) {
    irg->setOption(kv.first, kv.second);
  }
  return irg;
}

bool KernelVisitor::ScanInitListExpr::VisitDeclRefExpr(DeclRefExpr *expr) {
  value = InstructionParameter(expr->getNameInfo().getAsString());
  return true;
}
bool KernelVisitor::ScanInitListExpr::VisitInitListExpr(InitListExpr *expr) {

  if (skipSubInits) {
    return true;
  }

  if (hasSeenFirstIL) {
    HasSubInitListExpr visitor;
    visitor.TraverseStmt(*expr->children().begin());
    if (visitor.hasSubInitLists) {
      isVectorValue = true;
      // this is a vector of pairs or doubles.

      GetPairVisitor visitor;
      visitor.TraverseStmt(expr);
      if (!visitor.intsFound.empty()) {
        std::vector<std::pair<int, int>> tmp;
        for (int i = 0; i < visitor.intsFound.size(); i += 2) {
          tmp.push_back({visitor.intsFound[i], visitor.intsFound[i + 1]});
        }
        value = InstructionParameter(tmp);
      } else if (!visitor.realsFound.empty()) {
        std::vector<std::pair<double, double>> tmp;
        for (int i = 0; i < visitor.realsFound.size(); i += 2) {
          tmp.push_back({visitor.realsFound[i], visitor.realsFound[i + 1]});
        }
        value = InstructionParameter(tmp);
      } else {
        xacc::error("invalid vector<pair> type for IRGenerator options.");
      }

      skipSubInits = true;
    } else {

      // this is a vector...
      ScanInitListExpr visitor(true);
      visitor.TraverseStmt(expr);
      if (!visitor.intsFound.empty()) {
        value = InstructionParameter(visitor.intsFound);
      } else if (!visitor.realsFound.empty()) {
        value = InstructionParameter(visitor.realsFound);
      } else if (!visitor.stringsFound.empty()) {
        value = InstructionParameter(visitor.stringsFound);
      } else {
        xacc::error("invalid vector type for IRGenerator options.");
      }
    }
  } else {
    hasSeenFirstIL = true;
  }
  return true;
}

bool KernelVisitor::ScanInitListExpr::VisitStringLiteral(
    StringLiteral *literal) {

  if (isVectorValue) {
    stringsFound.push_back(literal->getString().str());
  } else {
    if (isFirstStringLiteral) {
      isFirstStringLiteral = false;
      key = literal->getString().str();
    } else {
      value = InstructionParameter(literal->getString().str());
    }
  }
  return true;
}

bool KernelVisitor::ScanInitListExpr::VisitFloatingLiteral(
    FloatingLiteral *literal) {

  if (isVectorValue) {

    realsFound.push_back(literal->getValue().convertToDouble());

  } else {
    value = InstructionParameter(literal->getValue().convertToDouble());
  }
  return true;
}

bool KernelVisitor::ScanInitListExpr::VisitIntegerLiteral(
    IntegerLiteral *literal) {

  if (isVectorValue) {

    intsFound.push_back((int)literal->getValue().getLimitedValue());

  } else {
    value = InstructionParameter((int)literal->getValue().getLimitedValue());
  }
  return true;
}

bool KernelVisitor::GetPairVisitor::VisitFloatingLiteral(
    FloatingLiteral *literal) {
  realsFound.push_back(literal->getValue().convertToDouble());
  return true;
}
bool KernelVisitor::GetPairVisitor::VisitIntegerLiteral(
    IntegerLiteral *literal) {
  intsFound.push_back((int)literal->getValue().getLimitedValue());
  return true;
}

std::shared_ptr<Function> KernelVisitor::CppToXACCIRVisitor::getFunction() {
  return function;
}

} // namespace compiler
} // namespace xacc

// SourceManager &SM = ci.getSourceManager();
// LangOptions &lo = ci.getLangOpts();
// lo.CPlusPlus11 = 1;
// auto xaccKernelStr =
//     Lexer::getSourceText(
//         CharSourceRange(F->getBody()->getSourceRange(), true), SM, lo)
//         .str();

// std::cout << "Check it out, I got the Lambda as a source string :)\n";
// std::cout << xaccKernelStr << "\n";