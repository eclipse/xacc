
// Generated from Quil.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"


namespace quil {


class  QuilLexer : public antlr4::Lexer {
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
