
// Generated from PyXACCIR.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "PyXACCIRListener.h"


namespace pyxacc {

/**
 * This class provides an empty implementation of PyXACCIRListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  PyXACCIRBaseListener : public PyXACCIRListener {
public:

  virtual void enterXaccsrc(PyXACCIRParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(PyXACCIRParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(PyXACCIRParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(PyXACCIRParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterMainprog(PyXACCIRParser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(PyXACCIRParser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(PyXACCIRParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(PyXACCIRParser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(PyXACCIRParser::LineContext * /*ctx*/) override { }
  virtual void exitLine(PyXACCIRParser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(PyXACCIRParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(PyXACCIRParser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(PyXACCIRParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(PyXACCIRParser::CommentContext * /*ctx*/) override { }

  virtual void enterParamlist(PyXACCIRParser::ParamlistContext * /*ctx*/) override { }
  virtual void exitParamlist(PyXACCIRParser::ParamlistContext * /*ctx*/) override { }

  virtual void enterParam(PyXACCIRParser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(PyXACCIRParser::ParamContext * /*ctx*/) override { }

  virtual void enterUop(PyXACCIRParser::UopContext * /*ctx*/) override { }
  virtual void exitUop(PyXACCIRParser::UopContext * /*ctx*/) override { }

  virtual void enterAllbitsOp(PyXACCIRParser::AllbitsOpContext * /*ctx*/) override { }
  virtual void exitAllbitsOp(PyXACCIRParser::AllbitsOpContext * /*ctx*/) override { }

  virtual void enterGate(PyXACCIRParser::GateContext * /*ctx*/) override { }
  virtual void exitGate(PyXACCIRParser::GateContext * /*ctx*/) override { }

  virtual void enterExplist(PyXACCIRParser::ExplistContext * /*ctx*/) override { }
  virtual void exitExplist(PyXACCIRParser::ExplistContext * /*ctx*/) override { }

  virtual void enterExp(PyXACCIRParser::ExpContext * /*ctx*/) override { }
  virtual void exitExp(PyXACCIRParser::ExpContext * /*ctx*/) override { }

  virtual void enterCoupler(PyXACCIRParser::CouplerContext * /*ctx*/) override { }
  virtual void exitCoupler(PyXACCIRParser::CouplerContext * /*ctx*/) override { }

  virtual void enterUnaryop(PyXACCIRParser::UnaryopContext * /*ctx*/) override { }
  virtual void exitUnaryop(PyXACCIRParser::UnaryopContext * /*ctx*/) override { }

  virtual void enterId(PyXACCIRParser::IdContext * /*ctx*/) override { }
  virtual void exitId(PyXACCIRParser::IdContext * /*ctx*/) override { }

  virtual void enterReal(PyXACCIRParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(PyXACCIRParser::RealContext * /*ctx*/) override { }

  virtual void enterString(PyXACCIRParser::StringContext * /*ctx*/) override { }
  virtual void exitString(PyXACCIRParser::StringContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace pyxacc
