
// Generated from Quil.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"


namespace quil {


class  QuilParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, DEFGATE = 11, DEFCIRCUIT = 12, MEASURE = 13, 
    LABEL = 14, HALT = 15, JUMP = 16, JUMPWHEN = 17, JUMPUNLESS = 18, RESET = 19, 
    WAIT = 20, NOP = 21, INCLUDE = 22, PRAGMA = 23, FALSE = 24, TRUE = 25, 
    NOT = 26, AND = 27, OR = 28, MOVE = 29, EXCHANGE = 30, PI = 31, I = 32, 
    SIN = 33, COS = 34, SQRT = 35, EXP = 36, CIS = 37, PLUS = 38, MINUS = 39, 
    TIMES = 40, DIVIDE = 41, POWER = 42, IDENTIFIER = 43, INT = 44, FLOAT = 45, 
    STRING = 46, PERIOD = 47, COMMA = 48, LPAREN = 49, RPAREN = 50, LBRACKET = 51, 
    RBRACKET = 52, COLON = 53, PERCENTAGE = 54, AT = 55, QUOTE = 56, UNDERSCORE = 57, 
    TAB = 58, NEWLINE = 59, COMMENT = 60, SPACE = 61, INVALID = 62
  };

  enum {
    RuleXaccsrc = 0, RuleXacckernel = 1, RuleTypedparam = 2, RuleVariable_param_name = 3, 
    RuleType = 4, RuleId = 5, RuleKernelcall = 6, RuleQuil = 7, RuleAllInstr = 8, 
    RuleInstr = 9, RuleGate = 10, RuleName = 11, RuleQubit = 12, RuleParam = 13, 
    RuleDefGate = 14, RuleVariable = 15, RuleMatrix = 16, RuleMatrixRow = 17, 
    RuleDefCircuit = 18, RuleQubitVariable = 19, RuleCircuitQubit = 20, 
    RuleCircuitGate = 21, RuleCircuitInstr = 22, RuleCircuit = 23, RuleMeasure = 24, 
    RuleAddr = 25, RuleClassicalBit = 26, RuleDefLabel = 27, RuleLabel = 28, 
    RuleHalt = 29, RuleJump = 30, RuleJumpWhen = 31, RuleJumpUnless = 32, 
    RuleResetState = 33, RuleWait = 34, RuleClassicalUnary = 35, RuleClassicalBinary = 36, 
    RuleNop = 37, RuleInclude = 38, RulePragma = 39, RulePragma_name = 40, 
    RuleExpression = 41, RuleSegment = 42, RuleFunction = 43, RuleSign = 44, 
    RuleNumber = 45, RuleImaginaryN = 46, RuleRealN = 47
  };

  QuilParser(antlr4::TokenStream *input);
  ~QuilParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class XaccsrcContext;
  class XacckernelContext;
  class TypedparamContext;
  class Variable_param_nameContext;
  class TypeContext;
  class IdContext;
  class KernelcallContext;
  class QuilContext;
  class AllInstrContext;
  class InstrContext;
  class GateContext;
  class NameContext;
  class QubitContext;
  class ParamContext;
  class DefGateContext;
  class VariableContext;
  class MatrixContext;
  class MatrixRowContext;
  class DefCircuitContext;
  class QubitVariableContext;
  class CircuitQubitContext;
  class CircuitGateContext;
  class CircuitInstrContext;
  class CircuitContext;
  class MeasureContext;
  class AddrContext;
  class ClassicalBitContext;
  class DefLabelContext;
  class LabelContext;
  class HaltContext;
  class JumpContext;
  class JumpWhenContext;
  class JumpUnlessContext;
  class ResetStateContext;
  class WaitContext;
  class ClassicalUnaryContext;
  class ClassicalBinaryContext;
  class NopContext;
  class IncludeContext;
  class PragmaContext;
  class Pragma_nameContext;
  class ExpressionContext;
  class SegmentContext;
  class FunctionContext;
  class SignContext;
  class NumberContext;
  class ImaginaryNContext;
  class RealNContext; 

  class  XaccsrcContext : public antlr4::ParserRuleContext {
  public:
    XaccsrcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    XacckernelContext *xacckernel();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XaccsrcContext* xaccsrc();

  class  XacckernelContext : public antlr4::ParserRuleContext {
  public:
    QuilParser::IdContext *kernelname = nullptr;;
    XacckernelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<TypedparamContext *> typedparam();
    TypedparamContext* typedparam(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    QuilContext *quil();
    IdContext *id();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XacckernelContext* xacckernel();

  class  TypedparamContext : public antlr4::ParserRuleContext {
  public:
    TypedparamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    Variable_param_nameContext *variable_param_name();
    antlr4::tree::TerminalNode *TIMES();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TypedparamContext* typedparam();

  class  Variable_param_nameContext : public antlr4::ParserRuleContext {
  public:
    Variable_param_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Variable_param_nameContext* variable_param_name();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    TypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdContext *> id();
    IdContext* id(size_t i);
    antlr4::tree::TerminalNode *COMMA();
    TypeContext *type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TypeContext* type();

  class  IdContext : public antlr4::ParserRuleContext {
  public:
    IdContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IdContext* id();

  class  KernelcallContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *kernelname = nullptr;;
    KernelcallContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  KernelcallContext* kernelcall();

  class  QuilContext : public antlr4::ParserRuleContext {
  public:
    QuilContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AllInstrContext *> allInstr();
    AllInstrContext* allInstr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NEWLINE();
    antlr4::tree::TerminalNode* NEWLINE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QuilContext* quil();

  class  AllInstrContext : public antlr4::ParserRuleContext {
  public:
    AllInstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DefGateContext *defGate();
    DefCircuitContext *defCircuit();
    InstrContext *instr();
    KernelcallContext *kernelcall();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AllInstrContext* allInstr();

  class  InstrContext : public antlr4::ParserRuleContext {
  public:
    InstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    GateContext *gate();
    MeasureContext *measure();
    DefLabelContext *defLabel();
    HaltContext *halt();
    JumpContext *jump();
    JumpWhenContext *jumpWhen();
    JumpUnlessContext *jumpUnless();
    ResetStateContext *resetState();
    WaitContext *wait();
    ClassicalUnaryContext *classicalUnary();
    ClassicalBinaryContext *classicalBinary();
    NopContext *nop();
    IncludeContext *include();
    PragmaContext *pragma();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  InstrContext* instr();

  class  GateContext : public antlr4::ParserRuleContext {
  public:
    GateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<QubitContext *> qubit();
    QubitContext* qubit(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GateContext* gate();

  class  NameContext : public antlr4::ParserRuleContext {
  public:
    NameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NameContext* name();

  class  QubitContext : public antlr4::ParserRuleContext {
  public:
    QubitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QubitContext* qubit();

  class  ParamContext : public antlr4::ParserRuleContext {
  public:
    ParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParamContext* param();

  class  DefGateContext : public antlr4::ParserRuleContext {
  public:
    DefGateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFGATE();
    NameContext *name();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *NEWLINE();
    MatrixContext *matrix();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<VariableContext *> variable();
    VariableContext* variable(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DefGateContext* defGate();

  class  VariableContext : public antlr4::ParserRuleContext {
  public:
    VariableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PERCENTAGE();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  VariableContext* variable();

  class  MatrixContext : public antlr4::ParserRuleContext {
  public:
    MatrixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<MatrixRowContext *> matrixRow();
    MatrixRowContext* matrixRow(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NEWLINE();
    antlr4::tree::TerminalNode* NEWLINE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MatrixContext* matrix();

  class  MatrixRowContext : public antlr4::ParserRuleContext {
  public:
    MatrixRowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TAB();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MatrixRowContext* matrixRow();

  class  DefCircuitContext : public antlr4::ParserRuleContext {
  public:
    DefCircuitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFCIRCUIT();
    NameContext *name();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *NEWLINE();
    CircuitContext *circuit();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<VariableContext *> variable();
    VariableContext* variable(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<QubitVariableContext *> qubitVariable();
    QubitVariableContext* qubitVariable(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DefCircuitContext* defCircuit();

  class  QubitVariableContext : public antlr4::ParserRuleContext {
  public:
    QubitVariableContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QubitVariableContext* qubitVariable();

  class  CircuitQubitContext : public antlr4::ParserRuleContext {
  public:
    CircuitQubitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    QubitContext *qubit();
    QubitVariableContext *qubitVariable();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitQubitContext* circuitQubit();

  class  CircuitGateContext : public antlr4::ParserRuleContext {
  public:
    CircuitGateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<CircuitQubitContext *> circuitQubit();
    CircuitQubitContext* circuitQubit(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitGateContext* circuitGate();

  class  CircuitInstrContext : public antlr4::ParserRuleContext {
  public:
    CircuitInstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CircuitGateContext *circuitGate();
    InstrContext *instr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitInstrContext* circuitInstr();

  class  CircuitContext : public antlr4::ParserRuleContext {
  public:
    CircuitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> TAB();
    antlr4::tree::TerminalNode* TAB(size_t i);
    std::vector<CircuitInstrContext *> circuitInstr();
    CircuitInstrContext* circuitInstr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NEWLINE();
    antlr4::tree::TerminalNode* NEWLINE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitContext* circuit();

  class  MeasureContext : public antlr4::ParserRuleContext {
  public:
    MeasureContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MEASURE();
    QubitContext *qubit();
    AddrContext *addr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MeasureContext* measure();

  class  AddrContext : public antlr4::ParserRuleContext {
  public:
    AddrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    ClassicalBitContext *classicalBit();
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AddrContext* addr();

  class  ClassicalBitContext : public antlr4::ParserRuleContext {
  public:
    ClassicalBitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> INT();
    antlr4::tree::TerminalNode* INT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalBitContext* classicalBit();

  class  DefLabelContext : public antlr4::ParserRuleContext {
  public:
    DefLabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LABEL();
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DefLabelContext* defLabel();

  class  LabelContext : public antlr4::ParserRuleContext {
  public:
    LabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LabelContext* label();

  class  HaltContext : public antlr4::ParserRuleContext {
  public:
    HaltContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HALT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  HaltContext* halt();

  class  JumpContext : public antlr4::ParserRuleContext {
  public:
    JumpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *JUMP();
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  JumpContext* jump();

  class  JumpWhenContext : public antlr4::ParserRuleContext {
  public:
    JumpWhenContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *JUMPWHEN();
    LabelContext *label();
    AddrContext *addr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  JumpWhenContext* jumpWhen();

  class  JumpUnlessContext : public antlr4::ParserRuleContext {
  public:
    JumpUnlessContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *JUMPUNLESS();
    LabelContext *label();
    AddrContext *addr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  JumpUnlessContext* jumpUnless();

  class  ResetStateContext : public antlr4::ParserRuleContext {
  public:
    ResetStateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RESET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ResetStateContext* resetState();

  class  WaitContext : public antlr4::ParserRuleContext {
  public:
    WaitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WAIT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  WaitContext* wait();

  class  ClassicalUnaryContext : public antlr4::ParserRuleContext {
  public:
    ClassicalUnaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AddrContext *addr();
    antlr4::tree::TerminalNode *TRUE();
    antlr4::tree::TerminalNode *FALSE();
    antlr4::tree::TerminalNode *NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalUnaryContext* classicalUnary();

  class  ClassicalBinaryContext : public antlr4::ParserRuleContext {
  public:
    ClassicalBinaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *MOVE();
    antlr4::tree::TerminalNode *EXCHANGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalBinaryContext* classicalBinary();

  class  NopContext : public antlr4::ParserRuleContext {
  public:
    NopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NOP();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NopContext* nop();

  class  IncludeContext : public antlr4::ParserRuleContext {
  public:
    IncludeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INCLUDE();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IncludeContext* include();

  class  PragmaContext : public antlr4::ParserRuleContext {
  public:
    PragmaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PRAGMA();
    antlr4::tree::TerminalNode *IDENTIFIER();
    std::vector<Pragma_nameContext *> pragma_name();
    Pragma_nameContext* pragma_name(size_t i);
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PragmaContext* pragma();

  class  Pragma_nameContext : public antlr4::ParserRuleContext {
  public:
    Pragma_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Pragma_nameContext* pragma_name();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    ExpressionContext() = default;
    void copyFrom(ExpressionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  SegmentExpContext : public ExpressionContext {
  public:
    SegmentExpContext(ExpressionContext *ctx);

    SegmentContext *segment();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  IdentifierExpContext : public ExpressionContext {
  public:
    IdentifierExpContext(ExpressionContext *ctx);

    antlr4::tree::TerminalNode *IDENTIFIER();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  NumberExpContext : public ExpressionContext {
  public:
    NumberExpContext(ExpressionContext *ctx);

    NumberContext *number();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  PowerExpContext : public ExpressionContext {
  public:
    PowerExpContext(ExpressionContext *ctx);

    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *POWER();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  MulDivExpContext : public ExpressionContext {
  public:
    MulDivExpContext(ExpressionContext *ctx);

    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *TIMES();
    antlr4::tree::TerminalNode *DIVIDE();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  ParenthesisExpContext : public ExpressionContext {
  public:
    ParenthesisExpContext(ExpressionContext *ctx);

    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  VariableExpContext : public ExpressionContext {
  public:
    VariableExpContext(ExpressionContext *ctx);

    VariableContext *variable();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  SignedExpContext : public ExpressionContext {
  public:
    SignedExpContext(ExpressionContext *ctx);

    SignContext *sign();
    ExpressionContext *expression();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  AddSubExpContext : public ExpressionContext {
  public:
    AddSubExpContext(ExpressionContext *ctx);

    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  VariablewithbracketContext : public ExpressionContext {
  public:
    VariablewithbracketContext(ExpressionContext *ctx);

    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *RBRACKET();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  FunctionExpContext : public ExpressionContext {
  public:
    FunctionExpContext(ExpressionContext *ctx);

    FunctionContext *function();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  SegmentContext : public antlr4::ParserRuleContext {
  public:
    SegmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACKET();
    std::vector<antlr4::tree::TerminalNode *> INT();
    antlr4::tree::TerminalNode* INT(size_t i);
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *RBRACKET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SegmentContext* segment();

  class  FunctionContext : public antlr4::ParserRuleContext {
  public:
    FunctionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SIN();
    antlr4::tree::TerminalNode *COS();
    antlr4::tree::TerminalNode *SQRT();
    antlr4::tree::TerminalNode *EXP();
    antlr4::tree::TerminalNode *CIS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FunctionContext* function();

  class  SignContext : public antlr4::ParserRuleContext {
  public:
    SignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SignContext* sign();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RealNContext *realN();
    ImaginaryNContext *imaginaryN();
    antlr4::tree::TerminalNode *I();
    antlr4::tree::TerminalNode *PI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  NumberContext* number();

  class  ImaginaryNContext : public antlr4::ParserRuleContext {
  public:
    ImaginaryNContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RealNContext *realN();
    antlr4::tree::TerminalNode *I();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ImaginaryNContext* imaginaryN();

  class  RealNContext : public antlr4::ParserRuleContext {
  public:
    RealNContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FLOAT();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RealNContext* realN();


  virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace quil
