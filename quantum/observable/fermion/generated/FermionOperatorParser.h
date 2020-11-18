
// Generated from FermionOperator.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"


namespace fermion {


class  FermionOperatorParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, COMMENT = 9, REAL = 10, INT = 11, WS = 12, EOL = 13
  };

  enum {
    RuleFermionSrc = 0, RulePlusorminus = 1, RuleTerm = 2, RuleFermion = 3, 
    RuleOp = 4, RuleCarat = 5, RuleCoeff = 6, RuleComplex = 7, RuleReal = 8, 
    RuleComment = 9, RuleScientific = 10
  };

  FermionOperatorParser(antlr4::TokenStream *input);
  ~FermionOperatorParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class FermionSrcContext;
  class PlusorminusContext;
  class TermContext;
  class FermionContext;
  class OpContext;
  class CaratContext;
  class CoeffContext;
  class ComplexContext;
  class RealContext;
  class CommentContext;
  class ScientificContext; 

  class  FermionSrcContext : public antlr4::ParserRuleContext {
  public:
    FermionSrcContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    std::vector<PlusorminusContext *> plusorminus();
    PlusorminusContext* plusorminus(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FermionSrcContext* fermionSrc();

  class  PlusorminusContext : public antlr4::ParserRuleContext {
  public:
    PlusorminusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  PlusorminusContext* plusorminus();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CoeffContext *coeff();
    std::vector<FermionContext *> fermion();
    FermionContext* fermion(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  TermContext* term();

  class  FermionContext : public antlr4::ParserRuleContext {
  public:
    FermionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    OpContext *op();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FermionContext* fermion();

  class  OpContext : public antlr4::ParserRuleContext {
  public:
    OpContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INT();
    CaratContext *carat();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  OpContext* op();

  class  CaratContext : public antlr4::ParserRuleContext {
  public:
    CaratContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CaratContext* carat();

  class  CoeffContext : public antlr4::ParserRuleContext {
  public:
    CoeffContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ComplexContext *complex();
    RealContext *real();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CoeffContext* coeff();

  class  ComplexContext : public antlr4::ParserRuleContext {
  public:
    ComplexContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<RealContext *> real();
    RealContext* real(size_t i);
    std::vector<antlr4::tree::TerminalNode *> INT();
    antlr4::tree::TerminalNode* INT(size_t i);
    std::vector<ScientificContext *> scientific();
    ScientificContext* scientific(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ComplexContext* complex();

  class  RealContext : public antlr4::ParserRuleContext {
  public:
    RealContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REAL();
    ScientificContext *scientific();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  RealContext* real();

  class  CommentContext : public antlr4::ParserRuleContext {
  public:
    CommentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMENT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CommentContext* comment();

  class  ScientificContext : public antlr4::ParserRuleContext {
  public:
    ScientificContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> INT();
    antlr4::tree::TerminalNode* INT(size_t i);
    antlr4::tree::TerminalNode *REAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ScientificContext* scientific();


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

}  // namespace fermion
