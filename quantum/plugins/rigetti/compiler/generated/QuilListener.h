
// Generated from Quil.g4 by ANTLR 4.9.1

#pragma once


#include "antlr4-runtime.h"
#include "QuilParser.h"


namespace quil {

/**
 * This interface defines an abstract listener for a parse tree produced by QuilParser.
 */
class  QuilListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterQuil(QuilParser::QuilContext *ctx) = 0;
  virtual void exitQuil(QuilParser::QuilContext *ctx) = 0;

  virtual void enterAllInstr(QuilParser::AllInstrContext *ctx) = 0;
  virtual void exitAllInstr(QuilParser::AllInstrContext *ctx) = 0;

  virtual void enterInstr(QuilParser::InstrContext *ctx) = 0;
  virtual void exitInstr(QuilParser::InstrContext *ctx) = 0;

  virtual void enterGate(QuilParser::GateContext *ctx) = 0;
  virtual void exitGate(QuilParser::GateContext *ctx) = 0;

  virtual void enterName(QuilParser::NameContext *ctx) = 0;
  virtual void exitName(QuilParser::NameContext *ctx) = 0;

  virtual void enterQubit(QuilParser::QubitContext *ctx) = 0;
  virtual void exitQubit(QuilParser::QubitContext *ctx) = 0;

  virtual void enterParam(QuilParser::ParamContext *ctx) = 0;
  virtual void exitParam(QuilParser::ParamContext *ctx) = 0;

  virtual void enterModifier(QuilParser::ModifierContext *ctx) = 0;
  virtual void exitModifier(QuilParser::ModifierContext *ctx) = 0;

  virtual void enterDefGate(QuilParser::DefGateContext *ctx) = 0;
  virtual void exitDefGate(QuilParser::DefGateContext *ctx) = 0;

  virtual void enterDefGateAsPauli(QuilParser::DefGateAsPauliContext *ctx) = 0;
  virtual void exitDefGateAsPauli(QuilParser::DefGateAsPauliContext *ctx) = 0;

  virtual void enterVariable(QuilParser::VariableContext *ctx) = 0;
  virtual void exitVariable(QuilParser::VariableContext *ctx) = 0;

  virtual void enterGatetype(QuilParser::GatetypeContext *ctx) = 0;
  virtual void exitGatetype(QuilParser::GatetypeContext *ctx) = 0;

  virtual void enterMatrix(QuilParser::MatrixContext *ctx) = 0;
  virtual void exitMatrix(QuilParser::MatrixContext *ctx) = 0;

  virtual void enterMatrixRow(QuilParser::MatrixRowContext *ctx) = 0;
  virtual void exitMatrixRow(QuilParser::MatrixRowContext *ctx) = 0;

  virtual void enterPauliTerms(QuilParser::PauliTermsContext *ctx) = 0;
  virtual void exitPauliTerms(QuilParser::PauliTermsContext *ctx) = 0;

  virtual void enterPauliTerm(QuilParser::PauliTermContext *ctx) = 0;
  virtual void exitPauliTerm(QuilParser::PauliTermContext *ctx) = 0;

  virtual void enterDefCircuit(QuilParser::DefCircuitContext *ctx) = 0;
  virtual void exitDefCircuit(QuilParser::DefCircuitContext *ctx) = 0;

  virtual void enterQubitVariable(QuilParser::QubitVariableContext *ctx) = 0;
  virtual void exitQubitVariable(QuilParser::QubitVariableContext *ctx) = 0;

  virtual void enterCircuitQubit(QuilParser::CircuitQubitContext *ctx) = 0;
  virtual void exitCircuitQubit(QuilParser::CircuitQubitContext *ctx) = 0;

  virtual void enterCircuitGate(QuilParser::CircuitGateContext *ctx) = 0;
  virtual void exitCircuitGate(QuilParser::CircuitGateContext *ctx) = 0;

  virtual void enterCircuitMeasure(QuilParser::CircuitMeasureContext *ctx) = 0;
  virtual void exitCircuitMeasure(QuilParser::CircuitMeasureContext *ctx) = 0;

  virtual void enterCircuitResetState(QuilParser::CircuitResetStateContext *ctx) = 0;
  virtual void exitCircuitResetState(QuilParser::CircuitResetStateContext *ctx) = 0;

  virtual void enterCircuitInstr(QuilParser::CircuitInstrContext *ctx) = 0;
  virtual void exitCircuitInstr(QuilParser::CircuitInstrContext *ctx) = 0;

  virtual void enterCircuit(QuilParser::CircuitContext *ctx) = 0;
  virtual void exitCircuit(QuilParser::CircuitContext *ctx) = 0;

  virtual void enterMeasure(QuilParser::MeasureContext *ctx) = 0;
  virtual void exitMeasure(QuilParser::MeasureContext *ctx) = 0;

  virtual void enterAddr(QuilParser::AddrContext *ctx) = 0;
  virtual void exitAddr(QuilParser::AddrContext *ctx) = 0;

  virtual void enterDefLabel(QuilParser::DefLabelContext *ctx) = 0;
  virtual void exitDefLabel(QuilParser::DefLabelContext *ctx) = 0;

  virtual void enterLabel(QuilParser::LabelContext *ctx) = 0;
  virtual void exitLabel(QuilParser::LabelContext *ctx) = 0;

  virtual void enterHalt(QuilParser::HaltContext *ctx) = 0;
  virtual void exitHalt(QuilParser::HaltContext *ctx) = 0;

  virtual void enterJump(QuilParser::JumpContext *ctx) = 0;
  virtual void exitJump(QuilParser::JumpContext *ctx) = 0;

  virtual void enterJumpWhen(QuilParser::JumpWhenContext *ctx) = 0;
  virtual void exitJumpWhen(QuilParser::JumpWhenContext *ctx) = 0;

  virtual void enterJumpUnless(QuilParser::JumpUnlessContext *ctx) = 0;
  virtual void exitJumpUnless(QuilParser::JumpUnlessContext *ctx) = 0;

  virtual void enterResetState(QuilParser::ResetStateContext *ctx) = 0;
  virtual void exitResetState(QuilParser::ResetStateContext *ctx) = 0;

  virtual void enterWait(QuilParser::WaitContext *ctx) = 0;
  virtual void exitWait(QuilParser::WaitContext *ctx) = 0;

  virtual void enterMemoryDescriptor(QuilParser::MemoryDescriptorContext *ctx) = 0;
  virtual void exitMemoryDescriptor(QuilParser::MemoryDescriptorContext *ctx) = 0;

  virtual void enterOffsetDescriptor(QuilParser::OffsetDescriptorContext *ctx) = 0;
  virtual void exitOffsetDescriptor(QuilParser::OffsetDescriptorContext *ctx) = 0;

  virtual void enterClassicalUnary(QuilParser::ClassicalUnaryContext *ctx) = 0;
  virtual void exitClassicalUnary(QuilParser::ClassicalUnaryContext *ctx) = 0;

  virtual void enterClassicalBinary(QuilParser::ClassicalBinaryContext *ctx) = 0;
  virtual void exitClassicalBinary(QuilParser::ClassicalBinaryContext *ctx) = 0;

  virtual void enterLogicalBinaryOp(QuilParser::LogicalBinaryOpContext *ctx) = 0;
  virtual void exitLogicalBinaryOp(QuilParser::LogicalBinaryOpContext *ctx) = 0;

  virtual void enterArithmeticBinaryOp(QuilParser::ArithmeticBinaryOpContext *ctx) = 0;
  virtual void exitArithmeticBinaryOp(QuilParser::ArithmeticBinaryOpContext *ctx) = 0;

  virtual void enterMove(QuilParser::MoveContext *ctx) = 0;
  virtual void exitMove(QuilParser::MoveContext *ctx) = 0;

  virtual void enterExchange(QuilParser::ExchangeContext *ctx) = 0;
  virtual void exitExchange(QuilParser::ExchangeContext *ctx) = 0;

  virtual void enterConvert(QuilParser::ConvertContext *ctx) = 0;
  virtual void exitConvert(QuilParser::ConvertContext *ctx) = 0;

  virtual void enterLoad(QuilParser::LoadContext *ctx) = 0;
  virtual void exitLoad(QuilParser::LoadContext *ctx) = 0;

  virtual void enterStore(QuilParser::StoreContext *ctx) = 0;
  virtual void exitStore(QuilParser::StoreContext *ctx) = 0;

  virtual void enterClassicalComparison(QuilParser::ClassicalComparisonContext *ctx) = 0;
  virtual void exitClassicalComparison(QuilParser::ClassicalComparisonContext *ctx) = 0;

  virtual void enterNop(QuilParser::NopContext *ctx) = 0;
  virtual void exitNop(QuilParser::NopContext *ctx) = 0;

  virtual void enterInclude(QuilParser::IncludeContext *ctx) = 0;
  virtual void exitInclude(QuilParser::IncludeContext *ctx) = 0;

  virtual void enterPragma(QuilParser::PragmaContext *ctx) = 0;
  virtual void exitPragma(QuilParser::PragmaContext *ctx) = 0;

  virtual void enterPragma_name(QuilParser::Pragma_nameContext *ctx) = 0;
  virtual void exitPragma_name(QuilParser::Pragma_nameContext *ctx) = 0;

  virtual void enterNumberExp(QuilParser::NumberExpContext *ctx) = 0;
  virtual void exitNumberExp(QuilParser::NumberExpContext *ctx) = 0;

  virtual void enterPowerExp(QuilParser::PowerExpContext *ctx) = 0;
  virtual void exitPowerExp(QuilParser::PowerExpContext *ctx) = 0;

  virtual void enterMulDivExp(QuilParser::MulDivExpContext *ctx) = 0;
  virtual void exitMulDivExp(QuilParser::MulDivExpContext *ctx) = 0;

  virtual void enterParenthesisExp(QuilParser::ParenthesisExpContext *ctx) = 0;
  virtual void exitParenthesisExp(QuilParser::ParenthesisExpContext *ctx) = 0;

  virtual void enterVariableExp(QuilParser::VariableExpContext *ctx) = 0;
  virtual void exitVariableExp(QuilParser::VariableExpContext *ctx) = 0;

  virtual void enterAddrExp(QuilParser::AddrExpContext *ctx) = 0;
  virtual void exitAddrExp(QuilParser::AddrExpContext *ctx) = 0;

  virtual void enterSignedExp(QuilParser::SignedExpContext *ctx) = 0;
  virtual void exitSignedExp(QuilParser::SignedExpContext *ctx) = 0;

  virtual void enterAddSubExp(QuilParser::AddSubExpContext *ctx) = 0;
  virtual void exitAddSubExp(QuilParser::AddSubExpContext *ctx) = 0;

  virtual void enterFunctionExp(QuilParser::FunctionExpContext *ctx) = 0;
  virtual void exitFunctionExp(QuilParser::FunctionExpContext *ctx) = 0;

  virtual void enterFunction(QuilParser::FunctionContext *ctx) = 0;
  virtual void exitFunction(QuilParser::FunctionContext *ctx) = 0;

  virtual void enterSign(QuilParser::SignContext *ctx) = 0;
  virtual void exitSign(QuilParser::SignContext *ctx) = 0;

  virtual void enterNumber(QuilParser::NumberContext *ctx) = 0;
  virtual void exitNumber(QuilParser::NumberContext *ctx) = 0;

  virtual void enterImaginaryN(QuilParser::ImaginaryNContext *ctx) = 0;
  virtual void exitImaginaryN(QuilParser::ImaginaryNContext *ctx) = 0;

  virtual void enterRealN(QuilParser::RealNContext *ctx) = 0;
  virtual void exitRealN(QuilParser::RealNContext *ctx) = 0;


};

}  // namespace quil
