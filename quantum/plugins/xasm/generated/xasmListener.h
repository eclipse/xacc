
// Generated from xasm.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "xasmParser.h"


namespace xasm {

/**
 * This interface defines an abstract listener for a parse tree produced by xasmParser.
 */
class  xasmListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(xasmParser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(xasmParser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(xasmParser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(xasmParser::XacckernelContext *ctx) = 0;

  virtual void enterXacclambda(xasmParser::XacclambdaContext *ctx) = 0;
  virtual void exitXacclambda(xasmParser::XacclambdaContext *ctx) = 0;

  virtual void enterTypedparam(xasmParser::TypedparamContext *ctx) = 0;
  virtual void exitTypedparam(xasmParser::TypedparamContext *ctx) = 0;

  virtual void enterVariable_param_name(xasmParser::Variable_param_nameContext *ctx) = 0;
  virtual void exitVariable_param_name(xasmParser::Variable_param_nameContext *ctx) = 0;

  virtual void enterType(xasmParser::TypeContext *ctx) = 0;
  virtual void exitType(xasmParser::TypeContext *ctx) = 0;

  virtual void enterMainprog(xasmParser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(xasmParser::MainprogContext *ctx) = 0;

  virtual void enterProgram(xasmParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(xasmParser::ProgramContext *ctx) = 0;

  virtual void enterLine(xasmParser::LineContext *ctx) = 0;
  virtual void exitLine(xasmParser::LineContext *ctx) = 0;

  virtual void enterStatement(xasmParser::StatementContext *ctx) = 0;
  virtual void exitStatement(xasmParser::StatementContext *ctx) = 0;

  virtual void enterComment(xasmParser::CommentContext *ctx) = 0;
  virtual void exitComment(xasmParser::CommentContext *ctx) = 0;

  virtual void enterForstmt(xasmParser::ForstmtContext *ctx) = 0;
  virtual void exitForstmt(xasmParser::ForstmtContext *ctx) = 0;

  virtual void enterIfstmt(xasmParser::IfstmtContext *ctx) = 0;
  virtual void exitIfstmt(xasmParser::IfstmtContext *ctx) = 0;

  virtual void enterInstruction(xasmParser::InstructionContext *ctx) = 0;
  virtual void exitInstruction(xasmParser::InstructionContext *ctx) = 0;

  virtual void enterBufferList(xasmParser::BufferListContext *ctx) = 0;
  virtual void exitBufferList(xasmParser::BufferListContext *ctx) = 0;

  virtual void enterParamList(xasmParser::ParamListContext *ctx) = 0;
  virtual void exitParamList(xasmParser::ParamListContext *ctx) = 0;

  virtual void enterParameter(xasmParser::ParameterContext *ctx) = 0;
  virtual void exitParameter(xasmParser::ParameterContext *ctx) = 0;

  virtual void enterComposite_generator(xasmParser::Composite_generatorContext *ctx) = 0;
  virtual void exitComposite_generator(xasmParser::Composite_generatorContext *ctx) = 0;

  virtual void enterBufferIndex(xasmParser::BufferIndexContext *ctx) = 0;
  virtual void exitBufferIndex(xasmParser::BufferIndexContext *ctx) = 0;

  virtual void enterOptionsMap(xasmParser::OptionsMapContext *ctx) = 0;
  virtual void exitOptionsMap(xasmParser::OptionsMapContext *ctx) = 0;

  virtual void enterOptionsType(xasmParser::OptionsTypeContext *ctx) = 0;
  virtual void exitOptionsType(xasmParser::OptionsTypeContext *ctx) = 0;

  virtual void enterExplist(xasmParser::ExplistContext *ctx) = 0;
  virtual void exitExplist(xasmParser::ExplistContext *ctx) = 0;

  virtual void enterExp(xasmParser::ExpContext *ctx) = 0;
  virtual void exitExp(xasmParser::ExpContext *ctx) = 0;

  virtual void enterUnaryop(xasmParser::UnaryopContext *ctx) = 0;
  virtual void exitUnaryop(xasmParser::UnaryopContext *ctx) = 0;

  virtual void enterId(xasmParser::IdContext *ctx) = 0;
  virtual void exitId(xasmParser::IdContext *ctx) = 0;

  virtual void enterReal(xasmParser::RealContext *ctx) = 0;
  virtual void exitReal(xasmParser::RealContext *ctx) = 0;

  virtual void enterString(xasmParser::StringContext *ctx) = 0;
  virtual void exitString(xasmParser::StringContext *ctx) = 0;


};

}  // namespace xasm
