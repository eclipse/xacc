
// Generated from OQASM2.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "OQASM2Parser.h"


namespace oqasm {

/**
 * This interface defines an abstract listener for a parse tree produced by OQASM2Parser.
 */
class  OQASM2Listener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterXaccsrc(OQASM2Parser::XaccsrcContext *ctx) = 0;
  virtual void exitXaccsrc(OQASM2Parser::XaccsrcContext *ctx) = 0;

  virtual void enterXacckernel(OQASM2Parser::XacckernelContext *ctx) = 0;
  virtual void exitXacckernel(OQASM2Parser::XacckernelContext *ctx) = 0;

  virtual void enterTypedparam(OQASM2Parser::TypedparamContext *ctx) = 0;
  virtual void exitTypedparam(OQASM2Parser::TypedparamContext *ctx) = 0;

  virtual void enterType(OQASM2Parser::TypeContext *ctx) = 0;
  virtual void exitType(OQASM2Parser::TypeContext *ctx) = 0;

  virtual void enterKernelcall(OQASM2Parser::KernelcallContext *ctx) = 0;
  virtual void exitKernelcall(OQASM2Parser::KernelcallContext *ctx) = 0;

  virtual void enterMainprog(OQASM2Parser::MainprogContext *ctx) = 0;
  virtual void exitMainprog(OQASM2Parser::MainprogContext *ctx) = 0;

  virtual void enterProgram(OQASM2Parser::ProgramContext *ctx) = 0;
  virtual void exitProgram(OQASM2Parser::ProgramContext *ctx) = 0;

  virtual void enterLine(OQASM2Parser::LineContext *ctx) = 0;
  virtual void exitLine(OQASM2Parser::LineContext *ctx) = 0;

  virtual void enterStatement(OQASM2Parser::StatementContext *ctx) = 0;
  virtual void exitStatement(OQASM2Parser::StatementContext *ctx) = 0;

  virtual void enterComment(OQASM2Parser::CommentContext *ctx) = 0;
  virtual void exitComment(OQASM2Parser::CommentContext *ctx) = 0;

  virtual void enterInclude(OQASM2Parser::IncludeContext *ctx) = 0;
  virtual void exitInclude(OQASM2Parser::IncludeContext *ctx) = 0;

  virtual void enterFilename(OQASM2Parser::FilenameContext *ctx) = 0;
  virtual void exitFilename(OQASM2Parser::FilenameContext *ctx) = 0;

  virtual void enterRegdecl(OQASM2Parser::RegdeclContext *ctx) = 0;
  virtual void exitRegdecl(OQASM2Parser::RegdeclContext *ctx) = 0;

  virtual void enterQregister(OQASM2Parser::QregisterContext *ctx) = 0;
  virtual void exitQregister(OQASM2Parser::QregisterContext *ctx) = 0;

  virtual void enterCregister(OQASM2Parser::CregisterContext *ctx) = 0;
  virtual void exitCregister(OQASM2Parser::CregisterContext *ctx) = 0;

  virtual void enterRegistersize(OQASM2Parser::RegistersizeContext *ctx) = 0;
  virtual void exitRegistersize(OQASM2Parser::RegistersizeContext *ctx) = 0;

  virtual void enterGatedecl(OQASM2Parser::GatedeclContext *ctx) = 0;
  virtual void exitGatedecl(OQASM2Parser::GatedeclContext *ctx) = 0;

  virtual void enterGatename(OQASM2Parser::GatenameContext *ctx) = 0;
  virtual void exitGatename(OQASM2Parser::GatenameContext *ctx) = 0;

  virtual void enterGatearglist(OQASM2Parser::GatearglistContext *ctx) = 0;
  virtual void exitGatearglist(OQASM2Parser::GatearglistContext *ctx) = 0;

  virtual void enterGatearg(OQASM2Parser::GateargContext *ctx) = 0;
  virtual void exitGatearg(OQASM2Parser::GateargContext *ctx) = 0;

  virtual void enterGatebody(OQASM2Parser::GatebodyContext *ctx) = 0;
  virtual void exitGatebody(OQASM2Parser::GatebodyContext *ctx) = 0;

  virtual void enterGateprog(OQASM2Parser::GateprogContext *ctx) = 0;
  virtual void exitGateprog(OQASM2Parser::GateprogContext *ctx) = 0;

  virtual void enterGateline(OQASM2Parser::GatelineContext *ctx) = 0;
  virtual void exitGateline(OQASM2Parser::GatelineContext *ctx) = 0;

  virtual void enterParamlist(OQASM2Parser::ParamlistContext *ctx) = 0;
  virtual void exitParamlist(OQASM2Parser::ParamlistContext *ctx) = 0;

  virtual void enterParam(OQASM2Parser::ParamContext *ctx) = 0;
  virtual void exitParam(OQASM2Parser::ParamContext *ctx) = 0;

  virtual void enterOpaque(OQASM2Parser::OpaqueContext *ctx) = 0;
  virtual void exitOpaque(OQASM2Parser::OpaqueContext *ctx) = 0;

  virtual void enterOpaquename(OQASM2Parser::OpaquenameContext *ctx) = 0;
  virtual void exitOpaquename(OQASM2Parser::OpaquenameContext *ctx) = 0;

  virtual void enterOpaquearglist(OQASM2Parser::OpaquearglistContext *ctx) = 0;
  virtual void exitOpaquearglist(OQASM2Parser::OpaquearglistContext *ctx) = 0;

  virtual void enterOpaquearg(OQASM2Parser::OpaqueargContext *ctx) = 0;
  virtual void exitOpaquearg(OQASM2Parser::OpaqueargContext *ctx) = 0;

  virtual void enterQop(OQASM2Parser::QopContext *ctx) = 0;
  virtual void exitQop(OQASM2Parser::QopContext *ctx) = 0;

  virtual void enterU(OQASM2Parser::UContext *ctx) = 0;
  virtual void exitU(OQASM2Parser::UContext *ctx) = 0;

  virtual void enterCX(OQASM2Parser::CXContext *ctx) = 0;
  virtual void exitCX(OQASM2Parser::CXContext *ctx) = 0;

  virtual void enterUserDefGate(OQASM2Parser::UserDefGateContext *ctx) = 0;
  virtual void exitUserDefGate(OQASM2Parser::UserDefGateContext *ctx) = 0;

  virtual void enterConditional(OQASM2Parser::ConditionalContext *ctx) = 0;
  virtual void exitConditional(OQASM2Parser::ConditionalContext *ctx) = 0;

  virtual void enterAction(OQASM2Parser::ActionContext *ctx) = 0;
  virtual void exitAction(OQASM2Parser::ActionContext *ctx) = 0;

  virtual void enterExplist(OQASM2Parser::ExplistContext *ctx) = 0;
  virtual void exitExplist(OQASM2Parser::ExplistContext *ctx) = 0;

  virtual void enterExp(OQASM2Parser::ExpContext *ctx) = 0;
  virtual void exitExp(OQASM2Parser::ExpContext *ctx) = 0;

  virtual void enterUnaryop(OQASM2Parser::UnaryopContext *ctx) = 0;
  virtual void exitUnaryop(OQASM2Parser::UnaryopContext *ctx) = 0;

  virtual void enterQreg(OQASM2Parser::QregContext *ctx) = 0;
  virtual void exitQreg(OQASM2Parser::QregContext *ctx) = 0;

  virtual void enterCreg(OQASM2Parser::CregContext *ctx) = 0;
  virtual void exitCreg(OQASM2Parser::CregContext *ctx) = 0;

  virtual void enterGate(OQASM2Parser::GateContext *ctx) = 0;
  virtual void exitGate(OQASM2Parser::GateContext *ctx) = 0;

  virtual void enterMeasure(OQASM2Parser::MeasureContext *ctx) = 0;
  virtual void exitMeasure(OQASM2Parser::MeasureContext *ctx) = 0;

  virtual void enterReeset(OQASM2Parser::ReesetContext *ctx) = 0;
  virtual void exitReeset(OQASM2Parser::ReesetContext *ctx) = 0;

  virtual void enterBarrier(OQASM2Parser::BarrierContext *ctx) = 0;
  virtual void exitBarrier(OQASM2Parser::BarrierContext *ctx) = 0;

  virtual void enterId(OQASM2Parser::IdContext *ctx) = 0;
  virtual void exitId(OQASM2Parser::IdContext *ctx) = 0;

  virtual void enterReal(OQASM2Parser::RealContext *ctx) = 0;
  virtual void exitReal(OQASM2Parser::RealContext *ctx) = 0;

  virtual void enterString(OQASM2Parser::StringContext *ctx) = 0;
  virtual void exitString(OQASM2Parser::StringContext *ctx) = 0;


};

}  // namespace oqasm
