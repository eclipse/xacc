
// Generated from Quil.g4 by ANTLR 4.9.1

#pragma once


#include "antlr4-runtime.h"


namespace quil {


class  QuilParser : public antlr4::Parser {
public:
  enum {
    DEFGATE = 1, DEFCIRCUIT = 2, MEASURE = 3, LABEL = 4, HALT = 5, JUMP = 6, 
    JUMPWHEN = 7, JUMPUNLESS = 8, RESET = 9, WAIT = 10, NOP = 11, INCLUDE = 12, 
    PRAGMA = 13, DECLARE = 14, SHARING = 15, OFFSET = 16, AS = 17, MATRIX = 18, 
    PERMUTATION = 19, PAULISUM = 20, NEG = 21, NOT = 22, TRUE = 23, FALSE = 24, 
    AND = 25, IOR = 26, XOR = 27, OR = 28, ADD = 29, SUB = 30, MUL = 31, 
    DIV = 32, MOVE = 33, EXCHANGE = 34, CONVERT = 35, EQ = 36, GT = 37, 
    GE = 38, LT = 39, LE = 40, LOAD = 41, STORE = 42, PI = 43, I = 44, SIN = 45, 
    COS = 46, SQRT = 47, EXP = 48, CIS = 49, PLUS = 50, MINUS = 51, TIMES = 52, 
    DIVIDE = 53, POWER = 54, CONTROLLED = 55, DAGGER = 56, FORKED = 57, 
    IDENTIFIER = 58, INT = 59, FLOAT = 60, STRING = 61, PERIOD = 62, COMMA = 63, 
    LPAREN = 64, RPAREN = 65, LBRACKET = 66, RBRACKET = 67, COLON = 68, 
    PERCENTAGE = 69, AT = 70, QUOTE = 71, UNDERSCORE = 72, TAB = 73, NEWLINE = 74, 
    COMMENT = 75, SPACE = 76, INVALID = 77
  };

  enum {
    RuleQuil = 0, RuleAllInstr = 1, RuleInstr = 2, RuleGate = 3, RuleName = 4, 
    RuleQubit = 5, RuleParam = 6, RuleModifier = 7, RuleDefGate = 8, RuleDefGateAsPauli = 9, 
    RuleVariable = 10, RuleGatetype = 11, RuleMatrix = 12, RuleMatrixRow = 13, 
    RulePauliTerms = 14, RulePauliTerm = 15, RuleDefCircuit = 16, RuleQubitVariable = 17, 
    RuleCircuitQubit = 18, RuleCircuitGate = 19, RuleCircuitMeasure = 20, 
    RuleCircuitResetState = 21, RuleCircuitInstr = 22, RuleCircuit = 23, 
    RuleMeasure = 24, RuleAddr = 25, RuleDefLabel = 26, RuleLabel = 27, 
    RuleHalt = 28, RuleJump = 29, RuleJumpWhen = 30, RuleJumpUnless = 31, 
    RuleResetState = 32, RuleWait = 33, RuleMemoryDescriptor = 34, RuleOffsetDescriptor = 35, 
    RuleClassicalUnary = 36, RuleClassicalBinary = 37, RuleLogicalBinaryOp = 38, 
    RuleArithmeticBinaryOp = 39, RuleMove = 40, RuleExchange = 41, RuleConvert = 42, 
    RuleLoad = 43, RuleStore = 44, RuleClassicalComparison = 45, RuleNop = 46, 
    RuleInclude = 47, RulePragma = 48, RulePragma_name = 49, RuleExpression = 50, 
    RuleFunction = 51, RuleSign = 52, RuleNumber = 53, RuleImaginaryN = 54, 
    RuleRealN = 55
  };

  explicit QuilParser(antlr4::TokenStream *input);
  ~QuilParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class QuilContext;
  class AllInstrContext;
  class InstrContext;
  class GateContext;
  class NameContext;
  class QubitContext;
  class ParamContext;
  class ModifierContext;
  class DefGateContext;
  class DefGateAsPauliContext;
  class VariableContext;
  class GatetypeContext;
  class MatrixContext;
  class MatrixRowContext;
  class PauliTermsContext;
  class PauliTermContext;
  class DefCircuitContext;
  class QubitVariableContext;
  class CircuitQubitContext;
  class CircuitGateContext;
  class CircuitMeasureContext;
  class CircuitResetStateContext;
  class CircuitInstrContext;
  class CircuitContext;
  class MeasureContext;
  class AddrContext;
  class DefLabelContext;
  class LabelContext;
  class HaltContext;
  class JumpContext;
  class JumpWhenContext;
  class JumpUnlessContext;
  class ResetStateContext;
  class WaitContext;
  class MemoryDescriptorContext;
  class OffsetDescriptorContext;
  class ClassicalUnaryContext;
  class ClassicalBinaryContext;
  class LogicalBinaryOpContext;
  class ArithmeticBinaryOpContext;
  class MoveContext;
  class ExchangeContext;
  class ConvertContext;
  class LoadContext;
  class StoreContext;
  class ClassicalComparisonContext;
  class NopContext;
  class IncludeContext;
  class PragmaContext;
  class Pragma_nameContext;
  class ExpressionContext;
  class FunctionContext;
  class SignContext;
  class NumberContext;
  class ImaginaryNContext;
  class RealNContext; 

  class  QuilContext : public antlr4::ParserRuleContext {
  public:
    QuilContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
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
    DefGateAsPauliContext *defGateAsPauli();
    DefCircuitContext *defCircuit();
    InstrContext *instr();

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
    ClassicalComparisonContext *classicalComparison();
    LoadContext *load();
    StoreContext *store();
    NopContext *nop();
    IncludeContext *include();
    PragmaContext *pragma();
    MemoryDescriptorContext *memoryDescriptor();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  InstrContext* instr();

  class  GateContext : public antlr4::ParserRuleContext {
  public:
    GateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    std::vector<ModifierContext *> modifier();
    ModifierContext* modifier(size_t i);
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

  class  ModifierContext : public antlr4::ParserRuleContext {
  public:
    ModifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONTROLLED();
    antlr4::tree::TerminalNode *DAGGER();
    antlr4::tree::TerminalNode *FORKED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ModifierContext* modifier();

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
    antlr4::tree::TerminalNode *AS();
    GatetypeContext *gatetype();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DefGateContext* defGate();

  class  DefGateAsPauliContext : public antlr4::ParserRuleContext {
  public:
    DefGateAsPauliContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFGATE();
    NameContext *name();
    antlr4::tree::TerminalNode *AS();
    antlr4::tree::TerminalNode *PAULISUM();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *NEWLINE();
    PauliTermsContext *pauliTerms();
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

  DefGateAsPauliContext* defGateAsPauli();

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

  class  GatetypeContext : public antlr4::ParserRuleContext {
  public:
    GatetypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MATRIX();
    antlr4::tree::TerminalNode *PERMUTATION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatetypeContext* gatetype();

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

  class  PauliTermsContext : public antlr4::ParserRuleContext {
  public:
    PauliTermsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PauliTermContext *> pauliTerm();
    PauliTermContext* pauliTerm(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NEWLINE();
    antlr4::tree::TerminalNode* NEWLINE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PauliTermsContext* pauliTerms();

  class  PauliTermContext : public antlr4::ParserRuleContext {
  public:
    PauliTermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TAB();
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<QubitVariableContext *> qubitVariable();
    QubitVariableContext* qubitVariable(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PauliTermContext* pauliTerm();

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

  class  CircuitMeasureContext : public antlr4::ParserRuleContext {
  public:
    CircuitMeasureContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MEASURE();
    CircuitQubitContext *circuitQubit();
    AddrContext *addr();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitMeasureContext* circuitMeasure();

  class  CircuitResetStateContext : public antlr4::ParserRuleContext {
  public:
    CircuitResetStateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RESET();
    CircuitQubitContext *circuitQubit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CircuitResetStateContext* circuitResetState();

  class  CircuitInstrContext : public antlr4::ParserRuleContext {
  public:
    CircuitInstrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CircuitGateContext *circuitGate();
    CircuitMeasureContext *circuitMeasure();
    CircuitResetStateContext *circuitResetState();
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
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *INT();
    antlr4::tree::TerminalNode *RBRACKET();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AddrContext* addr();

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
    QubitContext *qubit();

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

  class  MemoryDescriptorContext : public antlr4::ParserRuleContext {
  public:
    MemoryDescriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DECLARE();
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *LBRACKET();
    antlr4::tree::TerminalNode *INT();
    antlr4::tree::TerminalNode *RBRACKET();
    antlr4::tree::TerminalNode *SHARING();
    std::vector<OffsetDescriptorContext *> offsetDescriptor();
    OffsetDescriptorContext* offsetDescriptor(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MemoryDescriptorContext* memoryDescriptor();

  class  OffsetDescriptorContext : public antlr4::ParserRuleContext {
  public:
    OffsetDescriptorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OFFSET();
    antlr4::tree::TerminalNode *INT();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OffsetDescriptorContext* offsetDescriptor();

  class  ClassicalUnaryContext : public antlr4::ParserRuleContext {
  public:
    ClassicalUnaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AddrContext *addr();
    antlr4::tree::TerminalNode *NEG();
    antlr4::tree::TerminalNode *NOT();
    antlr4::tree::TerminalNode *TRUE();
    antlr4::tree::TerminalNode *FALSE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalUnaryContext* classicalUnary();

  class  ClassicalBinaryContext : public antlr4::ParserRuleContext {
  public:
    ClassicalBinaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LogicalBinaryOpContext *logicalBinaryOp();
    ArithmeticBinaryOpContext *arithmeticBinaryOp();
    MoveContext *move();
    ExchangeContext *exchange();
    ConvertContext *convert();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalBinaryContext* classicalBinary();

  class  LogicalBinaryOpContext : public antlr4::ParserRuleContext {
  public:
    LogicalBinaryOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *IOR();
    antlr4::tree::TerminalNode *XOR();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LogicalBinaryOpContext* logicalBinaryOp();

  class  ArithmeticBinaryOpContext : public antlr4::ParserRuleContext {
  public:
    ArithmeticBinaryOpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    antlr4::tree::TerminalNode *ADD();
    antlr4::tree::TerminalNode *SUB();
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *DIV();
    NumberContext *number();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ArithmeticBinaryOpContext* arithmeticBinaryOp();

  class  MoveContext : public antlr4::ParserRuleContext {
  public:
    MoveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MOVE();
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    NumberContext *number();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MoveContext* move();

  class  ExchangeContext : public antlr4::ParserRuleContext {
  public:
    ExchangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EXCHANGE();
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExchangeContext* exchange();

  class  ConvertContext : public antlr4::ParserRuleContext {
  public:
    ConvertContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONVERT();
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ConvertContext* convert();

  class  LoadContext : public antlr4::ParserRuleContext {
  public:
    LoadContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LOAD();
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LoadContext* load();

  class  StoreContext : public antlr4::ParserRuleContext {
  public:
    StoreContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STORE();
    antlr4::tree::TerminalNode *IDENTIFIER();
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    NumberContext *number();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StoreContext* store();

  class  ClassicalComparisonContext : public antlr4::ParserRuleContext {
  public:
    ClassicalComparisonContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<AddrContext *> addr();
    AddrContext* addr(size_t i);
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GE();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *LE();
    NumberContext *number();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ClassicalComparisonContext* classicalComparison();

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

  class  AddrExpContext : public ExpressionContext {
  public:
    AddrExpContext(ExpressionContext *ctx);

    AddrContext *addr();
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
    antlr4::tree::TerminalNode *MINUS();

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
