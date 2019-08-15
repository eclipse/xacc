
// Generated from PauliOperator.g4 by ANTLR 4.7.2


#include "PauliOperatorListener.h"

#include "PauliOperatorParser.h"


using namespace antlrcpp;
using namespace pauli;
using namespace antlr4;

PauliOperatorParser::PauliOperatorParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

PauliOperatorParser::~PauliOperatorParser() {
  delete _interpreter;
}

std::string PauliOperatorParser::getGrammarFileName() const {
  return "PauliOperator.g4";
}

const std::vector<std::string>& PauliOperatorParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& PauliOperatorParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- PauliSrcContext ------------------------------------------------------------------

PauliOperatorParser::PauliSrcContext::PauliSrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PauliOperatorParser::TermContext *> PauliOperatorParser::PauliSrcContext::term() {
  return getRuleContexts<PauliOperatorParser::TermContext>();
}

PauliOperatorParser::TermContext* PauliOperatorParser::PauliSrcContext::term(size_t i) {
  return getRuleContext<PauliOperatorParser::TermContext>(i);
}

std::vector<PauliOperatorParser::PlusorminusContext *> PauliOperatorParser::PauliSrcContext::plusorminus() {
  return getRuleContexts<PauliOperatorParser::PlusorminusContext>();
}

PauliOperatorParser::PlusorminusContext* PauliOperatorParser::PauliSrcContext::plusorminus(size_t i) {
  return getRuleContext<PauliOperatorParser::PlusorminusContext>(i);
}


size_t PauliOperatorParser::PauliSrcContext::getRuleIndex() const {
  return PauliOperatorParser::RulePauliSrc;
}

void PauliOperatorParser::PauliSrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPauliSrc(this);
}

void PauliOperatorParser::PauliSrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPauliSrc(this);
}

PauliOperatorParser::PauliSrcContext* PauliOperatorParser::pauliSrc() {
  PauliSrcContext *_localctx = _tracker.createInstance<PauliSrcContext>(_ctx, getState());
  enterRule(_localctx, 0, PauliOperatorParser::RulePauliSrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(16);
    term();
    setState(22);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PauliOperatorParser::T__0

    || _la == PauliOperatorParser::T__1) {
      setState(17);
      plusorminus();
      setState(18);
      term();
      setState(24);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PlusorminusContext ------------------------------------------------------------------

PauliOperatorParser::PlusorminusContext::PlusorminusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t PauliOperatorParser::PlusorminusContext::getRuleIndex() const {
  return PauliOperatorParser::RulePlusorminus;
}

void PauliOperatorParser::PlusorminusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPlusorminus(this);
}

void PauliOperatorParser::PlusorminusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPlusorminus(this);
}

PauliOperatorParser::PlusorminusContext* PauliOperatorParser::plusorminus() {
  PlusorminusContext *_localctx = _tracker.createInstance<PlusorminusContext>(_ctx, getState());
  enterRule(_localctx, 2, PauliOperatorParser::RulePlusorminus);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(25);
    _la = _input->LA(1);
    if (!(_la == PauliOperatorParser::T__0

    || _la == PauliOperatorParser::T__1)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TermContext ------------------------------------------------------------------

PauliOperatorParser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PauliOperatorParser::CoeffContext* PauliOperatorParser::TermContext::coeff() {
  return getRuleContext<PauliOperatorParser::CoeffContext>(0);
}

std::vector<PauliOperatorParser::PauliContext *> PauliOperatorParser::TermContext::pauli() {
  return getRuleContexts<PauliOperatorParser::PauliContext>();
}

PauliOperatorParser::PauliContext* PauliOperatorParser::TermContext::pauli(size_t i) {
  return getRuleContext<PauliOperatorParser::PauliContext>(i);
}


size_t PauliOperatorParser::TermContext::getRuleIndex() const {
  return PauliOperatorParser::RuleTerm;
}

void PauliOperatorParser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void PauliOperatorParser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

PauliOperatorParser::TermContext* PauliOperatorParser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 4, PauliOperatorParser::RuleTerm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(28);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == PauliOperatorParser::T__6

    || _la == PauliOperatorParser::REAL) {
      setState(27);
      coeff();
    }
    setState(33);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PauliOperatorParser::T__2)
      | (1ULL << PauliOperatorParser::T__3)
      | (1ULL << PauliOperatorParser::T__4)
      | (1ULL << PauliOperatorParser::T__5))) != 0)) {
      setState(30);
      pauli();
      setState(35);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PauliContext ------------------------------------------------------------------

PauliOperatorParser::PauliContext::PauliContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PauliOperatorParser::PauliContext::INT() {
  return getToken(PauliOperatorParser::INT, 0);
}


size_t PauliOperatorParser::PauliContext::getRuleIndex() const {
  return PauliOperatorParser::RulePauli;
}

void PauliOperatorParser::PauliContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPauli(this);
}

void PauliOperatorParser::PauliContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPauli(this);
}

PauliOperatorParser::PauliContext* PauliOperatorParser::pauli() {
  PauliContext *_localctx = _tracker.createInstance<PauliContext>(_ctx, getState());
  enterRule(_localctx, 6, PauliOperatorParser::RulePauli);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(39);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PauliOperatorParser::T__2:
      case PauliOperatorParser::T__3:
      case PauliOperatorParser::T__4: {
        enterOuterAlt(_localctx, 1);
        setState(36);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << PauliOperatorParser::T__2)
          | (1ULL << PauliOperatorParser::T__3)
          | (1ULL << PauliOperatorParser::T__4))) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(37);
        match(PauliOperatorParser::INT);
        break;
      }

      case PauliOperatorParser::T__5: {
        enterOuterAlt(_localctx, 2);
        setState(38);
        match(PauliOperatorParser::T__5);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CoeffContext ------------------------------------------------------------------

PauliOperatorParser::CoeffContext::CoeffContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PauliOperatorParser::ComplexContext* PauliOperatorParser::CoeffContext::complex() {
  return getRuleContext<PauliOperatorParser::ComplexContext>(0);
}

PauliOperatorParser::RealContext* PauliOperatorParser::CoeffContext::real() {
  return getRuleContext<PauliOperatorParser::RealContext>(0);
}


size_t PauliOperatorParser::CoeffContext::getRuleIndex() const {
  return PauliOperatorParser::RuleCoeff;
}

void PauliOperatorParser::CoeffContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCoeff(this);
}

void PauliOperatorParser::CoeffContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCoeff(this);
}

PauliOperatorParser::CoeffContext* PauliOperatorParser::coeff() {
  CoeffContext *_localctx = _tracker.createInstance<CoeffContext>(_ctx, getState());
  enterRule(_localctx, 8, PauliOperatorParser::RuleCoeff);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(43);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PauliOperatorParser::T__6: {
        enterOuterAlt(_localctx, 1);
        setState(41);
        complex();
        break;
      }

      case PauliOperatorParser::REAL: {
        enterOuterAlt(_localctx, 2);
        setState(42);
        real();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComplexContext ------------------------------------------------------------------

PauliOperatorParser::ComplexContext::ComplexContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PauliOperatorParser::RealContext *> PauliOperatorParser::ComplexContext::real() {
  return getRuleContexts<PauliOperatorParser::RealContext>();
}

PauliOperatorParser::RealContext* PauliOperatorParser::ComplexContext::real(size_t i) {
  return getRuleContext<PauliOperatorParser::RealContext>(i);
}

std::vector<tree::TerminalNode *> PauliOperatorParser::ComplexContext::INT() {
  return getTokens(PauliOperatorParser::INT);
}

tree::TerminalNode* PauliOperatorParser::ComplexContext::INT(size_t i) {
  return getToken(PauliOperatorParser::INT, i);
}


size_t PauliOperatorParser::ComplexContext::getRuleIndex() const {
  return PauliOperatorParser::RuleComplex;
}

void PauliOperatorParser::ComplexContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComplex(this);
}

void PauliOperatorParser::ComplexContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComplex(this);
}

PauliOperatorParser::ComplexContext* PauliOperatorParser::complex() {
  ComplexContext *_localctx = _tracker.createInstance<ComplexContext>(_ctx, getState());
  enterRule(_localctx, 10, PauliOperatorParser::RuleComplex);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(45);
    match(PauliOperatorParser::T__6);
    setState(48);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PauliOperatorParser::REAL: {
        setState(46);
        real();
        break;
      }

      case PauliOperatorParser::INT: {
        setState(47);
        match(PauliOperatorParser::INT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(50);
    match(PauliOperatorParser::T__7);
    setState(53);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PauliOperatorParser::REAL: {
        setState(51);
        real();
        break;
      }

      case PauliOperatorParser::INT: {
        setState(52);
        match(PauliOperatorParser::INT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(55);
    match(PauliOperatorParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

PauliOperatorParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PauliOperatorParser::RealContext::REAL() {
  return getToken(PauliOperatorParser::REAL, 0);
}


size_t PauliOperatorParser::RealContext::getRuleIndex() const {
  return PauliOperatorParser::RuleReal;
}

void PauliOperatorParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void PauliOperatorParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

PauliOperatorParser::RealContext* PauliOperatorParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 12, PauliOperatorParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(57);
    match(PauliOperatorParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CommentContext ------------------------------------------------------------------

PauliOperatorParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PauliOperatorParser::CommentContext::COMMENT() {
  return getToken(PauliOperatorParser::COMMENT, 0);
}


size_t PauliOperatorParser::CommentContext::getRuleIndex() const {
  return PauliOperatorParser::RuleComment;
}

void PauliOperatorParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void PauliOperatorParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PauliOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

PauliOperatorParser::CommentContext* PauliOperatorParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 14, PauliOperatorParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(59);
    match(PauliOperatorParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> PauliOperatorParser::_decisionToDFA;
atn::PredictionContextCache PauliOperatorParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN PauliOperatorParser::_atn;
std::vector<uint16_t> PauliOperatorParser::_serializedATN;

std::vector<std::string> PauliOperatorParser::_ruleNames = {
  "pauliSrc", "plusorminus", "term", "pauli", "coeff", "complex", "real", 
  "comment"
};

std::vector<std::string> PauliOperatorParser::_literalNames = {
  "", "'+'", "'-'", "'X'", "'Y'", "'Z'", "'I'", "'('", "','", "')'"
};

std::vector<std::string> PauliOperatorParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "COMMENT", "REAL", "INT", "WS", 
  "EOL"
};

dfa::Vocabulary PauliOperatorParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> PauliOperatorParser::_tokenNames;

PauliOperatorParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x10, 0x40, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 
    0x2, 0x7, 0x2, 0x17, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x1a, 0xb, 0x2, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x4, 0x5, 0x4, 0x1f, 0xa, 0x4, 0x3, 0x4, 0x7, 0x4, 
    0x22, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x25, 0xb, 0x4, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x5, 0x5, 0x2a, 0xa, 0x5, 0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 0x2e, 
    0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x33, 0xa, 0x7, 0x3, 
    0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x38, 0xa, 0x7, 0x3, 0x7, 0x3, 0x7, 
    0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x2, 0x2, 0xa, 0x2, 
    0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x2, 0x4, 0x3, 0x2, 0x3, 0x4, 0x3, 
    0x2, 0x5, 0x7, 0x2, 0x3e, 0x2, 0x12, 0x3, 0x2, 0x2, 0x2, 0x4, 0x1b, 
    0x3, 0x2, 0x2, 0x2, 0x6, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x8, 0x29, 0x3, 0x2, 
    0x2, 0x2, 0xa, 0x2d, 0x3, 0x2, 0x2, 0x2, 0xc, 0x2f, 0x3, 0x2, 0x2, 0x2, 
    0xe, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x10, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x12, 
    0x18, 0x5, 0x6, 0x4, 0x2, 0x13, 0x14, 0x5, 0x4, 0x3, 0x2, 0x14, 0x15, 
    0x5, 0x6, 0x4, 0x2, 0x15, 0x17, 0x3, 0x2, 0x2, 0x2, 0x16, 0x13, 0x3, 
    0x2, 0x2, 0x2, 0x17, 0x1a, 0x3, 0x2, 0x2, 0x2, 0x18, 0x16, 0x3, 0x2, 
    0x2, 0x2, 0x18, 0x19, 0x3, 0x2, 0x2, 0x2, 0x19, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x1a, 0x18, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x1c, 0x9, 0x2, 0x2, 0x2, 
    0x1c, 0x5, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x1f, 0x5, 0xa, 0x6, 0x2, 0x1e, 
    0x1d, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x23, 
    0x3, 0x2, 0x2, 0x2, 0x20, 0x22, 0x5, 0x8, 0x5, 0x2, 0x21, 0x20, 0x3, 
    0x2, 0x2, 0x2, 0x22, 0x25, 0x3, 0x2, 0x2, 0x2, 0x23, 0x21, 0x3, 0x2, 
    0x2, 0x2, 0x23, 0x24, 0x3, 0x2, 0x2, 0x2, 0x24, 0x7, 0x3, 0x2, 0x2, 
    0x2, 0x25, 0x23, 0x3, 0x2, 0x2, 0x2, 0x26, 0x27, 0x9, 0x3, 0x2, 0x2, 
    0x27, 0x2a, 0x7, 0xe, 0x2, 0x2, 0x28, 0x2a, 0x7, 0x8, 0x2, 0x2, 0x29, 
    0x26, 0x3, 0x2, 0x2, 0x2, 0x29, 0x28, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x9, 
    0x3, 0x2, 0x2, 0x2, 0x2b, 0x2e, 0x5, 0xc, 0x7, 0x2, 0x2c, 0x2e, 0x5, 
    0xe, 0x8, 0x2, 0x2d, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x2c, 0x3, 0x2, 
    0x2, 0x2, 0x2e, 0xb, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x32, 0x7, 0x9, 0x2, 
    0x2, 0x30, 0x33, 0x5, 0xe, 0x8, 0x2, 0x31, 0x33, 0x7, 0xe, 0x2, 0x2, 
    0x32, 0x30, 0x3, 0x2, 0x2, 0x2, 0x32, 0x31, 0x3, 0x2, 0x2, 0x2, 0x33, 
    0x34, 0x3, 0x2, 0x2, 0x2, 0x34, 0x37, 0x7, 0xa, 0x2, 0x2, 0x35, 0x38, 
    0x5, 0xe, 0x8, 0x2, 0x36, 0x38, 0x7, 0xe, 0x2, 0x2, 0x37, 0x35, 0x3, 
    0x2, 0x2, 0x2, 0x37, 0x36, 0x3, 0x2, 0x2, 0x2, 0x38, 0x39, 0x3, 0x2, 
    0x2, 0x2, 0x39, 0x3a, 0x7, 0xb, 0x2, 0x2, 0x3a, 0xd, 0x3, 0x2, 0x2, 
    0x2, 0x3b, 0x3c, 0x7, 0xd, 0x2, 0x2, 0x3c, 0xf, 0x3, 0x2, 0x2, 0x2, 
    0x3d, 0x3e, 0x7, 0xc, 0x2, 0x2, 0x3e, 0x11, 0x3, 0x2, 0x2, 0x2, 0x9, 
    0x18, 0x1e, 0x23, 0x29, 0x2d, 0x32, 0x37, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

PauliOperatorParser::Initializer PauliOperatorParser::_init;
