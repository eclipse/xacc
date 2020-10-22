
// Generated from Quil.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "QuilListener.h"


namespace quil {

/**
 * This class provides an empty implementation of QuilListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  QuilBaseListener : public QuilListener {
public:

  virtual void enterXaccsrc(QuilParser::XaccsrcContext * /*ctx*/) override { }
  virtual void exitXaccsrc(QuilParser::XaccsrcContext * /*ctx*/) override { }

  virtual void enterXacckernel(QuilParser::XacckernelContext * /*ctx*/) override { }
  virtual void exitXacckernel(QuilParser::XacckernelContext * /*ctx*/) override { }

  virtual void enterTypedparam(QuilParser::TypedparamContext * /*ctx*/) override { }
  virtual void exitTypedparam(QuilParser::TypedparamContext * /*ctx*/) override { }

  virtual void enterVariable_param_name(QuilParser::Variable_param_nameContext * /*ctx*/) override { }
  virtual void exitVariable_param_name(QuilParser::Variable_param_nameContext * /*ctx*/) override { }

  virtual void enterType(QuilParser::TypeContext * /*ctx*/) override { }
  virtual void exitType(QuilParser::TypeContext * /*ctx*/) override { }

  virtual void enterId(QuilParser::IdContext * /*ctx*/) override { }
  virtual void exitId(QuilParser::IdContext * /*ctx*/) override { }

  virtual void enterKernelcall(QuilParser::KernelcallContext * /*ctx*/) override { }
  virtual void exitKernelcall(QuilParser::KernelcallContext * /*ctx*/) override { }

  virtual void enterQuil(QuilParser::QuilContext * /*ctx*/) override { }
  virtual void exitQuil(QuilParser::QuilContext * /*ctx*/) override { }

  virtual void enterAllInstr(QuilParser::AllInstrContext * /*ctx*/) override { }
  virtual void exitAllInstr(QuilParser::AllInstrContext * /*ctx*/) override { }

  virtual void enterInstr(QuilParser::InstrContext * /*ctx*/) override { }
  virtual void exitInstr(QuilParser::InstrContext * /*ctx*/) override { }

  virtual void enterGate(QuilParser::GateContext * /*ctx*/) override { }
  virtual void exitGate(QuilParser::GateContext * /*ctx*/) override { }

  virtual void enterName(QuilParser::NameContext * /*ctx*/) override { }
  virtual void exitName(QuilParser::NameContext * /*ctx*/) override { }

  virtual void enterQubit(QuilParser::QubitContext * /*ctx*/) override { }
  virtual void exitQubit(QuilParser::QubitContext * /*ctx*/) override { }

  virtual void enterParam(QuilParser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(QuilParser::ParamContext * /*ctx*/) override { }

  virtual void enterDefGate(QuilParser::DefGateContext * /*ctx*/) override { }
  virtual void exitDefGate(QuilParser::DefGateContext * /*ctx*/) override { }

  virtual void enterVariable(QuilParser::VariableContext * /*ctx*/) override { }
  virtual void exitVariable(QuilParser::VariableContext * /*ctx*/) override { }

  virtual void enterMatrix(QuilParser::MatrixContext * /*ctx*/) override { }
  virtual void exitMatrix(QuilParser::MatrixContext * /*ctx*/) override { }

  virtual void enterMatrixRow(QuilParser::MatrixRowContext * /*ctx*/) override { }
  virtual void exitMatrixRow(QuilParser::MatrixRowContext * /*ctx*/) override { }

  virtual void enterDefCircuit(QuilParser::DefCircuitContext * /*ctx*/) override { }
  virtual void exitDefCircuit(QuilParser::DefCircuitContext * /*ctx*/) override { }

  virtual void enterQubitVariable(QuilParser::QubitVariableContext * /*ctx*/) override { }
  virtual void exitQubitVariable(QuilParser::QubitVariableContext * /*ctx*/) override { }

  virtual void enterCircuitQubit(QuilParser::CircuitQubitContext * /*ctx*/) override { }
  virtual void exitCircuitQubit(QuilParser::CircuitQubitContext * /*ctx*/) override { }

  virtual void enterCircuitGate(QuilParser::CircuitGateContext * /*ctx*/) override { }
  virtual void exitCircuitGate(QuilParser::CircuitGateContext * /*ctx*/) override { }

  virtual void enterCircuitInstr(QuilParser::CircuitInstrContext * /*ctx*/) override { }
  virtual void exitCircuitInstr(QuilParser::CircuitInstrContext * /*ctx*/) override { }

  virtual void enterCircuit(QuilParser::CircuitContext * /*ctx*/) override { }
  virtual void exitCircuit(QuilParser::CircuitContext * /*ctx*/) override { }

  virtual void enterMeasure(QuilParser::MeasureContext * /*ctx*/) override { }
  virtual void exitMeasure(QuilParser::MeasureContext * /*ctx*/) override { }

  virtual void enterAddr(QuilParser::AddrContext * /*ctx*/) override { }
  virtual void exitAddr(QuilParser::AddrContext * /*ctx*/) override { }

  virtual void enterClassicalBit(QuilParser::ClassicalBitContext * /*ctx*/) override { }
  virtual void exitClassicalBit(QuilParser::ClassicalBitContext * /*ctx*/) override { }

  virtual void enterDefLabel(QuilParser::DefLabelContext * /*ctx*/) override { }
  virtual void exitDefLabel(QuilParser::DefLabelContext * /*ctx*/) override { }

  virtual void enterLabel(QuilParser::LabelContext * /*ctx*/) override { }
  virtual void exitLabel(QuilParser::LabelContext * /*ctx*/) override { }

  virtual void enterHalt(QuilParser::HaltContext * /*ctx*/) override { }
  virtual void exitHalt(QuilParser::HaltContext * /*ctx*/) override { }

  virtual void enterJump(QuilParser::JumpContext * /*ctx*/) override { }
  virtual void exitJump(QuilParser::JumpContext * /*ctx*/) override { }

  virtual void enterJumpWhen(QuilParser::JumpWhenContext * /*ctx*/) override { }
  virtual void exitJumpWhen(QuilParser::JumpWhenContext * /*ctx*/) override { }

  virtual void enterJumpUnless(QuilParser::JumpUnlessContext * /*ctx*/) override { }
  virtual void exitJumpUnless(QuilParser::JumpUnlessContext * /*ctx*/) override { }

  virtual void enterResetState(QuilParser::ResetStateContext * /*ctx*/) override { }
  virtual void exitResetState(QuilParser::ResetStateContext * /*ctx*/) override { }

  virtual void enterWait(QuilParser::WaitContext * /*ctx*/) override { }
  virtual void exitWait(QuilParser::WaitContext * /*ctx*/) override { }

  virtual void enterClassicalUnary(QuilParser::ClassicalUnaryContext * /*ctx*/) override { }
  virtual void exitClassicalUnary(QuilParser::ClassicalUnaryContext * /*ctx*/) override { }

  virtual void enterClassicalBinary(QuilParser::ClassicalBinaryContext * /*ctx*/) override { }
  virtual void exitClassicalBinary(QuilParser::ClassicalBinaryContext * /*ctx*/) override { }

  virtual void enterNop(QuilParser::NopContext * /*ctx*/) override { }
  virtual void exitNop(QuilParser::NopContext * /*ctx*/) override { }

  virtual void enterInclude(QuilParser::IncludeContext * /*ctx*/) override { }
  virtual void exitInclude(QuilParser::IncludeContext * /*ctx*/) override { }

  virtual void enterPragma(QuilParser::PragmaContext * /*ctx*/) override { }
  virtual void exitPragma(QuilParser::PragmaContext * /*ctx*/) override { }

  virtual void enterPragma_name(QuilParser::Pragma_nameContext * /*ctx*/) override { }
  virtual void exitPragma_name(QuilParser::Pragma_nameContext * /*ctx*/) override { }

  virtual void enterSegmentExp(QuilParser::SegmentExpContext * /*ctx*/) override { }
  virtual void exitSegmentExp(QuilParser::SegmentExpContext * /*ctx*/) override { }

  virtual void enterIdentifierExp(QuilParser::IdentifierExpContext * /*ctx*/) override { }
  virtual void exitIdentifierExp(QuilParser::IdentifierExpContext * /*ctx*/) override { }

  virtual void enterNumberExp(QuilParser::NumberExpContext * /*ctx*/) override { }
  virtual void exitNumberExp(QuilParser::NumberExpContext * /*ctx*/) override { }

  virtual void enterPowerExp(QuilParser::PowerExpContext * /*ctx*/) override { }
  virtual void exitPowerExp(QuilParser::PowerExpContext * /*ctx*/) override { }

  virtual void enterMulDivExp(QuilParser::MulDivExpContext * /*ctx*/) override { }
  virtual void exitMulDivExp(QuilParser::MulDivExpContext * /*ctx*/) override { }

  virtual void enterParenthesisExp(QuilParser::ParenthesisExpContext * /*ctx*/) override { }
  virtual void exitParenthesisExp(QuilParser::ParenthesisExpContext * /*ctx*/) override { }

  virtual void enterVariableExp(QuilParser::VariableExpContext * /*ctx*/) override { }
  virtual void exitVariableExp(QuilParser::VariableExpContext * /*ctx*/) override { }

  virtual void enterSignedExp(QuilParser::SignedExpContext * /*ctx*/) override { }
  virtual void exitSignedExp(QuilParser::SignedExpContext * /*ctx*/) override { }

  virtual void enterAddSubExp(QuilParser::AddSubExpContext * /*ctx*/) override { }
  virtual void exitAddSubExp(QuilParser::AddSubExpContext * /*ctx*/) override { }

  virtual void enterVariablewithbracket(QuilParser::VariablewithbracketContext * /*ctx*/) override { }
  virtual void exitVariablewithbracket(QuilParser::VariablewithbracketContext * /*ctx*/) override { }

  virtual void enterFunctionExp(QuilParser::FunctionExpContext * /*ctx*/) override { }
  virtual void exitFunctionExp(QuilParser::FunctionExpContext * /*ctx*/) override { }

  virtual void enterSegment(QuilParser::SegmentContext * /*ctx*/) override { }
  virtual void exitSegment(QuilParser::SegmentContext * /*ctx*/) override { }

  virtual void enterFunction(QuilParser::FunctionContext * /*ctx*/) override { }
  virtual void exitFunction(QuilParser::FunctionContext * /*ctx*/) override { }

  virtual void enterSign(QuilParser::SignContext * /*ctx*/) override { }
  virtual void exitSign(QuilParser::SignContext * /*ctx*/) override { }

  virtual void enterNumber(QuilParser::NumberContext * /*ctx*/) override { }
  virtual void exitNumber(QuilParser::NumberContext * /*ctx*/) override { }

  virtual void enterImaginaryN(QuilParser::ImaginaryNContext * /*ctx*/) override { }
  virtual void exitImaginaryN(QuilParser::ImaginaryNContext * /*ctx*/) override { }

  virtual void enterRealN(QuilParser::RealNContext * /*ctx*/) override { }
  virtual void exitRealN(QuilParser::RealNContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace quil
