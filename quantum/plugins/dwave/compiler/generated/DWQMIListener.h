
// Generated from DWQMI.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "DWQMIParser.h"


namespace dwqmi {

/**
 * This interface defines an abstract listener for a parse tree produced by DWQMIParser.
 */
class  DWQMIListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(DWQMIParser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(DWQMIParser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(DWQMIParser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(DWQMIParser::XacckernelContext *ctx) = 0;

  virtual void enterKernelcall(DWQMIParser::KernelcallContext *ctx) = 0;
  virtual void exitKernelcall(DWQMIParser::KernelcallContext *ctx) = 0;

  virtual void enterTypedparam(DWQMIParser::TypedparamContext *ctx) = 0;
  virtual void exitTypedparam(DWQMIParser::TypedparamContext *ctx) = 0;

  virtual void enterMainprog(DWQMIParser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(DWQMIParser::MainprogContext *ctx) = 0;

  virtual void enterProgram(DWQMIParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(DWQMIParser::ProgramContext *ctx) = 0;

  virtual void enterLine(DWQMIParser::LineContext *ctx) = 0;
  virtual void exitLine(DWQMIParser::LineContext *ctx) = 0;

  virtual void enterStatement(DWQMIParser::StatementContext *ctx) = 0;
  virtual void exitStatement(DWQMIParser::StatementContext *ctx) = 0;

  virtual void enterComment(DWQMIParser::CommentContext *ctx) = 0;
  virtual void exitComment(DWQMIParser::CommentContext *ctx) = 0;

  virtual void enterInst(DWQMIParser::InstContext *ctx) = 0;
  virtual void exitInst(DWQMIParser::InstContext *ctx) = 0;

  virtual void enterAnnealdecl(DWQMIParser::AnnealdeclContext *ctx) = 0;
  virtual void exitAnnealdecl(DWQMIParser::AnnealdeclContext *ctx) = 0;

  virtual void enterTa(DWQMIParser::TaContext *ctx) = 0;
  virtual void exitTa(DWQMIParser::TaContext *ctx) = 0;

  virtual void enterTp(DWQMIParser::TpContext *ctx) = 0;
  virtual void exitTp(DWQMIParser::TpContext *ctx) = 0;

  virtual void enterTq(DWQMIParser::TqContext *ctx) = 0;
  virtual void exitTq(DWQMIParser::TqContext *ctx) = 0;

  virtual void enterAnneal(DWQMIParser::AnnealContext *ctx) = 0;
  virtual void exitAnneal(DWQMIParser::AnnealContext *ctx) = 0;

  virtual void enterId(DWQMIParser::IdContext *ctx) = 0;
  virtual void exitId(DWQMIParser::IdContext *ctx) = 0;

  virtual void enterReal(DWQMIParser::RealContext *ctx) = 0;
  virtual void exitReal(DWQMIParser::RealContext *ctx) = 0;

  virtual void enterString(DWQMIParser::StringContext *ctx) = 0;
  virtual void exitString(DWQMIParser::StringContext *ctx) = 0;

  virtual void enterDirection(DWQMIParser::DirectionContext *ctx) = 0;
  virtual void exitDirection(DWQMIParser::DirectionContext *ctx) = 0;

  virtual void enterForward(DWQMIParser::ForwardContext *ctx) = 0;
  virtual void exitForward(DWQMIParser::ForwardContext *ctx) = 0;

  virtual void enterReverse(DWQMIParser::ReverseContext *ctx) = 0;
  virtual void exitReverse(DWQMIParser::ReverseContext *ctx) = 0;


};

}  // namespace dwqmi
