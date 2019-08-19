
// Generated from pyxasm.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "pyxasmParser.h"


namespace pyxasm {

/**
 * This interface defines an abstract listener for a parse tree produced by pyxasmParser.
 */
class  pyxasmListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(pyxasmParser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(pyxasmParser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(pyxasmParser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(pyxasmParser::XacckernelContext *ctx) = 0;

  virtual void enterParam(pyxasmParser::ParamContext *ctx) = 0;
  virtual void exitParam(pyxasmParser::ParamContext *ctx) = 0;

  virtual void enterMainprog(pyxasmParser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(pyxasmParser::MainprogContext *ctx) = 0;

  virtual void enterProgram(pyxasmParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(pyxasmParser::ProgramContext *ctx) = 0;

  virtual void enterLine(pyxasmParser::LineContext *ctx) = 0;
  virtual void exitLine(pyxasmParser::LineContext *ctx) = 0;

  virtual void enterStatement(pyxasmParser::StatementContext *ctx) = 0;
  virtual void exitStatement(pyxasmParser::StatementContext *ctx) = 0;

  virtual void enterComment(pyxasmParser::CommentContext *ctx) = 0;
  virtual void exitComment(pyxasmParser::CommentContext *ctx) = 0;

  virtual void enterInstruction(pyxasmParser::InstructionContext *ctx) = 0;
  virtual void exitInstruction(pyxasmParser::InstructionContext *ctx) = 0;

  virtual void enterBufferIndex(pyxasmParser::BufferIndexContext *ctx) = 0;
  virtual void exitBufferIndex(pyxasmParser::BufferIndexContext *ctx) = 0;

  virtual void enterBitsOrParamType(pyxasmParser::BitsOrParamTypeContext *ctx) = 0;
  virtual void exitBitsOrParamType(pyxasmParser::BitsOrParamTypeContext *ctx) = 0;

  virtual void enterOptionsMap(pyxasmParser::OptionsMapContext *ctx) = 0;
  virtual void exitOptionsMap(pyxasmParser::OptionsMapContext *ctx) = 0;

  virtual void enterOptionsType(pyxasmParser::OptionsTypeContext *ctx) = 0;
  virtual void exitOptionsType(pyxasmParser::OptionsTypeContext *ctx) = 0;

  virtual void enterExplist(pyxasmParser::ExplistContext *ctx) = 0;
  virtual void exitExplist(pyxasmParser::ExplistContext *ctx) = 0;

  virtual void enterExp(pyxasmParser::ExpContext *ctx) = 0;
  virtual void exitExp(pyxasmParser::ExpContext *ctx) = 0;

  virtual void enterUnaryop(pyxasmParser::UnaryopContext *ctx) = 0;
  virtual void exitUnaryop(pyxasmParser::UnaryopContext *ctx) = 0;

  virtual void enterId(pyxasmParser::IdContext *ctx) = 0;
  virtual void exitId(pyxasmParser::IdContext *ctx) = 0;

  virtual void enterReal(pyxasmParser::RealContext *ctx) = 0;
  virtual void exitReal(pyxasmParser::RealContext *ctx) = 0;

  virtual void enterString(pyxasmParser::StringContext *ctx) = 0;
  virtual void exitString(pyxasmParser::StringContext *ctx) = 0;


};

}  // namespace pyxasm
