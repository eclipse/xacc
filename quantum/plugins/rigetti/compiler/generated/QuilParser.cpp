
// Generated from Quil.g4 by ANTLR 4.9.1


#include "QuilListener.h"

#include "QuilParser.h"


using namespace antlrcpp;
using namespace quil;
using namespace antlr4;

QuilParser::QuilParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

QuilParser::~QuilParser() {
  delete _interpreter;
}

std::string QuilParser::getGrammarFileName() const {
  return "Quil.g4";
}

const std::vector<std::string>& QuilParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& QuilParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- QuilContext ------------------------------------------------------------------

QuilParser::QuilContext::QuilContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::QuilContext::EOF() {
  return getToken(QuilParser::EOF, 0);
}

std::vector<QuilParser::AllInstrContext *> QuilParser::QuilContext::allInstr() {
  return getRuleContexts<QuilParser::AllInstrContext>();
}

QuilParser::AllInstrContext* QuilParser::QuilContext::allInstr(size_t i) {
  return getRuleContext<QuilParser::AllInstrContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::QuilContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode* QuilParser::QuilContext::NEWLINE(size_t i) {
  return getToken(QuilParser::NEWLINE, i);
}


size_t QuilParser::QuilContext::getRuleIndex() const {
  return QuilParser::RuleQuil;
}

void QuilParser::QuilContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQuil(this);
}

void QuilParser::QuilContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQuil(this);
}

QuilParser::QuilContext* QuilParser::quil() {
  QuilContext *_localctx = _tracker.createInstance<QuilContext>(_ctx, getState());
  enterRule(_localctx, 0, QuilParser::RuleQuil);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(113);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::DEFGATE)
      | (1ULL << QuilParser::DEFCIRCUIT)
      | (1ULL << QuilParser::MEASURE)
      | (1ULL << QuilParser::LABEL)
      | (1ULL << QuilParser::HALT)
      | (1ULL << QuilParser::JUMP)
      | (1ULL << QuilParser::JUMPWHEN)
      | (1ULL << QuilParser::JUMPUNLESS)
      | (1ULL << QuilParser::RESET)
      | (1ULL << QuilParser::WAIT)
      | (1ULL << QuilParser::NOP)
      | (1ULL << QuilParser::INCLUDE)
      | (1ULL << QuilParser::PRAGMA)
      | (1ULL << QuilParser::DECLARE)
      | (1ULL << QuilParser::NEG)
      | (1ULL << QuilParser::NOT)
      | (1ULL << QuilParser::TRUE)
      | (1ULL << QuilParser::FALSE)
      | (1ULL << QuilParser::AND)
      | (1ULL << QuilParser::IOR)
      | (1ULL << QuilParser::XOR)
      | (1ULL << QuilParser::OR)
      | (1ULL << QuilParser::ADD)
      | (1ULL << QuilParser::SUB)
      | (1ULL << QuilParser::MUL)
      | (1ULL << QuilParser::DIV)
      | (1ULL << QuilParser::MOVE)
      | (1ULL << QuilParser::EXCHANGE)
      | (1ULL << QuilParser::CONVERT)
      | (1ULL << QuilParser::EQ)
      | (1ULL << QuilParser::GT)
      | (1ULL << QuilParser::GE)
      | (1ULL << QuilParser::LT)
      | (1ULL << QuilParser::LE)
      | (1ULL << QuilParser::LOAD)
      | (1ULL << QuilParser::STORE)
      | (1ULL << QuilParser::CONTROLLED)
      | (1ULL << QuilParser::DAGGER)
      | (1ULL << QuilParser::FORKED)
      | (1ULL << QuilParser::IDENTIFIER))) != 0)) {
      setState(112);
      allInstr();
    }
    setState(123);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(116); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(115);
          match(QuilParser::NEWLINE);
          setState(118); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == QuilParser::NEWLINE);
        setState(120);
        allInstr(); 
      }
      setState(125);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
    }
    setState(129);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::NEWLINE) {
      setState(126);
      match(QuilParser::NEWLINE);
      setState(131);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(132);
    match(QuilParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AllInstrContext ------------------------------------------------------------------

QuilParser::AllInstrContext::AllInstrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::DefGateContext* QuilParser::AllInstrContext::defGate() {
  return getRuleContext<QuilParser::DefGateContext>(0);
}

QuilParser::DefGateAsPauliContext* QuilParser::AllInstrContext::defGateAsPauli() {
  return getRuleContext<QuilParser::DefGateAsPauliContext>(0);
}

QuilParser::DefCircuitContext* QuilParser::AllInstrContext::defCircuit() {
  return getRuleContext<QuilParser::DefCircuitContext>(0);
}

QuilParser::InstrContext* QuilParser::AllInstrContext::instr() {
  return getRuleContext<QuilParser::InstrContext>(0);
}


size_t QuilParser::AllInstrContext::getRuleIndex() const {
  return QuilParser::RuleAllInstr;
}

void QuilParser::AllInstrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAllInstr(this);
}

void QuilParser::AllInstrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAllInstr(this);
}

QuilParser::AllInstrContext* QuilParser::allInstr() {
  AllInstrContext *_localctx = _tracker.createInstance<AllInstrContext>(_ctx, getState());
  enterRule(_localctx, 2, QuilParser::RuleAllInstr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(138);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(134);
      defGate();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(135);
      defGateAsPauli();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(136);
      defCircuit();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(137);
      instr();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstrContext ------------------------------------------------------------------

QuilParser::InstrContext::InstrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::GateContext* QuilParser::InstrContext::gate() {
  return getRuleContext<QuilParser::GateContext>(0);
}

QuilParser::MeasureContext* QuilParser::InstrContext::measure() {
  return getRuleContext<QuilParser::MeasureContext>(0);
}

QuilParser::DefLabelContext* QuilParser::InstrContext::defLabel() {
  return getRuleContext<QuilParser::DefLabelContext>(0);
}

QuilParser::HaltContext* QuilParser::InstrContext::halt() {
  return getRuleContext<QuilParser::HaltContext>(0);
}

QuilParser::JumpContext* QuilParser::InstrContext::jump() {
  return getRuleContext<QuilParser::JumpContext>(0);
}

QuilParser::JumpWhenContext* QuilParser::InstrContext::jumpWhen() {
  return getRuleContext<QuilParser::JumpWhenContext>(0);
}

QuilParser::JumpUnlessContext* QuilParser::InstrContext::jumpUnless() {
  return getRuleContext<QuilParser::JumpUnlessContext>(0);
}

QuilParser::ResetStateContext* QuilParser::InstrContext::resetState() {
  return getRuleContext<QuilParser::ResetStateContext>(0);
}

QuilParser::WaitContext* QuilParser::InstrContext::wait() {
  return getRuleContext<QuilParser::WaitContext>(0);
}

QuilParser::ClassicalUnaryContext* QuilParser::InstrContext::classicalUnary() {
  return getRuleContext<QuilParser::ClassicalUnaryContext>(0);
}

QuilParser::ClassicalBinaryContext* QuilParser::InstrContext::classicalBinary() {
  return getRuleContext<QuilParser::ClassicalBinaryContext>(0);
}

QuilParser::ClassicalComparisonContext* QuilParser::InstrContext::classicalComparison() {
  return getRuleContext<QuilParser::ClassicalComparisonContext>(0);
}

QuilParser::LoadContext* QuilParser::InstrContext::load() {
  return getRuleContext<QuilParser::LoadContext>(0);
}

QuilParser::StoreContext* QuilParser::InstrContext::store() {
  return getRuleContext<QuilParser::StoreContext>(0);
}

QuilParser::NopContext* QuilParser::InstrContext::nop() {
  return getRuleContext<QuilParser::NopContext>(0);
}

QuilParser::IncludeContext* QuilParser::InstrContext::include() {
  return getRuleContext<QuilParser::IncludeContext>(0);
}

QuilParser::PragmaContext* QuilParser::InstrContext::pragma() {
  return getRuleContext<QuilParser::PragmaContext>(0);
}

QuilParser::MemoryDescriptorContext* QuilParser::InstrContext::memoryDescriptor() {
  return getRuleContext<QuilParser::MemoryDescriptorContext>(0);
}


size_t QuilParser::InstrContext::getRuleIndex() const {
  return QuilParser::RuleInstr;
}

void QuilParser::InstrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInstr(this);
}

void QuilParser::InstrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInstr(this);
}

QuilParser::InstrContext* QuilParser::instr() {
  InstrContext *_localctx = _tracker.createInstance<InstrContext>(_ctx, getState());
  enterRule(_localctx, 4, QuilParser::RuleInstr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(158);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::CONTROLLED:
      case QuilParser::DAGGER:
      case QuilParser::FORKED:
      case QuilParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 1);
        setState(140);
        gate();
        break;
      }

      case QuilParser::MEASURE: {
        enterOuterAlt(_localctx, 2);
        setState(141);
        measure();
        break;
      }

      case QuilParser::LABEL: {
        enterOuterAlt(_localctx, 3);
        setState(142);
        defLabel();
        break;
      }

      case QuilParser::HALT: {
        enterOuterAlt(_localctx, 4);
        setState(143);
        halt();
        break;
      }

      case QuilParser::JUMP: {
        enterOuterAlt(_localctx, 5);
        setState(144);
        jump();
        break;
      }

      case QuilParser::JUMPWHEN: {
        enterOuterAlt(_localctx, 6);
        setState(145);
        jumpWhen();
        break;
      }

      case QuilParser::JUMPUNLESS: {
        enterOuterAlt(_localctx, 7);
        setState(146);
        jumpUnless();
        break;
      }

      case QuilParser::RESET: {
        enterOuterAlt(_localctx, 8);
        setState(147);
        resetState();
        break;
      }

      case QuilParser::WAIT: {
        enterOuterAlt(_localctx, 9);
        setState(148);
        wait();
        break;
      }

      case QuilParser::NEG:
      case QuilParser::NOT:
      case QuilParser::TRUE:
      case QuilParser::FALSE: {
        enterOuterAlt(_localctx, 10);
        setState(149);
        classicalUnary();
        break;
      }

      case QuilParser::AND:
      case QuilParser::IOR:
      case QuilParser::XOR:
      case QuilParser::OR:
      case QuilParser::ADD:
      case QuilParser::SUB:
      case QuilParser::MUL:
      case QuilParser::DIV:
      case QuilParser::MOVE:
      case QuilParser::EXCHANGE:
      case QuilParser::CONVERT: {
        enterOuterAlt(_localctx, 11);
        setState(150);
        classicalBinary();
        break;
      }

      case QuilParser::EQ:
      case QuilParser::GT:
      case QuilParser::GE:
      case QuilParser::LT:
      case QuilParser::LE: {
        enterOuterAlt(_localctx, 12);
        setState(151);
        classicalComparison();
        break;
      }

      case QuilParser::LOAD: {
        enterOuterAlt(_localctx, 13);
        setState(152);
        load();
        break;
      }

      case QuilParser::STORE: {
        enterOuterAlt(_localctx, 14);
        setState(153);
        store();
        break;
      }

      case QuilParser::NOP: {
        enterOuterAlt(_localctx, 15);
        setState(154);
        nop();
        break;
      }

      case QuilParser::INCLUDE: {
        enterOuterAlt(_localctx, 16);
        setState(155);
        include();
        break;
      }

      case QuilParser::PRAGMA: {
        enterOuterAlt(_localctx, 17);
        setState(156);
        pragma();
        break;
      }

      case QuilParser::DECLARE: {
        enterOuterAlt(_localctx, 18);
        setState(157);
        memoryDescriptor();
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

//----------------- GateContext ------------------------------------------------------------------

QuilParser::GateContext::GateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::NameContext* QuilParser::GateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

std::vector<QuilParser::ModifierContext *> QuilParser::GateContext::modifier() {
  return getRuleContexts<QuilParser::ModifierContext>();
}

QuilParser::ModifierContext* QuilParser::GateContext::modifier(size_t i) {
  return getRuleContext<QuilParser::ModifierContext>(i);
}

tree::TerminalNode* QuilParser::GateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::ParamContext *> QuilParser::GateContext::param() {
  return getRuleContexts<QuilParser::ParamContext>();
}

QuilParser::ParamContext* QuilParser::GateContext::param(size_t i) {
  return getRuleContext<QuilParser::ParamContext>(i);
}

tree::TerminalNode* QuilParser::GateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitContext *> QuilParser::GateContext::qubit() {
  return getRuleContexts<QuilParser::QubitContext>();
}

QuilParser::QubitContext* QuilParser::GateContext::qubit(size_t i) {
  return getRuleContext<QuilParser::QubitContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::GateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::GateContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::GateContext::getRuleIndex() const {
  return QuilParser::RuleGate;
}

void QuilParser::GateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGate(this);
}

void QuilParser::GateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGate(this);
}

QuilParser::GateContext* QuilParser::gate() {
  GateContext *_localctx = _tracker.createInstance<GateContext>(_ctx, getState());
  enterRule(_localctx, 6, QuilParser::RuleGate);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(163);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::CONTROLLED)
      | (1ULL << QuilParser::DAGGER)
      | (1ULL << QuilParser::FORKED))) != 0)) {
      setState(160);
      modifier();
      setState(165);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(166);
    name();
    setState(178);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(167);
      match(QuilParser::LPAREN);
      setState(168);
      param();
      setState(173);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(169);
        match(QuilParser::COMMA);
        setState(170);
        param();
        setState(175);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(176);
      match(QuilParser::RPAREN);
    }
    setState(181); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(180);
      qubit();
      setState(183); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NameContext ------------------------------------------------------------------

QuilParser::NameContext::NameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::NameContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::NameContext::getRuleIndex() const {
  return QuilParser::RuleName;
}

void QuilParser::NameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterName(this);
}

void QuilParser::NameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitName(this);
}

QuilParser::NameContext* QuilParser::name() {
  NameContext *_localctx = _tracker.createInstance<NameContext>(_ctx, getState());
  enterRule(_localctx, 8, QuilParser::RuleName);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(185);
    match(QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QubitContext ------------------------------------------------------------------

QuilParser::QubitContext::QubitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::QubitContext::INT() {
  return getToken(QuilParser::INT, 0);
}


size_t QuilParser::QubitContext::getRuleIndex() const {
  return QuilParser::RuleQubit;
}

void QuilParser::QubitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQubit(this);
}

void QuilParser::QubitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQubit(this);
}

QuilParser::QubitContext* QuilParser::qubit() {
  QubitContext *_localctx = _tracker.createInstance<QubitContext>(_ctx, getState());
  enterRule(_localctx, 10, QuilParser::RuleQubit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(187);
    match(QuilParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext ------------------------------------------------------------------

QuilParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::ExpressionContext* QuilParser::ParamContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}


size_t QuilParser::ParamContext::getRuleIndex() const {
  return QuilParser::RuleParam;
}

void QuilParser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void QuilParser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}

QuilParser::ParamContext* QuilParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 12, QuilParser::RuleParam);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(189);
    expression(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ModifierContext ------------------------------------------------------------------

QuilParser::ModifierContext::ModifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::ModifierContext::CONTROLLED() {
  return getToken(QuilParser::CONTROLLED, 0);
}

tree::TerminalNode* QuilParser::ModifierContext::DAGGER() {
  return getToken(QuilParser::DAGGER, 0);
}

tree::TerminalNode* QuilParser::ModifierContext::FORKED() {
  return getToken(QuilParser::FORKED, 0);
}


size_t QuilParser::ModifierContext::getRuleIndex() const {
  return QuilParser::RuleModifier;
}

void QuilParser::ModifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterModifier(this);
}

void QuilParser::ModifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitModifier(this);
}

QuilParser::ModifierContext* QuilParser::modifier() {
  ModifierContext *_localctx = _tracker.createInstance<ModifierContext>(_ctx, getState());
  enterRule(_localctx, 14, QuilParser::RuleModifier);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(191);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::CONTROLLED)
      | (1ULL << QuilParser::DAGGER)
      | (1ULL << QuilParser::FORKED))) != 0))) {
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

//----------------- DefGateContext ------------------------------------------------------------------

QuilParser::DefGateContext::DefGateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::DefGateContext::DEFGATE() {
  return getToken(QuilParser::DEFGATE, 0);
}

QuilParser::NameContext* QuilParser::DefGateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode* QuilParser::DefGateContext::COLON() {
  return getToken(QuilParser::COLON, 0);
}

tree::TerminalNode* QuilParser::DefGateContext::NEWLINE() {
  return getToken(QuilParser::NEWLINE, 0);
}

QuilParser::MatrixContext* QuilParser::DefGateContext::matrix() {
  return getRuleContext<QuilParser::MatrixContext>(0);
}

tree::TerminalNode* QuilParser::DefGateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::VariableContext *> QuilParser::DefGateContext::variable() {
  return getRuleContexts<QuilParser::VariableContext>();
}

QuilParser::VariableContext* QuilParser::DefGateContext::variable(size_t i) {
  return getRuleContext<QuilParser::VariableContext>(i);
}

tree::TerminalNode* QuilParser::DefGateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

tree::TerminalNode* QuilParser::DefGateContext::AS() {
  return getToken(QuilParser::AS, 0);
}

QuilParser::GatetypeContext* QuilParser::DefGateContext::gatetype() {
  return getRuleContext<QuilParser::GatetypeContext>(0);
}

std::vector<tree::TerminalNode *> QuilParser::DefGateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::DefGateContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::DefGateContext::getRuleIndex() const {
  return QuilParser::RuleDefGate;
}

void QuilParser::DefGateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefGate(this);
}

void QuilParser::DefGateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefGate(this);
}

QuilParser::DefGateContext* QuilParser::defGate() {
  DefGateContext *_localctx = _tracker.createInstance<DefGateContext>(_ctx, getState());
  enterRule(_localctx, 16, QuilParser::RuleDefGate);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(193);
    match(QuilParser::DEFGATE);
    setState(194);
    name();
    setState(208);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::LPAREN: {
        setState(195);
        match(QuilParser::LPAREN);
        setState(196);
        variable();
        setState(201);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == QuilParser::COMMA) {
          setState(197);
          match(QuilParser::COMMA);
          setState(198);
          variable();
          setState(203);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(204);
        match(QuilParser::RPAREN);
        break;
      }

      case QuilParser::AS: {
        setState(206);
        match(QuilParser::AS);
        setState(207);
        gatetype();
        break;
      }

      case QuilParser::COLON: {
        break;
      }

    default:
      break;
    }
    setState(210);
    match(QuilParser::COLON);
    setState(211);
    match(QuilParser::NEWLINE);
    setState(212);
    matrix();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefGateAsPauliContext ------------------------------------------------------------------

QuilParser::DefGateAsPauliContext::DefGateAsPauliContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::DEFGATE() {
  return getToken(QuilParser::DEFGATE, 0);
}

QuilParser::NameContext* QuilParser::DefGateAsPauliContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::AS() {
  return getToken(QuilParser::AS, 0);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::PAULISUM() {
  return getToken(QuilParser::PAULISUM, 0);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::COLON() {
  return getToken(QuilParser::COLON, 0);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::NEWLINE() {
  return getToken(QuilParser::NEWLINE, 0);
}

QuilParser::PauliTermsContext* QuilParser::DefGateAsPauliContext::pauliTerms() {
  return getRuleContext<QuilParser::PauliTermsContext>(0);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::VariableContext *> QuilParser::DefGateAsPauliContext::variable() {
  return getRuleContexts<QuilParser::VariableContext>();
}

QuilParser::VariableContext* QuilParser::DefGateAsPauliContext::variable(size_t i) {
  return getRuleContext<QuilParser::VariableContext>(i);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitVariableContext *> QuilParser::DefGateAsPauliContext::qubitVariable() {
  return getRuleContexts<QuilParser::QubitVariableContext>();
}

QuilParser::QubitVariableContext* QuilParser::DefGateAsPauliContext::qubitVariable(size_t i) {
  return getRuleContext<QuilParser::QubitVariableContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::DefGateAsPauliContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::DefGateAsPauliContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::DefGateAsPauliContext::getRuleIndex() const {
  return QuilParser::RuleDefGateAsPauli;
}

void QuilParser::DefGateAsPauliContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefGateAsPauli(this);
}

void QuilParser::DefGateAsPauliContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefGateAsPauli(this);
}

QuilParser::DefGateAsPauliContext* QuilParser::defGateAsPauli() {
  DefGateAsPauliContext *_localctx = _tracker.createInstance<DefGateAsPauliContext>(_ctx, getState());
  enterRule(_localctx, 18, QuilParser::RuleDefGateAsPauli);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(214);
    match(QuilParser::DEFGATE);
    setState(215);
    name();
    setState(227);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(216);
      match(QuilParser::LPAREN);
      setState(217);
      variable();
      setState(222);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(218);
        match(QuilParser::COMMA);
        setState(219);
        variable();
        setState(224);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(225);
      match(QuilParser::RPAREN);
    }
    setState(230); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(229);
      qubitVariable();
      setState(232); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::IDENTIFIER);
    setState(234);
    match(QuilParser::AS);
    setState(235);
    match(QuilParser::PAULISUM);
    setState(236);
    match(QuilParser::COLON);
    setState(237);
    match(QuilParser::NEWLINE);
    setState(238);
    pauliTerms();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableContext ------------------------------------------------------------------

QuilParser::VariableContext::VariableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::VariableContext::PERCENTAGE() {
  return getToken(QuilParser::PERCENTAGE, 0);
}

tree::TerminalNode* QuilParser::VariableContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::VariableContext::getRuleIndex() const {
  return QuilParser::RuleVariable;
}

void QuilParser::VariableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariable(this);
}

void QuilParser::VariableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariable(this);
}

QuilParser::VariableContext* QuilParser::variable() {
  VariableContext *_localctx = _tracker.createInstance<VariableContext>(_ctx, getState());
  enterRule(_localctx, 20, QuilParser::RuleVariable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(240);
    match(QuilParser::PERCENTAGE);
    setState(241);
    match(QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GatetypeContext ------------------------------------------------------------------

QuilParser::GatetypeContext::GatetypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::GatetypeContext::MATRIX() {
  return getToken(QuilParser::MATRIX, 0);
}

tree::TerminalNode* QuilParser::GatetypeContext::PERMUTATION() {
  return getToken(QuilParser::PERMUTATION, 0);
}


size_t QuilParser::GatetypeContext::getRuleIndex() const {
  return QuilParser::RuleGatetype;
}

void QuilParser::GatetypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatetype(this);
}

void QuilParser::GatetypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatetype(this);
}

QuilParser::GatetypeContext* QuilParser::gatetype() {
  GatetypeContext *_localctx = _tracker.createInstance<GatetypeContext>(_ctx, getState());
  enterRule(_localctx, 22, QuilParser::RuleGatetype);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(243);
    _la = _input->LA(1);
    if (!(_la == QuilParser::MATRIX

    || _la == QuilParser::PERMUTATION)) {
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

//----------------- MatrixContext ------------------------------------------------------------------

QuilParser::MatrixContext::MatrixContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<QuilParser::MatrixRowContext *> QuilParser::MatrixContext::matrixRow() {
  return getRuleContexts<QuilParser::MatrixRowContext>();
}

QuilParser::MatrixRowContext* QuilParser::MatrixContext::matrixRow(size_t i) {
  return getRuleContext<QuilParser::MatrixRowContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::MatrixContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode* QuilParser::MatrixContext::NEWLINE(size_t i) {
  return getToken(QuilParser::NEWLINE, i);
}


size_t QuilParser::MatrixContext::getRuleIndex() const {
  return QuilParser::RuleMatrix;
}

void QuilParser::MatrixContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMatrix(this);
}

void QuilParser::MatrixContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMatrix(this);
}

QuilParser::MatrixContext* QuilParser::matrix() {
  MatrixContext *_localctx = _tracker.createInstance<MatrixContext>(_ctx, getState());
  enterRule(_localctx, 24, QuilParser::RuleMatrix);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(250);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(245);
        matrixRow();
        setState(246);
        match(QuilParser::NEWLINE); 
      }
      setState(252);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx);
    }
    setState(253);
    matrixRow();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MatrixRowContext ------------------------------------------------------------------

QuilParser::MatrixRowContext::MatrixRowContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::MatrixRowContext::TAB() {
  return getToken(QuilParser::TAB, 0);
}

std::vector<QuilParser::ExpressionContext *> QuilParser::MatrixRowContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext* QuilParser::MatrixRowContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::MatrixRowContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::MatrixRowContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::MatrixRowContext::getRuleIndex() const {
  return QuilParser::RuleMatrixRow;
}

void QuilParser::MatrixRowContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMatrixRow(this);
}

void QuilParser::MatrixRowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMatrixRow(this);
}

QuilParser::MatrixRowContext* QuilParser::matrixRow() {
  MatrixRowContext *_localctx = _tracker.createInstance<MatrixRowContext>(_ctx, getState());
  enterRule(_localctx, 26, QuilParser::RuleMatrixRow);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(255);
    match(QuilParser::TAB);
    setState(256);
    expression(0);
    setState(261);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::COMMA) {
      setState(257);
      match(QuilParser::COMMA);
      setState(258);
      expression(0);
      setState(263);
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

//----------------- PauliTermsContext ------------------------------------------------------------------

QuilParser::PauliTermsContext::PauliTermsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<QuilParser::PauliTermContext *> QuilParser::PauliTermsContext::pauliTerm() {
  return getRuleContexts<QuilParser::PauliTermContext>();
}

QuilParser::PauliTermContext* QuilParser::PauliTermsContext::pauliTerm(size_t i) {
  return getRuleContext<QuilParser::PauliTermContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::PauliTermsContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode* QuilParser::PauliTermsContext::NEWLINE(size_t i) {
  return getToken(QuilParser::NEWLINE, i);
}


size_t QuilParser::PauliTermsContext::getRuleIndex() const {
  return QuilParser::RulePauliTerms;
}

void QuilParser::PauliTermsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPauliTerms(this);
}

void QuilParser::PauliTermsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPauliTerms(this);
}

QuilParser::PauliTermsContext* QuilParser::pauliTerms() {
  PauliTermsContext *_localctx = _tracker.createInstance<PauliTermsContext>(_ctx, getState());
  enterRule(_localctx, 28, QuilParser::RulePauliTerms);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(269);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(264);
        pauliTerm();
        setState(265);
        match(QuilParser::NEWLINE); 
      }
      setState(271);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    }
    setState(272);
    pauliTerm();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PauliTermContext ------------------------------------------------------------------

QuilParser::PauliTermContext::PauliTermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::PauliTermContext::TAB() {
  return getToken(QuilParser::TAB, 0);
}

tree::TerminalNode* QuilParser::PauliTermContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

tree::TerminalNode* QuilParser::PauliTermContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

QuilParser::ExpressionContext* QuilParser::PauliTermContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

tree::TerminalNode* QuilParser::PauliTermContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitVariableContext *> QuilParser::PauliTermContext::qubitVariable() {
  return getRuleContexts<QuilParser::QubitVariableContext>();
}

QuilParser::QubitVariableContext* QuilParser::PauliTermContext::qubitVariable(size_t i) {
  return getRuleContext<QuilParser::QubitVariableContext>(i);
}


size_t QuilParser::PauliTermContext::getRuleIndex() const {
  return QuilParser::RulePauliTerm;
}

void QuilParser::PauliTermContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPauliTerm(this);
}

void QuilParser::PauliTermContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPauliTerm(this);
}

QuilParser::PauliTermContext* QuilParser::pauliTerm() {
  PauliTermContext *_localctx = _tracker.createInstance<PauliTermContext>(_ctx, getState());
  enterRule(_localctx, 30, QuilParser::RulePauliTerm);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(274);
    match(QuilParser::TAB);
    setState(275);
    match(QuilParser::IDENTIFIER);
    setState(276);
    match(QuilParser::LPAREN);
    setState(277);
    expression(0);
    setState(278);
    match(QuilParser::RPAREN);
    setState(280); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(279);
      qubitVariable();
      setState(282); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefCircuitContext ------------------------------------------------------------------

QuilParser::DefCircuitContext::DefCircuitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::DefCircuitContext::DEFCIRCUIT() {
  return getToken(QuilParser::DEFCIRCUIT, 0);
}

QuilParser::NameContext* QuilParser::DefCircuitContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode* QuilParser::DefCircuitContext::COLON() {
  return getToken(QuilParser::COLON, 0);
}

tree::TerminalNode* QuilParser::DefCircuitContext::NEWLINE() {
  return getToken(QuilParser::NEWLINE, 0);
}

QuilParser::CircuitContext* QuilParser::DefCircuitContext::circuit() {
  return getRuleContext<QuilParser::CircuitContext>(0);
}

tree::TerminalNode* QuilParser::DefCircuitContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::VariableContext *> QuilParser::DefCircuitContext::variable() {
  return getRuleContexts<QuilParser::VariableContext>();
}

QuilParser::VariableContext* QuilParser::DefCircuitContext::variable(size_t i) {
  return getRuleContext<QuilParser::VariableContext>(i);
}

tree::TerminalNode* QuilParser::DefCircuitContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitVariableContext *> QuilParser::DefCircuitContext::qubitVariable() {
  return getRuleContexts<QuilParser::QubitVariableContext>();
}

QuilParser::QubitVariableContext* QuilParser::DefCircuitContext::qubitVariable(size_t i) {
  return getRuleContext<QuilParser::QubitVariableContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::DefCircuitContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::DefCircuitContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::DefCircuitContext::getRuleIndex() const {
  return QuilParser::RuleDefCircuit;
}

void QuilParser::DefCircuitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefCircuit(this);
}

void QuilParser::DefCircuitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefCircuit(this);
}

QuilParser::DefCircuitContext* QuilParser::defCircuit() {
  DefCircuitContext *_localctx = _tracker.createInstance<DefCircuitContext>(_ctx, getState());
  enterRule(_localctx, 32, QuilParser::RuleDefCircuit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(284);
    match(QuilParser::DEFCIRCUIT);
    setState(285);
    name();
    setState(297);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(286);
      match(QuilParser::LPAREN);
      setState(287);
      variable();
      setState(292);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(288);
        match(QuilParser::COMMA);
        setState(289);
        variable();
        setState(294);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(295);
      match(QuilParser::RPAREN);
    }
    setState(302);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::IDENTIFIER) {
      setState(299);
      qubitVariable();
      setState(304);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(305);
    match(QuilParser::COLON);
    setState(306);
    match(QuilParser::NEWLINE);
    setState(307);
    circuit();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QubitVariableContext ------------------------------------------------------------------

QuilParser::QubitVariableContext::QubitVariableContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::QubitVariableContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::QubitVariableContext::getRuleIndex() const {
  return QuilParser::RuleQubitVariable;
}

void QuilParser::QubitVariableContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQubitVariable(this);
}

void QuilParser::QubitVariableContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQubitVariable(this);
}

QuilParser::QubitVariableContext* QuilParser::qubitVariable() {
  QubitVariableContext *_localctx = _tracker.createInstance<QubitVariableContext>(_ctx, getState());
  enterRule(_localctx, 34, QuilParser::RuleQubitVariable);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(309);
    match(QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitQubitContext ------------------------------------------------------------------

QuilParser::CircuitQubitContext::CircuitQubitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::QubitContext* QuilParser::CircuitQubitContext::qubit() {
  return getRuleContext<QuilParser::QubitContext>(0);
}

QuilParser::QubitVariableContext* QuilParser::CircuitQubitContext::qubitVariable() {
  return getRuleContext<QuilParser::QubitVariableContext>(0);
}


size_t QuilParser::CircuitQubitContext::getRuleIndex() const {
  return QuilParser::RuleCircuitQubit;
}

void QuilParser::CircuitQubitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitQubit(this);
}

void QuilParser::CircuitQubitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitQubit(this);
}

QuilParser::CircuitQubitContext* QuilParser::circuitQubit() {
  CircuitQubitContext *_localctx = _tracker.createInstance<CircuitQubitContext>(_ctx, getState());
  enterRule(_localctx, 36, QuilParser::RuleCircuitQubit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(313);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::INT: {
        enterOuterAlt(_localctx, 1);
        setState(311);
        qubit();
        break;
      }

      case QuilParser::IDENTIFIER: {
        enterOuterAlt(_localctx, 2);
        setState(312);
        qubitVariable();
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

//----------------- CircuitGateContext ------------------------------------------------------------------

QuilParser::CircuitGateContext::CircuitGateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::NameContext* QuilParser::CircuitGateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode* QuilParser::CircuitGateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::ParamContext *> QuilParser::CircuitGateContext::param() {
  return getRuleContexts<QuilParser::ParamContext>();
}

QuilParser::ParamContext* QuilParser::CircuitGateContext::param(size_t i) {
  return getRuleContext<QuilParser::ParamContext>(i);
}

tree::TerminalNode* QuilParser::CircuitGateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::CircuitQubitContext *> QuilParser::CircuitGateContext::circuitQubit() {
  return getRuleContexts<QuilParser::CircuitQubitContext>();
}

QuilParser::CircuitQubitContext* QuilParser::CircuitGateContext::circuitQubit(size_t i) {
  return getRuleContext<QuilParser::CircuitQubitContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::CircuitGateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode* QuilParser::CircuitGateContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}


size_t QuilParser::CircuitGateContext::getRuleIndex() const {
  return QuilParser::RuleCircuitGate;
}

void QuilParser::CircuitGateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitGate(this);
}

void QuilParser::CircuitGateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitGate(this);
}

QuilParser::CircuitGateContext* QuilParser::circuitGate() {
  CircuitGateContext *_localctx = _tracker.createInstance<CircuitGateContext>(_ctx, getState());
  enterRule(_localctx, 38, QuilParser::RuleCircuitGate);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(315);
    name();
    setState(327);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(316);
      match(QuilParser::LPAREN);
      setState(317);
      param();
      setState(322);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(318);
        match(QuilParser::COMMA);
        setState(319);
        param();
        setState(324);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(325);
      match(QuilParser::RPAREN);
    }
    setState(330); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(329);
      circuitQubit();
      setState(332); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::IDENTIFIER

    || _la == QuilParser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitMeasureContext ------------------------------------------------------------------

QuilParser::CircuitMeasureContext::CircuitMeasureContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::CircuitMeasureContext::MEASURE() {
  return getToken(QuilParser::MEASURE, 0);
}

QuilParser::CircuitQubitContext* QuilParser::CircuitMeasureContext::circuitQubit() {
  return getRuleContext<QuilParser::CircuitQubitContext>(0);
}

QuilParser::AddrContext* QuilParser::CircuitMeasureContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}


size_t QuilParser::CircuitMeasureContext::getRuleIndex() const {
  return QuilParser::RuleCircuitMeasure;
}

void QuilParser::CircuitMeasureContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitMeasure(this);
}

void QuilParser::CircuitMeasureContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitMeasure(this);
}

QuilParser::CircuitMeasureContext* QuilParser::circuitMeasure() {
  CircuitMeasureContext *_localctx = _tracker.createInstance<CircuitMeasureContext>(_ctx, getState());
  enterRule(_localctx, 40, QuilParser::RuleCircuitMeasure);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(334);
    match(QuilParser::MEASURE);
    setState(335);
    circuitQubit();
    setState(337);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::IDENTIFIER

    || _la == QuilParser::LBRACKET) {
      setState(336);
      addr();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitResetStateContext ------------------------------------------------------------------

QuilParser::CircuitResetStateContext::CircuitResetStateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::CircuitResetStateContext::RESET() {
  return getToken(QuilParser::RESET, 0);
}

QuilParser::CircuitQubitContext* QuilParser::CircuitResetStateContext::circuitQubit() {
  return getRuleContext<QuilParser::CircuitQubitContext>(0);
}


size_t QuilParser::CircuitResetStateContext::getRuleIndex() const {
  return QuilParser::RuleCircuitResetState;
}

void QuilParser::CircuitResetStateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitResetState(this);
}

void QuilParser::CircuitResetStateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitResetState(this);
}

QuilParser::CircuitResetStateContext* QuilParser::circuitResetState() {
  CircuitResetStateContext *_localctx = _tracker.createInstance<CircuitResetStateContext>(_ctx, getState());
  enterRule(_localctx, 42, QuilParser::RuleCircuitResetState);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(339);
    match(QuilParser::RESET);
    setState(341);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::IDENTIFIER

    || _la == QuilParser::INT) {
      setState(340);
      circuitQubit();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitInstrContext ------------------------------------------------------------------

QuilParser::CircuitInstrContext::CircuitInstrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::CircuitGateContext* QuilParser::CircuitInstrContext::circuitGate() {
  return getRuleContext<QuilParser::CircuitGateContext>(0);
}

QuilParser::CircuitMeasureContext* QuilParser::CircuitInstrContext::circuitMeasure() {
  return getRuleContext<QuilParser::CircuitMeasureContext>(0);
}

QuilParser::CircuitResetStateContext* QuilParser::CircuitInstrContext::circuitResetState() {
  return getRuleContext<QuilParser::CircuitResetStateContext>(0);
}

QuilParser::InstrContext* QuilParser::CircuitInstrContext::instr() {
  return getRuleContext<QuilParser::InstrContext>(0);
}


size_t QuilParser::CircuitInstrContext::getRuleIndex() const {
  return QuilParser::RuleCircuitInstr;
}

void QuilParser::CircuitInstrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitInstr(this);
}

void QuilParser::CircuitInstrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitInstr(this);
}

QuilParser::CircuitInstrContext* QuilParser::circuitInstr() {
  CircuitInstrContext *_localctx = _tracker.createInstance<CircuitInstrContext>(_ctx, getState());
  enterRule(_localctx, 44, QuilParser::RuleCircuitInstr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(347);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(343);
      circuitGate();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(344);
      circuitMeasure();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(345);
      circuitResetState();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(346);
      instr();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitContext ------------------------------------------------------------------

QuilParser::CircuitContext::CircuitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> QuilParser::CircuitContext::TAB() {
  return getTokens(QuilParser::TAB);
}

tree::TerminalNode* QuilParser::CircuitContext::TAB(size_t i) {
  return getToken(QuilParser::TAB, i);
}

std::vector<QuilParser::CircuitInstrContext *> QuilParser::CircuitContext::circuitInstr() {
  return getRuleContexts<QuilParser::CircuitInstrContext>();
}

QuilParser::CircuitInstrContext* QuilParser::CircuitContext::circuitInstr(size_t i) {
  return getRuleContext<QuilParser::CircuitInstrContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::CircuitContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode* QuilParser::CircuitContext::NEWLINE(size_t i) {
  return getToken(QuilParser::NEWLINE, i);
}


size_t QuilParser::CircuitContext::getRuleIndex() const {
  return QuilParser::RuleCircuit;
}

void QuilParser::CircuitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuit(this);
}

void QuilParser::CircuitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuit(this);
}

QuilParser::CircuitContext* QuilParser::circuit() {
  CircuitContext *_localctx = _tracker.createInstance<CircuitContext>(_ctx, getState());
  enterRule(_localctx, 46, QuilParser::RuleCircuit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(355);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(349);
        match(QuilParser::TAB);
        setState(350);
        circuitInstr();
        setState(351);
        match(QuilParser::NEWLINE); 
      }
      setState(357);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    }
    setState(358);
    match(QuilParser::TAB);
    setState(359);
    circuitInstr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MeasureContext ------------------------------------------------------------------

QuilParser::MeasureContext::MeasureContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::MeasureContext::MEASURE() {
  return getToken(QuilParser::MEASURE, 0);
}

QuilParser::QubitContext* QuilParser::MeasureContext::qubit() {
  return getRuleContext<QuilParser::QubitContext>(0);
}

QuilParser::AddrContext* QuilParser::MeasureContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}


size_t QuilParser::MeasureContext::getRuleIndex() const {
  return QuilParser::RuleMeasure;
}

void QuilParser::MeasureContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMeasure(this);
}

void QuilParser::MeasureContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMeasure(this);
}

QuilParser::MeasureContext* QuilParser::measure() {
  MeasureContext *_localctx = _tracker.createInstance<MeasureContext>(_ctx, getState());
  enterRule(_localctx, 48, QuilParser::RuleMeasure);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(361);
    match(QuilParser::MEASURE);
    setState(362);
    qubit();
    setState(364);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::IDENTIFIER

    || _la == QuilParser::LBRACKET) {
      setState(363);
      addr();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddrContext ------------------------------------------------------------------

QuilParser::AddrContext::AddrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::AddrContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

tree::TerminalNode* QuilParser::AddrContext::LBRACKET() {
  return getToken(QuilParser::LBRACKET, 0);
}

tree::TerminalNode* QuilParser::AddrContext::INT() {
  return getToken(QuilParser::INT, 0);
}

tree::TerminalNode* QuilParser::AddrContext::RBRACKET() {
  return getToken(QuilParser::RBRACKET, 0);
}


size_t QuilParser::AddrContext::getRuleIndex() const {
  return QuilParser::RuleAddr;
}

void QuilParser::AddrContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddr(this);
}

void QuilParser::AddrContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddr(this);
}

QuilParser::AddrContext* QuilParser::addr() {
  AddrContext *_localctx = _tracker.createInstance<AddrContext>(_ctx, getState());
  enterRule(_localctx, 50, QuilParser::RuleAddr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(373);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(366);
      match(QuilParser::IDENTIFIER);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(368);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == QuilParser::IDENTIFIER) {
        setState(367);
        match(QuilParser::IDENTIFIER);
      }
      setState(370);
      match(QuilParser::LBRACKET);
      setState(371);
      match(QuilParser::INT);
      setState(372);
      match(QuilParser::RBRACKET);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefLabelContext ------------------------------------------------------------------

QuilParser::DefLabelContext::DefLabelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::DefLabelContext::LABEL() {
  return getToken(QuilParser::LABEL, 0);
}

QuilParser::LabelContext* QuilParser::DefLabelContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}


size_t QuilParser::DefLabelContext::getRuleIndex() const {
  return QuilParser::RuleDefLabel;
}

void QuilParser::DefLabelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefLabel(this);
}

void QuilParser::DefLabelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefLabel(this);
}

QuilParser::DefLabelContext* QuilParser::defLabel() {
  DefLabelContext *_localctx = _tracker.createInstance<DefLabelContext>(_ctx, getState());
  enterRule(_localctx, 52, QuilParser::RuleDefLabel);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(375);
    match(QuilParser::LABEL);
    setState(376);
    label();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LabelContext ------------------------------------------------------------------

QuilParser::LabelContext::LabelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::LabelContext::AT() {
  return getToken(QuilParser::AT, 0);
}

tree::TerminalNode* QuilParser::LabelContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::LabelContext::getRuleIndex() const {
  return QuilParser::RuleLabel;
}

void QuilParser::LabelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLabel(this);
}

void QuilParser::LabelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLabel(this);
}

QuilParser::LabelContext* QuilParser::label() {
  LabelContext *_localctx = _tracker.createInstance<LabelContext>(_ctx, getState());
  enterRule(_localctx, 54, QuilParser::RuleLabel);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(378);
    match(QuilParser::AT);
    setState(379);
    match(QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HaltContext ------------------------------------------------------------------

QuilParser::HaltContext::HaltContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::HaltContext::HALT() {
  return getToken(QuilParser::HALT, 0);
}


size_t QuilParser::HaltContext::getRuleIndex() const {
  return QuilParser::RuleHalt;
}

void QuilParser::HaltContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHalt(this);
}

void QuilParser::HaltContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHalt(this);
}

QuilParser::HaltContext* QuilParser::halt() {
  HaltContext *_localctx = _tracker.createInstance<HaltContext>(_ctx, getState());
  enterRule(_localctx, 56, QuilParser::RuleHalt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(381);
    match(QuilParser::HALT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpContext ------------------------------------------------------------------

QuilParser::JumpContext::JumpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::JumpContext::JUMP() {
  return getToken(QuilParser::JUMP, 0);
}

QuilParser::LabelContext* QuilParser::JumpContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}


size_t QuilParser::JumpContext::getRuleIndex() const {
  return QuilParser::RuleJump;
}

void QuilParser::JumpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterJump(this);
}

void QuilParser::JumpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitJump(this);
}

QuilParser::JumpContext* QuilParser::jump() {
  JumpContext *_localctx = _tracker.createInstance<JumpContext>(_ctx, getState());
  enterRule(_localctx, 58, QuilParser::RuleJump);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(383);
    match(QuilParser::JUMP);
    setState(384);
    label();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpWhenContext ------------------------------------------------------------------

QuilParser::JumpWhenContext::JumpWhenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::JumpWhenContext::JUMPWHEN() {
  return getToken(QuilParser::JUMPWHEN, 0);
}

QuilParser::LabelContext* QuilParser::JumpWhenContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}

QuilParser::AddrContext* QuilParser::JumpWhenContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}


size_t QuilParser::JumpWhenContext::getRuleIndex() const {
  return QuilParser::RuleJumpWhen;
}

void QuilParser::JumpWhenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterJumpWhen(this);
}

void QuilParser::JumpWhenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitJumpWhen(this);
}

QuilParser::JumpWhenContext* QuilParser::jumpWhen() {
  JumpWhenContext *_localctx = _tracker.createInstance<JumpWhenContext>(_ctx, getState());
  enterRule(_localctx, 60, QuilParser::RuleJumpWhen);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(386);
    match(QuilParser::JUMPWHEN);
    setState(387);
    label();
    setState(388);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpUnlessContext ------------------------------------------------------------------

QuilParser::JumpUnlessContext::JumpUnlessContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::JumpUnlessContext::JUMPUNLESS() {
  return getToken(QuilParser::JUMPUNLESS, 0);
}

QuilParser::LabelContext* QuilParser::JumpUnlessContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}

QuilParser::AddrContext* QuilParser::JumpUnlessContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}


size_t QuilParser::JumpUnlessContext::getRuleIndex() const {
  return QuilParser::RuleJumpUnless;
}

void QuilParser::JumpUnlessContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterJumpUnless(this);
}

void QuilParser::JumpUnlessContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitJumpUnless(this);
}

QuilParser::JumpUnlessContext* QuilParser::jumpUnless() {
  JumpUnlessContext *_localctx = _tracker.createInstance<JumpUnlessContext>(_ctx, getState());
  enterRule(_localctx, 62, QuilParser::RuleJumpUnless);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(390);
    match(QuilParser::JUMPUNLESS);
    setState(391);
    label();
    setState(392);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ResetStateContext ------------------------------------------------------------------

QuilParser::ResetStateContext::ResetStateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::ResetStateContext::RESET() {
  return getToken(QuilParser::RESET, 0);
}

QuilParser::QubitContext* QuilParser::ResetStateContext::qubit() {
  return getRuleContext<QuilParser::QubitContext>(0);
}


size_t QuilParser::ResetStateContext::getRuleIndex() const {
  return QuilParser::RuleResetState;
}

void QuilParser::ResetStateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterResetState(this);
}

void QuilParser::ResetStateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitResetState(this);
}

QuilParser::ResetStateContext* QuilParser::resetState() {
  ResetStateContext *_localctx = _tracker.createInstance<ResetStateContext>(_ctx, getState());
  enterRule(_localctx, 64, QuilParser::RuleResetState);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(394);
    match(QuilParser::RESET);
    setState(396);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::INT) {
      setState(395);
      qubit();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WaitContext ------------------------------------------------------------------

QuilParser::WaitContext::WaitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::WaitContext::WAIT() {
  return getToken(QuilParser::WAIT, 0);
}


size_t QuilParser::WaitContext::getRuleIndex() const {
  return QuilParser::RuleWait;
}

void QuilParser::WaitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWait(this);
}

void QuilParser::WaitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWait(this);
}

QuilParser::WaitContext* QuilParser::wait() {
  WaitContext *_localctx = _tracker.createInstance<WaitContext>(_ctx, getState());
  enterRule(_localctx, 66, QuilParser::RuleWait);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(398);
    match(QuilParser::WAIT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MemoryDescriptorContext ------------------------------------------------------------------

QuilParser::MemoryDescriptorContext::MemoryDescriptorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::DECLARE() {
  return getToken(QuilParser::DECLARE, 0);
}

std::vector<tree::TerminalNode *> QuilParser::MemoryDescriptorContext::IDENTIFIER() {
  return getTokens(QuilParser::IDENTIFIER);
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::IDENTIFIER(size_t i) {
  return getToken(QuilParser::IDENTIFIER, i);
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::LBRACKET() {
  return getToken(QuilParser::LBRACKET, 0);
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::INT() {
  return getToken(QuilParser::INT, 0);
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::RBRACKET() {
  return getToken(QuilParser::RBRACKET, 0);
}

tree::TerminalNode* QuilParser::MemoryDescriptorContext::SHARING() {
  return getToken(QuilParser::SHARING, 0);
}

std::vector<QuilParser::OffsetDescriptorContext *> QuilParser::MemoryDescriptorContext::offsetDescriptor() {
  return getRuleContexts<QuilParser::OffsetDescriptorContext>();
}

QuilParser::OffsetDescriptorContext* QuilParser::MemoryDescriptorContext::offsetDescriptor(size_t i) {
  return getRuleContext<QuilParser::OffsetDescriptorContext>(i);
}


size_t QuilParser::MemoryDescriptorContext::getRuleIndex() const {
  return QuilParser::RuleMemoryDescriptor;
}

void QuilParser::MemoryDescriptorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemoryDescriptor(this);
}

void QuilParser::MemoryDescriptorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemoryDescriptor(this);
}

QuilParser::MemoryDescriptorContext* QuilParser::memoryDescriptor() {
  MemoryDescriptorContext *_localctx = _tracker.createInstance<MemoryDescriptorContext>(_ctx, getState());
  enterRule(_localctx, 68, QuilParser::RuleMemoryDescriptor);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(400);
    match(QuilParser::DECLARE);
    setState(401);
    match(QuilParser::IDENTIFIER);
    setState(402);
    match(QuilParser::IDENTIFIER);
    setState(406);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LBRACKET) {
      setState(403);
      match(QuilParser::LBRACKET);
      setState(404);
      match(QuilParser::INT);
      setState(405);
      match(QuilParser::RBRACKET);
    }
    setState(416);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::SHARING) {
      setState(408);
      match(QuilParser::SHARING);
      setState(409);
      match(QuilParser::IDENTIFIER);
      setState(413);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::OFFSET) {
        setState(410);
        offsetDescriptor();
        setState(415);
        _errHandler->sync(this);
        _la = _input->LA(1);
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

//----------------- OffsetDescriptorContext ------------------------------------------------------------------

QuilParser::OffsetDescriptorContext::OffsetDescriptorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::OffsetDescriptorContext::OFFSET() {
  return getToken(QuilParser::OFFSET, 0);
}

tree::TerminalNode* QuilParser::OffsetDescriptorContext::INT() {
  return getToken(QuilParser::INT, 0);
}

tree::TerminalNode* QuilParser::OffsetDescriptorContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::OffsetDescriptorContext::getRuleIndex() const {
  return QuilParser::RuleOffsetDescriptor;
}

void QuilParser::OffsetDescriptorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOffsetDescriptor(this);
}

void QuilParser::OffsetDescriptorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOffsetDescriptor(this);
}

QuilParser::OffsetDescriptorContext* QuilParser::offsetDescriptor() {
  OffsetDescriptorContext *_localctx = _tracker.createInstance<OffsetDescriptorContext>(_ctx, getState());
  enterRule(_localctx, 70, QuilParser::RuleOffsetDescriptor);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(418);
    match(QuilParser::OFFSET);
    setState(419);
    match(QuilParser::INT);
    setState(420);
    match(QuilParser::IDENTIFIER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassicalUnaryContext ------------------------------------------------------------------

QuilParser::ClassicalUnaryContext::ClassicalUnaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::AddrContext* QuilParser::ClassicalUnaryContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}

tree::TerminalNode* QuilParser::ClassicalUnaryContext::NEG() {
  return getToken(QuilParser::NEG, 0);
}

tree::TerminalNode* QuilParser::ClassicalUnaryContext::NOT() {
  return getToken(QuilParser::NOT, 0);
}

tree::TerminalNode* QuilParser::ClassicalUnaryContext::TRUE() {
  return getToken(QuilParser::TRUE, 0);
}

tree::TerminalNode* QuilParser::ClassicalUnaryContext::FALSE() {
  return getToken(QuilParser::FALSE, 0);
}


size_t QuilParser::ClassicalUnaryContext::getRuleIndex() const {
  return QuilParser::RuleClassicalUnary;
}

void QuilParser::ClassicalUnaryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalUnary(this);
}

void QuilParser::ClassicalUnaryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalUnary(this);
}

QuilParser::ClassicalUnaryContext* QuilParser::classicalUnary() {
  ClassicalUnaryContext *_localctx = _tracker.createInstance<ClassicalUnaryContext>(_ctx, getState());
  enterRule(_localctx, 72, QuilParser::RuleClassicalUnary);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(422);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::NEG)
      | (1ULL << QuilParser::NOT)
      | (1ULL << QuilParser::TRUE)
      | (1ULL << QuilParser::FALSE))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(423);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassicalBinaryContext ------------------------------------------------------------------

QuilParser::ClassicalBinaryContext::ClassicalBinaryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::LogicalBinaryOpContext* QuilParser::ClassicalBinaryContext::logicalBinaryOp() {
  return getRuleContext<QuilParser::LogicalBinaryOpContext>(0);
}

QuilParser::ArithmeticBinaryOpContext* QuilParser::ClassicalBinaryContext::arithmeticBinaryOp() {
  return getRuleContext<QuilParser::ArithmeticBinaryOpContext>(0);
}

QuilParser::MoveContext* QuilParser::ClassicalBinaryContext::move() {
  return getRuleContext<QuilParser::MoveContext>(0);
}

QuilParser::ExchangeContext* QuilParser::ClassicalBinaryContext::exchange() {
  return getRuleContext<QuilParser::ExchangeContext>(0);
}

QuilParser::ConvertContext* QuilParser::ClassicalBinaryContext::convert() {
  return getRuleContext<QuilParser::ConvertContext>(0);
}


size_t QuilParser::ClassicalBinaryContext::getRuleIndex() const {
  return QuilParser::RuleClassicalBinary;
}

void QuilParser::ClassicalBinaryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalBinary(this);
}

void QuilParser::ClassicalBinaryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalBinary(this);
}

QuilParser::ClassicalBinaryContext* QuilParser::classicalBinary() {
  ClassicalBinaryContext *_localctx = _tracker.createInstance<ClassicalBinaryContext>(_ctx, getState());
  enterRule(_localctx, 74, QuilParser::RuleClassicalBinary);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(430);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::AND:
      case QuilParser::IOR:
      case QuilParser::XOR:
      case QuilParser::OR: {
        enterOuterAlt(_localctx, 1);
        setState(425);
        logicalBinaryOp();
        break;
      }

      case QuilParser::ADD:
      case QuilParser::SUB:
      case QuilParser::MUL:
      case QuilParser::DIV: {
        enterOuterAlt(_localctx, 2);
        setState(426);
        arithmeticBinaryOp();
        break;
      }

      case QuilParser::MOVE: {
        enterOuterAlt(_localctx, 3);
        setState(427);
        move();
        break;
      }

      case QuilParser::EXCHANGE: {
        enterOuterAlt(_localctx, 4);
        setState(428);
        exchange();
        break;
      }

      case QuilParser::CONVERT: {
        enterOuterAlt(_localctx, 5);
        setState(429);
        convert();
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

//----------------- LogicalBinaryOpContext ------------------------------------------------------------------

QuilParser::LogicalBinaryOpContext::LogicalBinaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<QuilParser::AddrContext *> QuilParser::LogicalBinaryOpContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::LogicalBinaryOpContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

tree::TerminalNode* QuilParser::LogicalBinaryOpContext::AND() {
  return getToken(QuilParser::AND, 0);
}

tree::TerminalNode* QuilParser::LogicalBinaryOpContext::OR() {
  return getToken(QuilParser::OR, 0);
}

tree::TerminalNode* QuilParser::LogicalBinaryOpContext::IOR() {
  return getToken(QuilParser::IOR, 0);
}

tree::TerminalNode* QuilParser::LogicalBinaryOpContext::XOR() {
  return getToken(QuilParser::XOR, 0);
}

tree::TerminalNode* QuilParser::LogicalBinaryOpContext::INT() {
  return getToken(QuilParser::INT, 0);
}


size_t QuilParser::LogicalBinaryOpContext::getRuleIndex() const {
  return QuilParser::RuleLogicalBinaryOp;
}

void QuilParser::LogicalBinaryOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalBinaryOp(this);
}

void QuilParser::LogicalBinaryOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalBinaryOp(this);
}

QuilParser::LogicalBinaryOpContext* QuilParser::logicalBinaryOp() {
  LogicalBinaryOpContext *_localctx = _tracker.createInstance<LogicalBinaryOpContext>(_ctx, getState());
  enterRule(_localctx, 76, QuilParser::RuleLogicalBinaryOp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(432);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::AND)
      | (1ULL << QuilParser::IOR)
      | (1ULL << QuilParser::XOR)
      | (1ULL << QuilParser::OR))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(433);
    addr();
    setState(436);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::IDENTIFIER:
      case QuilParser::LBRACKET: {
        setState(434);
        addr();
        break;
      }

      case QuilParser::INT: {
        setState(435);
        match(QuilParser::INT);
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

//----------------- ArithmeticBinaryOpContext ------------------------------------------------------------------

QuilParser::ArithmeticBinaryOpContext::ArithmeticBinaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<QuilParser::AddrContext *> QuilParser::ArithmeticBinaryOpContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::ArithmeticBinaryOpContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

tree::TerminalNode* QuilParser::ArithmeticBinaryOpContext::ADD() {
  return getToken(QuilParser::ADD, 0);
}

tree::TerminalNode* QuilParser::ArithmeticBinaryOpContext::SUB() {
  return getToken(QuilParser::SUB, 0);
}

tree::TerminalNode* QuilParser::ArithmeticBinaryOpContext::MUL() {
  return getToken(QuilParser::MUL, 0);
}

tree::TerminalNode* QuilParser::ArithmeticBinaryOpContext::DIV() {
  return getToken(QuilParser::DIV, 0);
}

QuilParser::NumberContext* QuilParser::ArithmeticBinaryOpContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}


size_t QuilParser::ArithmeticBinaryOpContext::getRuleIndex() const {
  return QuilParser::RuleArithmeticBinaryOp;
}

void QuilParser::ArithmeticBinaryOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArithmeticBinaryOp(this);
}

void QuilParser::ArithmeticBinaryOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArithmeticBinaryOp(this);
}

QuilParser::ArithmeticBinaryOpContext* QuilParser::arithmeticBinaryOp() {
  ArithmeticBinaryOpContext *_localctx = _tracker.createInstance<ArithmeticBinaryOpContext>(_ctx, getState());
  enterRule(_localctx, 78, QuilParser::RuleArithmeticBinaryOp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(438);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::ADD)
      | (1ULL << QuilParser::SUB)
      | (1ULL << QuilParser::MUL)
      | (1ULL << QuilParser::DIV))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(439);
    addr();
    setState(442);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::IDENTIFIER:
      case QuilParser::LBRACKET: {
        setState(440);
        addr();
        break;
      }

      case QuilParser::PI:
      case QuilParser::I:
      case QuilParser::MINUS:
      case QuilParser::INT:
      case QuilParser::FLOAT: {
        setState(441);
        number();
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

//----------------- MoveContext ------------------------------------------------------------------

QuilParser::MoveContext::MoveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::MoveContext::MOVE() {
  return getToken(QuilParser::MOVE, 0);
}

std::vector<QuilParser::AddrContext *> QuilParser::MoveContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::MoveContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

QuilParser::NumberContext* QuilParser::MoveContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}


size_t QuilParser::MoveContext::getRuleIndex() const {
  return QuilParser::RuleMove;
}

void QuilParser::MoveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMove(this);
}

void QuilParser::MoveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMove(this);
}

QuilParser::MoveContext* QuilParser::move() {
  MoveContext *_localctx = _tracker.createInstance<MoveContext>(_ctx, getState());
  enterRule(_localctx, 80, QuilParser::RuleMove);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(444);
    match(QuilParser::MOVE);
    setState(445);
    addr();
    setState(448);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::IDENTIFIER:
      case QuilParser::LBRACKET: {
        setState(446);
        addr();
        break;
      }

      case QuilParser::PI:
      case QuilParser::I:
      case QuilParser::MINUS:
      case QuilParser::INT:
      case QuilParser::FLOAT: {
        setState(447);
        number();
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

//----------------- ExchangeContext ------------------------------------------------------------------

QuilParser::ExchangeContext::ExchangeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::ExchangeContext::EXCHANGE() {
  return getToken(QuilParser::EXCHANGE, 0);
}

std::vector<QuilParser::AddrContext *> QuilParser::ExchangeContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::ExchangeContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}


size_t QuilParser::ExchangeContext::getRuleIndex() const {
  return QuilParser::RuleExchange;
}

void QuilParser::ExchangeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExchange(this);
}

void QuilParser::ExchangeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExchange(this);
}

QuilParser::ExchangeContext* QuilParser::exchange() {
  ExchangeContext *_localctx = _tracker.createInstance<ExchangeContext>(_ctx, getState());
  enterRule(_localctx, 82, QuilParser::RuleExchange);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(450);
    match(QuilParser::EXCHANGE);
    setState(451);
    addr();
    setState(452);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConvertContext ------------------------------------------------------------------

QuilParser::ConvertContext::ConvertContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::ConvertContext::CONVERT() {
  return getToken(QuilParser::CONVERT, 0);
}

std::vector<QuilParser::AddrContext *> QuilParser::ConvertContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::ConvertContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}


size_t QuilParser::ConvertContext::getRuleIndex() const {
  return QuilParser::RuleConvert;
}

void QuilParser::ConvertContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConvert(this);
}

void QuilParser::ConvertContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConvert(this);
}

QuilParser::ConvertContext* QuilParser::convert() {
  ConvertContext *_localctx = _tracker.createInstance<ConvertContext>(_ctx, getState());
  enterRule(_localctx, 84, QuilParser::RuleConvert);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(454);
    match(QuilParser::CONVERT);
    setState(455);
    addr();
    setState(456);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LoadContext ------------------------------------------------------------------

QuilParser::LoadContext::LoadContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::LoadContext::LOAD() {
  return getToken(QuilParser::LOAD, 0);
}

std::vector<QuilParser::AddrContext *> QuilParser::LoadContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::LoadContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

tree::TerminalNode* QuilParser::LoadContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}


size_t QuilParser::LoadContext::getRuleIndex() const {
  return QuilParser::RuleLoad;
}

void QuilParser::LoadContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLoad(this);
}

void QuilParser::LoadContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLoad(this);
}

QuilParser::LoadContext* QuilParser::load() {
  LoadContext *_localctx = _tracker.createInstance<LoadContext>(_ctx, getState());
  enterRule(_localctx, 86, QuilParser::RuleLoad);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(458);
    match(QuilParser::LOAD);
    setState(459);
    addr();
    setState(460);
    match(QuilParser::IDENTIFIER);
    setState(461);
    addr();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StoreContext ------------------------------------------------------------------

QuilParser::StoreContext::StoreContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::StoreContext::STORE() {
  return getToken(QuilParser::STORE, 0);
}

tree::TerminalNode* QuilParser::StoreContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

std::vector<QuilParser::AddrContext *> QuilParser::StoreContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::StoreContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

QuilParser::NumberContext* QuilParser::StoreContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}


size_t QuilParser::StoreContext::getRuleIndex() const {
  return QuilParser::RuleStore;
}

void QuilParser::StoreContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStore(this);
}

void QuilParser::StoreContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStore(this);
}

QuilParser::StoreContext* QuilParser::store() {
  StoreContext *_localctx = _tracker.createInstance<StoreContext>(_ctx, getState());
  enterRule(_localctx, 88, QuilParser::RuleStore);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(463);
    match(QuilParser::STORE);
    setState(464);
    match(QuilParser::IDENTIFIER);
    setState(465);
    addr();
    setState(468);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::IDENTIFIER:
      case QuilParser::LBRACKET: {
        setState(466);
        addr();
        break;
      }

      case QuilParser::PI:
      case QuilParser::I:
      case QuilParser::MINUS:
      case QuilParser::INT:
      case QuilParser::FLOAT: {
        setState(467);
        number();
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

//----------------- ClassicalComparisonContext ------------------------------------------------------------------

QuilParser::ClassicalComparisonContext::ClassicalComparisonContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<QuilParser::AddrContext *> QuilParser::ClassicalComparisonContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext* QuilParser::ClassicalComparisonContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

tree::TerminalNode* QuilParser::ClassicalComparisonContext::EQ() {
  return getToken(QuilParser::EQ, 0);
}

tree::TerminalNode* QuilParser::ClassicalComparisonContext::GT() {
  return getToken(QuilParser::GT, 0);
}

tree::TerminalNode* QuilParser::ClassicalComparisonContext::GE() {
  return getToken(QuilParser::GE, 0);
}

tree::TerminalNode* QuilParser::ClassicalComparisonContext::LT() {
  return getToken(QuilParser::LT, 0);
}

tree::TerminalNode* QuilParser::ClassicalComparisonContext::LE() {
  return getToken(QuilParser::LE, 0);
}

QuilParser::NumberContext* QuilParser::ClassicalComparisonContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}


size_t QuilParser::ClassicalComparisonContext::getRuleIndex() const {
  return QuilParser::RuleClassicalComparison;
}

void QuilParser::ClassicalComparisonContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalComparison(this);
}

void QuilParser::ClassicalComparisonContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalComparison(this);
}

QuilParser::ClassicalComparisonContext* QuilParser::classicalComparison() {
  ClassicalComparisonContext *_localctx = _tracker.createInstance<ClassicalComparisonContext>(_ctx, getState());
  enterRule(_localctx, 90, QuilParser::RuleClassicalComparison);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(470);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::EQ)
      | (1ULL << QuilParser::GT)
      | (1ULL << QuilParser::GE)
      | (1ULL << QuilParser::LT)
      | (1ULL << QuilParser::LE))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(471);
    addr();
    setState(472);
    addr();
    setState(475);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case QuilParser::IDENTIFIER:
      case QuilParser::LBRACKET: {
        setState(473);
        addr();
        break;
      }

      case QuilParser::PI:
      case QuilParser::I:
      case QuilParser::MINUS:
      case QuilParser::INT:
      case QuilParser::FLOAT: {
        setState(474);
        number();
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

//----------------- NopContext ------------------------------------------------------------------

QuilParser::NopContext::NopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::NopContext::NOP() {
  return getToken(QuilParser::NOP, 0);
}


size_t QuilParser::NopContext::getRuleIndex() const {
  return QuilParser::RuleNop;
}

void QuilParser::NopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNop(this);
}

void QuilParser::NopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNop(this);
}

QuilParser::NopContext* QuilParser::nop() {
  NopContext *_localctx = _tracker.createInstance<NopContext>(_ctx, getState());
  enterRule(_localctx, 92, QuilParser::RuleNop);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(477);
    match(QuilParser::NOP);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext ------------------------------------------------------------------

QuilParser::IncludeContext::IncludeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::IncludeContext::INCLUDE() {
  return getToken(QuilParser::INCLUDE, 0);
}

tree::TerminalNode* QuilParser::IncludeContext::STRING() {
  return getToken(QuilParser::STRING, 0);
}


size_t QuilParser::IncludeContext::getRuleIndex() const {
  return QuilParser::RuleInclude;
}

void QuilParser::IncludeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInclude(this);
}

void QuilParser::IncludeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInclude(this);
}

QuilParser::IncludeContext* QuilParser::include() {
  IncludeContext *_localctx = _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 94, QuilParser::RuleInclude);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(479);
    match(QuilParser::INCLUDE);
    setState(480);
    match(QuilParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PragmaContext ------------------------------------------------------------------

QuilParser::PragmaContext::PragmaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::PragmaContext::PRAGMA() {
  return getToken(QuilParser::PRAGMA, 0);
}

tree::TerminalNode* QuilParser::PragmaContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

std::vector<QuilParser::Pragma_nameContext *> QuilParser::PragmaContext::pragma_name() {
  return getRuleContexts<QuilParser::Pragma_nameContext>();
}

QuilParser::Pragma_nameContext* QuilParser::PragmaContext::pragma_name(size_t i) {
  return getRuleContext<QuilParser::Pragma_nameContext>(i);
}

tree::TerminalNode* QuilParser::PragmaContext::STRING() {
  return getToken(QuilParser::STRING, 0);
}


size_t QuilParser::PragmaContext::getRuleIndex() const {
  return QuilParser::RulePragma;
}

void QuilParser::PragmaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma(this);
}

void QuilParser::PragmaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma(this);
}

QuilParser::PragmaContext* QuilParser::pragma() {
  PragmaContext *_localctx = _tracker.createInstance<PragmaContext>(_ctx, getState());
  enterRule(_localctx, 96, QuilParser::RulePragma);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(482);
    match(QuilParser::PRAGMA);
    setState(483);
    match(QuilParser::IDENTIFIER);
    setState(487);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::IDENTIFIER

    || _la == QuilParser::INT) {
      setState(484);
      pragma_name();
      setState(489);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(491);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::STRING) {
      setState(490);
      match(QuilParser::STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pragma_nameContext ------------------------------------------------------------------

QuilParser::Pragma_nameContext::Pragma_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::Pragma_nameContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

tree::TerminalNode* QuilParser::Pragma_nameContext::INT() {
  return getToken(QuilParser::INT, 0);
}


size_t QuilParser::Pragma_nameContext::getRuleIndex() const {
  return QuilParser::RulePragma_name;
}

void QuilParser::Pragma_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma_name(this);
}

void QuilParser::Pragma_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma_name(this);
}

QuilParser::Pragma_nameContext* QuilParser::pragma_name() {
  Pragma_nameContext *_localctx = _tracker.createInstance<Pragma_nameContext>(_ctx, getState());
  enterRule(_localctx, 98, QuilParser::RulePragma_name);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(493);
    _la = _input->LA(1);
    if (!(_la == QuilParser::IDENTIFIER

    || _la == QuilParser::INT)) {
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

//----------------- ExpressionContext ------------------------------------------------------------------

QuilParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t QuilParser::ExpressionContext::getRuleIndex() const {
  return QuilParser::RuleExpression;
}

void QuilParser::ExpressionContext::copyFrom(ExpressionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- NumberExpContext ------------------------------------------------------------------

QuilParser::NumberContext* QuilParser::NumberExpContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}

QuilParser::NumberExpContext::NumberExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::NumberExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumberExp(this);
}
void QuilParser::NumberExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumberExp(this);
}
//----------------- PowerExpContext ------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *> QuilParser::PowerExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext* QuilParser::PowerExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode* QuilParser::PowerExpContext::POWER() {
  return getToken(QuilParser::POWER, 0);
}

QuilParser::PowerExpContext::PowerExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::PowerExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPowerExp(this);
}
void QuilParser::PowerExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPowerExp(this);
}
//----------------- MulDivExpContext ------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *> QuilParser::MulDivExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext* QuilParser::MulDivExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode* QuilParser::MulDivExpContext::TIMES() {
  return getToken(QuilParser::TIMES, 0);
}

tree::TerminalNode* QuilParser::MulDivExpContext::DIVIDE() {
  return getToken(QuilParser::DIVIDE, 0);
}

QuilParser::MulDivExpContext::MulDivExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::MulDivExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMulDivExp(this);
}
void QuilParser::MulDivExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMulDivExp(this);
}
//----------------- ParenthesisExpContext ------------------------------------------------------------------

tree::TerminalNode* QuilParser::ParenthesisExpContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

QuilParser::ExpressionContext* QuilParser::ParenthesisExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

tree::TerminalNode* QuilParser::ParenthesisExpContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

QuilParser::ParenthesisExpContext::ParenthesisExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::ParenthesisExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenthesisExp(this);
}
void QuilParser::ParenthesisExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParenthesisExp(this);
}
//----------------- VariableExpContext ------------------------------------------------------------------

QuilParser::VariableContext* QuilParser::VariableExpContext::variable() {
  return getRuleContext<QuilParser::VariableContext>(0);
}

QuilParser::VariableExpContext::VariableExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::VariableExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableExp(this);
}
void QuilParser::VariableExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableExp(this);
}
//----------------- AddrExpContext ------------------------------------------------------------------

QuilParser::AddrContext* QuilParser::AddrExpContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}

QuilParser::AddrExpContext::AddrExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::AddrExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddrExp(this);
}
void QuilParser::AddrExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddrExp(this);
}
//----------------- SignedExpContext ------------------------------------------------------------------

QuilParser::SignContext* QuilParser::SignedExpContext::sign() {
  return getRuleContext<QuilParser::SignContext>(0);
}

QuilParser::ExpressionContext* QuilParser::SignedExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

QuilParser::SignedExpContext::SignedExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::SignedExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSignedExp(this);
}
void QuilParser::SignedExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSignedExp(this);
}
//----------------- AddSubExpContext ------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *> QuilParser::AddSubExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext* QuilParser::AddSubExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode* QuilParser::AddSubExpContext::PLUS() {
  return getToken(QuilParser::PLUS, 0);
}

tree::TerminalNode* QuilParser::AddSubExpContext::MINUS() {
  return getToken(QuilParser::MINUS, 0);
}

QuilParser::AddSubExpContext::AddSubExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::AddSubExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddSubExp(this);
}
void QuilParser::AddSubExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddSubExp(this);
}
//----------------- FunctionExpContext ------------------------------------------------------------------

QuilParser::FunctionContext* QuilParser::FunctionExpContext::function() {
  return getRuleContext<QuilParser::FunctionContext>(0);
}

tree::TerminalNode* QuilParser::FunctionExpContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

QuilParser::ExpressionContext* QuilParser::FunctionExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

tree::TerminalNode* QuilParser::FunctionExpContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

QuilParser::FunctionExpContext::FunctionExpContext(ExpressionContext *ctx) { copyFrom(ctx); }

void QuilParser::FunctionExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExp(this);
}
void QuilParser::FunctionExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExp(this);
}

QuilParser::ExpressionContext* QuilParser::expression() {
   return expression(0);
}

QuilParser::ExpressionContext* QuilParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  QuilParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  QuilParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 100;
  enterRecursionRule(_localctx, 100, QuilParser::RuleExpression, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(511);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 45, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<ParenthesisExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(496);
      match(QuilParser::LPAREN);
      setState(497);
      expression(0);
      setState(498);
      match(QuilParser::RPAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<SignedExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(500);
      sign();
      setState(501);
      expression(8);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<FunctionExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(503);
      function();
      setState(504);
      match(QuilParser::LPAREN);
      setState(505);
      expression(0);
      setState(506);
      match(QuilParser::RPAREN);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<NumberExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(508);
      number();
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<VariableExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(509);
      variable();
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<AddrExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(510);
      addr();
      break;
    }

    default:
      break;
    }
    _ctx->stop = _input->LT(-1);
    setState(524);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(522);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<PowerExpContext>(_tracker.createInstance<ExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(513);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(514);
          match(QuilParser::POWER);
          setState(515);
          expression(7);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<MulDivExpContext>(_tracker.createInstance<ExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(516);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(517);
          _la = _input->LA(1);
          if (!(_la == QuilParser::TIMES

          || _la == QuilParser::DIVIDE)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(518);
          expression(7);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AddSubExpContext>(_tracker.createInstance<ExpressionContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(519);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(520);
          _la = _input->LA(1);
          if (!(_la == QuilParser::PLUS

          || _la == QuilParser::MINUS)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(521);
          expression(6);
          break;
        }

        default:
          break;
        } 
      }
      setState(526);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- FunctionContext ------------------------------------------------------------------

QuilParser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::FunctionContext::SIN() {
  return getToken(QuilParser::SIN, 0);
}

tree::TerminalNode* QuilParser::FunctionContext::COS() {
  return getToken(QuilParser::COS, 0);
}

tree::TerminalNode* QuilParser::FunctionContext::SQRT() {
  return getToken(QuilParser::SQRT, 0);
}

tree::TerminalNode* QuilParser::FunctionContext::EXP() {
  return getToken(QuilParser::EXP, 0);
}

tree::TerminalNode* QuilParser::FunctionContext::CIS() {
  return getToken(QuilParser::CIS, 0);
}


size_t QuilParser::FunctionContext::getRuleIndex() const {
  return QuilParser::RuleFunction;
}

void QuilParser::FunctionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunction(this);
}

void QuilParser::FunctionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunction(this);
}

QuilParser::FunctionContext* QuilParser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 102, QuilParser::RuleFunction);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(527);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << QuilParser::SIN)
      | (1ULL << QuilParser::COS)
      | (1ULL << QuilParser::SQRT)
      | (1ULL << QuilParser::EXP)
      | (1ULL << QuilParser::CIS))) != 0))) {
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

//----------------- SignContext ------------------------------------------------------------------

QuilParser::SignContext::SignContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::SignContext::PLUS() {
  return getToken(QuilParser::PLUS, 0);
}

tree::TerminalNode* QuilParser::SignContext::MINUS() {
  return getToken(QuilParser::MINUS, 0);
}


size_t QuilParser::SignContext::getRuleIndex() const {
  return QuilParser::RuleSign;
}

void QuilParser::SignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSign(this);
}

void QuilParser::SignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSign(this);
}

QuilParser::SignContext* QuilParser::sign() {
  SignContext *_localctx = _tracker.createInstance<SignContext>(_ctx, getState());
  enterRule(_localctx, 104, QuilParser::RuleSign);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(529);
    _la = _input->LA(1);
    if (!(_la == QuilParser::PLUS

    || _la == QuilParser::MINUS)) {
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

//----------------- NumberContext ------------------------------------------------------------------

QuilParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::RealNContext* QuilParser::NumberContext::realN() {
  return getRuleContext<QuilParser::RealNContext>(0);
}

QuilParser::ImaginaryNContext* QuilParser::NumberContext::imaginaryN() {
  return getRuleContext<QuilParser::ImaginaryNContext>(0);
}

tree::TerminalNode* QuilParser::NumberContext::I() {
  return getToken(QuilParser::I, 0);
}

tree::TerminalNode* QuilParser::NumberContext::PI() {
  return getToken(QuilParser::PI, 0);
}

tree::TerminalNode* QuilParser::NumberContext::MINUS() {
  return getToken(QuilParser::MINUS, 0);
}


size_t QuilParser::NumberContext::getRuleIndex() const {
  return QuilParser::RuleNumber;
}

void QuilParser::NumberContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumber(this);
}

void QuilParser::NumberContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumber(this);
}

QuilParser::NumberContext* QuilParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 106, QuilParser::RuleNumber);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(532);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::MINUS) {
      setState(531);
      match(QuilParser::MINUS);
    }
    setState(538);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
    case 1: {
      setState(534);
      realN();
      break;
    }

    case 2: {
      setState(535);
      imaginaryN();
      break;
    }

    case 3: {
      setState(536);
      match(QuilParser::I);
      break;
    }

    case 4: {
      setState(537);
      match(QuilParser::PI);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImaginaryNContext ------------------------------------------------------------------

QuilParser::ImaginaryNContext::ImaginaryNContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

QuilParser::RealNContext* QuilParser::ImaginaryNContext::realN() {
  return getRuleContext<QuilParser::RealNContext>(0);
}

tree::TerminalNode* QuilParser::ImaginaryNContext::I() {
  return getToken(QuilParser::I, 0);
}


size_t QuilParser::ImaginaryNContext::getRuleIndex() const {
  return QuilParser::RuleImaginaryN;
}

void QuilParser::ImaginaryNContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImaginaryN(this);
}

void QuilParser::ImaginaryNContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImaginaryN(this);
}

QuilParser::ImaginaryNContext* QuilParser::imaginaryN() {
  ImaginaryNContext *_localctx = _tracker.createInstance<ImaginaryNContext>(_ctx, getState());
  enterRule(_localctx, 108, QuilParser::RuleImaginaryN);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(540);
    realN();
    setState(541);
    match(QuilParser::I);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealNContext ------------------------------------------------------------------

QuilParser::RealNContext::RealNContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* QuilParser::RealNContext::FLOAT() {
  return getToken(QuilParser::FLOAT, 0);
}

tree::TerminalNode* QuilParser::RealNContext::INT() {
  return getToken(QuilParser::INT, 0);
}


size_t QuilParser::RealNContext::getRuleIndex() const {
  return QuilParser::RuleRealN;
}

void QuilParser::RealNContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRealN(this);
}

void QuilParser::RealNContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRealN(this);
}

QuilParser::RealNContext* QuilParser::realN() {
  RealNContext *_localctx = _tracker.createInstance<RealNContext>(_ctx, getState());
  enterRule(_localctx, 110, QuilParser::RuleRealN);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(543);
    _la = _input->LA(1);
    if (!(_la == QuilParser::INT

    || _la == QuilParser::FLOAT)) {
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

bool QuilParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 50: return expressionSempred(dynamic_cast<ExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool QuilParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 7);
    case 1: return precpred(_ctx, 6);
    case 2: return precpred(_ctx, 5);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> QuilParser::_decisionToDFA;
atn::PredictionContextCache QuilParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN QuilParser::_atn;
std::vector<uint16_t> QuilParser::_serializedATN;

std::vector<std::string> QuilParser::_ruleNames = {
  "quil", "allInstr", "instr", "gate", "name", "qubit", "param", "modifier", 
  "defGate", "defGateAsPauli", "variable", "gatetype", "matrix", "matrixRow", 
  "pauliTerms", "pauliTerm", "defCircuit", "qubitVariable", "circuitQubit", 
  "circuitGate", "circuitMeasure", "circuitResetState", "circuitInstr", 
  "circuit", "measure", "addr", "defLabel", "label", "halt", "jump", "jumpWhen", 
  "jumpUnless", "resetState", "wait", "memoryDescriptor", "offsetDescriptor", 
  "classicalUnary", "classicalBinary", "logicalBinaryOp", "arithmeticBinaryOp", 
  "move", "exchange", "convert", "load", "store", "classicalComparison", 
  "nop", "include", "pragma", "pragma_name", "expression", "function", "sign", 
  "number", "imaginaryN", "realN"
};

std::vector<std::string> QuilParser::_literalNames = {
  "", "'DEFGATE'", "'DEFCIRCUIT'", "'MEASURE'", "'LABEL'", "'HALT'", "'JUMP'", 
  "'JUMP-WHEN'", "'JUMP-UNLESS'", "'RESET'", "'WAIT'", "'NOP'", "'INCLUDE'", 
  "'PRAGMA'", "'DECLARE'", "'SHARING'", "'OFFSET'", "'AS'", "'MATRIX'", 
  "'PERMUTATION'", "'PAULI-SUM'", "'NEG'", "'NOT'", "'TRUE'", "'FALSE'", 
  "'AND'", "'IOR'", "'XOR'", "'OR'", "'ADD'", "'SUB'", "'MUL'", "'DIV'", 
  "'MOVE'", "'EXCHANGE'", "'CONVERT'", "'EQ'", "'GT'", "'GE'", "'LT'", "'LE'", 
  "'LOAD'", "'STORE'", "'pi'", "'i'", "'SIN'", "'COS'", "'SQRT'", "'EXP'", 
  "'CIS'", "'+'", "'-'", "'*'", "'/'", "'^'", "'CONTROLLED'", "'DAGGER'", 
  "'FORKED'", "", "", "", "", "'.'", "','", "'('", "')'", "'['", "']'", 
  "':'", "'%'", "'@'", "'\"'", "'_'", "'    '", "", "", "' '"
};

std::vector<std::string> QuilParser::_symbolicNames = {
  "", "DEFGATE", "DEFCIRCUIT", "MEASURE", "LABEL", "HALT", "JUMP", "JUMPWHEN", 
  "JUMPUNLESS", "RESET", "WAIT", "NOP", "INCLUDE", "PRAGMA", "DECLARE", 
  "SHARING", "OFFSET", "AS", "MATRIX", "PERMUTATION", "PAULISUM", "NEG", 
  "NOT", "TRUE", "FALSE", "AND", "IOR", "XOR", "OR", "ADD", "SUB", "MUL", 
  "DIV", "MOVE", "EXCHANGE", "CONVERT", "EQ", "GT", "GE", "LT", "LE", "LOAD", 
  "STORE", "PI", "I", "SIN", "COS", "SQRT", "EXP", "CIS", "PLUS", "MINUS", 
  "TIMES", "DIVIDE", "POWER", "CONTROLLED", "DAGGER", "FORKED", "IDENTIFIER", 
  "INT", "FLOAT", "STRING", "PERIOD", "COMMA", "LPAREN", "RPAREN", "LBRACKET", 
  "RBRACKET", "COLON", "PERCENTAGE", "AT", "QUOTE", "UNDERSCORE", "TAB", 
  "NEWLINE", "COMMENT", "SPACE", "INVALID"
};

dfa::Vocabulary QuilParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> QuilParser::_tokenNames;

QuilParser::Initializer::Initializer() {
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
    0x3, 0x4f, 0x224, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 
    0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 
    0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 
    0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 
    0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x4, 0x18, 0x9, 
    0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 
    0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 
    0x1f, 0x9, 0x1f, 0x4, 0x20, 0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 
    0x9, 0x22, 0x4, 0x23, 0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 
    0x25, 0x4, 0x26, 0x9, 0x26, 0x4, 0x27, 0x9, 0x27, 0x4, 0x28, 0x9, 0x28, 
    0x4, 0x29, 0x9, 0x29, 0x4, 0x2a, 0x9, 0x2a, 0x4, 0x2b, 0x9, 0x2b, 0x4, 
    0x2c, 0x9, 0x2c, 0x4, 0x2d, 0x9, 0x2d, 0x4, 0x2e, 0x9, 0x2e, 0x4, 0x2f, 
    0x9, 0x2f, 0x4, 0x30, 0x9, 0x30, 0x4, 0x31, 0x9, 0x31, 0x4, 0x32, 0x9, 
    0x32, 0x4, 0x33, 0x9, 0x33, 0x4, 0x34, 0x9, 0x34, 0x4, 0x35, 0x9, 0x35, 
    0x4, 0x36, 0x9, 0x36, 0x4, 0x37, 0x9, 0x37, 0x4, 0x38, 0x9, 0x38, 0x4, 
    0x39, 0x9, 0x39, 0x3, 0x2, 0x5, 0x2, 0x74, 0xa, 0x2, 0x3, 0x2, 0x6, 
    0x2, 0x77, 0xa, 0x2, 0xd, 0x2, 0xe, 0x2, 0x78, 0x3, 0x2, 0x7, 0x2, 0x7c, 
    0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x7f, 0xb, 0x2, 0x3, 0x2, 0x7, 0x2, 0x82, 
    0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x85, 0xb, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x8d, 0xa, 0x3, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0xa1, 0xa, 0x4, 0x3, 0x5, 0x7, 
    0x5, 0xa4, 0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0xa7, 0xb, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x7, 0x5, 0xae, 0xa, 0x5, 0xc, 0x5, 
    0xe, 0x5, 0xb1, 0xb, 0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0xb5, 0xa, 0x5, 
    0x3, 0x5, 0x6, 0x5, 0xb8, 0xa, 0x5, 0xd, 0x5, 0xe, 0x5, 0xb9, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 
    0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x7, 0xa, 
    0xca, 0xa, 0xa, 0xc, 0xa, 0xe, 0xa, 0xcd, 0xb, 0xa, 0x3, 0xa, 0x3, 0xa, 
    0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 0xd3, 0xa, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x7, 0xb, 0xdf, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xe2, 0xb, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x5, 0xb, 0xe6, 0xa, 0xb, 0x3, 0xb, 0x6, 0xb, 0xe9, 0xa, 
    0xb, 0xd, 0xb, 0xe, 0xb, 0xea, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xd, 0x3, 0xd, 
    0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x7, 0xe, 0xfb, 0xa, 0xe, 0xc, 0xe, 0xe, 
    0xe, 0xfe, 0xb, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x7, 0xf, 0x106, 0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 0x109, 0xb, 
    0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0x10e, 0xa, 0x10, 0xc, 
    0x10, 0xe, 0x10, 0x111, 0xb, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x6, 0x11, 0x11b, 
    0xa, 0x11, 0xd, 0x11, 0xe, 0x11, 0x11c, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 
    0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x7, 0x12, 0x125, 0xa, 0x12, 0xc, 0x12, 
    0xe, 0x12, 0x128, 0xb, 0x12, 0x3, 0x12, 0x3, 0x12, 0x5, 0x12, 0x12c, 
    0xa, 0x12, 0x3, 0x12, 0x7, 0x12, 0x12f, 0xa, 0x12, 0xc, 0x12, 0xe, 0x12, 
    0x132, 0xb, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x13c, 0xa, 0x14, 0x3, 0x15, 
    0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x7, 0x15, 0x143, 0xa, 0x15, 
    0xc, 0x15, 0xe, 0x15, 0x146, 0xb, 0x15, 0x3, 0x15, 0x3, 0x15, 0x5, 0x15, 
    0x14a, 0xa, 0x15, 0x3, 0x15, 0x6, 0x15, 0x14d, 0xa, 0x15, 0xd, 0x15, 
    0xe, 0x15, 0x14e, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x5, 0x16, 0x154, 
    0xa, 0x16, 0x3, 0x17, 0x3, 0x17, 0x5, 0x17, 0x158, 0xa, 0x17, 0x3, 0x18, 
    0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x5, 0x18, 0x15e, 0xa, 0x18, 0x3, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x7, 0x19, 0x164, 0xa, 0x19, 0xc, 0x19, 
    0xe, 0x19, 0x167, 0xb, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x1a, 
    0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x16f, 0xa, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 
    0x5, 0x1b, 0x173, 0xa, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x5, 0x1b, 
    0x178, 0xa, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 
    0x3, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 
    0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
    0x3, 0x21, 0x3, 0x22, 0x3, 0x22, 0x5, 0x22, 0x18f, 0xa, 0x22, 0x3, 0x23, 
    0x3, 0x23, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 
    0x24, 0x5, 0x24, 0x199, 0xa, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 
    0x7, 0x24, 0x19e, 0xa, 0x24, 0xc, 0x24, 0xe, 0x24, 0x1a1, 0xb, 0x24, 
    0x5, 0x24, 0x1a3, 0xa, 0x24, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 
    0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x3, 0x27, 0x3, 0x27, 0x3, 0x27, 0x3, 
    0x27, 0x3, 0x27, 0x5, 0x27, 0x1b1, 0xa, 0x27, 0x3, 0x28, 0x3, 0x28, 
    0x3, 0x28, 0x3, 0x28, 0x5, 0x28, 0x1b7, 0xa, 0x28, 0x3, 0x29, 0x3, 0x29, 
    0x3, 0x29, 0x3, 0x29, 0x5, 0x29, 0x1bd, 0xa, 0x29, 0x3, 0x2a, 0x3, 0x2a, 
    0x3, 0x2a, 0x3, 0x2a, 0x5, 0x2a, 0x1c3, 0xa, 0x2a, 0x3, 0x2b, 0x3, 0x2b, 
    0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 0x3, 
    0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2e, 0x3, 0x2e, 
    0x3, 0x2e, 0x3, 0x2e, 0x3, 0x2e, 0x5, 0x2e, 0x1d7, 0xa, 0x2e, 0x3, 0x2f, 
    0x3, 0x2f, 0x3, 0x2f, 0x3, 0x2f, 0x3, 0x2f, 0x5, 0x2f, 0x1de, 0xa, 0x2f, 
    0x3, 0x30, 0x3, 0x30, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x3, 0x32, 0x3, 
    0x32, 0x3, 0x32, 0x7, 0x32, 0x1e8, 0xa, 0x32, 0xc, 0x32, 0xe, 0x32, 
    0x1eb, 0xb, 0x32, 0x3, 0x32, 0x5, 0x32, 0x1ee, 0xa, 0x32, 0x3, 0x33, 
    0x3, 0x33, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 
    0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 
    0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x5, 0x34, 0x202, 0xa, 0x34, 
    0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 
    0x34, 0x3, 0x34, 0x3, 0x34, 0x7, 0x34, 0x20d, 0xa, 0x34, 0xc, 0x34, 
    0xe, 0x34, 0x210, 0xb, 0x34, 0x3, 0x35, 0x3, 0x35, 0x3, 0x36, 0x3, 0x36, 
    0x3, 0x37, 0x5, 0x37, 0x217, 0xa, 0x37, 0x3, 0x37, 0x3, 0x37, 0x3, 0x37, 
    0x3, 0x37, 0x5, 0x37, 0x21d, 0xa, 0x37, 0x3, 0x38, 0x3, 0x38, 0x3, 0x38, 
    0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x2, 0x3, 0x66, 0x3a, 0x2, 0x4, 0x6, 
    0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 
    0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 
    0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 
    0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 
    0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x2, 0xd, 0x3, 0x2, 0x39, 0x3b, 0x3, 0x2, 
    0x14, 0x15, 0x3, 0x2, 0x17, 0x1a, 0x3, 0x2, 0x1b, 0x1e, 0x3, 0x2, 0x1f, 
    0x22, 0x3, 0x2, 0x26, 0x2a, 0x3, 0x2, 0x3c, 0x3d, 0x3, 0x2, 0x36, 0x37, 
    0x3, 0x2, 0x34, 0x35, 0x3, 0x2, 0x2f, 0x33, 0x3, 0x2, 0x3d, 0x3e, 0x2, 
    0x23c, 0x2, 0x73, 0x3, 0x2, 0x2, 0x2, 0x4, 0x8c, 0x3, 0x2, 0x2, 0x2, 
    0x6, 0xa0, 0x3, 0x2, 0x2, 0x2, 0x8, 0xa5, 0x3, 0x2, 0x2, 0x2, 0xa, 0xbb, 
    0x3, 0x2, 0x2, 0x2, 0xc, 0xbd, 0x3, 0x2, 0x2, 0x2, 0xe, 0xbf, 0x3, 0x2, 
    0x2, 0x2, 0x10, 0xc1, 0x3, 0x2, 0x2, 0x2, 0x12, 0xc3, 0x3, 0x2, 0x2, 
    0x2, 0x14, 0xd8, 0x3, 0x2, 0x2, 0x2, 0x16, 0xf2, 0x3, 0x2, 0x2, 0x2, 
    0x18, 0xf5, 0x3, 0x2, 0x2, 0x2, 0x1a, 0xfc, 0x3, 0x2, 0x2, 0x2, 0x1c, 
    0x101, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x10f, 0x3, 0x2, 0x2, 0x2, 0x20, 0x114, 
    0x3, 0x2, 0x2, 0x2, 0x22, 0x11e, 0x3, 0x2, 0x2, 0x2, 0x24, 0x137, 0x3, 
    0x2, 0x2, 0x2, 0x26, 0x13b, 0x3, 0x2, 0x2, 0x2, 0x28, 0x13d, 0x3, 0x2, 
    0x2, 0x2, 0x2a, 0x150, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x155, 0x3, 0x2, 0x2, 
    0x2, 0x2e, 0x15d, 0x3, 0x2, 0x2, 0x2, 0x30, 0x165, 0x3, 0x2, 0x2, 0x2, 
    0x32, 0x16b, 0x3, 0x2, 0x2, 0x2, 0x34, 0x177, 0x3, 0x2, 0x2, 0x2, 0x36, 
    0x179, 0x3, 0x2, 0x2, 0x2, 0x38, 0x17c, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x17f, 
    0x3, 0x2, 0x2, 0x2, 0x3c, 0x181, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x184, 0x3, 
    0x2, 0x2, 0x2, 0x40, 0x188, 0x3, 0x2, 0x2, 0x2, 0x42, 0x18c, 0x3, 0x2, 
    0x2, 0x2, 0x44, 0x190, 0x3, 0x2, 0x2, 0x2, 0x46, 0x192, 0x3, 0x2, 0x2, 
    0x2, 0x48, 0x1a4, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x1a8, 0x3, 0x2, 0x2, 0x2, 
    0x4c, 0x1b0, 0x3, 0x2, 0x2, 0x2, 0x4e, 0x1b2, 0x3, 0x2, 0x2, 0x2, 0x50, 
    0x1b8, 0x3, 0x2, 0x2, 0x2, 0x52, 0x1be, 0x3, 0x2, 0x2, 0x2, 0x54, 0x1c4, 
    0x3, 0x2, 0x2, 0x2, 0x56, 0x1c8, 0x3, 0x2, 0x2, 0x2, 0x58, 0x1cc, 0x3, 
    0x2, 0x2, 0x2, 0x5a, 0x1d1, 0x3, 0x2, 0x2, 0x2, 0x5c, 0x1d8, 0x3, 0x2, 
    0x2, 0x2, 0x5e, 0x1df, 0x3, 0x2, 0x2, 0x2, 0x60, 0x1e1, 0x3, 0x2, 0x2, 
    0x2, 0x62, 0x1e4, 0x3, 0x2, 0x2, 0x2, 0x64, 0x1ef, 0x3, 0x2, 0x2, 0x2, 
    0x66, 0x201, 0x3, 0x2, 0x2, 0x2, 0x68, 0x211, 0x3, 0x2, 0x2, 0x2, 0x6a, 
    0x213, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x216, 0x3, 0x2, 0x2, 0x2, 0x6e, 0x21e, 
    0x3, 0x2, 0x2, 0x2, 0x70, 0x221, 0x3, 0x2, 0x2, 0x2, 0x72, 0x74, 0x5, 
    0x4, 0x3, 0x2, 0x73, 0x72, 0x3, 0x2, 0x2, 0x2, 0x73, 0x74, 0x3, 0x2, 
    0x2, 0x2, 0x74, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x75, 0x77, 0x7, 0x4c, 0x2, 
    0x2, 0x76, 0x75, 0x3, 0x2, 0x2, 0x2, 0x77, 0x78, 0x3, 0x2, 0x2, 0x2, 
    0x78, 0x76, 0x3, 0x2, 0x2, 0x2, 0x78, 0x79, 0x3, 0x2, 0x2, 0x2, 0x79, 
    0x7a, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x7c, 0x5, 0x4, 0x3, 0x2, 0x7b, 0x76, 
    0x3, 0x2, 0x2, 0x2, 0x7c, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7b, 0x3, 
    0x2, 0x2, 0x2, 0x7d, 0x7e, 0x3, 0x2, 0x2, 0x2, 0x7e, 0x83, 0x3, 0x2, 
    0x2, 0x2, 0x7f, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x80, 0x82, 0x7, 0x4c, 0x2, 
    0x2, 0x81, 0x80, 0x3, 0x2, 0x2, 0x2, 0x82, 0x85, 0x3, 0x2, 0x2, 0x2, 
    0x83, 0x81, 0x3, 0x2, 0x2, 0x2, 0x83, 0x84, 0x3, 0x2, 0x2, 0x2, 0x84, 
    0x86, 0x3, 0x2, 0x2, 0x2, 0x85, 0x83, 0x3, 0x2, 0x2, 0x2, 0x86, 0x87, 
    0x7, 0x2, 0x2, 0x3, 0x87, 0x3, 0x3, 0x2, 0x2, 0x2, 0x88, 0x8d, 0x5, 
    0x12, 0xa, 0x2, 0x89, 0x8d, 0x5, 0x14, 0xb, 0x2, 0x8a, 0x8d, 0x5, 0x22, 
    0x12, 0x2, 0x8b, 0x8d, 0x5, 0x6, 0x4, 0x2, 0x8c, 0x88, 0x3, 0x2, 0x2, 
    0x2, 0x8c, 0x89, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8a, 0x3, 0x2, 0x2, 0x2, 
    0x8c, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x8d, 0x5, 0x3, 0x2, 0x2, 0x2, 0x8e, 
    0xa1, 0x5, 0x8, 0x5, 0x2, 0x8f, 0xa1, 0x5, 0x32, 0x1a, 0x2, 0x90, 0xa1, 
    0x5, 0x36, 0x1c, 0x2, 0x91, 0xa1, 0x5, 0x3a, 0x1e, 0x2, 0x92, 0xa1, 
    0x5, 0x3c, 0x1f, 0x2, 0x93, 0xa1, 0x5, 0x3e, 0x20, 0x2, 0x94, 0xa1, 
    0x5, 0x40, 0x21, 0x2, 0x95, 0xa1, 0x5, 0x42, 0x22, 0x2, 0x96, 0xa1, 
    0x5, 0x44, 0x23, 0x2, 0x97, 0xa1, 0x5, 0x4a, 0x26, 0x2, 0x98, 0xa1, 
    0x5, 0x4c, 0x27, 0x2, 0x99, 0xa1, 0x5, 0x5c, 0x2f, 0x2, 0x9a, 0xa1, 
    0x5, 0x58, 0x2d, 0x2, 0x9b, 0xa1, 0x5, 0x5a, 0x2e, 0x2, 0x9c, 0xa1, 
    0x5, 0x5e, 0x30, 0x2, 0x9d, 0xa1, 0x5, 0x60, 0x31, 0x2, 0x9e, 0xa1, 
    0x5, 0x62, 0x32, 0x2, 0x9f, 0xa1, 0x5, 0x46, 0x24, 0x2, 0xa0, 0x8e, 
    0x3, 0x2, 0x2, 0x2, 0xa0, 0x8f, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x90, 0x3, 
    0x2, 0x2, 0x2, 0xa0, 0x91, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x92, 0x3, 0x2, 
    0x2, 0x2, 0xa0, 0x93, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x94, 0x3, 0x2, 0x2, 
    0x2, 0xa0, 0x95, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x96, 0x3, 0x2, 0x2, 0x2, 
    0xa0, 0x97, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x98, 0x3, 0x2, 0x2, 0x2, 0xa0, 
    0x99, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x9a, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x9b, 
    0x3, 0x2, 0x2, 0x2, 0xa0, 0x9c, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x9d, 0x3, 
    0x2, 0x2, 0x2, 0xa0, 0x9e, 0x3, 0x2, 0x2, 0x2, 0xa0, 0x9f, 0x3, 0x2, 
    0x2, 0x2, 0xa1, 0x7, 0x3, 0x2, 0x2, 0x2, 0xa2, 0xa4, 0x5, 0x10, 0x9, 
    0x2, 0xa3, 0xa2, 0x3, 0x2, 0x2, 0x2, 0xa4, 0xa7, 0x3, 0x2, 0x2, 0x2, 
    0xa5, 0xa3, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa6, 0x3, 0x2, 0x2, 0x2, 0xa6, 
    0xa8, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa5, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xb4, 
    0x5, 0xa, 0x6, 0x2, 0xa9, 0xaa, 0x7, 0x42, 0x2, 0x2, 0xaa, 0xaf, 0x5, 
    0xe, 0x8, 0x2, 0xab, 0xac, 0x7, 0x41, 0x2, 0x2, 0xac, 0xae, 0x5, 0xe, 
    0x8, 0x2, 0xad, 0xab, 0x3, 0x2, 0x2, 0x2, 0xae, 0xb1, 0x3, 0x2, 0x2, 
    0x2, 0xaf, 0xad, 0x3, 0x2, 0x2, 0x2, 0xaf, 0xb0, 0x3, 0x2, 0x2, 0x2, 
    0xb0, 0xb2, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xaf, 0x3, 0x2, 0x2, 0x2, 0xb2, 
    0xb3, 0x7, 0x43, 0x2, 0x2, 0xb3, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xb4, 0xa9, 
    0x3, 0x2, 0x2, 0x2, 0xb4, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xb5, 0xb7, 0x3, 
    0x2, 0x2, 0x2, 0xb6, 0xb8, 0x5, 0xc, 0x7, 0x2, 0xb7, 0xb6, 0x3, 0x2, 
    0x2, 0x2, 0xb8, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xb9, 0xb7, 0x3, 0x2, 0x2, 
    0x2, 0xb9, 0xba, 0x3, 0x2, 0x2, 0x2, 0xba, 0x9, 0x3, 0x2, 0x2, 0x2, 
    0xbb, 0xbc, 0x7, 0x3c, 0x2, 0x2, 0xbc, 0xb, 0x3, 0x2, 0x2, 0x2, 0xbd, 
    0xbe, 0x7, 0x3d, 0x2, 0x2, 0xbe, 0xd, 0x3, 0x2, 0x2, 0x2, 0xbf, 0xc0, 
    0x5, 0x66, 0x34, 0x2, 0xc0, 0xf, 0x3, 0x2, 0x2, 0x2, 0xc1, 0xc2, 0x9, 
    0x2, 0x2, 0x2, 0xc2, 0x11, 0x3, 0x2, 0x2, 0x2, 0xc3, 0xc4, 0x7, 0x3, 
    0x2, 0x2, 0xc4, 0xd2, 0x5, 0xa, 0x6, 0x2, 0xc5, 0xc6, 0x7, 0x42, 0x2, 
    0x2, 0xc6, 0xcb, 0x5, 0x16, 0xc, 0x2, 0xc7, 0xc8, 0x7, 0x41, 0x2, 0x2, 
    0xc8, 0xca, 0x5, 0x16, 0xc, 0x2, 0xc9, 0xc7, 0x3, 0x2, 0x2, 0x2, 0xca, 
    0xcd, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xc9, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 
    0x3, 0x2, 0x2, 0x2, 0xcc, 0xce, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xcb, 0x3, 
    0x2, 0x2, 0x2, 0xce, 0xcf, 0x7, 0x43, 0x2, 0x2, 0xcf, 0xd3, 0x3, 0x2, 
    0x2, 0x2, 0xd0, 0xd1, 0x7, 0x13, 0x2, 0x2, 0xd1, 0xd3, 0x5, 0x18, 0xd, 
    0x2, 0xd2, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd0, 0x3, 0x2, 0x2, 0x2, 
    0xd2, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd3, 0xd4, 0x3, 0x2, 0x2, 0x2, 0xd4, 
    0xd5, 0x7, 0x46, 0x2, 0x2, 0xd5, 0xd6, 0x7, 0x4c, 0x2, 0x2, 0xd6, 0xd7, 
    0x5, 0x1a, 0xe, 0x2, 0xd7, 0x13, 0x3, 0x2, 0x2, 0x2, 0xd8, 0xd9, 0x7, 
    0x3, 0x2, 0x2, 0xd9, 0xe5, 0x5, 0xa, 0x6, 0x2, 0xda, 0xdb, 0x7, 0x42, 
    0x2, 0x2, 0xdb, 0xe0, 0x5, 0x16, 0xc, 0x2, 0xdc, 0xdd, 0x7, 0x41, 0x2, 
    0x2, 0xdd, 0xdf, 0x5, 0x16, 0xc, 0x2, 0xde, 0xdc, 0x3, 0x2, 0x2, 0x2, 
    0xdf, 0xe2, 0x3, 0x2, 0x2, 0x2, 0xe0, 0xde, 0x3, 0x2, 0x2, 0x2, 0xe0, 
    0xe1, 0x3, 0x2, 0x2, 0x2, 0xe1, 0xe3, 0x3, 0x2, 0x2, 0x2, 0xe2, 0xe0, 
    0x3, 0x2, 0x2, 0x2, 0xe3, 0xe4, 0x7, 0x43, 0x2, 0x2, 0xe4, 0xe6, 0x3, 
    0x2, 0x2, 0x2, 0xe5, 0xda, 0x3, 0x2, 0x2, 0x2, 0xe5, 0xe6, 0x3, 0x2, 
    0x2, 0x2, 0xe6, 0xe8, 0x3, 0x2, 0x2, 0x2, 0xe7, 0xe9, 0x5, 0x24, 0x13, 
    0x2, 0xe8, 0xe7, 0x3, 0x2, 0x2, 0x2, 0xe9, 0xea, 0x3, 0x2, 0x2, 0x2, 
    0xea, 0xe8, 0x3, 0x2, 0x2, 0x2, 0xea, 0xeb, 0x3, 0x2, 0x2, 0x2, 0xeb, 
    0xec, 0x3, 0x2, 0x2, 0x2, 0xec, 0xed, 0x7, 0x13, 0x2, 0x2, 0xed, 0xee, 
    0x7, 0x16, 0x2, 0x2, 0xee, 0xef, 0x7, 0x46, 0x2, 0x2, 0xef, 0xf0, 0x7, 
    0x4c, 0x2, 0x2, 0xf0, 0xf1, 0x5, 0x1e, 0x10, 0x2, 0xf1, 0x15, 0x3, 0x2, 
    0x2, 0x2, 0xf2, 0xf3, 0x7, 0x47, 0x2, 0x2, 0xf3, 0xf4, 0x7, 0x3c, 0x2, 
    0x2, 0xf4, 0x17, 0x3, 0x2, 0x2, 0x2, 0xf5, 0xf6, 0x9, 0x3, 0x2, 0x2, 
    0xf6, 0x19, 0x3, 0x2, 0x2, 0x2, 0xf7, 0xf8, 0x5, 0x1c, 0xf, 0x2, 0xf8, 
    0xf9, 0x7, 0x4c, 0x2, 0x2, 0xf9, 0xfb, 0x3, 0x2, 0x2, 0x2, 0xfa, 0xf7, 
    0x3, 0x2, 0x2, 0x2, 0xfb, 0xfe, 0x3, 0x2, 0x2, 0x2, 0xfc, 0xfa, 0x3, 
    0x2, 0x2, 0x2, 0xfc, 0xfd, 0x3, 0x2, 0x2, 0x2, 0xfd, 0xff, 0x3, 0x2, 
    0x2, 0x2, 0xfe, 0xfc, 0x3, 0x2, 0x2, 0x2, 0xff, 0x100, 0x5, 0x1c, 0xf, 
    0x2, 0x100, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x101, 0x102, 0x7, 0x4b, 0x2, 
    0x2, 0x102, 0x107, 0x5, 0x66, 0x34, 0x2, 0x103, 0x104, 0x7, 0x41, 0x2, 
    0x2, 0x104, 0x106, 0x5, 0x66, 0x34, 0x2, 0x105, 0x103, 0x3, 0x2, 0x2, 
    0x2, 0x106, 0x109, 0x3, 0x2, 0x2, 0x2, 0x107, 0x105, 0x3, 0x2, 0x2, 
    0x2, 0x107, 0x108, 0x3, 0x2, 0x2, 0x2, 0x108, 0x1d, 0x3, 0x2, 0x2, 0x2, 
    0x109, 0x107, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10b, 0x5, 0x20, 0x11, 0x2, 
    0x10b, 0x10c, 0x7, 0x4c, 0x2, 0x2, 0x10c, 0x10e, 0x3, 0x2, 0x2, 0x2, 
    0x10d, 0x10a, 0x3, 0x2, 0x2, 0x2, 0x10e, 0x111, 0x3, 0x2, 0x2, 0x2, 
    0x10f, 0x10d, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x110, 0x3, 0x2, 0x2, 0x2, 
    0x110, 0x112, 0x3, 0x2, 0x2, 0x2, 0x111, 0x10f, 0x3, 0x2, 0x2, 0x2, 
    0x112, 0x113, 0x5, 0x20, 0x11, 0x2, 0x113, 0x1f, 0x3, 0x2, 0x2, 0x2, 
    0x114, 0x115, 0x7, 0x4b, 0x2, 0x2, 0x115, 0x116, 0x7, 0x3c, 0x2, 0x2, 
    0x116, 0x117, 0x7, 0x42, 0x2, 0x2, 0x117, 0x118, 0x5, 0x66, 0x34, 0x2, 
    0x118, 0x11a, 0x7, 0x43, 0x2, 0x2, 0x119, 0x11b, 0x5, 0x24, 0x13, 0x2, 
    0x11a, 0x119, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x11c, 0x3, 0x2, 0x2, 0x2, 
    0x11c, 0x11a, 0x3, 0x2, 0x2, 0x2, 0x11c, 0x11d, 0x3, 0x2, 0x2, 0x2, 
    0x11d, 0x21, 0x3, 0x2, 0x2, 0x2, 0x11e, 0x11f, 0x7, 0x4, 0x2, 0x2, 0x11f, 
    0x12b, 0x5, 0xa, 0x6, 0x2, 0x120, 0x121, 0x7, 0x42, 0x2, 0x2, 0x121, 
    0x126, 0x5, 0x16, 0xc, 0x2, 0x122, 0x123, 0x7, 0x41, 0x2, 0x2, 0x123, 
    0x125, 0x5, 0x16, 0xc, 0x2, 0x124, 0x122, 0x3, 0x2, 0x2, 0x2, 0x125, 
    0x128, 0x3, 0x2, 0x2, 0x2, 0x126, 0x124, 0x3, 0x2, 0x2, 0x2, 0x126, 
    0x127, 0x3, 0x2, 0x2, 0x2, 0x127, 0x129, 0x3, 0x2, 0x2, 0x2, 0x128, 
    0x126, 0x3, 0x2, 0x2, 0x2, 0x129, 0x12a, 0x7, 0x43, 0x2, 0x2, 0x12a, 
    0x12c, 0x3, 0x2, 0x2, 0x2, 0x12b, 0x120, 0x3, 0x2, 0x2, 0x2, 0x12b, 
    0x12c, 0x3, 0x2, 0x2, 0x2, 0x12c, 0x130, 0x3, 0x2, 0x2, 0x2, 0x12d, 
    0x12f, 0x5, 0x24, 0x13, 0x2, 0x12e, 0x12d, 0x3, 0x2, 0x2, 0x2, 0x12f, 
    0x132, 0x3, 0x2, 0x2, 0x2, 0x130, 0x12e, 0x3, 0x2, 0x2, 0x2, 0x130, 
    0x131, 0x3, 0x2, 0x2, 0x2, 0x131, 0x133, 0x3, 0x2, 0x2, 0x2, 0x132, 
    0x130, 0x3, 0x2, 0x2, 0x2, 0x133, 0x134, 0x7, 0x46, 0x2, 0x2, 0x134, 
    0x135, 0x7, 0x4c, 0x2, 0x2, 0x135, 0x136, 0x5, 0x30, 0x19, 0x2, 0x136, 
    0x23, 0x3, 0x2, 0x2, 0x2, 0x137, 0x138, 0x7, 0x3c, 0x2, 0x2, 0x138, 
    0x25, 0x3, 0x2, 0x2, 0x2, 0x139, 0x13c, 0x5, 0xc, 0x7, 0x2, 0x13a, 0x13c, 
    0x5, 0x24, 0x13, 0x2, 0x13b, 0x139, 0x3, 0x2, 0x2, 0x2, 0x13b, 0x13a, 
    0x3, 0x2, 0x2, 0x2, 0x13c, 0x27, 0x3, 0x2, 0x2, 0x2, 0x13d, 0x149, 0x5, 
    0xa, 0x6, 0x2, 0x13e, 0x13f, 0x7, 0x42, 0x2, 0x2, 0x13f, 0x144, 0x5, 
    0xe, 0x8, 0x2, 0x140, 0x141, 0x7, 0x41, 0x2, 0x2, 0x141, 0x143, 0x5, 
    0xe, 0x8, 0x2, 0x142, 0x140, 0x3, 0x2, 0x2, 0x2, 0x143, 0x146, 0x3, 
    0x2, 0x2, 0x2, 0x144, 0x142, 0x3, 0x2, 0x2, 0x2, 0x144, 0x145, 0x3, 
    0x2, 0x2, 0x2, 0x145, 0x147, 0x3, 0x2, 0x2, 0x2, 0x146, 0x144, 0x3, 
    0x2, 0x2, 0x2, 0x147, 0x148, 0x7, 0x43, 0x2, 0x2, 0x148, 0x14a, 0x3, 
    0x2, 0x2, 0x2, 0x149, 0x13e, 0x3, 0x2, 0x2, 0x2, 0x149, 0x14a, 0x3, 
    0x2, 0x2, 0x2, 0x14a, 0x14c, 0x3, 0x2, 0x2, 0x2, 0x14b, 0x14d, 0x5, 
    0x26, 0x14, 0x2, 0x14c, 0x14b, 0x3, 0x2, 0x2, 0x2, 0x14d, 0x14e, 0x3, 
    0x2, 0x2, 0x2, 0x14e, 0x14c, 0x3, 0x2, 0x2, 0x2, 0x14e, 0x14f, 0x3, 
    0x2, 0x2, 0x2, 0x14f, 0x29, 0x3, 0x2, 0x2, 0x2, 0x150, 0x151, 0x7, 0x5, 
    0x2, 0x2, 0x151, 0x153, 0x5, 0x26, 0x14, 0x2, 0x152, 0x154, 0x5, 0x34, 
    0x1b, 0x2, 0x153, 0x152, 0x3, 0x2, 0x2, 0x2, 0x153, 0x154, 0x3, 0x2, 
    0x2, 0x2, 0x154, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x155, 0x157, 0x7, 0xb, 0x2, 
    0x2, 0x156, 0x158, 0x5, 0x26, 0x14, 0x2, 0x157, 0x156, 0x3, 0x2, 0x2, 
    0x2, 0x157, 0x158, 0x3, 0x2, 0x2, 0x2, 0x158, 0x2d, 0x3, 0x2, 0x2, 0x2, 
    0x159, 0x15e, 0x5, 0x28, 0x15, 0x2, 0x15a, 0x15e, 0x5, 0x2a, 0x16, 0x2, 
    0x15b, 0x15e, 0x5, 0x2c, 0x17, 0x2, 0x15c, 0x15e, 0x5, 0x6, 0x4, 0x2, 
    0x15d, 0x159, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x15a, 0x3, 0x2, 0x2, 0x2, 
    0x15d, 0x15b, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x15c, 0x3, 0x2, 0x2, 0x2, 
    0x15e, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x15f, 0x160, 0x7, 0x4b, 0x2, 0x2, 
    0x160, 0x161, 0x5, 0x2e, 0x18, 0x2, 0x161, 0x162, 0x7, 0x4c, 0x2, 0x2, 
    0x162, 0x164, 0x3, 0x2, 0x2, 0x2, 0x163, 0x15f, 0x3, 0x2, 0x2, 0x2, 
    0x164, 0x167, 0x3, 0x2, 0x2, 0x2, 0x165, 0x163, 0x3, 0x2, 0x2, 0x2, 
    0x165, 0x166, 0x3, 0x2, 0x2, 0x2, 0x166, 0x168, 0x3, 0x2, 0x2, 0x2, 
    0x167, 0x165, 0x3, 0x2, 0x2, 0x2, 0x168, 0x169, 0x7, 0x4b, 0x2, 0x2, 
    0x169, 0x16a, 0x5, 0x2e, 0x18, 0x2, 0x16a, 0x31, 0x3, 0x2, 0x2, 0x2, 
    0x16b, 0x16c, 0x7, 0x5, 0x2, 0x2, 0x16c, 0x16e, 0x5, 0xc, 0x7, 0x2, 
    0x16d, 0x16f, 0x5, 0x34, 0x1b, 0x2, 0x16e, 0x16d, 0x3, 0x2, 0x2, 0x2, 
    0x16e, 0x16f, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x33, 0x3, 0x2, 0x2, 0x2, 0x170, 
    0x178, 0x7, 0x3c, 0x2, 0x2, 0x171, 0x173, 0x7, 0x3c, 0x2, 0x2, 0x172, 
    0x171, 0x3, 0x2, 0x2, 0x2, 0x172, 0x173, 0x3, 0x2, 0x2, 0x2, 0x173, 
    0x174, 0x3, 0x2, 0x2, 0x2, 0x174, 0x175, 0x7, 0x44, 0x2, 0x2, 0x175, 
    0x176, 0x7, 0x3d, 0x2, 0x2, 0x176, 0x178, 0x7, 0x45, 0x2, 0x2, 0x177, 
    0x170, 0x3, 0x2, 0x2, 0x2, 0x177, 0x172, 0x3, 0x2, 0x2, 0x2, 0x178, 
    0x35, 0x3, 0x2, 0x2, 0x2, 0x179, 0x17a, 0x7, 0x6, 0x2, 0x2, 0x17a, 0x17b, 
    0x5, 0x38, 0x1d, 0x2, 0x17b, 0x37, 0x3, 0x2, 0x2, 0x2, 0x17c, 0x17d, 
    0x7, 0x48, 0x2, 0x2, 0x17d, 0x17e, 0x7, 0x3c, 0x2, 0x2, 0x17e, 0x39, 
    0x3, 0x2, 0x2, 0x2, 0x17f, 0x180, 0x7, 0x7, 0x2, 0x2, 0x180, 0x3b, 0x3, 
    0x2, 0x2, 0x2, 0x181, 0x182, 0x7, 0x8, 0x2, 0x2, 0x182, 0x183, 0x5, 
    0x38, 0x1d, 0x2, 0x183, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x184, 0x185, 0x7, 
    0x9, 0x2, 0x2, 0x185, 0x186, 0x5, 0x38, 0x1d, 0x2, 0x186, 0x187, 0x5, 
    0x34, 0x1b, 0x2, 0x187, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x188, 0x189, 0x7, 
    0xa, 0x2, 0x2, 0x189, 0x18a, 0x5, 0x38, 0x1d, 0x2, 0x18a, 0x18b, 0x5, 
    0x34, 0x1b, 0x2, 0x18b, 0x41, 0x3, 0x2, 0x2, 0x2, 0x18c, 0x18e, 0x7, 
    0xb, 0x2, 0x2, 0x18d, 0x18f, 0x5, 0xc, 0x7, 0x2, 0x18e, 0x18d, 0x3, 
    0x2, 0x2, 0x2, 0x18e, 0x18f, 0x3, 0x2, 0x2, 0x2, 0x18f, 0x43, 0x3, 0x2, 
    0x2, 0x2, 0x190, 0x191, 0x7, 0xc, 0x2, 0x2, 0x191, 0x45, 0x3, 0x2, 0x2, 
    0x2, 0x192, 0x193, 0x7, 0x10, 0x2, 0x2, 0x193, 0x194, 0x7, 0x3c, 0x2, 
    0x2, 0x194, 0x198, 0x7, 0x3c, 0x2, 0x2, 0x195, 0x196, 0x7, 0x44, 0x2, 
    0x2, 0x196, 0x197, 0x7, 0x3d, 0x2, 0x2, 0x197, 0x199, 0x7, 0x45, 0x2, 
    0x2, 0x198, 0x195, 0x3, 0x2, 0x2, 0x2, 0x198, 0x199, 0x3, 0x2, 0x2, 
    0x2, 0x199, 0x1a2, 0x3, 0x2, 0x2, 0x2, 0x19a, 0x19b, 0x7, 0x11, 0x2, 
    0x2, 0x19b, 0x19f, 0x7, 0x3c, 0x2, 0x2, 0x19c, 0x19e, 0x5, 0x48, 0x25, 
    0x2, 0x19d, 0x19c, 0x3, 0x2, 0x2, 0x2, 0x19e, 0x1a1, 0x3, 0x2, 0x2, 
    0x2, 0x19f, 0x19d, 0x3, 0x2, 0x2, 0x2, 0x19f, 0x1a0, 0x3, 0x2, 0x2, 
    0x2, 0x1a0, 0x1a3, 0x3, 0x2, 0x2, 0x2, 0x1a1, 0x19f, 0x3, 0x2, 0x2, 
    0x2, 0x1a2, 0x19a, 0x3, 0x2, 0x2, 0x2, 0x1a2, 0x1a3, 0x3, 0x2, 0x2, 
    0x2, 0x1a3, 0x47, 0x3, 0x2, 0x2, 0x2, 0x1a4, 0x1a5, 0x7, 0x12, 0x2, 
    0x2, 0x1a5, 0x1a6, 0x7, 0x3d, 0x2, 0x2, 0x1a6, 0x1a7, 0x7, 0x3c, 0x2, 
    0x2, 0x1a7, 0x49, 0x3, 0x2, 0x2, 0x2, 0x1a8, 0x1a9, 0x9, 0x4, 0x2, 0x2, 
    0x1a9, 0x1aa, 0x5, 0x34, 0x1b, 0x2, 0x1aa, 0x4b, 0x3, 0x2, 0x2, 0x2, 
    0x1ab, 0x1b1, 0x5, 0x4e, 0x28, 0x2, 0x1ac, 0x1b1, 0x5, 0x50, 0x29, 0x2, 
    0x1ad, 0x1b1, 0x5, 0x52, 0x2a, 0x2, 0x1ae, 0x1b1, 0x5, 0x54, 0x2b, 0x2, 
    0x1af, 0x1b1, 0x5, 0x56, 0x2c, 0x2, 0x1b0, 0x1ab, 0x3, 0x2, 0x2, 0x2, 
    0x1b0, 0x1ac, 0x3, 0x2, 0x2, 0x2, 0x1b0, 0x1ad, 0x3, 0x2, 0x2, 0x2, 
    0x1b0, 0x1ae, 0x3, 0x2, 0x2, 0x2, 0x1b0, 0x1af, 0x3, 0x2, 0x2, 0x2, 
    0x1b1, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1b3, 0x9, 0x5, 0x2, 0x2, 0x1b3, 
    0x1b6, 0x5, 0x34, 0x1b, 0x2, 0x1b4, 0x1b7, 0x5, 0x34, 0x1b, 0x2, 0x1b5, 
    0x1b7, 0x7, 0x3d, 0x2, 0x2, 0x1b6, 0x1b4, 0x3, 0x2, 0x2, 0x2, 0x1b6, 
    0x1b5, 0x3, 0x2, 0x2, 0x2, 0x1b7, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x1b8, 0x1b9, 
    0x9, 0x6, 0x2, 0x2, 0x1b9, 0x1bc, 0x5, 0x34, 0x1b, 0x2, 0x1ba, 0x1bd, 
    0x5, 0x34, 0x1b, 0x2, 0x1bb, 0x1bd, 0x5, 0x6c, 0x37, 0x2, 0x1bc, 0x1ba, 
    0x3, 0x2, 0x2, 0x2, 0x1bc, 0x1bb, 0x3, 0x2, 0x2, 0x2, 0x1bd, 0x51, 0x3, 
    0x2, 0x2, 0x2, 0x1be, 0x1bf, 0x7, 0x23, 0x2, 0x2, 0x1bf, 0x1c2, 0x5, 
    0x34, 0x1b, 0x2, 0x1c0, 0x1c3, 0x5, 0x34, 0x1b, 0x2, 0x1c1, 0x1c3, 0x5, 
    0x6c, 0x37, 0x2, 0x1c2, 0x1c0, 0x3, 0x2, 0x2, 0x2, 0x1c2, 0x1c1, 0x3, 
    0x2, 0x2, 0x2, 0x1c3, 0x53, 0x3, 0x2, 0x2, 0x2, 0x1c4, 0x1c5, 0x7, 0x24, 
    0x2, 0x2, 0x1c5, 0x1c6, 0x5, 0x34, 0x1b, 0x2, 0x1c6, 0x1c7, 0x5, 0x34, 
    0x1b, 0x2, 0x1c7, 0x55, 0x3, 0x2, 0x2, 0x2, 0x1c8, 0x1c9, 0x7, 0x25, 
    0x2, 0x2, 0x1c9, 0x1ca, 0x5, 0x34, 0x1b, 0x2, 0x1ca, 0x1cb, 0x5, 0x34, 
    0x1b, 0x2, 0x1cb, 0x57, 0x3, 0x2, 0x2, 0x2, 0x1cc, 0x1cd, 0x7, 0x2b, 
    0x2, 0x2, 0x1cd, 0x1ce, 0x5, 0x34, 0x1b, 0x2, 0x1ce, 0x1cf, 0x7, 0x3c, 
    0x2, 0x2, 0x1cf, 0x1d0, 0x5, 0x34, 0x1b, 0x2, 0x1d0, 0x59, 0x3, 0x2, 
    0x2, 0x2, 0x1d1, 0x1d2, 0x7, 0x2c, 0x2, 0x2, 0x1d2, 0x1d3, 0x7, 0x3c, 
    0x2, 0x2, 0x1d3, 0x1d6, 0x5, 0x34, 0x1b, 0x2, 0x1d4, 0x1d7, 0x5, 0x34, 
    0x1b, 0x2, 0x1d5, 0x1d7, 0x5, 0x6c, 0x37, 0x2, 0x1d6, 0x1d4, 0x3, 0x2, 
    0x2, 0x2, 0x1d6, 0x1d5, 0x3, 0x2, 0x2, 0x2, 0x1d7, 0x5b, 0x3, 0x2, 0x2, 
    0x2, 0x1d8, 0x1d9, 0x9, 0x7, 0x2, 0x2, 0x1d9, 0x1da, 0x5, 0x34, 0x1b, 
    0x2, 0x1da, 0x1dd, 0x5, 0x34, 0x1b, 0x2, 0x1db, 0x1de, 0x5, 0x34, 0x1b, 
    0x2, 0x1dc, 0x1de, 0x5, 0x6c, 0x37, 0x2, 0x1dd, 0x1db, 0x3, 0x2, 0x2, 
    0x2, 0x1dd, 0x1dc, 0x3, 0x2, 0x2, 0x2, 0x1de, 0x5d, 0x3, 0x2, 0x2, 0x2, 
    0x1df, 0x1e0, 0x7, 0xd, 0x2, 0x2, 0x1e0, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x1e1, 
    0x1e2, 0x7, 0xe, 0x2, 0x2, 0x1e2, 0x1e3, 0x7, 0x3f, 0x2, 0x2, 0x1e3, 
    0x61, 0x3, 0x2, 0x2, 0x2, 0x1e4, 0x1e5, 0x7, 0xf, 0x2, 0x2, 0x1e5, 0x1e9, 
    0x7, 0x3c, 0x2, 0x2, 0x1e6, 0x1e8, 0x5, 0x64, 0x33, 0x2, 0x1e7, 0x1e6, 
    0x3, 0x2, 0x2, 0x2, 0x1e8, 0x1eb, 0x3, 0x2, 0x2, 0x2, 0x1e9, 0x1e7, 
    0x3, 0x2, 0x2, 0x2, 0x1e9, 0x1ea, 0x3, 0x2, 0x2, 0x2, 0x1ea, 0x1ed, 
    0x3, 0x2, 0x2, 0x2, 0x1eb, 0x1e9, 0x3, 0x2, 0x2, 0x2, 0x1ec, 0x1ee, 
    0x7, 0x3f, 0x2, 0x2, 0x1ed, 0x1ec, 0x3, 0x2, 0x2, 0x2, 0x1ed, 0x1ee, 
    0x3, 0x2, 0x2, 0x2, 0x1ee, 0x63, 0x3, 0x2, 0x2, 0x2, 0x1ef, 0x1f0, 0x9, 
    0x8, 0x2, 0x2, 0x1f0, 0x65, 0x3, 0x2, 0x2, 0x2, 0x1f1, 0x1f2, 0x8, 0x34, 
    0x1, 0x2, 0x1f2, 0x1f3, 0x7, 0x42, 0x2, 0x2, 0x1f3, 0x1f4, 0x5, 0x66, 
    0x34, 0x2, 0x1f4, 0x1f5, 0x7, 0x43, 0x2, 0x2, 0x1f5, 0x202, 0x3, 0x2, 
    0x2, 0x2, 0x1f6, 0x1f7, 0x5, 0x6a, 0x36, 0x2, 0x1f7, 0x1f8, 0x5, 0x66, 
    0x34, 0xa, 0x1f8, 0x202, 0x3, 0x2, 0x2, 0x2, 0x1f9, 0x1fa, 0x5, 0x68, 
    0x35, 0x2, 0x1fa, 0x1fb, 0x7, 0x42, 0x2, 0x2, 0x1fb, 0x1fc, 0x5, 0x66, 
    0x34, 0x2, 0x1fc, 0x1fd, 0x7, 0x43, 0x2, 0x2, 0x1fd, 0x202, 0x3, 0x2, 
    0x2, 0x2, 0x1fe, 0x202, 0x5, 0x6c, 0x37, 0x2, 0x1ff, 0x202, 0x5, 0x16, 
    0xc, 0x2, 0x200, 0x202, 0x5, 0x34, 0x1b, 0x2, 0x201, 0x1f1, 0x3, 0x2, 
    0x2, 0x2, 0x201, 0x1f6, 0x3, 0x2, 0x2, 0x2, 0x201, 0x1f9, 0x3, 0x2, 
    0x2, 0x2, 0x201, 0x1fe, 0x3, 0x2, 0x2, 0x2, 0x201, 0x1ff, 0x3, 0x2, 
    0x2, 0x2, 0x201, 0x200, 0x3, 0x2, 0x2, 0x2, 0x202, 0x20e, 0x3, 0x2, 
    0x2, 0x2, 0x203, 0x204, 0xc, 0x9, 0x2, 0x2, 0x204, 0x205, 0x7, 0x38, 
    0x2, 0x2, 0x205, 0x20d, 0x5, 0x66, 0x34, 0x9, 0x206, 0x207, 0xc, 0x8, 
    0x2, 0x2, 0x207, 0x208, 0x9, 0x9, 0x2, 0x2, 0x208, 0x20d, 0x5, 0x66, 
    0x34, 0x9, 0x209, 0x20a, 0xc, 0x7, 0x2, 0x2, 0x20a, 0x20b, 0x9, 0xa, 
    0x2, 0x2, 0x20b, 0x20d, 0x5, 0x66, 0x34, 0x8, 0x20c, 0x203, 0x3, 0x2, 
    0x2, 0x2, 0x20c, 0x206, 0x3, 0x2, 0x2, 0x2, 0x20c, 0x209, 0x3, 0x2, 
    0x2, 0x2, 0x20d, 0x210, 0x3, 0x2, 0x2, 0x2, 0x20e, 0x20c, 0x3, 0x2, 
    0x2, 0x2, 0x20e, 0x20f, 0x3, 0x2, 0x2, 0x2, 0x20f, 0x67, 0x3, 0x2, 0x2, 
    0x2, 0x210, 0x20e, 0x3, 0x2, 0x2, 0x2, 0x211, 0x212, 0x9, 0xb, 0x2, 
    0x2, 0x212, 0x69, 0x3, 0x2, 0x2, 0x2, 0x213, 0x214, 0x9, 0xa, 0x2, 0x2, 
    0x214, 0x6b, 0x3, 0x2, 0x2, 0x2, 0x215, 0x217, 0x7, 0x35, 0x2, 0x2, 
    0x216, 0x215, 0x3, 0x2, 0x2, 0x2, 0x216, 0x217, 0x3, 0x2, 0x2, 0x2, 
    0x217, 0x21c, 0x3, 0x2, 0x2, 0x2, 0x218, 0x21d, 0x5, 0x70, 0x39, 0x2, 
    0x219, 0x21d, 0x5, 0x6e, 0x38, 0x2, 0x21a, 0x21d, 0x7, 0x2e, 0x2, 0x2, 
    0x21b, 0x21d, 0x7, 0x2d, 0x2, 0x2, 0x21c, 0x218, 0x3, 0x2, 0x2, 0x2, 
    0x21c, 0x219, 0x3, 0x2, 0x2, 0x2, 0x21c, 0x21a, 0x3, 0x2, 0x2, 0x2, 
    0x21c, 0x21b, 0x3, 0x2, 0x2, 0x2, 0x21d, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x21e, 
    0x21f, 0x5, 0x70, 0x39, 0x2, 0x21f, 0x220, 0x7, 0x2e, 0x2, 0x2, 0x220, 
    0x6f, 0x3, 0x2, 0x2, 0x2, 0x221, 0x222, 0x9, 0xc, 0x2, 0x2, 0x222, 0x71, 
    0x3, 0x2, 0x2, 0x2, 0x34, 0x73, 0x78, 0x7d, 0x83, 0x8c, 0xa0, 0xa5, 
    0xaf, 0xb4, 0xb9, 0xcb, 0xd2, 0xe0, 0xe5, 0xea, 0xfc, 0x107, 0x10f, 
    0x11c, 0x126, 0x12b, 0x130, 0x13b, 0x144, 0x149, 0x14e, 0x153, 0x157, 
    0x15d, 0x165, 0x16e, 0x172, 0x177, 0x18e, 0x198, 0x19f, 0x1a2, 0x1b0, 
    0x1b6, 0x1bc, 0x1c2, 0x1d6, 0x1dd, 0x1e9, 0x1ed, 0x201, 0x20c, 0x20e, 
    0x216, 0x21c, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

QuilParser::Initializer QuilParser::_init;
