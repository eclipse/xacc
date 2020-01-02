
// Generated from xasm.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"


namespace xasm {


class  xasmParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, COMMENT = 38, 
    ID = 39, REAL = 40, INT = 41, STRING = 42, WS = 43, EOL = 44
  };

  enum {
    RuleXaccsrc = 0, RuleXacckernel = 1, RuleXacclambda = 2, RuleTypedparam = 3, 
    RuleType = 4, RuleMainprog = 5, RuleProgram = 6, RuleLine = 7, RuleStatement = 8, 
    RuleComment = 9, RuleForstmt = 10, RuleInstruction = 11, RuleBufferList = 12, 
    RuleParamList = 13, RuleParameter = 14, RuleComposite_generator = 15, 
    RuleBufferIndex = 16, RuleOptionsMap = 17, RuleOptionsType = 18, RuleExplist = 19, 
    RuleExp = 20, RuleUnaryop = 21, RuleId = 22, RuleReal = 23, RuleString = 24
  };

  xasmParser(antlr4::TokenStream *input);
  ~xasmParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class XaccsrcContext;
  class XacckernelContext;
  class XacclambdaContext;
  class TypedparamContext;
  class TypeContext;
  class MainprogContext;
  class ProgramContext;
  class LineContext;
  class StatementContext;
  class CommentContext;
  class ForstmtContext;
  class InstructionContext;
  class BufferListContext;
  class ParamListContext;
  class ParameterContext;
  class Composite_generatorContext;
  class BufferIndexContext;
  class OptionsMapContext;
  class OptionsTypeContext;
  class ExplistContext;
  class ExpContext;
  class UnaryopContext;
  class IdContext;
  class RealContext;
  class StringContext; 

  class  XaccsrcContext : public antlr4::ParserRuleContext {
  public:
    XaccsrcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    XacckernelContext *xacckernel();
    XacclambdaContext *xacclambda();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XaccsrcContext* xaccsrc();

  class  XacckernelContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *kernelname = nullptr;;
    XacckernelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TypedparamContext *> typedparam();
    TypedparamContext* typedparam(size_t i);
    MainprogContext *mainprog();
    IdContext *id();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XacckernelContext* xacckernel();

  class  XacclambdaContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *acceleratorbuffer = nullptr;;
    XacclambdaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MainprogContext *mainprog();
    IdContext *id();
    std::vector<TypedparamContext *> typedparam();
    TypedparamContext* typedparam(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  XacclambdaContext* xacclambda();

  class  TypedparamContext : public antlr4::ParserRuleContext {
  public:
    TypedparamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    IdContext *id();

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

  class  MainprogContext : public antlr4::ParserRuleContext {
  public:
    MainprogContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
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

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  LineContext* line();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    InstructionContext *instruction();
    Composite_generatorContext *composite_generator();
    ForstmtContext *forstmt();

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

  class  ForstmtContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *varname = nullptr;;
    antlr4::Token *start = nullptr;;
    antlr4::Token *comparator = nullptr;;
    antlr4::Token *end = nullptr;;
    antlr4::Token *inc_or_dec = nullptr;;
    xasmParser::ProgramContext *forScope = nullptr;;
    ForstmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdContext *> id();
    IdContext* id(size_t i);
    std::vector<antlr4::tree::TerminalNode *> INT();
    antlr4::tree::TerminalNode* INT(size_t i);
    ProgramContext *program();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ForstmtContext* forstmt();

  class  InstructionContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *inst_name = nullptr;;
    xasmParser::BufferListContext *buffer_list = nullptr;;
    xasmParser::ParamListContext *param_list = nullptr;;
    InstructionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    BufferListContext *bufferList();
    ParamListContext *paramList();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  InstructionContext* instruction();

  class  BufferListContext : public antlr4::ParserRuleContext {
  public:
    BufferListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<BufferIndexContext *> bufferIndex();
    BufferIndexContext* bufferIndex(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BufferListContext* bufferList();

  class  ParamListContext : public antlr4::ParserRuleContext {
  public:
    ParamListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ParameterContext *> parameter();
    ParameterContext* parameter(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParamListContext* paramList();

  class  ParameterContext : public antlr4::ParserRuleContext {
  public:
    ParameterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpContext *exp();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ParameterContext* parameter();

  class  Composite_generatorContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *composite_name = nullptr;;
    xasmParser::IdContext *buffer_name = nullptr;;
    xasmParser::ParamListContext *composite_params = nullptr;;
    xasmParser::OptionsMapContext *composite_options = nullptr;;
    Composite_generatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdContext *> id();
    IdContext* id(size_t i);
    ParamListContext *paramList();
    OptionsMapContext *optionsMap();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Composite_generatorContext* composite_generator();

  class  BufferIndexContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::IdContext *buffer_name = nullptr;;
    antlr4::Token *idx = nullptr;;
    xasmParser::ExpContext *var_idx = nullptr;;
    BufferIndexContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    antlr4::tree::TerminalNode *INT();
    ExpContext *exp();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BufferIndexContext* bufferIndex();

  class  OptionsMapContext : public antlr4::ParserRuleContext {
  public:
    OptionsMapContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<OptionsTypeContext *> optionsType();
    OptionsTypeContext* optionsType(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OptionsMapContext* optionsMap();

  class  OptionsTypeContext : public antlr4::ParserRuleContext {
  public:
    xasmParser::StringContext *key = nullptr;;
    xasmParser::ExpContext *value = nullptr;;
    OptionsTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StringContext *string();
    ExpContext *exp();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OptionsTypeContext* optionsType();

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
    xasmParser::IdContext *var_name = nullptr;;
    antlr4::Token *idx = nullptr;;
    ExpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdContext *id();
    std::vector<ExpContext *> exp();
    ExpContext* exp(size_t i);
    UnaryopContext *unaryop();
    StringContext *string();
    RealContext *real();
    antlr4::tree::TerminalNode *INT();

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

}  // namespace xasm
