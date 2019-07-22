
// Generated from XACCLang.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "XACCLangParser.h"


namespace xacclang {

/**
 * This interface defines an abstract listener for a parse tree produced by XACCLangParser.
 */
class  XACCLangListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(XACCLangParser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(XACCLangParser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(XACCLangParser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(XACCLangParser::XacckernelContext *ctx) = 0;

  virtual void enterTypedparam(XACCLangParser::TypedparamContext *ctx) = 0;
  virtual void exitTypedparam(XACCLangParser::TypedparamContext *ctx) = 0;

  virtual void enterType(XACCLangParser::TypeContext *ctx) = 0;
  virtual void exitType(XACCLangParser::TypeContext *ctx) = 0;

  virtual void enterKernelcall(XACCLangParser::KernelcallContext *ctx) = 0;
  virtual void exitKernelcall(XACCLangParser::KernelcallContext *ctx) = 0;

  virtual void enterMainprog(XACCLangParser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(XACCLangParser::MainprogContext *ctx) = 0;

  virtual void enterProgram(XACCLangParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(XACCLangParser::ProgramContext *ctx) = 0;

  virtual void enterLine(XACCLangParser::LineContext *ctx) = 0;
  virtual void exitLine(XACCLangParser::LineContext *ctx) = 0;

  virtual void enterStatement(XACCLangParser::StatementContext *ctx) = 0;
  virtual void exitStatement(XACCLangParser::StatementContext *ctx) = 0;

  virtual void enterComment(XACCLangParser::CommentContext *ctx) = 0;
  virtual void exitComment(XACCLangParser::CommentContext *ctx) = 0;

  virtual void enterParamlist(XACCLangParser::ParamlistContext *ctx) = 0;
  virtual void exitParamlist(XACCLangParser::ParamlistContext *ctx) = 0;

  virtual void enterParam(XACCLangParser::ParamContext *ctx) = 0;
  virtual void exitParam(XACCLangParser::ParamContext *ctx) = 0;

  virtual void enterUop(XACCLangParser::UopContext *ctx) = 0;
  virtual void exitUop(XACCLangParser::UopContext *ctx) = 0;

  virtual void enterGate(XACCLangParser::GateContext *ctx) = 0;
  virtual void exitGate(XACCLangParser::GateContext *ctx) = 0;

  virtual void enterExplist(XACCLangParser::ExplistContext *ctx) = 0;
  virtual void exitExplist(XACCLangParser::ExplistContext *ctx) = 0;

  virtual void enterExp(XACCLangParser::ExpContext *ctx) = 0;
  virtual void exitExp(XACCLangParser::ExpContext *ctx) = 0;

  virtual void enterKey(XACCLangParser::KeyContext *ctx) = 0;
  virtual void exitKey(XACCLangParser::KeyContext *ctx) = 0;

  virtual void enterCoupler(XACCLangParser::CouplerContext *ctx) = 0;
  virtual void exitCoupler(XACCLangParser::CouplerContext *ctx) = 0;

  virtual void enterUnaryop(XACCLangParser::UnaryopContext *ctx) = 0;
  virtual void exitUnaryop(XACCLangParser::UnaryopContext *ctx) = 0;

  virtual void enterId(XACCLangParser::IdContext *ctx) = 0;
  virtual void exitId(XACCLangParser::IdContext *ctx) = 0;

  virtual void enterReal(XACCLangParser::RealContext *ctx) = 0;
  virtual void exitReal(XACCLangParser::RealContext *ctx) = 0;

  virtual void enterString(XACCLangParser::StringContext *ctx) = 0;
  virtual void exitString(XACCLangParser::StringContext *ctx) = 0;


};

}  // namespace xacclang
