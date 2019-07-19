
// Generated from XACCLang.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "XACCLangListener.h"


namespace xacclang {

/**
 * This class provides an empty implementation of XACCLangListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  XACCLangBaseListener : public XACCLangListener {
public:

  virtual void enterXaccsrc(XACCLangParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(XACCLangParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(XACCLangParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(XACCLangParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterTypedparam(XACCLangParser::TypedparamContext * /*ctx*/) override { }
  virtual void exitTypedparam(XACCLangParser::TypedparamContext * /*ctx*/) override { }

  virtual void enterType(XACCLangParser::TypeContext * /*ctx*/) override { }
  virtual void exitType(XACCLangParser::TypeContext * /*ctx*/) override { }

  virtual void enterKernelcall(XACCLangParser::KernelcallContext * /*ctx*/) override { }
  virtual void exitKernelcall(XACCLangParser::KernelcallContext * /*ctx*/) override { }

  virtual void enterMainprog(XACCLangParser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(XACCLangParser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(XACCLangParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(XACCLangParser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(XACCLangParser::LineContext * /*ctx*/) override { }
  virtual void exitLine(XACCLangParser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(XACCLangParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(XACCLangParser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(XACCLangParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(XACCLangParser::CommentContext * /*ctx*/) override { }

  virtual void enterParamlist(XACCLangParser::ParamlistContext * /*ctx*/) override { }
  virtual void exitParamlist(XACCLangParser::ParamlistContext * /*ctx*/) override { }

  virtual void enterParam(XACCLangParser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(XACCLangParser::ParamContext * /*ctx*/) override { }

  virtual void enterUop(XACCLangParser::UopContext * /*ctx*/) override { }
  virtual void exitUop(XACCLangParser::UopContext * /*ctx*/) override { }

  virtual void enterAllbitsOp(XACCLangParser::AllbitsOpContext * /*ctx*/) override { }
  virtual void exitAllbitsOp(XACCLangParser::AllbitsOpContext * /*ctx*/) override { }

  virtual void enterGate(XACCLangParser::GateContext * /*ctx*/) override { }
  virtual void exitGate(XACCLangParser::GateContext * /*ctx*/) override { }

  virtual void enterExplist(XACCLangParser::ExplistContext * /*ctx*/) override { }
  virtual void exitExplist(XACCLangParser::ExplistContext * /*ctx*/) override { }

  virtual void enterExp(XACCLangParser::ExpContext * /*ctx*/) override { }
  virtual void exitExp(XACCLangParser::ExpContext * /*ctx*/) override { }

  virtual void enterKey(XACCLangParser::KeyContext * /*ctx*/) override { }
  virtual void exitKey(XACCLangParser::KeyContext * /*ctx*/) override { }

  virtual void enterCoupler(XACCLangParser::CouplerContext * /*ctx*/) override { }
  virtual void exitCoupler(XACCLangParser::CouplerContext * /*ctx*/) override { }

  virtual void enterUnaryop(XACCLangParser::UnaryopContext * /*ctx*/) override { }
  virtual void exitUnaryop(XACCLangParser::UnaryopContext * /*ctx*/) override { }

  virtual void enterId(XACCLangParser::IdContext * /*ctx*/) override { }
  virtual void exitId(XACCLangParser::IdContext * /*ctx*/) override { }

  virtual void enterReal(XACCLangParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(XACCLangParser::RealContext * /*ctx*/) override { }

  virtual void enterString(XACCLangParser::StringContext * /*ctx*/) override { }
  virtual void exitString(XACCLangParser::StringContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace xacclang
