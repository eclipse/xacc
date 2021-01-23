
// Generated from Quil.g4 by ANTLR 4.9.1

#pragma once


#include "antlr4-runtime.h"


namespace quil {


class  QuilLexer : public antlr4::Lexer {
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

  explicit QuilLexer(antlr4::CharStream *input);
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
