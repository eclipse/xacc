
// Generated from Quil.g4 by ANTLR 4.7.1

#pragma once

#include "antlr4-runtime.h"

namespace quil {

class QuilLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1,
    T__1 = 2,
    T__2 = 3,
    T__3 = 4,
    T__4 = 5,
    T__5 = 6,
    T__6 = 7,
    DEFGATE = 8,
    DEFCIRCUIT = 9,
    MEASURE = 10,
    LABEL = 11,
    HALT = 12,
    JUMP = 13,
    JUMPWHEN = 14,
    JUMPUNLESS = 15,
    RESET = 16,
    WAIT = 17,
    NOP = 18,
    INCLUDE = 19,
    PRAGMA = 20,
    FALSE = 21,
    TRUE = 22,
    NOT = 23,
    AND = 24,
    OR = 25,
    MOVE = 26,
    EXCHANGE = 27,
    PI = 28,
    I = 29,
    SIN = 30,
    COS = 31,
    SQRT = 32,
    EXP = 33,
    CIS = 34,
    PLUS = 35,
    MINUS = 36,
    TIMES = 37,
    DIVIDE = 38,
    POWER = 39,
    IDENTIFIER = 40,
    INT = 41,
    FLOAT = 42,
    STRING = 43,
    PERIOD = 44,
    COMMA = 45,
    LPAREN = 46,
    RPAREN = 47,
    LBRACKET = 48,
    RBRACKET = 49,
    COLON = 50,
    PERCENTAGE = 51,
    AT = 52,
    QUOTE = 53,
    UNDERSCORE = 54,
    TAB = 55,
    NEWLINE = 56,
    COMMENT = 57,
    SPACE = 58,
    INVALID = 59
  };

  QuilLexer(antlr4::CharStream *input);
  ~QuilLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string> &getRuleNames() const override;

  virtual const std::vector<std::string> &getChannelNames() const override;
  virtual const std::vector<std::string> &getModeNames() const override;
  virtual const std::vector<std::string> &
  getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary &getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN &getATN() const override;

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

} // namespace quil
