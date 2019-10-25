
// Generated from OQASM2.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"


namespace oqasm {


class  OQASM2Parser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    COMMENT = 33, QREG = 34, CREG = 35, GATE = 36, MEASURE = 37, RESET = 38, 
    BARRIER = 39, OPENQASM = 40, OPAQUE = 41, ID = 42, REAL = 43, INT = 44, 
    STRING = 45, WS = 46, EOL = 47
  };

  enum {
    RuleXaccsrc = 0, RuleXacckernel = 1, RuleTypedparam = 2, RuleType = 3, 
    RuleKernelcall = 4, RuleMainprog = 5, RuleProgram = 6, RuleLine = 7, 
    RuleStatement = 8, RuleComment = 9, RuleInclude = 10, RuleFilename = 11, 
    RuleRegdecl = 12, RuleQregister = 13, RuleCregister = 14, RuleRegistersize = 15, 
    RuleGatedecl = 16, RuleGatename = 17, RuleGatearglist = 18, RuleGatearg = 19, 
    RuleGatebody = 20, RuleGateprog = 21, RuleGateline = 22, RuleParamlist = 23, 
    RuleParam = 24, RuleOpaque = 25, RuleOpaquename = 26, RuleOpaquearglist = 27, 
    RuleOpaquearg = 28, RuleQop = 29, RuleUop = 30, RuleConditional = 31, 
    RuleAction = 32, RuleExplist = 33, RuleExp = 34, RuleUnaryop = 35, RuleQreg = 36, 
    RuleCreg = 37, RuleGate = 38, RuleMeasure = 39, RuleReeset = 40, RuleBarrier = 41, 
    RuleId = 42, RuleReal = 43, RuleString = 44
  };

  OQASM2Parser(antlr4::TokenStream *input);
  ~OQASM2Parser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class XaccsrcContext;
  class XacckernelContext;
  class TypedparamContext;
  class TypeContext;
  class KernelcallContext;
  class MainprogContext;
  class ProgramContext;
  class LineContext;
  class StatementContext;
  class CommentContext;
  class IncludeContext;
  class FilenameContext;
  class RegdeclContext;
  class QregisterContext;
  class CregisterContext;
  class RegistersizeContext;
  class GatedeclContext;
  class GatenameContext;
  class GatearglistContext;
  class GateargContext;
  class GatebodyContext;
  class GateprogContext;
  class GatelineContext;
  class ParamlistContext;
  class ParamContext;
  class OpaqueContext;
  class OpaquenameContext;
  class OpaquearglistContext;
  class OpaqueargContext;
  class QopContext;
  class UopContext;
  class ConditionalContext;
  class ActionContext;
  class ExplistContext;
  class ExpContext;
  class UnaryopContext;
  class QregContext;
  class CregContext;
  class GateContext;
  class MeasureContext;
  class ReesetContext;
  class BarrierContext;
  class IdContext;
  class RealContext;
  class StringContext; 

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
    OQASM2Parser::IdContext *kernelname = nullptr;;
    OQASM2Parser::IdContext *acceleratorbuffer = nullptr;;
    XacckernelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MainprogContext *mainprog();
    std::vector<IdContext *> id();
    IdContext* id(size_t i);
    std::vector<TypedparamContext *> typedparam();
    TypedparamContext* typedparam(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XacckernelContext* xacckernel();

  class  TypedparamContext : public antlr4::ParserRuleContext {
  public:
    TypedparamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    ParamContext *param();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TypedparamContext* typedparam();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    TypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TypeContext* type();

  class  KernelcallContext : public antlr4::ParserRuleContext {
  public:
    OQASM2Parser::IdContext *kernelname = nullptr;;
    KernelcallContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    std::vector<ParamContext *> param();
    ParamContext* param(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  KernelcallContext* kernelcall();

  class  MainprogContext : public antlr4::ParserRuleContext {
  public:
    MainprogContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<CommentContext *> comment();
    CommentContext* comment(size_t i);
    antlr4::tree::TerminalNode *OPENQASM();
    RealContext *real();
    ProgramContext *program();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MainprogContext* mainprog();

  class  ProgramContext : public antlr4::ParserRuleContext {
  public:
    ProgramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LineContext *> line();
    LineContext* line(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ProgramContext* program();

  class  LineContext : public antlr4::ParserRuleContext {
  public:
    LineContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);
    CommentContext *comment();
    std::vector<IncludeContext *> include();
    IncludeContext* include(size_t i);
    antlr4::tree::TerminalNode *OPENQASM();
    RealContext *real();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LineContext* line();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RegdeclContext *regdecl();
    GatedeclContext *gatedecl();
    OpaqueContext *opaque();
    QopContext *qop();
    ConditionalContext *conditional();
    KernelcallContext *kernelcall();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StatementContext* statement();

  class  CommentContext : public antlr4::ParserRuleContext {
  public:
    CommentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CommentContext* comment();

  class  IncludeContext : public antlr4::ParserRuleContext {
  public:
    IncludeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FilenameContext *filename();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IncludeContext* include();

  class  FilenameContext : public antlr4::ParserRuleContext {
  public:
    FilenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StringContext *string();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FilenameContext* filename();

  class  RegdeclContext : public antlr4::ParserRuleContext {
  public:
    RegdeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    QregContext *qreg();
    QregisterContext *qregister();
    CregContext *creg();
    CregisterContext *cregister();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RegdeclContext* regdecl();

  class  QregisterContext : public antlr4::ParserRuleContext {
  public:
    QregisterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    RegistersizeContext *registersize();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QregisterContext* qregister();

  class  CregisterContext : public antlr4::ParserRuleContext {
  public:
    CregisterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    RegistersizeContext *registersize();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CregisterContext* cregister();

  class  RegistersizeContext : public antlr4::ParserRuleContext {
  public:
    RegistersizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RegistersizeContext* registersize();

  class  GatedeclContext : public antlr4::ParserRuleContext {
  public:
    GatedeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    GateContext *gate();
    GatenameContext *gatename();
    GatearglistContext *gatearglist();
    GatebodyContext *gatebody();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatedeclContext* gatedecl();

  class  GatenameContext : public antlr4::ParserRuleContext {
  public:
    GatenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    ParamlistContext *paramlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatenameContext* gatename();

  class  GatearglistContext : public antlr4::ParserRuleContext {
  public:
    GatearglistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<GateargContext *> gatearg();
    GateargContext* gatearg(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatearglistContext* gatearglist();

  class  GateargContext : public antlr4::ParserRuleContext {
  public:
    GateargContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GateargContext* gatearg();

  class  GatebodyContext : public antlr4::ParserRuleContext {
  public:
    GatebodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    GateprogContext *gateprog();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatebodyContext* gatebody();

  class  GateprogContext : public antlr4::ParserRuleContext {
  public:
    GateprogContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<GatelineContext *> gateline();
    GatelineContext* gateline(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GateprogContext* gateprog();

  class  GatelineContext : public antlr4::ParserRuleContext {
  public:
    GatelineContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    UopContext *uop();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GatelineContext* gateline();

  class  ParamlistContext : public antlr4::ParserRuleContext {
  public:
    ParamlistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ParamContext *param();
    ParamlistContext *paramlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParamlistContext* paramlist();

  class  ParamContext : public antlr4::ParserRuleContext {
  public:
    ParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParamContext* param();

  class  OpaqueContext : public antlr4::ParserRuleContext {
  public:
    OpaqueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPAQUE();
    OpaquenameContext *opaquename();
    OpaquearglistContext *opaquearglist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpaqueContext* opaque();

  class  OpaquenameContext : public antlr4::ParserRuleContext {
  public:
    OpaquenameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    ParamlistContext *paramlist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpaquenameContext* opaquename();

  class  OpaquearglistContext : public antlr4::ParserRuleContext {
  public:
    OpaquearglistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    OpaqueargContext *opaquearg();
    OpaquearglistContext *opaquearglist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpaquearglistContext* opaquearglist();

  class  OpaqueargContext : public antlr4::ParserRuleContext {
  public:
    OpaqueargContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    antlr4::tree::TerminalNode *INT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpaqueargContext* opaquearg();

  class  QopContext : public antlr4::ParserRuleContext {
  public:
    QopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    UopContext *uop();
    ReesetContext *reeset();
    MeasureContext *measure();
    BarrierContext *barrier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QopContext* qop();

  class  UopContext : public antlr4::ParserRuleContext {
  public:
    UopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    UopContext() = default;
    void copyFrom(UopContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  UContext : public UopContext {
  public:
    UContext(UopContext *ctx);

    GateargContext *gatearg();
    ExplistContext *explist();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  CXContext : public UopContext {
  public:
    CXContext(UopContext *ctx);

    std::vector<GateargContext *> gatearg();
    GateargContext* gatearg(size_t i);
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  class  UserDefGateContext : public UopContext {
  public:
    UserDefGateContext(UopContext *ctx);

    GatenameContext *gatename();
    GatearglistContext *gatearglist();
    ExplistContext *explist();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
  };

  UopContext* uop();

  class  ConditionalContext : public antlr4::ParserRuleContext {
  public:
    ConditionalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    antlr4::tree::TerminalNode *INT();
    ActionContext *action();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ConditionalContext* conditional();

  class  ActionContext : public antlr4::ParserRuleContext {
  public:
    ActionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    QopContext *qop();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ActionContext* action();

  class  ExplistContext : public antlr4::ParserRuleContext {
  public:
    ExplistContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpContext *> exp();
    ExpContext* exp(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExplistContext* explist();

  class  ExpContext : public antlr4::ParserRuleContext {
  public:
    ExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    RealContext *real();
    antlr4::tree::TerminalNode *INT();
    IdContext *id();
    std::vector<ExpContext *> exp();
    ExpContext* exp(size_t i);
    UnaryopContext *unaryop();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExpContext* exp();
  ExpContext* exp(int precedence);
  class  UnaryopContext : public antlr4::ParserRuleContext {
  public:
    UnaryopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  UnaryopContext* unaryop();

  class  QregContext : public antlr4::ParserRuleContext {
  public:
    QregContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *QREG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  QregContext* qreg();

  class  CregContext : public antlr4::ParserRuleContext {
  public:
    CregContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CREG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CregContext* creg();

  class  GateContext : public antlr4::ParserRuleContext {
  public:
    GateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GATE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  GateContext* gate();

  class  MeasureContext : public antlr4::ParserRuleContext {
  public:
    OQASM2Parser::GateargContext *qbit = nullptr;;
    OQASM2Parser::GateargContext *cbit = nullptr;;
    MeasureContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MEASURE();
    QregisterContext *qregister();
    CregisterContext *cregister();
    std::vector<GateargContext *> gatearg();
    GateargContext* gatearg(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  MeasureContext* measure();

  class  ReesetContext : public antlr4::ParserRuleContext {
  public:
    ReesetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RESET();
    GateargContext *gatearg();
    QregisterContext *qregister();
    CregisterContext *cregister();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ReesetContext* reeset();

  class  BarrierContext : public antlr4::ParserRuleContext {
  public:
    BarrierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BARRIER();
    GatearglistContext *gatearglist();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BarrierContext* barrier();

  class  IdContext : public antlr4::ParserRuleContext {
  public:
    IdContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IdContext* id();

  class  RealContext : public antlr4::ParserRuleContext {
  public:
    RealContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RealContext* real();

  class  StringContext : public antlr4::ParserRuleContext {
  public:
    StringContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StringContext* string();


  virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool expSempred(ExpContext *_localctx, size_t predicateIndex);

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

}  // namespace oqasm
