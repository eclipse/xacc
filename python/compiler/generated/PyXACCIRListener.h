
// Generated from PyXACCIR.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "PyXACCIRParser.h"


namespace pyxacc {

/**
 * This interface defines an abstract listener for a parse tree produced by PyXACCIRParser.
 */
class  PyXACCIRListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(PyXACCIRParser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(PyXACCIRParser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(PyXACCIRParser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(PyXACCIRParser::XacckernelContext *ctx) = 0;

  virtual void enterMainprog(PyXACCIRParser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(PyXACCIRParser::MainprogContext *ctx) = 0;

  virtual void enterProgram(PyXACCIRParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(PyXACCIRParser::ProgramContext *ctx) = 0;

  virtual void enterLine(PyXACCIRParser::LineContext *ctx) = 0;
  virtual void exitLine(PyXACCIRParser::LineContext *ctx) = 0;

  virtual void enterStatement(PyXACCIRParser::StatementContext *ctx) = 0;
  virtual void exitStatement(PyXACCIRParser::StatementContext *ctx) = 0;

  virtual void enterComment(PyXACCIRParser::CommentContext *ctx) = 0;
  virtual void exitComment(PyXACCIRParser::CommentContext *ctx) = 0;

  virtual void enterParamlist(PyXACCIRParser::ParamlistContext *ctx) = 0;
  virtual void exitParamlist(PyXACCIRParser::ParamlistContext *ctx) = 0;

  virtual void enterParam(PyXACCIRParser::ParamContext *ctx) = 0;
  virtual void exitParam(PyXACCIRParser::ParamContext *ctx) = 0;

  virtual void enterUop(PyXACCIRParser::UopContext *ctx) = 0;
  virtual void exitUop(PyXACCIRParser::UopContext *ctx) = 0;

  virtual void enterGate(PyXACCIRParser::GateContext *ctx) = 0;
  virtual void exitGate(PyXACCIRParser::GateContext *ctx) = 0;

  virtual void enterExplist(PyXACCIRParser::ExplistContext *ctx) = 0;
  virtual void exitExplist(PyXACCIRParser::ExplistContext *ctx) = 0;

  virtual void enterExp(PyXACCIRParser::ExpContext *ctx) = 0;
  virtual void exitExp(PyXACCIRParser::ExpContext *ctx) = 0;

  virtual void enterUnaryop(PyXACCIRParser::UnaryopContext *ctx) = 0;
  virtual void exitUnaryop(PyXACCIRParser::UnaryopContext *ctx) = 0;

  virtual void enterId(PyXACCIRParser::IdContext *ctx) = 0;
  virtual void exitId(PyXACCIRParser::IdContext *ctx) = 0;

  virtual void enterReal(PyXACCIRParser::RealContext *ctx) = 0;
  virtual void exitReal(PyXACCIRParser::RealContext *ctx) = 0;

  virtual void enterString(PyXACCIRParser::StringContext *ctx) = 0;
  virtual void exitString(PyXACCIRParser::StringContext *ctx) = 0;


};

}  // namespace pyxacc
