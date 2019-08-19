
// Generated from pyxasm.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "pyxasmListener.h"


namespace pyxasm {

/**
 * This class provides an empty implementation of pyxasmListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  pyxasmBaseListener : public pyxasmListener {
public:

  virtual void enterXaccsrc(pyxasmParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(pyxasmParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(pyxasmParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(pyxasmParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterParam(pyxasmParser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(pyxasmParser::ParamContext * /*ctx*/) override { }

  virtual void enterMainprog(pyxasmParser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(pyxasmParser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(pyxasmParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(pyxasmParser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(pyxasmParser::LineContext * /*ctx*/) override { }
  virtual void exitLine(pyxasmParser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(pyxasmParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(pyxasmParser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(pyxasmParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(pyxasmParser::CommentContext * /*ctx*/) override { }

  virtual void enterInstruction(pyxasmParser::InstructionContext * /*ctx*/) override { }
  virtual void exitInstruction(pyxasmParser::InstructionContext * /*ctx*/) override { }

  virtual void enterBufferIndex(pyxasmParser::BufferIndexContext * /*ctx*/) override { }
  virtual void exitBufferIndex(pyxasmParser::BufferIndexContext * /*ctx*/) override { }

  virtual void enterBitsOrParamType(pyxasmParser::BitsOrParamTypeContext * /*ctx*/) override { }
  virtual void exitBitsOrParamType(pyxasmParser::BitsOrParamTypeContext * /*ctx*/) override { }

  virtual void enterOptionsMap(pyxasmParser::OptionsMapContext * /*ctx*/) override { }
  virtual void exitOptionsMap(pyxasmParser::OptionsMapContext * /*ctx*/) override { }

  virtual void enterOptionsType(pyxasmParser::OptionsTypeContext * /*ctx*/) override { }
  virtual void exitOptionsType(pyxasmParser::OptionsTypeContext * /*ctx*/) override { }

  virtual void enterExplist(pyxasmParser::ExplistContext * /*ctx*/) override { }
  virtual void exitExplist(pyxasmParser::ExplistContext * /*ctx*/) override { }

  virtual void enterExp(pyxasmParser::ExpContext * /*ctx*/) override { }
  virtual void exitExp(pyxasmParser::ExpContext * /*ctx*/) override { }

  virtual void enterUnaryop(pyxasmParser::UnaryopContext * /*ctx*/) override { }
  virtual void exitUnaryop(pyxasmParser::UnaryopContext * /*ctx*/) override { }

  virtual void enterId(pyxasmParser::IdContext * /*ctx*/) override { }
  virtual void exitId(pyxasmParser::IdContext * /*ctx*/) override { }

  virtual void enterReal(pyxasmParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(pyxasmParser::RealContext * /*ctx*/) override { }

  virtual void enterString(pyxasmParser::StringContext * /*ctx*/) override { }
  virtual void exitString(pyxasmParser::StringContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace pyxasm
