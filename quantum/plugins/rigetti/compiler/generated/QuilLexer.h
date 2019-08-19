
// Generated from Quil.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"


namespace quil {


class  QuilLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, DEFGATE = 9, DEFCIRCUIT = 10, MEASURE = 11, LABEL = 12, HALT = 13, 
    JUMP = 14, JUMPWHEN = 15, JUMPUNLESS = 16, RESET = 17, WAIT = 18, NOP = 19, 
    INCLUDE = 20, PRAGMA = 21, FALSE = 22, TRUE = 23, NOT = 24, AND = 25, 
    OR = 26, MOVE = 27, EXCHANGE = 28, PI = 29, I = 30, SIN = 31, COS = 32, 
    SQRT = 33, EXP = 34, CIS = 35, PLUS = 36, MINUS = 37, TIMES = 38, DIVIDE = 39, 
    POWER = 40, IDENTIFIER = 41, INT = 42, FLOAT = 43, STRING = 44, PERIOD = 45, 
    COMMA = 46, LPAREN = 47, RPAREN = 48, LBRACKET = 49, RBRACKET = 50, 
    COLON = 51, PERCENTAGE = 52, AT = 53, QUOTE = 54, UNDERSCORE = 55, TAB = 56, 
    NEWLINE = 57, COMMENT = 58, SPACE = 59, INVALID = 60
  };

  QuilLexer(antlr4::CharStream *input);
  ~QuilLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace quil
