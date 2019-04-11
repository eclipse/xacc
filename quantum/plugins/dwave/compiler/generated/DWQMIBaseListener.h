
// Generated from DWQMI.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "DWQMIListener.h"


namespace dwqmi {

/**
 * This class provides an empty implementation of DWQMIListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  DWQMIBaseListener : public DWQMIListener {
public:

  virtual void enterXaccsrc(DWQMIParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(DWQMIParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(DWQMIParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(DWQMIParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterKernelcall(DWQMIParser::KernelcallContext * /*ctx*/) override { }
  virtual void exitKernelcall(DWQMIParser::KernelcallContext * /*ctx*/) override { }

  virtual void enterTypedparam(DWQMIParser::TypedparamContext * /*ctx*/) override { }
  virtual void exitTypedparam(DWQMIParser::TypedparamContext * /*ctx*/) override { }

  virtual void enterMainprog(DWQMIParser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(DWQMIParser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(DWQMIParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(DWQMIParser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(DWQMIParser::LineContext * /*ctx*/) override { }
  virtual void exitLine(DWQMIParser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(DWQMIParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(DWQMIParser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(DWQMIParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(DWQMIParser::CommentContext * /*ctx*/) override { }

  virtual void enterInst(DWQMIParser::InstContext * /*ctx*/) override { }
  virtual void exitInst(DWQMIParser::InstContext * /*ctx*/) override { }

  virtual void enterAnnealdecl(DWQMIParser::AnnealdeclContext * /*ctx*/) override { }
  virtual void exitAnnealdecl(DWQMIParser::AnnealdeclContext * /*ctx*/) override { }

  virtual void enterTa(DWQMIParser::TaContext * /*ctx*/) override { }
  virtual void exitTa(DWQMIParser::TaContext * /*ctx*/) override { }

  virtual void enterTp(DWQMIParser::TpContext * /*ctx*/) override { }
  virtual void exitTp(DWQMIParser::TpContext * /*ctx*/) override { }

  virtual void enterTq(DWQMIParser::TqContext * /*ctx*/) override { }
  virtual void exitTq(DWQMIParser::TqContext * /*ctx*/) override { }

  virtual void enterAnneal(DWQMIParser::AnnealContext * /*ctx*/) override { }
  virtual void exitAnneal(DWQMIParser::AnnealContext * /*ctx*/) override { }

  virtual void enterId(DWQMIParser::IdContext * /*ctx*/) override { }
  virtual void exitId(DWQMIParser::IdContext * /*ctx*/) override { }

  virtual void enterReal(DWQMIParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(DWQMIParser::RealContext * /*ctx*/) override { }

  virtual void enterString(DWQMIParser::StringContext * /*ctx*/) override { }
  virtual void exitString(DWQMIParser::StringContext * /*ctx*/) override { }

  virtual void enterDirection(DWQMIParser::DirectionContext * /*ctx*/) override { }
  virtual void exitDirection(DWQMIParser::DirectionContext * /*ctx*/) override { }

  virtual void enterForward(DWQMIParser::ForwardContext * /*ctx*/) override { }
  virtual void exitForward(DWQMIParser::ForwardContext * /*ctx*/) override { }

  virtual void enterReverse(DWQMIParser::ReverseContext * /*ctx*/) override { }
  virtual void exitReverse(DWQMIParser::ReverseContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace dwqmi
