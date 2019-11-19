
// Generated from OQASM2.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "OQASM2Listener.h"


namespace oqasm {

/**
 * This class provides an empty implementation of OQASM2Listener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  OQASM2BaseListener : public OQASM2Listener {
public:

  virtual void enterXaccsrc(OQASM2Parser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(OQASM2Parser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(OQASM2Parser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(OQASM2Parser::XacckernelContext * /*ctx*/) override { }

  virtual void enterTypedparam(OQASM2Parser::TypedparamContext * /*ctx*/) override { }
  virtual void exitTypedparam(OQASM2Parser::TypedparamContext * /*ctx*/) override { }

  virtual void enterType(OQASM2Parser::TypeContext * /*ctx*/) override { }
  virtual void exitType(OQASM2Parser::TypeContext * /*ctx*/) override { }

  virtual void enterKernelcall(OQASM2Parser::KernelcallContext * /*ctx*/) override { }
  virtual void exitKernelcall(OQASM2Parser::KernelcallContext * /*ctx*/) override { }

  virtual void enterMainprog(OQASM2Parser::MainprogContext * /*ctx*/) override { }
  virtual void exitMainprog(OQASM2Parser::MainprogContext * /*ctx*/) override { }

  virtual void enterProgram(OQASM2Parser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(OQASM2Parser::ProgramContext * /*ctx*/) override { }

  virtual void enterLine(OQASM2Parser::LineContext * /*ctx*/) override { }
  virtual void exitLine(OQASM2Parser::LineContext * /*ctx*/) override { }

  virtual void enterStatement(OQASM2Parser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(OQASM2Parser::StatementContext * /*ctx*/) override { }

  virtual void enterComment(OQASM2Parser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(OQASM2Parser::CommentContext * /*ctx*/) override { }

  virtual void enterInclude(OQASM2Parser::IncludeContext * /*ctx*/) override { }
  virtual void exitInclude(OQASM2Parser::IncludeContext * /*ctx*/) override { }

  virtual void enterFilename(OQASM2Parser::FilenameContext * /*ctx*/) override { }
  virtual void exitFilename(OQASM2Parser::FilenameContext * /*ctx*/) override { }

  virtual void enterRegdecl(OQASM2Parser::RegdeclContext * /*ctx*/) override { }
  virtual void exitRegdecl(OQASM2Parser::RegdeclContext * /*ctx*/) override { }

  virtual void enterQregister(OQASM2Parser::QregisterContext * /*ctx*/) override { }
  virtual void exitQregister(OQASM2Parser::QregisterContext * /*ctx*/) override { }

  virtual void enterCregister(OQASM2Parser::CregisterContext * /*ctx*/) override { }
  virtual void exitCregister(OQASM2Parser::CregisterContext * /*ctx*/) override { }

  virtual void enterRegistersize(OQASM2Parser::RegistersizeContext * /*ctx*/) override { }
  virtual void exitRegistersize(OQASM2Parser::RegistersizeContext * /*ctx*/) override { }

  virtual void enterGatedecl(OQASM2Parser::GatedeclContext * /*ctx*/) override { }
  virtual void exitGatedecl(OQASM2Parser::GatedeclContext * /*ctx*/) override { }

  virtual void enterGatename(OQASM2Parser::GatenameContext * /*ctx*/) override { }
  virtual void exitGatename(OQASM2Parser::GatenameContext * /*ctx*/) override { }

  virtual void enterGatearglist(OQASM2Parser::GatearglistContext * /*ctx*/) override { }
  virtual void exitGatearglist(OQASM2Parser::GatearglistContext * /*ctx*/) override { }

  virtual void enterGatearg(OQASM2Parser::GateargContext * /*ctx*/) override { }
  virtual void exitGatearg(OQASM2Parser::GateargContext * /*ctx*/) override { }

  virtual void enterGatebody(OQASM2Parser::GatebodyContext * /*ctx*/) override { }
  virtual void exitGatebody(OQASM2Parser::GatebodyContext * /*ctx*/) override { }

  virtual void enterGateprog(OQASM2Parser::GateprogContext * /*ctx*/) override { }
  virtual void exitGateprog(OQASM2Parser::GateprogContext * /*ctx*/) override { }

  virtual void enterGateline(OQASM2Parser::GatelineContext * /*ctx*/) override { }
  virtual void exitGateline(OQASM2Parser::GatelineContext * /*ctx*/) override { }

  virtual void enterParamlist(OQASM2Parser::ParamlistContext * /*ctx*/) override { }
  virtual void exitParamlist(OQASM2Parser::ParamlistContext * /*ctx*/) override { }

  virtual void enterParam(OQASM2Parser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(OQASM2Parser::ParamContext * /*ctx*/) override { }

  virtual void enterOpaque(OQASM2Parser::OpaqueContext * /*ctx*/) override { }
  virtual void exitOpaque(OQASM2Parser::OpaqueContext * /*ctx*/) override { }

  virtual void enterOpaquename(OQASM2Parser::OpaquenameContext * /*ctx*/) override { }
  virtual void exitOpaquename(OQASM2Parser::OpaquenameContext * /*ctx*/) override { }

  virtual void enterOpaquearglist(OQASM2Parser::OpaquearglistContext * /*ctx*/) override { }
  virtual void exitOpaquearglist(OQASM2Parser::OpaquearglistContext * /*ctx*/) override { }

  virtual void enterOpaquearg(OQASM2Parser::OpaqueargContext * /*ctx*/) override { }
  virtual void exitOpaquearg(OQASM2Parser::OpaqueargContext * /*ctx*/) override { }

  virtual void enterQop(OQASM2Parser::QopContext * /*ctx*/) override { }
  virtual void exitQop(OQASM2Parser::QopContext * /*ctx*/) override { }

  virtual void enterU(OQASM2Parser::UContext * /*ctx*/) override { }
  virtual void exitU(OQASM2Parser::UContext * /*ctx*/) override { }

  virtual void enterCX(OQASM2Parser::CXContext * /*ctx*/) override { }
  virtual void exitCX(OQASM2Parser::CXContext * /*ctx*/) override { }

  virtual void enterUserDefGate(OQASM2Parser::UserDefGateContext * /*ctx*/) override { }
  virtual void exitUserDefGate(OQASM2Parser::UserDefGateContext * /*ctx*/) override { }

  virtual void enterConditional(OQASM2Parser::ConditionalContext * /*ctx*/) override { }
  virtual void exitConditional(OQASM2Parser::ConditionalContext * /*ctx*/) override { }

  virtual void enterAction(OQASM2Parser::ActionContext * /*ctx*/) override { }
  virtual void exitAction(OQASM2Parser::ActionContext * /*ctx*/) override { }

  virtual void enterExplist(OQASM2Parser::ExplistContext * /*ctx*/) override { }
  virtual void exitExplist(OQASM2Parser::ExplistContext * /*ctx*/) override { }

  virtual void enterExp(OQASM2Parser::ExpContext * /*ctx*/) override { }
  virtual void exitExp(OQASM2Parser::ExpContext * /*ctx*/) override { }

  virtual void enterUnaryop(OQASM2Parser::UnaryopContext * /*ctx*/) override { }
  virtual void exitUnaryop(OQASM2Parser::UnaryopContext * /*ctx*/) override { }

  virtual void enterQreg(OQASM2Parser::QregContext * /*ctx*/) override { }
  virtual void exitQreg(OQASM2Parser::QregContext * /*ctx*/) override { }

  virtual void enterCreg(OQASM2Parser::CregContext * /*ctx*/) override { }
  virtual void exitCreg(OQASM2Parser::CregContext * /*ctx*/) override { }

  virtual void enterGate(OQASM2Parser::GateContext * /*ctx*/) override { }
  virtual void exitGate(OQASM2Parser::GateContext * /*ctx*/) override { }

  virtual void enterMeasure(OQASM2Parser::MeasureContext * /*ctx*/) override { }
  virtual void exitMeasure(OQASM2Parser::MeasureContext * /*ctx*/) override { }

  virtual void enterReeset(OQASM2Parser::ReesetContext * /*ctx*/) override { }
  virtual void exitReeset(OQASM2Parser::ReesetContext * /*ctx*/) override { }

  virtual void enterBarrier(OQASM2Parser::BarrierContext * /*ctx*/) override { }
  virtual void exitBarrier(OQASM2Parser::BarrierContext * /*ctx*/) override { }

  virtual void enterId(OQASM2Parser::IdContext * /*ctx*/) override { }
  virtual void exitId(OQASM2Parser::IdContext * /*ctx*/) override { }

  virtual void enterReal(OQASM2Parser::RealContext * /*ctx*/) override { }
  virtual void exitReal(OQASM2Parser::RealContext * /*ctx*/) override { }

  virtual void enterString(OQASM2Parser::StringContext * /*ctx*/) override { }
  virtual void exitString(OQASM2Parser::StringContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace oqasm
