
// Generated from FermionOperator.g4 by ANTLR 4.7.2


#include "FermionOperatorListener.h"

#include "FermionOperatorParser.h"


using namespace antlrcpp;
using namespace fermion;
using namespace antlr4;

FermionOperatorParser::FermionOperatorParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

FermionOperatorParser::~FermionOperatorParser() {
  delete _interpreter;
}

std::string FermionOperatorParser::getGrammarFileName() const {
  return "FermionOperator.g4";
}

const std::vector<std::string>& FermionOperatorParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& FermionOperatorParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- FermionSrcContext ------------------------------------------------------------------

FermionOperatorParser::FermionSrcContext::FermionSrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<FermionOperatorParser::TermContext *> FermionOperatorParser::FermionSrcContext::term() {
  return getRuleContexts<FermionOperatorParser::TermContext>();
}

FermionOperatorParser::TermContext* FermionOperatorParser::FermionSrcContext::term(size_t i) {
  return getRuleContext<FermionOperatorParser::TermContext>(i);
}

std::vector<FermionOperatorParser::PlusorminusContext *> FermionOperatorParser::FermionSrcContext::plusorminus() {
  return getRuleContexts<FermionOperatorParser::PlusorminusContext>();
}

FermionOperatorParser::PlusorminusContext* FermionOperatorParser::FermionSrcContext::plusorminus(size_t i) {
  return getRuleContext<FermionOperatorParser::PlusorminusContext>(i);
}


size_t FermionOperatorParser::FermionSrcContext::getRuleIndex() const {
  return FermionOperatorParser::RuleFermionSrc;
}

void FermionOperatorParser::FermionSrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFermionSrc(this);
}

void FermionOperatorParser::FermionSrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFermionSrc(this);
}

FermionOperatorParser::FermionSrcContext* FermionOperatorParser::fermionSrc() {
  FermionSrcContext *_localctx = _tracker.createInstance<FermionSrcContext>(_ctx, getState());
  enterRule(_localctx, 0, FermionOperatorParser::RuleFermionSrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(20);
    term();
    setState(26);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FermionOperatorParser::T__0

    || _la == FermionOperatorParser::T__1) {
      setState(21);
      plusorminus();
      setState(22);
      term();
      setState(28);
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

FermionOperatorParser::PlusorminusContext::PlusorminusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t FermionOperatorParser::PlusorminusContext::getRuleIndex() const {
  return FermionOperatorParser::RulePlusorminus;
}

void FermionOperatorParser::PlusorminusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPlusorminus(this);
}

void FermionOperatorParser::PlusorminusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPlusorminus(this);
}

FermionOperatorParser::PlusorminusContext* FermionOperatorParser::plusorminus() {
  PlusorminusContext *_localctx = _tracker.createInstance<PlusorminusContext>(_ctx, getState());
  enterRule(_localctx, 2, FermionOperatorParser::RulePlusorminus);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(29);
    _la = _input->LA(1);
    if (!(_la == FermionOperatorParser::T__0

    || _la == FermionOperatorParser::T__1)) {
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

FermionOperatorParser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FermionOperatorParser::CoeffContext* FermionOperatorParser::TermContext::coeff() {
  return getRuleContext<FermionOperatorParser::CoeffContext>(0);
}

std::vector<FermionOperatorParser::FermionContext *> FermionOperatorParser::TermContext::fermion() {
  return getRuleContexts<FermionOperatorParser::FermionContext>();
}

FermionOperatorParser::FermionContext* FermionOperatorParser::TermContext::fermion(size_t i) {
  return getRuleContext<FermionOperatorParser::FermionContext>(i);
}


size_t FermionOperatorParser::TermContext::getRuleIndex() const {
  return FermionOperatorParser::RuleTerm;
}

void FermionOperatorParser::TermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTerm(this);
}

void FermionOperatorParser::TermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTerm(this);
}

FermionOperatorParser::TermContext* FermionOperatorParser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 4, FermionOperatorParser::RuleTerm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(32);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FermionOperatorParser::T__3

    || _la == FermionOperatorParser::REAL) {
      setState(31);
      coeff();
    }
    setState(37);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FermionOperatorParser::INT) {
      setState(34);
      fermion();
      setState(39);
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

//----------------- FermionContext ------------------------------------------------------------------

FermionOperatorParser::FermionContext::FermionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FermionOperatorParser::OpContext* FermionOperatorParser::FermionContext::op() {
  return getRuleContext<FermionOperatorParser::OpContext>(0);
}


size_t FermionOperatorParser::FermionContext::getRuleIndex() const {
  return FermionOperatorParser::RuleFermion;
}

void FermionOperatorParser::FermionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFermion(this);
}

void FermionOperatorParser::FermionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFermion(this);
}

FermionOperatorParser::FermionContext* FermionOperatorParser::fermion() {
  FermionContext *_localctx = _tracker.createInstance<FermionContext>(_ctx, getState());
  enterRule(_localctx, 6, FermionOperatorParser::RuleFermion);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(40);
    op();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OpContext ------------------------------------------------------------------

FermionOperatorParser::OpContext::OpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FermionOperatorParser::OpContext::INT() {
  return getToken(FermionOperatorParser::INT, 0);
}

FermionOperatorParser::CaratContext* FermionOperatorParser::OpContext::carat() {
  return getRuleContext<FermionOperatorParser::CaratContext>(0);
}


size_t FermionOperatorParser::OpContext::getRuleIndex() const {
  return FermionOperatorParser::RuleOp;
}

void FermionOperatorParser::OpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOp(this);
}

void FermionOperatorParser::OpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOp(this);
}

FermionOperatorParser::OpContext* FermionOperatorParser::op() {
  OpContext *_localctx = _tracker.createInstance<OpContext>(_ctx, getState());
  enterRule(_localctx, 8, FermionOperatorParser::RuleOp);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(42);
    match(FermionOperatorParser::INT);
    setState(44);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FermionOperatorParser::T__2) {
      setState(43);
      carat();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CaratContext ------------------------------------------------------------------

FermionOperatorParser::CaratContext::CaratContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t FermionOperatorParser::CaratContext::getRuleIndex() const {
  return FermionOperatorParser::RuleCarat;
}

void FermionOperatorParser::CaratContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCarat(this);
}

void FermionOperatorParser::CaratContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCarat(this);
}

FermionOperatorParser::CaratContext* FermionOperatorParser::carat() {
  CaratContext *_localctx = _tracker.createInstance<CaratContext>(_ctx, getState());
  enterRule(_localctx, 10, FermionOperatorParser::RuleCarat);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(46);
    match(FermionOperatorParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CoeffContext ------------------------------------------------------------------

FermionOperatorParser::CoeffContext::CoeffContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

FermionOperatorParser::ComplexContext* FermionOperatorParser::CoeffContext::complex() {
  return getRuleContext<FermionOperatorParser::ComplexContext>(0);
}

FermionOperatorParser::RealContext* FermionOperatorParser::CoeffContext::real() {
  return getRuleContext<FermionOperatorParser::RealContext>(0);
}


size_t FermionOperatorParser::CoeffContext::getRuleIndex() const {
  return FermionOperatorParser::RuleCoeff;
}

void FermionOperatorParser::CoeffContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCoeff(this);
}

void FermionOperatorParser::CoeffContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCoeff(this);
}

FermionOperatorParser::CoeffContext* FermionOperatorParser::coeff() {
  CoeffContext *_localctx = _tracker.createInstance<CoeffContext>(_ctx, getState());
  enterRule(_localctx, 12, FermionOperatorParser::RuleCoeff);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(50);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FermionOperatorParser::T__3: {
        enterOuterAlt(_localctx, 1);
        setState(48);
        complex();
        break;
      }

      case FermionOperatorParser::REAL: {
        enterOuterAlt(_localctx, 2);
        setState(49);
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

FermionOperatorParser::ComplexContext::ComplexContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<FermionOperatorParser::RealContext *> FermionOperatorParser::ComplexContext::real() {
  return getRuleContexts<FermionOperatorParser::RealContext>();
}

FermionOperatorParser::RealContext* FermionOperatorParser::ComplexContext::real(size_t i) {
  return getRuleContext<FermionOperatorParser::RealContext>(i);
}

std::vector<tree::TerminalNode *> FermionOperatorParser::ComplexContext::INT() {
  return getTokens(FermionOperatorParser::INT);
}

tree::TerminalNode* FermionOperatorParser::ComplexContext::INT(size_t i) {
  return getToken(FermionOperatorParser::INT, i);
}


size_t FermionOperatorParser::ComplexContext::getRuleIndex() const {
  return FermionOperatorParser::RuleComplex;
}

void FermionOperatorParser::ComplexContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComplex(this);
}

void FermionOperatorParser::ComplexContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComplex(this);
}

FermionOperatorParser::ComplexContext* FermionOperatorParser::complex() {
  ComplexContext *_localctx = _tracker.createInstance<ComplexContext>(_ctx, getState());
  enterRule(_localctx, 14, FermionOperatorParser::RuleComplex);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(52);
    match(FermionOperatorParser::T__3);
    setState(55);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FermionOperatorParser::REAL: {
        setState(53);
        real();
        break;
      }

      case FermionOperatorParser::INT: {
        setState(54);
        match(FermionOperatorParser::INT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(57);
    match(FermionOperatorParser::T__4);
    setState(60);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FermionOperatorParser::REAL: {
        setState(58);
        real();
        break;
      }

      case FermionOperatorParser::INT: {
        setState(59);
        match(FermionOperatorParser::INT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(62);
    match(FermionOperatorParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

FermionOperatorParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FermionOperatorParser::RealContext::REAL() {
  return getToken(FermionOperatorParser::REAL, 0);
}


size_t FermionOperatorParser::RealContext::getRuleIndex() const {
  return FermionOperatorParser::RuleReal;
}

void FermionOperatorParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void FermionOperatorParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

FermionOperatorParser::RealContext* FermionOperatorParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 16, FermionOperatorParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(64);
    match(FermionOperatorParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CommentContext ------------------------------------------------------------------

FermionOperatorParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* FermionOperatorParser::CommentContext::COMMENT() {
  return getToken(FermionOperatorParser::COMMENT, 0);
}


size_t FermionOperatorParser::CommentContext::getRuleIndex() const {
  return FermionOperatorParser::RuleComment;
}

void FermionOperatorParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void FermionOperatorParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

FermionOperatorParser::CommentContext* FermionOperatorParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 18, FermionOperatorParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(66);
    match(FermionOperatorParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> FermionOperatorParser::_decisionToDFA;
atn::PredictionContextCache FermionOperatorParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN FermionOperatorParser::_atn;
std::vector<uint16_t> FermionOperatorParser::_serializedATN;

std::vector<std::string> FermionOperatorParser::_ruleNames = {
  "fermionSrc", "plusorminus", "term", "fermion", "op", "carat", "coeff", 
  "complex", "real", "comment"
};

std::vector<std::string> FermionOperatorParser::_literalNames = {
  "", "'+'", "'-'", "'^'", "'('", "','", "')'"
};

std::vector<std::string> FermionOperatorParser::_symbolicNames = {
  "", "", "", "", "", "", "", "COMMENT", "REAL", "INT", "WS", "EOL"
};

dfa::Vocabulary FermionOperatorParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> FermionOperatorParser::_tokenNames;

FermionOperatorParser::Initializer::Initializer() {
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
    0x3, 0xd, 0x47, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x7, 0x2, 0x1b, 0xa, 0x2, 
    0xc, 0x2, 0xe, 0x2, 0x1e, 0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x5, 
    0x4, 0x23, 0xa, 0x4, 0x3, 0x4, 0x7, 0x4, 0x26, 0xa, 0x4, 0xc, 0x4, 0xe, 
    0x4, 0x29, 0xb, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 
    0x2f, 0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x35, 
    0xa, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 0x3a, 0xa, 0x9, 0x3, 
    0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 0x3f, 0xa, 0x9, 0x3, 0x9, 0x3, 0x9, 
    0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x2, 0x2, 0xc, 0x2, 
    0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x2, 0x3, 0x3, 0x2, 
    0x3, 0x4, 0x2, 0x43, 0x2, 0x16, 0x3, 0x2, 0x2, 0x2, 0x4, 0x1f, 0x3, 
    0x2, 0x2, 0x2, 0x6, 0x22, 0x3, 0x2, 0x2, 0x2, 0x8, 0x2a, 0x3, 0x2, 0x2, 
    0x2, 0xa, 0x2c, 0x3, 0x2, 0x2, 0x2, 0xc, 0x30, 0x3, 0x2, 0x2, 0x2, 0xe, 
    0x34, 0x3, 0x2, 0x2, 0x2, 0x10, 0x36, 0x3, 0x2, 0x2, 0x2, 0x12, 0x42, 
    0x3, 0x2, 0x2, 0x2, 0x14, 0x44, 0x3, 0x2, 0x2, 0x2, 0x16, 0x1c, 0x5, 
    0x6, 0x4, 0x2, 0x17, 0x18, 0x5, 0x4, 0x3, 0x2, 0x18, 0x19, 0x5, 0x6, 
    0x4, 0x2, 0x19, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x17, 0x3, 0x2, 0x2, 
    0x2, 0x1b, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x1a, 0x3, 0x2, 0x2, 0x2, 
    0x1c, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x3, 0x3, 0x2, 0x2, 0x2, 0x1e, 
    0x1c, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x20, 0x9, 0x2, 0x2, 0x2, 0x20, 0x5, 
    0x3, 0x2, 0x2, 0x2, 0x21, 0x23, 0x5, 0xe, 0x8, 0x2, 0x22, 0x21, 0x3, 
    0x2, 0x2, 0x2, 0x22, 0x23, 0x3, 0x2, 0x2, 0x2, 0x23, 0x27, 0x3, 0x2, 
    0x2, 0x2, 0x24, 0x26, 0x5, 0x8, 0x5, 0x2, 0x25, 0x24, 0x3, 0x2, 0x2, 
    0x2, 0x26, 0x29, 0x3, 0x2, 0x2, 0x2, 0x27, 0x25, 0x3, 0x2, 0x2, 0x2, 
    0x27, 0x28, 0x3, 0x2, 0x2, 0x2, 0x28, 0x7, 0x3, 0x2, 0x2, 0x2, 0x29, 
    0x27, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x2b, 0x5, 0xa, 0x6, 0x2, 0x2b, 0x9, 
    0x3, 0x2, 0x2, 0x2, 0x2c, 0x2e, 0x7, 0xb, 0x2, 0x2, 0x2d, 0x2f, 0x5, 
    0xc, 0x7, 0x2, 0x2e, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x2f, 0x3, 0x2, 
    0x2, 0x2, 0x2f, 0xb, 0x3, 0x2, 0x2, 0x2, 0x30, 0x31, 0x7, 0x5, 0x2, 
    0x2, 0x31, 0xd, 0x3, 0x2, 0x2, 0x2, 0x32, 0x35, 0x5, 0x10, 0x9, 0x2, 
    0x33, 0x35, 0x5, 0x12, 0xa, 0x2, 0x34, 0x32, 0x3, 0x2, 0x2, 0x2, 0x34, 
    0x33, 0x3, 0x2, 0x2, 0x2, 0x35, 0xf, 0x3, 0x2, 0x2, 0x2, 0x36, 0x39, 
    0x7, 0x6, 0x2, 0x2, 0x37, 0x3a, 0x5, 0x12, 0xa, 0x2, 0x38, 0x3a, 0x7, 
    0xb, 0x2, 0x2, 0x39, 0x37, 0x3, 0x2, 0x2, 0x2, 0x39, 0x38, 0x3, 0x2, 
    0x2, 0x2, 0x3a, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x3e, 0x7, 0x7, 0x2, 
    0x2, 0x3c, 0x3f, 0x5, 0x12, 0xa, 0x2, 0x3d, 0x3f, 0x7, 0xb, 0x2, 0x2, 
    0x3e, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3f, 
    0x40, 0x3, 0x2, 0x2, 0x2, 0x40, 0x41, 0x7, 0x8, 0x2, 0x2, 0x41, 0x11, 
    0x3, 0x2, 0x2, 0x2, 0x42, 0x43, 0x7, 0xa, 0x2, 0x2, 0x43, 0x13, 0x3, 
    0x2, 0x2, 0x2, 0x44, 0x45, 0x7, 0x9, 0x2, 0x2, 0x45, 0x15, 0x3, 0x2, 
    0x2, 0x2, 0x9, 0x1c, 0x22, 0x27, 0x2e, 0x34, 0x39, 0x3e, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

FermionOperatorParser::Initializer FermionOperatorParser::_init;
