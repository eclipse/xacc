
// Generated from xasm.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "xasmListener.h"


namespace xasm {

/**
 * This class provides an empty implementation of xasmListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  xasmBaseListener : public xasmListener {
public:

  virtual void enterXaccsrc(xasmParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(xasmParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(xasmParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(xasmParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterXacclambda(xasmParser::XacclambdaContext * /*ctx*/) override { }
  virtual void exitXacclambda(xasmParser::XacclambdaContext * /*ctx*/) override { }

  virtual void enterTypedparam(xasmParser::TypedparamContext * /*ctx*/) override { }
  virtual void exitTypedparam(xasmParser::TypedparamContext * /*ctx*/) override { }

  virtual void enterVariable_param_name(xasmParser::Variable_param_nameContext * /*ctx*/) override { }
  virtual void exitVariable_param_name(xasmParser::Variable_param_nameContext * /*ctx*/) override { }

  virtual void enterType(xasmParser::TypeContext * /*ctx*/) override { }
  virtual void exitType(xasmParser::TypeContext * /*ctx*/) override { }

  virtual void enterMainprog(xasmParser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(xasmParser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(xasmParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(xasmParser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(xasmParser::LineContext * /*ctx*/) override { }
  virtual void exitLine(xasmParser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(xasmParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(xasmParser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(xasmParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(xasmParser::CommentContext * /*ctx*/) override { }

  virtual void enterForstmt(xasmParser::ForstmtContext * /*ctx*/) override { }
  virtual void exitForstmt(xasmParser::ForstmtContext * /*ctx*/) override { }

  virtual void enterIfstmt(xasmParser::IfstmtContext * /*ctx*/) override { }
  virtual void exitIfstmt(xasmParser::IfstmtContext * /*ctx*/) override { }

  virtual void enterInstruction(xasmParser::InstructionContext * /*ctx*/) override { }
  virtual void exitInstruction(xasmParser::InstructionContext * /*ctx*/) override { }

  virtual void enterBufferList(xasmParser::BufferListContext * /*ctx*/) override { }
  virtual void exitBufferList(xasmParser::BufferListContext * /*ctx*/) override { }

  virtual void enterParamList(xasmParser::ParamListContext * /*ctx*/) override { }
  virtual void exitParamList(xasmParser::ParamListContext * /*ctx*/) override { }

  virtual void enterParameter(xasmParser::ParameterContext * /*ctx*/) override { }
  virtual void exitParameter(xasmParser::ParameterContext * /*ctx*/) override { }

  virtual void enterComposite_generator(xasmParser::Composite_generatorContext * /*ctx*/) override { }
  virtual void exitComposite_generator(xasmParser::Composite_generatorContext * /*ctx*/) override { }

  virtual void enterBufferIndex(xasmParser::BufferIndexContext * /*ctx*/) override { }
  virtual void exitBufferIndex(xasmParser::BufferIndexContext * /*ctx*/) override { }

  virtual void enterOptionsMap(xasmParser::OptionsMapContext * /*ctx*/) override { }
  virtual void exitOptionsMap(xasmParser::OptionsMapContext * /*ctx*/) override { }

  virtual void enterOptionsType(xasmParser::OptionsTypeContext * /*ctx*/) override { }
  virtual void exitOptionsType(xasmParser::OptionsTypeContext * /*ctx*/) override { }

  virtual void enterExplist(xasmParser::ExplistContext * /*ctx*/) override { }
  virtual void exitExplist(xasmParser::ExplistContext * /*ctx*/) override { }

  virtual void enterExp(xasmParser::ExpContext * /*ctx*/) override { }
  virtual void exitExp(xasmParser::ExpContext * /*ctx*/) override { }

  virtual void enterUnaryop(xasmParser::UnaryopContext * /*ctx*/) override { }
  virtual void exitUnaryop(xasmParser::UnaryopContext * /*ctx*/) override { }

  virtual void enterId(xasmParser::IdContext * /*ctx*/) override { }
  virtual void exitId(xasmParser::IdContext * /*ctx*/) override { }

  virtual void enterReal(xasmParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(xasmParser::RealContext * /*ctx*/) override { }

  virtual void enterString(xasmParser::StringContext * /*ctx*/) override { }
  virtual void exitString(xasmParser::StringContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace xasm
