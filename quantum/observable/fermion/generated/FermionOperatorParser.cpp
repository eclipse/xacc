
// Generated from FermionOperator.g4 by ANTLR 4.8


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
    setState(22);
    term();
    setState(28);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FermionOperatorParser::T__0

    || _la == FermionOperatorParser::T__1) {
      setState(23);
      plusorminus();
      setState(24);
      term();
      setState(30);
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
    setState(31);
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
    setState(34);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      setState(33);
      coeff();
      break;
    }

    }
    setState(39);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == FermionOperatorParser::INT) {
      setState(36);
      fermion();
      setState(41);
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
    setState(42);
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
    setState(44);
    match(FermionOperatorParser::INT);
    setState(46);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FermionOperatorParser::T__2) {
      setState(45);
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
    setState(48);
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
    setState(52);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case FermionOperatorParser::T__3: {
        enterOuterAlt(_localctx, 1);
        setState(50);
        complex();
        break;
      }

      case FermionOperatorParser::T__6:
      case FermionOperatorParser::T__7:
      case FermionOperatorParser::REAL:
      case FermionOperatorParser::INT: {
        enterOuterAlt(_localctx, 2);
        setState(51);
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

std::vector<FermionOperatorParser::ScientificContext *> FermionOperatorParser::ComplexContext::scientific() {
  return getRuleContexts<FermionOperatorParser::ScientificContext>();
}

FermionOperatorParser::ScientificContext* FermionOperatorParser::ComplexContext::scientific(size_t i) {
  return getRuleContext<FermionOperatorParser::ScientificContext>(i);
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
    setState(54);
    match(FermionOperatorParser::T__3);
    setState(58);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
    case 1: {
      setState(55);
      real();
      break;
    }

    case 2: {
      setState(56);
      match(FermionOperatorParser::INT);
      break;
    }

    case 3: {
      setState(57);
      scientific();
      break;
    }

    }
    setState(60);
    match(FermionOperatorParser::T__4);
    setState(64);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      setState(61);
      real();
      break;
    }

    case 2: {
      setState(62);
      match(FermionOperatorParser::INT);
      break;
    }

    case 3: {
      setState(63);
      scientific();
      break;
    }

    }
    setState(66);
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

FermionOperatorParser::ScientificContext* FermionOperatorParser::RealContext::scientific() {
  return getRuleContext<FermionOperatorParser::ScientificContext>(0);
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
    setState(70);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(68);
      match(FermionOperatorParser::REAL);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(69);
      scientific();
      break;
    }

    }
   
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
    setState(72);
    match(FermionOperatorParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ScientificContext ------------------------------------------------------------------

FermionOperatorParser::ScientificContext::ScientificContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> FermionOperatorParser::ScientificContext::INT() {
  return getTokens(FermionOperatorParser::INT);
}

tree::TerminalNode* FermionOperatorParser::ScientificContext::INT(size_t i) {
  return getToken(FermionOperatorParser::INT, i);
}

tree::TerminalNode* FermionOperatorParser::ScientificContext::REAL() {
  return getToken(FermionOperatorParser::REAL, 0);
}


size_t FermionOperatorParser::ScientificContext::getRuleIndex() const {
  return FermionOperatorParser::RuleScientific;
}

void FermionOperatorParser::ScientificContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterScientific(this);
}

void FermionOperatorParser::ScientificContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<FermionOperatorListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitScientific(this);
}

FermionOperatorParser::ScientificContext* FermionOperatorParser::scientific() {
  ScientificContext *_localctx = _tracker.createInstance<ScientificContext>(_ctx, getState());
  enterRule(_localctx, 20, FermionOperatorParser::RuleScientific);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(74);
      _la = _input->LA(1);
      if (!(_la == FermionOperatorParser::REAL

      || _la == FermionOperatorParser::INT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      break;
    }

    }
    setState(78);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == FermionOperatorParser::T__6

    || _la == FermionOperatorParser::T__7) {
      setState(77);
      _la = _input->LA(1);
      if (!(_la == FermionOperatorParser::T__6

      || _la == FermionOperatorParser::T__7)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(80);
    match(FermionOperatorParser::INT);
   
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
  "complex", "real", "comment", "scientific"
};

std::vector<std::string> FermionOperatorParser::_literalNames = {
  "", "'+'", "'-'", "'^'", "'('", "','", "')'", "'e'", "'E'"
};

std::vector<std::string> FermionOperatorParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "COMMENT", "REAL", "INT", "WS", "EOL"
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
    0x3, 0xf, 0x55, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x7, 
    0x2, 0x1d, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x20, 0xb, 0x2, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x4, 0x5, 0x4, 0x25, 0xa, 0x4, 0x3, 0x4, 0x7, 0x4, 0x28, 0xa, 
    0x4, 0xc, 0x4, 0xe, 0x4, 0x2b, 0xb, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 
    0x3, 0x6, 0x5, 0x6, 0x31, 0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 0x3, 
    0x8, 0x5, 0x8, 0x37, 0xa, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 
    0x5, 0x9, 0x3d, 0xa, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 
    0x9, 0x43, 0xa, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 
    0x49, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 0x5, 0xc, 0x4e, 0xa, 0xc, 
    0x3, 0xc, 0x5, 0xc, 0x51, 0xa, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x2, 
    0x2, 0xd, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 
    0x2, 0x5, 0x3, 0x2, 0x3, 0x4, 0x3, 0x2, 0xc, 0xd, 0x3, 0x2, 0x9, 0xa, 
    0x2, 0x55, 0x2, 0x18, 0x3, 0x2, 0x2, 0x2, 0x4, 0x21, 0x3, 0x2, 0x2, 
    0x2, 0x6, 0x24, 0x3, 0x2, 0x2, 0x2, 0x8, 0x2c, 0x3, 0x2, 0x2, 0x2, 0xa, 
    0x2e, 0x3, 0x2, 0x2, 0x2, 0xc, 0x32, 0x3, 0x2, 0x2, 0x2, 0xe, 0x36, 
    0x3, 0x2, 0x2, 0x2, 0x10, 0x38, 0x3, 0x2, 0x2, 0x2, 0x12, 0x48, 0x3, 
    0x2, 0x2, 0x2, 0x14, 0x4a, 0x3, 0x2, 0x2, 0x2, 0x16, 0x4d, 0x3, 0x2, 
    0x2, 0x2, 0x18, 0x1e, 0x5, 0x6, 0x4, 0x2, 0x19, 0x1a, 0x5, 0x4, 0x3, 
    0x2, 0x1a, 0x1b, 0x5, 0x6, 0x4, 0x2, 0x1b, 0x1d, 0x3, 0x2, 0x2, 0x2, 
    0x1c, 0x19, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x20, 0x3, 0x2, 0x2, 0x2, 0x1e, 
    0x1c, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x3, 
    0x3, 0x2, 0x2, 0x2, 0x20, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x21, 0x22, 0x9, 
    0x2, 0x2, 0x2, 0x22, 0x5, 0x3, 0x2, 0x2, 0x2, 0x23, 0x25, 0x5, 0xe, 
    0x8, 0x2, 0x24, 0x23, 0x3, 0x2, 0x2, 0x2, 0x24, 0x25, 0x3, 0x2, 0x2, 
    0x2, 0x25, 0x29, 0x3, 0x2, 0x2, 0x2, 0x26, 0x28, 0x5, 0x8, 0x5, 0x2, 
    0x27, 0x26, 0x3, 0x2, 0x2, 0x2, 0x28, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x29, 
    0x27, 0x3, 0x2, 0x2, 0x2, 0x29, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x2b, 0x29, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x2d, 0x5, 
    0xa, 0x6, 0x2, 0x2d, 0x9, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x30, 0x7, 0xd, 
    0x2, 0x2, 0x2f, 0x31, 0x5, 0xc, 0x7, 0x2, 0x30, 0x2f, 0x3, 0x2, 0x2, 
    0x2, 0x30, 0x31, 0x3, 0x2, 0x2, 0x2, 0x31, 0xb, 0x3, 0x2, 0x2, 0x2, 
    0x32, 0x33, 0x7, 0x5, 0x2, 0x2, 0x33, 0xd, 0x3, 0x2, 0x2, 0x2, 0x34, 
    0x37, 0x5, 0x10, 0x9, 0x2, 0x35, 0x37, 0x5, 0x12, 0xa, 0x2, 0x36, 0x34, 
    0x3, 0x2, 0x2, 0x2, 0x36, 0x35, 0x3, 0x2, 0x2, 0x2, 0x37, 0xf, 0x3, 
    0x2, 0x2, 0x2, 0x38, 0x3c, 0x7, 0x6, 0x2, 0x2, 0x39, 0x3d, 0x5, 0x12, 
    0xa, 0x2, 0x3a, 0x3d, 0x7, 0xd, 0x2, 0x2, 0x3b, 0x3d, 0x5, 0x16, 0xc, 
    0x2, 0x3c, 0x39, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3a, 0x3, 0x2, 0x2, 0x2, 
    0x3c, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x3d, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x3e, 
    0x42, 0x7, 0x7, 0x2, 0x2, 0x3f, 0x43, 0x5, 0x12, 0xa, 0x2, 0x40, 0x43, 
    0x7, 0xd, 0x2, 0x2, 0x41, 0x43, 0x5, 0x16, 0xc, 0x2, 0x42, 0x3f, 0x3, 
    0x2, 0x2, 0x2, 0x42, 0x40, 0x3, 0x2, 0x2, 0x2, 0x42, 0x41, 0x3, 0x2, 
    0x2, 0x2, 0x43, 0x44, 0x3, 0x2, 0x2, 0x2, 0x44, 0x45, 0x7, 0x8, 0x2, 
    0x2, 0x45, 0x11, 0x3, 0x2, 0x2, 0x2, 0x46, 0x49, 0x7, 0xc, 0x2, 0x2, 
    0x47, 0x49, 0x5, 0x16, 0xc, 0x2, 0x48, 0x46, 0x3, 0x2, 0x2, 0x2, 0x48, 
    0x47, 0x3, 0x2, 0x2, 0x2, 0x49, 0x13, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x4b, 
    0x7, 0xb, 0x2, 0x2, 0x4b, 0x15, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4e, 0x9, 
    0x3, 0x2, 0x2, 0x4d, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4e, 0x3, 0x2, 
    0x2, 0x2, 0x4e, 0x50, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x51, 0x9, 0x4, 0x2, 
    0x2, 0x50, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x3, 0x2, 0x2, 0x2, 
    0x51, 0x52, 0x3, 0x2, 0x2, 0x2, 0x52, 0x53, 0x7, 0xd, 0x2, 0x2, 0x53, 
    0x17, 0x3, 0x2, 0x2, 0x2, 0xc, 0x1e, 0x24, 0x29, 0x30, 0x36, 0x3c, 0x42, 
    0x48, 0x4d, 0x50, 
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
