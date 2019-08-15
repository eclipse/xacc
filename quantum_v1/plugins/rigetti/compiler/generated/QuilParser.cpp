
// Generated from Quil.g4 by ANTLR 4.7.1

#include "QuilListener.h"

#include "QuilParser.h"

using namespace antlrcpp;
using namespace quil;
using namespace antlr4;

QuilParser::QuilParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA,
                                             _sharedContextCache);
}

QuilParser::~QuilParser() { delete _interpreter; }

std::string QuilParser::getGrammarFileName() const { return "Quil.g4"; }

const std::vector<std::string> &QuilParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary &QuilParser::getVocabulary() const { return _vocabulary; }

//----------------- XaccsrcContext
//------------------------------------------------------------------

QuilParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<QuilParser::XacckernelContext *>
QuilParser::XaccsrcContext::xacckernel() {
  return getRuleContexts<QuilParser::XacckernelContext>();
}

QuilParser::XacckernelContext *
QuilParser::XaccsrcContext::xacckernel(size_t i) {
  return getRuleContext<QuilParser::XacckernelContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::XaccsrcContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode *QuilParser::XaccsrcContext::NEWLINE(size_t i) {
  return getToken(QuilParser::NEWLINE, i);
}

size_t QuilParser::XaccsrcContext::getRuleIndex() const {
  return QuilParser::RuleXaccsrc;
}

void QuilParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void QuilParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

QuilParser::XaccsrcContext *QuilParser::xaccsrc() {
  XaccsrcContext *_localctx =
      _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, QuilParser::RuleXaccsrc);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(101);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::T__0) {
      setState(92);
      xacckernel();
      setState(96);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::NEWLINE) {
        setState(93);
        match(QuilParser::NEWLINE);
        setState(98);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(103);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- XacckernelContext
//------------------------------------------------------------------

QuilParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::QuilContext *QuilParser::XacckernelContext::quil() {
  return getRuleContext<QuilParser::QuilContext>(0);
}

std::vector<tree::TerminalNode *> QuilParser::XacckernelContext::IDENTIFIER() {
  return getTokens(QuilParser::IDENTIFIER);
}

tree::TerminalNode *QuilParser::XacckernelContext::IDENTIFIER(size_t i) {
  return getToken(QuilParser::IDENTIFIER, i);
}

std::vector<QuilParser::TypedparamContext *>
QuilParser::XacckernelContext::typedparam() {
  return getRuleContexts<QuilParser::TypedparamContext>();
}

QuilParser::TypedparamContext *
QuilParser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<QuilParser::TypedparamContext>(i);
}

size_t QuilParser::XacckernelContext::getRuleIndex() const {
  return QuilParser::RuleXacckernel;
}

void QuilParser::XacckernelContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void QuilParser::XacckernelContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

QuilParser::XacckernelContext *QuilParser::xacckernel() {
  XacckernelContext *_localctx =
      _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, QuilParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(104);
    match(QuilParser::T__0);
    setState(105);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname =
        match(QuilParser::IDENTIFIER);
    setState(106);
    match(QuilParser::LPAREN);
    setState(107);
    match(QuilParser::T__1);
    setState(108);
    dynamic_cast<XacckernelContext *>(_localctx)->acceleratorbuffer =
        match(QuilParser::IDENTIFIER);
    setState(113);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::COMMA) {
      setState(109);
      match(QuilParser::COMMA);
      setState(110);
      typedparam();
      setState(115);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(116);
    match(QuilParser::RPAREN);
    setState(117);
    match(QuilParser::T__2);
    setState(118);
    quil();
    setState(119);
    match(QuilParser::T__3);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypedparamContext
//------------------------------------------------------------------

QuilParser::TypedparamContext::TypedparamContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::TypeContext *QuilParser::TypedparamContext::type() {
  return getRuleContext<QuilParser::TypeContext>(0);
}

tree::TerminalNode *QuilParser::TypedparamContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

size_t QuilParser::TypedparamContext::getRuleIndex() const {
  return QuilParser::RuleTypedparam;
}

void QuilParser::TypedparamContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypedparam(this);
}

void QuilParser::TypedparamContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypedparam(this);
}

QuilParser::TypedparamContext *QuilParser::typedparam() {
  TypedparamContext *_localctx =
      _tracker.createInstance<TypedparamContext>(_ctx, getState());
  enterRule(_localctx, 4, QuilParser::RuleTypedparam);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    type();
    setState(122);
    match(QuilParser::IDENTIFIER);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext
//------------------------------------------------------------------

QuilParser::TypeContext::TypeContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

size_t QuilParser::TypeContext::getRuleIndex() const {
  return QuilParser::RuleType;
}

void QuilParser::TypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType(this);
}

void QuilParser::TypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType(this);
}

QuilParser::TypeContext *QuilParser::type() {
  TypeContext *_localctx =
      _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 6, QuilParser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(124);
    _la = _input->LA(1);
    if (!((((_la & ~0x3fULL) == 0) &&
           ((1ULL << _la) &
            ((1ULL << QuilParser::T__4) | (1ULL << QuilParser::T__5) |
             (1ULL << QuilParser::T__6))) != 0))) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- KernelcallContext
//------------------------------------------------------------------

QuilParser::KernelcallContext::KernelcallContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<tree::TerminalNode *> QuilParser::KernelcallContext::IDENTIFIER() {
  return getTokens(QuilParser::IDENTIFIER);
}

tree::TerminalNode *QuilParser::KernelcallContext::IDENTIFIER(size_t i) {
  return getToken(QuilParser::IDENTIFIER, i);
}

size_t QuilParser::KernelcallContext::getRuleIndex() const {
  return QuilParser::RuleKernelcall;
}

void QuilParser::KernelcallContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKernelcall(this);
}

void QuilParser::KernelcallContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKernelcall(this);
}

QuilParser::KernelcallContext *QuilParser::kernelcall() {
  KernelcallContext *_localctx =
      _tracker.createInstance<KernelcallContext>(_ctx, getState());
  enterRule(_localctx, 8, QuilParser::RuleKernelcall);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(126);
    dynamic_cast<KernelcallContext *>(_localctx)->kernelname =
        match(QuilParser::IDENTIFIER);
    setState(127);
    match(QuilParser::LPAREN);
    setState(129);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::IDENTIFIER) {
      setState(128);
      match(QuilParser::IDENTIFIER);
    }
    setState(135);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::COMMA) {
      setState(131);
      match(QuilParser::COMMA);
      setState(132);
      match(QuilParser::IDENTIFIER);
      setState(137);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(138);
    match(QuilParser::RPAREN);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QuilContext
//------------------------------------------------------------------

QuilParser::QuilContext::QuilContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<QuilParser::AllInstrContext *> QuilParser::QuilContext::allInstr() {
  return getRuleContexts<QuilParser::AllInstrContext>();
}

QuilParser::AllInstrContext *QuilParser::QuilContext::allInstr(size_t i) {
  return getRuleContext<QuilParser::AllInstrContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::QuilContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode *QuilParser::QuilContext::NEWLINE(size_t i) {
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

QuilParser::QuilContext *QuilParser::quil() {
  QuilContext *_localctx =
      _tracker.createInstance<QuilContext>(_ctx, getState());
  enterRule(_localctx, 10, QuilParser::RuleQuil);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(141);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~0x3fULL) == 0) &&
         ((1ULL << _la) &
          ((1ULL << QuilParser::DEFGATE) | (1ULL << QuilParser::DEFCIRCUIT) |
           (1ULL << QuilParser::MEASURE) | (1ULL << QuilParser::LABEL) |
           (1ULL << QuilParser::HALT) | (1ULL << QuilParser::JUMP) |
           (1ULL << QuilParser::JUMPWHEN) | (1ULL << QuilParser::JUMPUNLESS) |
           (1ULL << QuilParser::RESET) | (1ULL << QuilParser::WAIT) |
           (1ULL << QuilParser::NOP) | (1ULL << QuilParser::INCLUDE) |
           (1ULL << QuilParser::PRAGMA) | (1ULL << QuilParser::FALSE) |
           (1ULL << QuilParser::TRUE) | (1ULL << QuilParser::NOT) |
           (1ULL << QuilParser::AND) | (1ULL << QuilParser::OR) |
           (1ULL << QuilParser::MOVE) | (1ULL << QuilParser::EXCHANGE) |
           (1ULL << QuilParser::IDENTIFIER))) != 0)) {
      setState(140);
      allInstr();
    }
    setState(151);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7,
                                                                     _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(144);
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(143);
          match(QuilParser::NEWLINE);
          setState(146);
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == QuilParser::NEWLINE);
        setState(148);
        allInstr();
      }
      setState(153);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input,
                                                                       7, _ctx);
    }
    setState(157);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::NEWLINE) {
      setState(154);
      match(QuilParser::NEWLINE);
      setState(159);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AllInstrContext
//------------------------------------------------------------------

QuilParser::AllInstrContext::AllInstrContext(ParserRuleContext *parent,
                                             size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::DefGateContext *QuilParser::AllInstrContext::defGate() {
  return getRuleContext<QuilParser::DefGateContext>(0);
}

QuilParser::DefCircuitContext *QuilParser::AllInstrContext::defCircuit() {
  return getRuleContext<QuilParser::DefCircuitContext>(0);
}

QuilParser::InstrContext *QuilParser::AllInstrContext::instr() {
  return getRuleContext<QuilParser::InstrContext>(0);
}

QuilParser::KernelcallContext *QuilParser::AllInstrContext::kernelcall() {
  return getRuleContext<QuilParser::KernelcallContext>(0);
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

QuilParser::AllInstrContext *QuilParser::allInstr() {
  AllInstrContext *_localctx =
      _tracker.createInstance<AllInstrContext>(_ctx, getState());
  enterRule(_localctx, 12, QuilParser::RuleAllInstr);

  auto onExit = finally([=] { exitRule(); });
  try {
    setState(164);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
        _input, 9, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(160);
      defGate();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(161);
      defCircuit();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(162);
      instr();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(163);
      kernelcall();
      break;
    }
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstrContext
//------------------------------------------------------------------

QuilParser::InstrContext::InstrContext(ParserRuleContext *parent,
                                       size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::GateContext *QuilParser::InstrContext::gate() {
  return getRuleContext<QuilParser::GateContext>(0);
}

QuilParser::MeasureContext *QuilParser::InstrContext::measure() {
  return getRuleContext<QuilParser::MeasureContext>(0);
}

QuilParser::DefLabelContext *QuilParser::InstrContext::defLabel() {
  return getRuleContext<QuilParser::DefLabelContext>(0);
}

QuilParser::HaltContext *QuilParser::InstrContext::halt() {
  return getRuleContext<QuilParser::HaltContext>(0);
}

QuilParser::JumpContext *QuilParser::InstrContext::jump() {
  return getRuleContext<QuilParser::JumpContext>(0);
}

QuilParser::JumpWhenContext *QuilParser::InstrContext::jumpWhen() {
  return getRuleContext<QuilParser::JumpWhenContext>(0);
}

QuilParser::JumpUnlessContext *QuilParser::InstrContext::jumpUnless() {
  return getRuleContext<QuilParser::JumpUnlessContext>(0);
}

QuilParser::ResetStateContext *QuilParser::InstrContext::resetState() {
  return getRuleContext<QuilParser::ResetStateContext>(0);
}

QuilParser::WaitContext *QuilParser::InstrContext::wait() {
  return getRuleContext<QuilParser::WaitContext>(0);
}

QuilParser::ClassicalUnaryContext *QuilParser::InstrContext::classicalUnary() {
  return getRuleContext<QuilParser::ClassicalUnaryContext>(0);
}

QuilParser::ClassicalBinaryContext *
QuilParser::InstrContext::classicalBinary() {
  return getRuleContext<QuilParser::ClassicalBinaryContext>(0);
}

QuilParser::NopContext *QuilParser::InstrContext::nop() {
  return getRuleContext<QuilParser::NopContext>(0);
}

QuilParser::IncludeContext *QuilParser::InstrContext::include() {
  return getRuleContext<QuilParser::IncludeContext>(0);
}

QuilParser::PragmaContext *QuilParser::InstrContext::pragma() {
  return getRuleContext<QuilParser::PragmaContext>(0);
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

QuilParser::InstrContext *QuilParser::instr() {
  InstrContext *_localctx =
      _tracker.createInstance<InstrContext>(_ctx, getState());
  enterRule(_localctx, 14, QuilParser::RuleInstr);

  auto onExit = finally([=] { exitRule(); });
  try {
    setState(180);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
    case QuilParser::IDENTIFIER: {
      enterOuterAlt(_localctx, 1);
      setState(166);
      gate();
      break;
    }

    case QuilParser::MEASURE: {
      enterOuterAlt(_localctx, 2);
      setState(167);
      measure();
      break;
    }

    case QuilParser::LABEL: {
      enterOuterAlt(_localctx, 3);
      setState(168);
      defLabel();
      break;
    }

    case QuilParser::HALT: {
      enterOuterAlt(_localctx, 4);
      setState(169);
      halt();
      break;
    }

    case QuilParser::JUMP: {
      enterOuterAlt(_localctx, 5);
      setState(170);
      jump();
      break;
    }

    case QuilParser::JUMPWHEN: {
      enterOuterAlt(_localctx, 6);
      setState(171);
      jumpWhen();
      break;
    }

    case QuilParser::JUMPUNLESS: {
      enterOuterAlt(_localctx, 7);
      setState(172);
      jumpUnless();
      break;
    }

    case QuilParser::RESET: {
      enterOuterAlt(_localctx, 8);
      setState(173);
      resetState();
      break;
    }

    case QuilParser::WAIT: {
      enterOuterAlt(_localctx, 9);
      setState(174);
      wait();
      break;
    }

    case QuilParser::FALSE:
    case QuilParser::TRUE:
    case QuilParser::NOT: {
      enterOuterAlt(_localctx, 10);
      setState(175);
      classicalUnary();
      break;
    }

    case QuilParser::AND:
    case QuilParser::OR:
    case QuilParser::MOVE:
    case QuilParser::EXCHANGE: {
      enterOuterAlt(_localctx, 11);
      setState(176);
      classicalBinary();
      break;
    }

    case QuilParser::NOP: {
      enterOuterAlt(_localctx, 12);
      setState(177);
      nop();
      break;
    }

    case QuilParser::INCLUDE: {
      enterOuterAlt(_localctx, 13);
      setState(178);
      include();
      break;
    }

    case QuilParser::PRAGMA: {
      enterOuterAlt(_localctx, 14);
      setState(179);
      pragma();
      break;
    }

    default:
      throw NoViableAltException(this);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GateContext
//------------------------------------------------------------------

QuilParser::GateContext::GateContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::NameContext *QuilParser::GateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode *QuilParser::GateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::ParamContext *> QuilParser::GateContext::param() {
  return getRuleContexts<QuilParser::ParamContext>();
}

QuilParser::ParamContext *QuilParser::GateContext::param(size_t i) {
  return getRuleContext<QuilParser::ParamContext>(i);
}

tree::TerminalNode *QuilParser::GateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitContext *> QuilParser::GateContext::qubit() {
  return getRuleContexts<QuilParser::QubitContext>();
}

QuilParser::QubitContext *QuilParser::GateContext::qubit(size_t i) {
  return getRuleContext<QuilParser::QubitContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::GateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode *QuilParser::GateContext::COMMA(size_t i) {
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

QuilParser::GateContext *QuilParser::gate() {
  GateContext *_localctx =
      _tracker.createInstance<GateContext>(_ctx, getState());
  enterRule(_localctx, 16, QuilParser::RuleGate);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(182);
    name();
    setState(194);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(183);
      match(QuilParser::LPAREN);
      setState(184);
      param();
      setState(189);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(185);
        match(QuilParser::COMMA);
        setState(186);
        param();
        setState(191);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(192);
      match(QuilParser::RPAREN);
    }
    setState(197);
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(196);
      qubit();
      setState(199);
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::INT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NameContext
//------------------------------------------------------------------

QuilParser::NameContext::NameContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::NameContext::IDENTIFIER() {
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

QuilParser::NameContext *QuilParser::name() {
  NameContext *_localctx =
      _tracker.createInstance<NameContext>(_ctx, getState());
  enterRule(_localctx, 18, QuilParser::RuleName);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(201);
    match(QuilParser::IDENTIFIER);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QubitContext
//------------------------------------------------------------------

QuilParser::QubitContext::QubitContext(ParserRuleContext *parent,
                                       size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::QubitContext::INT() {
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

QuilParser::QubitContext *QuilParser::qubit() {
  QubitContext *_localctx =
      _tracker.createInstance<QubitContext>(_ctx, getState());
  enterRule(_localctx, 20, QuilParser::RuleQubit);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(203);
    match(QuilParser::INT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext
//------------------------------------------------------------------

QuilParser::ParamContext::ParamContext(ParserRuleContext *parent,
                                       size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::ExpressionContext *QuilParser::ParamContext::expression() {
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

QuilParser::ParamContext *QuilParser::param() {
  ParamContext *_localctx =
      _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 22, QuilParser::RuleParam);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(205);
    expression(0);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefGateContext
//------------------------------------------------------------------

QuilParser::DefGateContext::DefGateContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::DefGateContext::DEFGATE() {
  return getToken(QuilParser::DEFGATE, 0);
}

QuilParser::NameContext *QuilParser::DefGateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode *QuilParser::DefGateContext::COLON() {
  return getToken(QuilParser::COLON, 0);
}

tree::TerminalNode *QuilParser::DefGateContext::NEWLINE() {
  return getToken(QuilParser::NEWLINE, 0);
}

QuilParser::MatrixContext *QuilParser::DefGateContext::matrix() {
  return getRuleContext<QuilParser::MatrixContext>(0);
}

tree::TerminalNode *QuilParser::DefGateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::VariableContext *>
QuilParser::DefGateContext::variable() {
  return getRuleContexts<QuilParser::VariableContext>();
}

QuilParser::VariableContext *QuilParser::DefGateContext::variable(size_t i) {
  return getRuleContext<QuilParser::VariableContext>(i);
}

tree::TerminalNode *QuilParser::DefGateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> QuilParser::DefGateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode *QuilParser::DefGateContext::COMMA(size_t i) {
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

QuilParser::DefGateContext *QuilParser::defGate() {
  DefGateContext *_localctx =
      _tracker.createInstance<DefGateContext>(_ctx, getState());
  enterRule(_localctx, 24, QuilParser::RuleDefGate);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(207);
    match(QuilParser::DEFGATE);
    setState(208);
    name();
    setState(220);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(209);
      match(QuilParser::LPAREN);
      setState(210);
      variable();
      setState(215);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(211);
        match(QuilParser::COMMA);
        setState(212);
        variable();
        setState(217);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(218);
      match(QuilParser::RPAREN);
    }
    setState(222);
    match(QuilParser::COLON);
    setState(223);
    match(QuilParser::NEWLINE);
    setState(224);
    matrix();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VariableContext
//------------------------------------------------------------------

QuilParser::VariableContext::VariableContext(ParserRuleContext *parent,
                                             size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::VariableContext::PERCENTAGE() {
  return getToken(QuilParser::PERCENTAGE, 0);
}

tree::TerminalNode *QuilParser::VariableContext::IDENTIFIER() {
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

QuilParser::VariableContext *QuilParser::variable() {
  VariableContext *_localctx =
      _tracker.createInstance<VariableContext>(_ctx, getState());
  enterRule(_localctx, 26, QuilParser::RuleVariable);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(226);
    match(QuilParser::PERCENTAGE);
    setState(227);
    match(QuilParser::IDENTIFIER);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MatrixContext
//------------------------------------------------------------------

QuilParser::MatrixContext::MatrixContext(ParserRuleContext *parent,
                                         size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<QuilParser::MatrixRowContext *>
QuilParser::MatrixContext::matrixRow() {
  return getRuleContexts<QuilParser::MatrixRowContext>();
}

QuilParser::MatrixRowContext *QuilParser::MatrixContext::matrixRow(size_t i) {
  return getRuleContext<QuilParser::MatrixRowContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::MatrixContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode *QuilParser::MatrixContext::NEWLINE(size_t i) {
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

QuilParser::MatrixContext *QuilParser::matrix() {
  MatrixContext *_localctx =
      _tracker.createInstance<MatrixContext>(_ctx, getState());
  enterRule(_localctx, 28, QuilParser::RuleMatrix);

  auto onExit = finally([=] { exitRule(); });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(234);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16,
                                                                     _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(229);
        matrixRow();
        setState(230);
        match(QuilParser::NEWLINE);
      }
      setState(236);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
          _input, 16, _ctx);
    }
    setState(237);
    matrixRow();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MatrixRowContext
//------------------------------------------------------------------

QuilParser::MatrixRowContext::MatrixRowContext(ParserRuleContext *parent,
                                               size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::MatrixRowContext::TAB() {
  return getToken(QuilParser::TAB, 0);
}

std::vector<QuilParser::ExpressionContext *>
QuilParser::MatrixRowContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext *
QuilParser::MatrixRowContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::MatrixRowContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode *QuilParser::MatrixRowContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}

size_t QuilParser::MatrixRowContext::getRuleIndex() const {
  return QuilParser::RuleMatrixRow;
}

void QuilParser::MatrixRowContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMatrixRow(this);
}

void QuilParser::MatrixRowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMatrixRow(this);
}

QuilParser::MatrixRowContext *QuilParser::matrixRow() {
  MatrixRowContext *_localctx =
      _tracker.createInstance<MatrixRowContext>(_ctx, getState());
  enterRule(_localctx, 30, QuilParser::RuleMatrixRow);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(239);
    match(QuilParser::TAB);
    setState(240);
    expression(0);
    setState(245);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::COMMA) {
      setState(241);
      match(QuilParser::COMMA);
      setState(242);
      expression(0);
      setState(247);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefCircuitContext
//------------------------------------------------------------------

QuilParser::DefCircuitContext::DefCircuitContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::DefCircuitContext::DEFCIRCUIT() {
  return getToken(QuilParser::DEFCIRCUIT, 0);
}

QuilParser::NameContext *QuilParser::DefCircuitContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode *QuilParser::DefCircuitContext::COLON() {
  return getToken(QuilParser::COLON, 0);
}

tree::TerminalNode *QuilParser::DefCircuitContext::NEWLINE() {
  return getToken(QuilParser::NEWLINE, 0);
}

QuilParser::CircuitContext *QuilParser::DefCircuitContext::circuit() {
  return getRuleContext<QuilParser::CircuitContext>(0);
}

tree::TerminalNode *QuilParser::DefCircuitContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::VariableContext *>
QuilParser::DefCircuitContext::variable() {
  return getRuleContexts<QuilParser::VariableContext>();
}

QuilParser::VariableContext *QuilParser::DefCircuitContext::variable(size_t i) {
  return getRuleContext<QuilParser::VariableContext>(i);
}

tree::TerminalNode *QuilParser::DefCircuitContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::QubitVariableContext *>
QuilParser::DefCircuitContext::qubitVariable() {
  return getRuleContexts<QuilParser::QubitVariableContext>();
}

QuilParser::QubitVariableContext *
QuilParser::DefCircuitContext::qubitVariable(size_t i) {
  return getRuleContext<QuilParser::QubitVariableContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::DefCircuitContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode *QuilParser::DefCircuitContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}

size_t QuilParser::DefCircuitContext::getRuleIndex() const {
  return QuilParser::RuleDefCircuit;
}

void QuilParser::DefCircuitContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefCircuit(this);
}

void QuilParser::DefCircuitContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefCircuit(this);
}

QuilParser::DefCircuitContext *QuilParser::defCircuit() {
  DefCircuitContext *_localctx =
      _tracker.createInstance<DefCircuitContext>(_ctx, getState());
  enterRule(_localctx, 32, QuilParser::RuleDefCircuit);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(248);
    match(QuilParser::DEFCIRCUIT);
    setState(249);
    name();
    setState(261);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(250);
      match(QuilParser::LPAREN);
      setState(251);
      variable();
      setState(256);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(252);
        match(QuilParser::COMMA);
        setState(253);
        variable();
        setState(258);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(259);
      match(QuilParser::RPAREN);
    }
    setState(266);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::IDENTIFIER) {
      setState(263);
      qubitVariable();
      setState(268);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(269);
    match(QuilParser::COLON);
    setState(270);
    match(QuilParser::NEWLINE);
    setState(271);
    circuit();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QubitVariableContext
//------------------------------------------------------------------

QuilParser::QubitVariableContext::QubitVariableContext(
    ParserRuleContext *parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::QubitVariableContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

size_t QuilParser::QubitVariableContext::getRuleIndex() const {
  return QuilParser::RuleQubitVariable;
}

void QuilParser::QubitVariableContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQubitVariable(this);
}

void QuilParser::QubitVariableContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQubitVariable(this);
}

QuilParser::QubitVariableContext *QuilParser::qubitVariable() {
  QubitVariableContext *_localctx =
      _tracker.createInstance<QubitVariableContext>(_ctx, getState());
  enterRule(_localctx, 34, QuilParser::RuleQubitVariable);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(273);
    match(QuilParser::IDENTIFIER);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitQubitContext
//------------------------------------------------------------------

QuilParser::CircuitQubitContext::CircuitQubitContext(ParserRuleContext *parent,
                                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::QubitContext *QuilParser::CircuitQubitContext::qubit() {
  return getRuleContext<QuilParser::QubitContext>(0);
}

QuilParser::QubitVariableContext *
QuilParser::CircuitQubitContext::qubitVariable() {
  return getRuleContext<QuilParser::QubitVariableContext>(0);
}

size_t QuilParser::CircuitQubitContext::getRuleIndex() const {
  return QuilParser::RuleCircuitQubit;
}

void QuilParser::CircuitQubitContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitQubit(this);
}

void QuilParser::CircuitQubitContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitQubit(this);
}

QuilParser::CircuitQubitContext *QuilParser::circuitQubit() {
  CircuitQubitContext *_localctx =
      _tracker.createInstance<CircuitQubitContext>(_ctx, getState());
  enterRule(_localctx, 36, QuilParser::RuleCircuitQubit);

  auto onExit = finally([=] { exitRule(); });
  try {
    setState(277);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
    case QuilParser::INT: {
      enterOuterAlt(_localctx, 1);
      setState(275);
      qubit();
      break;
    }

    case QuilParser::IDENTIFIER: {
      enterOuterAlt(_localctx, 2);
      setState(276);
      qubitVariable();
      break;
    }

    default:
      throw NoViableAltException(this);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitGateContext
//------------------------------------------------------------------

QuilParser::CircuitGateContext::CircuitGateContext(ParserRuleContext *parent,
                                                   size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::NameContext *QuilParser::CircuitGateContext::name() {
  return getRuleContext<QuilParser::NameContext>(0);
}

tree::TerminalNode *QuilParser::CircuitGateContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

std::vector<QuilParser::ParamContext *>
QuilParser::CircuitGateContext::param() {
  return getRuleContexts<QuilParser::ParamContext>();
}

QuilParser::ParamContext *QuilParser::CircuitGateContext::param(size_t i) {
  return getRuleContext<QuilParser::ParamContext>(i);
}

tree::TerminalNode *QuilParser::CircuitGateContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

std::vector<QuilParser::CircuitQubitContext *>
QuilParser::CircuitGateContext::circuitQubit() {
  return getRuleContexts<QuilParser::CircuitQubitContext>();
}

QuilParser::CircuitQubitContext *
QuilParser::CircuitGateContext::circuitQubit(size_t i) {
  return getRuleContext<QuilParser::CircuitQubitContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::CircuitGateContext::COMMA() {
  return getTokens(QuilParser::COMMA);
}

tree::TerminalNode *QuilParser::CircuitGateContext::COMMA(size_t i) {
  return getToken(QuilParser::COMMA, i);
}

size_t QuilParser::CircuitGateContext::getRuleIndex() const {
  return QuilParser::RuleCircuitGate;
}

void QuilParser::CircuitGateContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitGate(this);
}

void QuilParser::CircuitGateContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitGate(this);
}

QuilParser::CircuitGateContext *QuilParser::circuitGate() {
  CircuitGateContext *_localctx =
      _tracker.createInstance<CircuitGateContext>(_ctx, getState());
  enterRule(_localctx, 38, QuilParser::RuleCircuitGate);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(279);
    name();
    setState(291);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LPAREN) {
      setState(280);
      match(QuilParser::LPAREN);
      setState(281);
      param();
      setState(286);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == QuilParser::COMMA) {
        setState(282);
        match(QuilParser::COMMA);
        setState(283);
        param();
        setState(288);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(289);
      match(QuilParser::RPAREN);
    }
    setState(294);
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(293);
      circuitQubit();
      setState(296);
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::IDENTIFIER

             || _la == QuilParser::INT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitInstrContext
//------------------------------------------------------------------

QuilParser::CircuitInstrContext::CircuitInstrContext(ParserRuleContext *parent,
                                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::CircuitGateContext *QuilParser::CircuitInstrContext::circuitGate() {
  return getRuleContext<QuilParser::CircuitGateContext>(0);
}

QuilParser::InstrContext *QuilParser::CircuitInstrContext::instr() {
  return getRuleContext<QuilParser::InstrContext>(0);
}

size_t QuilParser::CircuitInstrContext::getRuleIndex() const {
  return QuilParser::RuleCircuitInstr;
}

void QuilParser::CircuitInstrContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCircuitInstr(this);
}

void QuilParser::CircuitInstrContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCircuitInstr(this);
}

QuilParser::CircuitInstrContext *QuilParser::circuitInstr() {
  CircuitInstrContext *_localctx =
      _tracker.createInstance<CircuitInstrContext>(_ctx, getState());
  enterRule(_localctx, 40, QuilParser::RuleCircuitInstr);

  auto onExit = finally([=] { exitRule(); });
  try {
    setState(300);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
        _input, 25, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(298);
      circuitGate();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(299);
      instr();
      break;
    }
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CircuitContext
//------------------------------------------------------------------

QuilParser::CircuitContext::CircuitContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<tree::TerminalNode *> QuilParser::CircuitContext::TAB() {
  return getTokens(QuilParser::TAB);
}

tree::TerminalNode *QuilParser::CircuitContext::TAB(size_t i) {
  return getToken(QuilParser::TAB, i);
}

std::vector<QuilParser::CircuitInstrContext *>
QuilParser::CircuitContext::circuitInstr() {
  return getRuleContexts<QuilParser::CircuitInstrContext>();
}

QuilParser::CircuitInstrContext *
QuilParser::CircuitContext::circuitInstr(size_t i) {
  return getRuleContext<QuilParser::CircuitInstrContext>(i);
}

std::vector<tree::TerminalNode *> QuilParser::CircuitContext::NEWLINE() {
  return getTokens(QuilParser::NEWLINE);
}

tree::TerminalNode *QuilParser::CircuitContext::NEWLINE(size_t i) {
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

QuilParser::CircuitContext *QuilParser::circuit() {
  CircuitContext *_localctx =
      _tracker.createInstance<CircuitContext>(_ctx, getState());
  enterRule(_localctx, 42, QuilParser::RuleCircuit);

  auto onExit = finally([=] { exitRule(); });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(308);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26,
                                                                     _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(302);
        match(QuilParser::TAB);
        setState(303);
        circuitInstr();
        setState(304);
        match(QuilParser::NEWLINE);
      }
      setState(310);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
          _input, 26, _ctx);
    }
    setState(311);
    match(QuilParser::TAB);
    setState(312);
    circuitInstr();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MeasureContext
//------------------------------------------------------------------

QuilParser::MeasureContext::MeasureContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::MeasureContext::MEASURE() {
  return getToken(QuilParser::MEASURE, 0);
}

QuilParser::QubitContext *QuilParser::MeasureContext::qubit() {
  return getRuleContext<QuilParser::QubitContext>(0);
}

QuilParser::AddrContext *QuilParser::MeasureContext::addr() {
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

QuilParser::MeasureContext *QuilParser::measure() {
  MeasureContext *_localctx =
      _tracker.createInstance<MeasureContext>(_ctx, getState());
  enterRule(_localctx, 44, QuilParser::RuleMeasure);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(314);
    match(QuilParser::MEASURE);
    setState(315);
    qubit();
    setState(317);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::LBRACKET) {
      setState(316);
      addr();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddrContext
//------------------------------------------------------------------

QuilParser::AddrContext::AddrContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::AddrContext::LBRACKET() {
  return getToken(QuilParser::LBRACKET, 0);
}

QuilParser::ClassicalBitContext *QuilParser::AddrContext::classicalBit() {
  return getRuleContext<QuilParser::ClassicalBitContext>(0);
}

tree::TerminalNode *QuilParser::AddrContext::RBRACKET() {
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

QuilParser::AddrContext *QuilParser::addr() {
  AddrContext *_localctx =
      _tracker.createInstance<AddrContext>(_ctx, getState());
  enterRule(_localctx, 46, QuilParser::RuleAddr);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(319);
    match(QuilParser::LBRACKET);
    setState(320);
    classicalBit();
    setState(321);
    match(QuilParser::RBRACKET);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassicalBitContext
//------------------------------------------------------------------

QuilParser::ClassicalBitContext::ClassicalBitContext(ParserRuleContext *parent,
                                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<tree::TerminalNode *> QuilParser::ClassicalBitContext::INT() {
  return getTokens(QuilParser::INT);
}

tree::TerminalNode *QuilParser::ClassicalBitContext::INT(size_t i) {
  return getToken(QuilParser::INT, i);
}

size_t QuilParser::ClassicalBitContext::getRuleIndex() const {
  return QuilParser::RuleClassicalBit;
}

void QuilParser::ClassicalBitContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalBit(this);
}

void QuilParser::ClassicalBitContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalBit(this);
}

QuilParser::ClassicalBitContext *QuilParser::classicalBit() {
  ClassicalBitContext *_localctx =
      _tracker.createInstance<ClassicalBitContext>(_ctx, getState());
  enterRule(_localctx, 48, QuilParser::RuleClassicalBit);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(324);
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(323);
      match(QuilParser::INT);
      setState(326);
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == QuilParser::INT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefLabelContext
//------------------------------------------------------------------

QuilParser::DefLabelContext::DefLabelContext(ParserRuleContext *parent,
                                             size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::DefLabelContext::LABEL() {
  return getToken(QuilParser::LABEL, 0);
}

QuilParser::LabelContext *QuilParser::DefLabelContext::label() {
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

QuilParser::DefLabelContext *QuilParser::defLabel() {
  DefLabelContext *_localctx =
      _tracker.createInstance<DefLabelContext>(_ctx, getState());
  enterRule(_localctx, 50, QuilParser::RuleDefLabel);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(328);
    match(QuilParser::LABEL);
    setState(329);
    label();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LabelContext
//------------------------------------------------------------------

QuilParser::LabelContext::LabelContext(ParserRuleContext *parent,
                                       size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::LabelContext::AT() {
  return getToken(QuilParser::AT, 0);
}

tree::TerminalNode *QuilParser::LabelContext::IDENTIFIER() {
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

QuilParser::LabelContext *QuilParser::label() {
  LabelContext *_localctx =
      _tracker.createInstance<LabelContext>(_ctx, getState());
  enterRule(_localctx, 52, QuilParser::RuleLabel);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(331);
    match(QuilParser::AT);
    setState(332);
    match(QuilParser::IDENTIFIER);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HaltContext
//------------------------------------------------------------------

QuilParser::HaltContext::HaltContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::HaltContext::HALT() {
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

QuilParser::HaltContext *QuilParser::halt() {
  HaltContext *_localctx =
      _tracker.createInstance<HaltContext>(_ctx, getState());
  enterRule(_localctx, 54, QuilParser::RuleHalt);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(334);
    match(QuilParser::HALT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpContext
//------------------------------------------------------------------

QuilParser::JumpContext::JumpContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::JumpContext::JUMP() {
  return getToken(QuilParser::JUMP, 0);
}

QuilParser::LabelContext *QuilParser::JumpContext::label() {
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

QuilParser::JumpContext *QuilParser::jump() {
  JumpContext *_localctx =
      _tracker.createInstance<JumpContext>(_ctx, getState());
  enterRule(_localctx, 56, QuilParser::RuleJump);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(336);
    match(QuilParser::JUMP);
    setState(337);
    label();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpWhenContext
//------------------------------------------------------------------

QuilParser::JumpWhenContext::JumpWhenContext(ParserRuleContext *parent,
                                             size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::JumpWhenContext::JUMPWHEN() {
  return getToken(QuilParser::JUMPWHEN, 0);
}

QuilParser::LabelContext *QuilParser::JumpWhenContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}

QuilParser::AddrContext *QuilParser::JumpWhenContext::addr() {
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

QuilParser::JumpWhenContext *QuilParser::jumpWhen() {
  JumpWhenContext *_localctx =
      _tracker.createInstance<JumpWhenContext>(_ctx, getState());
  enterRule(_localctx, 58, QuilParser::RuleJumpWhen);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(339);
    match(QuilParser::JUMPWHEN);
    setState(340);
    label();
    setState(341);
    addr();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JumpUnlessContext
//------------------------------------------------------------------

QuilParser::JumpUnlessContext::JumpUnlessContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::JumpUnlessContext::JUMPUNLESS() {
  return getToken(QuilParser::JUMPUNLESS, 0);
}

QuilParser::LabelContext *QuilParser::JumpUnlessContext::label() {
  return getRuleContext<QuilParser::LabelContext>(0);
}

QuilParser::AddrContext *QuilParser::JumpUnlessContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}

size_t QuilParser::JumpUnlessContext::getRuleIndex() const {
  return QuilParser::RuleJumpUnless;
}

void QuilParser::JumpUnlessContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterJumpUnless(this);
}

void QuilParser::JumpUnlessContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitJumpUnless(this);
}

QuilParser::JumpUnlessContext *QuilParser::jumpUnless() {
  JumpUnlessContext *_localctx =
      _tracker.createInstance<JumpUnlessContext>(_ctx, getState());
  enterRule(_localctx, 60, QuilParser::RuleJumpUnless);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(343);
    match(QuilParser::JUMPUNLESS);
    setState(344);
    label();
    setState(345);
    addr();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ResetStateContext
//------------------------------------------------------------------

QuilParser::ResetStateContext::ResetStateContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::ResetStateContext::RESET() {
  return getToken(QuilParser::RESET, 0);
}

size_t QuilParser::ResetStateContext::getRuleIndex() const {
  return QuilParser::RuleResetState;
}

void QuilParser::ResetStateContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterResetState(this);
}

void QuilParser::ResetStateContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitResetState(this);
}

QuilParser::ResetStateContext *QuilParser::resetState() {
  ResetStateContext *_localctx =
      _tracker.createInstance<ResetStateContext>(_ctx, getState());
  enterRule(_localctx, 62, QuilParser::RuleResetState);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(347);
    match(QuilParser::RESET);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WaitContext
//------------------------------------------------------------------

QuilParser::WaitContext::WaitContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::WaitContext::WAIT() {
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

QuilParser::WaitContext *QuilParser::wait() {
  WaitContext *_localctx =
      _tracker.createInstance<WaitContext>(_ctx, getState());
  enterRule(_localctx, 64, QuilParser::RuleWait);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(349);
    match(QuilParser::WAIT);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassicalUnaryContext
//------------------------------------------------------------------

QuilParser::ClassicalUnaryContext::ClassicalUnaryContext(
    ParserRuleContext *parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::AddrContext *QuilParser::ClassicalUnaryContext::addr() {
  return getRuleContext<QuilParser::AddrContext>(0);
}

tree::TerminalNode *QuilParser::ClassicalUnaryContext::TRUE() {
  return getToken(QuilParser::TRUE, 0);
}

tree::TerminalNode *QuilParser::ClassicalUnaryContext::FALSE() {
  return getToken(QuilParser::FALSE, 0);
}

tree::TerminalNode *QuilParser::ClassicalUnaryContext::NOT() {
  return getToken(QuilParser::NOT, 0);
}

size_t QuilParser::ClassicalUnaryContext::getRuleIndex() const {
  return QuilParser::RuleClassicalUnary;
}

void QuilParser::ClassicalUnaryContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalUnary(this);
}

void QuilParser::ClassicalUnaryContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalUnary(this);
}

QuilParser::ClassicalUnaryContext *QuilParser::classicalUnary() {
  ClassicalUnaryContext *_localctx =
      _tracker.createInstance<ClassicalUnaryContext>(_ctx, getState());
  enterRule(_localctx, 66, QuilParser::RuleClassicalUnary);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(351);
    _la = _input->LA(1);
    if (!((((_la & ~0x3fULL) == 0) &&
           ((1ULL << _la) &
            ((1ULL << QuilParser::FALSE) | (1ULL << QuilParser::TRUE) |
             (1ULL << QuilParser::NOT))) != 0))) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(352);
    addr();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassicalBinaryContext
//------------------------------------------------------------------

QuilParser::ClassicalBinaryContext::ClassicalBinaryContext(
    ParserRuleContext *parent, size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

std::vector<QuilParser::AddrContext *>
QuilParser::ClassicalBinaryContext::addr() {
  return getRuleContexts<QuilParser::AddrContext>();
}

QuilParser::AddrContext *QuilParser::ClassicalBinaryContext::addr(size_t i) {
  return getRuleContext<QuilParser::AddrContext>(i);
}

tree::TerminalNode *QuilParser::ClassicalBinaryContext::AND() {
  return getToken(QuilParser::AND, 0);
}

tree::TerminalNode *QuilParser::ClassicalBinaryContext::OR() {
  return getToken(QuilParser::OR, 0);
}

tree::TerminalNode *QuilParser::ClassicalBinaryContext::MOVE() {
  return getToken(QuilParser::MOVE, 0);
}

tree::TerminalNode *QuilParser::ClassicalBinaryContext::EXCHANGE() {
  return getToken(QuilParser::EXCHANGE, 0);
}

size_t QuilParser::ClassicalBinaryContext::getRuleIndex() const {
  return QuilParser::RuleClassicalBinary;
}

void QuilParser::ClassicalBinaryContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterClassicalBinary(this);
}

void QuilParser::ClassicalBinaryContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitClassicalBinary(this);
}

QuilParser::ClassicalBinaryContext *QuilParser::classicalBinary() {
  ClassicalBinaryContext *_localctx =
      _tracker.createInstance<ClassicalBinaryContext>(_ctx, getState());
  enterRule(_localctx, 68, QuilParser::RuleClassicalBinary);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(354);
    _la = _input->LA(1);
    if (!((((_la & ~0x3fULL) == 0) &&
           ((1ULL << _la) &
            ((1ULL << QuilParser::AND) | (1ULL << QuilParser::OR) |
             (1ULL << QuilParser::MOVE) | (1ULL << QuilParser::EXCHANGE))) !=
               0))) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(355);
    addr();
    setState(356);
    addr();

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NopContext
//------------------------------------------------------------------

QuilParser::NopContext::NopContext(ParserRuleContext *parent,
                                   size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::NopContext::NOP() {
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

QuilParser::NopContext *QuilParser::nop() {
  NopContext *_localctx = _tracker.createInstance<NopContext>(_ctx, getState());
  enterRule(_localctx, 70, QuilParser::RuleNop);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(358);
    match(QuilParser::NOP);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext
//------------------------------------------------------------------

QuilParser::IncludeContext::IncludeContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::IncludeContext::INCLUDE() {
  return getToken(QuilParser::INCLUDE, 0);
}

tree::TerminalNode *QuilParser::IncludeContext::STRING() {
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

QuilParser::IncludeContext *QuilParser::include() {
  IncludeContext *_localctx =
      _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 72, QuilParser::RuleInclude);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(360);
    match(QuilParser::INCLUDE);
    setState(361);
    match(QuilParser::STRING);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PragmaContext
//------------------------------------------------------------------

QuilParser::PragmaContext::PragmaContext(ParserRuleContext *parent,
                                         size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::PragmaContext::PRAGMA() {
  return getToken(QuilParser::PRAGMA, 0);
}

tree::TerminalNode *QuilParser::PragmaContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

std::vector<QuilParser::Pragma_nameContext *>
QuilParser::PragmaContext::pragma_name() {
  return getRuleContexts<QuilParser::Pragma_nameContext>();
}

QuilParser::Pragma_nameContext *
QuilParser::PragmaContext::pragma_name(size_t i) {
  return getRuleContext<QuilParser::Pragma_nameContext>(i);
}

tree::TerminalNode *QuilParser::PragmaContext::STRING() {
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

QuilParser::PragmaContext *QuilParser::pragma() {
  PragmaContext *_localctx =
      _tracker.createInstance<PragmaContext>(_ctx, getState());
  enterRule(_localctx, 74, QuilParser::RulePragma);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(363);
    match(QuilParser::PRAGMA);
    setState(364);
    match(QuilParser::IDENTIFIER);
    setState(368);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == QuilParser::IDENTIFIER

           || _la == QuilParser::INT) {
      setState(365);
      pragma_name();
      setState(370);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(372);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == QuilParser::STRING) {
      setState(371);
      match(QuilParser::STRING);
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Pragma_nameContext
//------------------------------------------------------------------

QuilParser::Pragma_nameContext::Pragma_nameContext(ParserRuleContext *parent,
                                                   size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::Pragma_nameContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

tree::TerminalNode *QuilParser::Pragma_nameContext::INT() {
  return getToken(QuilParser::INT, 0);
}

size_t QuilParser::Pragma_nameContext::getRuleIndex() const {
  return QuilParser::RulePragma_name;
}

void QuilParser::Pragma_nameContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPragma_name(this);
}

void QuilParser::Pragma_nameContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPragma_name(this);
}

QuilParser::Pragma_nameContext *QuilParser::pragma_name() {
  Pragma_nameContext *_localctx =
      _tracker.createInstance<Pragma_nameContext>(_ctx, getState());
  enterRule(_localctx, 76, QuilParser::RulePragma_name);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(374);
    _la = _input->LA(1);
    if (!(_la == QuilParser::IDENTIFIER

          || _la == QuilParser::INT)) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext
//------------------------------------------------------------------

QuilParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

size_t QuilParser::ExpressionContext::getRuleIndex() const {
  return QuilParser::RuleExpression;
}

void QuilParser::ExpressionContext::copyFrom(ExpressionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- SegmentExpContext
//------------------------------------------------------------------

QuilParser::SegmentContext *QuilParser::SegmentExpContext::segment() {
  return getRuleContext<QuilParser::SegmentContext>(0);
}

QuilParser::SegmentExpContext::SegmentExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::SegmentExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSegmentExp(this);
}
void QuilParser::SegmentExpContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSegmentExp(this);
}
//----------------- IdentifierExpContext
//------------------------------------------------------------------

tree::TerminalNode *QuilParser::IdentifierExpContext::IDENTIFIER() {
  return getToken(QuilParser::IDENTIFIER, 0);
}

QuilParser::IdentifierExpContext::IdentifierExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::IdentifierExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifierExp(this);
}
void QuilParser::IdentifierExpContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifierExp(this);
}
//----------------- NumberExpContext
//------------------------------------------------------------------

QuilParser::NumberContext *QuilParser::NumberExpContext::number() {
  return getRuleContext<QuilParser::NumberContext>(0);
}

QuilParser::NumberExpContext::NumberExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::NumberExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNumberExp(this);
}
void QuilParser::NumberExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNumberExp(this);
}
//----------------- PowerExpContext
//------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *>
QuilParser::PowerExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext *
QuilParser::PowerExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode *QuilParser::PowerExpContext::POWER() {
  return getToken(QuilParser::POWER, 0);
}

QuilParser::PowerExpContext::PowerExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

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
//----------------- MulDivExpContext
//------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *>
QuilParser::MulDivExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext *
QuilParser::MulDivExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode *QuilParser::MulDivExpContext::TIMES() {
  return getToken(QuilParser::TIMES, 0);
}

tree::TerminalNode *QuilParser::MulDivExpContext::DIVIDE() {
  return getToken(QuilParser::DIVIDE, 0);
}

QuilParser::MulDivExpContext::MulDivExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::MulDivExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMulDivExp(this);
}
void QuilParser::MulDivExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMulDivExp(this);
}
//----------------- ParenthesisExpContext
//------------------------------------------------------------------

tree::TerminalNode *QuilParser::ParenthesisExpContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

QuilParser::ExpressionContext *QuilParser::ParenthesisExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

tree::TerminalNode *QuilParser::ParenthesisExpContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

QuilParser::ParenthesisExpContext::ParenthesisExpContext(
    ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::ParenthesisExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParenthesisExp(this);
}
void QuilParser::ParenthesisExpContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParenthesisExp(this);
}
//----------------- VariableExpContext
//------------------------------------------------------------------

QuilParser::VariableContext *QuilParser::VariableExpContext::variable() {
  return getRuleContext<QuilParser::VariableContext>(0);
}

QuilParser::VariableExpContext::VariableExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::VariableExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariableExp(this);
}
void QuilParser::VariableExpContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariableExp(this);
}
//----------------- SignedExpContext
//------------------------------------------------------------------

QuilParser::SignContext *QuilParser::SignedExpContext::sign() {
  return getRuleContext<QuilParser::SignContext>(0);
}

QuilParser::ExpressionContext *QuilParser::SignedExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

QuilParser::SignedExpContext::SignedExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::SignedExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSignedExp(this);
}
void QuilParser::SignedExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSignedExp(this);
}
//----------------- AddSubExpContext
//------------------------------------------------------------------

std::vector<QuilParser::ExpressionContext *>
QuilParser::AddSubExpContext::expression() {
  return getRuleContexts<QuilParser::ExpressionContext>();
}

QuilParser::ExpressionContext *
QuilParser::AddSubExpContext::expression(size_t i) {
  return getRuleContext<QuilParser::ExpressionContext>(i);
}

tree::TerminalNode *QuilParser::AddSubExpContext::PLUS() {
  return getToken(QuilParser::PLUS, 0);
}

tree::TerminalNode *QuilParser::AddSubExpContext::MINUS() {
  return getToken(QuilParser::MINUS, 0);
}

QuilParser::AddSubExpContext::AddSubExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::AddSubExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddSubExp(this);
}
void QuilParser::AddSubExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddSubExp(this);
}
//----------------- FunctionExpContext
//------------------------------------------------------------------

QuilParser::FunctionContext *QuilParser::FunctionExpContext::function() {
  return getRuleContext<QuilParser::FunctionContext>(0);
}

tree::TerminalNode *QuilParser::FunctionExpContext::LPAREN() {
  return getToken(QuilParser::LPAREN, 0);
}

QuilParser::ExpressionContext *QuilParser::FunctionExpContext::expression() {
  return getRuleContext<QuilParser::ExpressionContext>(0);
}

tree::TerminalNode *QuilParser::FunctionExpContext::RPAREN() {
  return getToken(QuilParser::RPAREN, 0);
}

QuilParser::FunctionExpContext::FunctionExpContext(ExpressionContext *ctx) {
  copyFrom(ctx);
}

void QuilParser::FunctionExpContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExp(this);
}
void QuilParser::FunctionExpContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExp(this);
}

QuilParser::ExpressionContext *QuilParser::expression() {
  return expression(0);
}

QuilParser::ExpressionContext *QuilParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  QuilParser::ExpressionContext *_localctx =
      _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  QuilParser::ExpressionContext *previousContext = _localctx;
  size_t startState = 78;
  enterRecursionRule(_localctx, 78, QuilParser::RuleExpression, precedence);

  size_t _la = 0;

  auto onExit = finally([=] { unrollRecursionContexts(parentContext); });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(393);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
    case QuilParser::LPAREN: {
      _localctx = _tracker.createInstance<ParenthesisExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(377);
      match(QuilParser::LPAREN);
      setState(378);
      expression(0);
      setState(379);
      match(QuilParser::RPAREN);
      break;
    }

    case QuilParser::PLUS:
    case QuilParser::MINUS: {
      _localctx = _tracker.createInstance<SignedExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(381);
      sign();
      setState(382);
      expression(9);
      break;
    }

    case QuilParser::SIN:
    case QuilParser::COS:
    case QuilParser::SQRT:
    case QuilParser::EXP:
    case QuilParser::CIS: {
      _localctx = _tracker.createInstance<FunctionExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(384);
      function();
      setState(385);
      match(QuilParser::LPAREN);
      setState(386);
      expression(0);
      setState(387);
      match(QuilParser::RPAREN);
      break;
    }

    case QuilParser::LBRACKET: {
      _localctx = _tracker.createInstance<SegmentExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(389);
      segment();
      break;
    }

    case QuilParser::PI:
    case QuilParser::I:
    case QuilParser::INT:
    case QuilParser::FLOAT: {
      _localctx = _tracker.createInstance<NumberExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(390);
      number();
      break;
    }

    case QuilParser::PERCENTAGE: {
      _localctx = _tracker.createInstance<VariableExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(391);
      variable();
      break;
    }

    case QuilParser::IDENTIFIER: {
      _localctx = _tracker.createInstance<IdentifierExpContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(392);
      match(QuilParser::IDENTIFIER);
      break;
    }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(406);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33,
                                                                     _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(404);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
            _input, 32, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<PowerExpContext>(
              _tracker.createInstance<ExpressionContext>(parentContext,
                                                         parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(395);

          if (!(precpred(_ctx, 8)))
            throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(396);
          match(QuilParser::POWER);
          setState(397);
          expression(8);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<MulDivExpContext>(
              _tracker.createInstance<ExpressionContext>(parentContext,
                                                         parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(398);

          if (!(precpred(_ctx, 7)))
            throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(399);
          _la = _input->LA(1);
          if (!(_la == QuilParser::TIMES

                || _la == QuilParser::DIVIDE)) {
            _errHandler->recoverInline(this);
          } else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(400);
          expression(8);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AddSubExpContext>(
              _tracker.createInstance<ExpressionContext>(parentContext,
                                                         parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpression);
          setState(401);

          if (!(precpred(_ctx, 6)))
            throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(402);
          _la = _input->LA(1);
          if (!(_la == QuilParser::PLUS

                || _la == QuilParser::MINUS)) {
            _errHandler->recoverInline(this);
          } else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(403);
          expression(7);
          break;
        }
        }
      }
      setState(408);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
          _input, 33, _ctx);
    }
  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- SegmentContext
//------------------------------------------------------------------

QuilParser::SegmentContext::SegmentContext(ParserRuleContext *parent,
                                           size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::SegmentContext::LBRACKET() {
  return getToken(QuilParser::LBRACKET, 0);
}

std::vector<tree::TerminalNode *> QuilParser::SegmentContext::INT() {
  return getTokens(QuilParser::INT);
}

tree::TerminalNode *QuilParser::SegmentContext::INT(size_t i) {
  return getToken(QuilParser::INT, i);
}

tree::TerminalNode *QuilParser::SegmentContext::MINUS() {
  return getToken(QuilParser::MINUS, 0);
}

tree::TerminalNode *QuilParser::SegmentContext::RBRACKET() {
  return getToken(QuilParser::RBRACKET, 0);
}

size_t QuilParser::SegmentContext::getRuleIndex() const {
  return QuilParser::RuleSegment;
}

void QuilParser::SegmentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSegment(this);
}

void QuilParser::SegmentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSegment(this);
}

QuilParser::SegmentContext *QuilParser::segment() {
  SegmentContext *_localctx =
      _tracker.createInstance<SegmentContext>(_ctx, getState());
  enterRule(_localctx, 80, QuilParser::RuleSegment);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(409);
    match(QuilParser::LBRACKET);
    setState(410);
    match(QuilParser::INT);
    setState(411);
    match(QuilParser::MINUS);
    setState(412);
    match(QuilParser::INT);
    setState(413);
    match(QuilParser::RBRACKET);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionContext
//------------------------------------------------------------------

QuilParser::FunctionContext::FunctionContext(ParserRuleContext *parent,
                                             size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::FunctionContext::SIN() {
  return getToken(QuilParser::SIN, 0);
}

tree::TerminalNode *QuilParser::FunctionContext::COS() {
  return getToken(QuilParser::COS, 0);
}

tree::TerminalNode *QuilParser::FunctionContext::SQRT() {
  return getToken(QuilParser::SQRT, 0);
}

tree::TerminalNode *QuilParser::FunctionContext::EXP() {
  return getToken(QuilParser::EXP, 0);
}

tree::TerminalNode *QuilParser::FunctionContext::CIS() {
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

QuilParser::FunctionContext *QuilParser::function() {
  FunctionContext *_localctx =
      _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 82, QuilParser::RuleFunction);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(415);
    _la = _input->LA(1);
    if (!((((_la & ~0x3fULL) == 0) &&
           ((1ULL << _la) &
            ((1ULL << QuilParser::SIN) | (1ULL << QuilParser::COS) |
             (1ULL << QuilParser::SQRT) | (1ULL << QuilParser::EXP) |
             (1ULL << QuilParser::CIS))) != 0))) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SignContext
//------------------------------------------------------------------

QuilParser::SignContext::SignContext(ParserRuleContext *parent,
                                     size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::SignContext::PLUS() {
  return getToken(QuilParser::PLUS, 0);
}

tree::TerminalNode *QuilParser::SignContext::MINUS() {
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

QuilParser::SignContext *QuilParser::sign() {
  SignContext *_localctx =
      _tracker.createInstance<SignContext>(_ctx, getState());
  enterRule(_localctx, 84, QuilParser::RuleSign);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(417);
    _la = _input->LA(1);
    if (!(_la == QuilParser::PLUS

          || _la == QuilParser::MINUS)) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext
//------------------------------------------------------------------

QuilParser::NumberContext::NumberContext(ParserRuleContext *parent,
                                         size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::RealNContext *QuilParser::NumberContext::realN() {
  return getRuleContext<QuilParser::RealNContext>(0);
}

QuilParser::ImaginaryNContext *QuilParser::NumberContext::imaginaryN() {
  return getRuleContext<QuilParser::ImaginaryNContext>(0);
}

tree::TerminalNode *QuilParser::NumberContext::I() {
  return getToken(QuilParser::I, 0);
}

tree::TerminalNode *QuilParser::NumberContext::PI() {
  return getToken(QuilParser::PI, 0);
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

QuilParser::NumberContext *QuilParser::number() {
  NumberContext *_localctx =
      _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 86, QuilParser::RuleNumber);

  auto onExit = finally([=] { exitRule(); });
  try {
    setState(423);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(
        _input, 34, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(419);
      realN();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(420);
      imaginaryN();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(421);
      match(QuilParser::I);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(422);
      match(QuilParser::PI);
      break;
    }
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImaginaryNContext
//------------------------------------------------------------------

QuilParser::ImaginaryNContext::ImaginaryNContext(ParserRuleContext *parent,
                                                 size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

QuilParser::RealNContext *QuilParser::ImaginaryNContext::realN() {
  return getRuleContext<QuilParser::RealNContext>(0);
}

tree::TerminalNode *QuilParser::ImaginaryNContext::I() {
  return getToken(QuilParser::I, 0);
}

size_t QuilParser::ImaginaryNContext::getRuleIndex() const {
  return QuilParser::RuleImaginaryN;
}

void QuilParser::ImaginaryNContext::enterRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImaginaryN(this);
}

void QuilParser::ImaginaryNContext::exitRule(
    tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<QuilListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImaginaryN(this);
}

QuilParser::ImaginaryNContext *QuilParser::imaginaryN() {
  ImaginaryNContext *_localctx =
      _tracker.createInstance<ImaginaryNContext>(_ctx, getState());
  enterRule(_localctx, 88, QuilParser::RuleImaginaryN);

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(425);
    realN();
    setState(426);
    match(QuilParser::I);

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealNContext
//------------------------------------------------------------------

QuilParser::RealNContext::RealNContext(ParserRuleContext *parent,
                                       size_t invokingState)
    : ParserRuleContext(parent, invokingState) {}

tree::TerminalNode *QuilParser::RealNContext::FLOAT() {
  return getToken(QuilParser::FLOAT, 0);
}

tree::TerminalNode *QuilParser::RealNContext::INT() {
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

QuilParser::RealNContext *QuilParser::realN() {
  RealNContext *_localctx =
      _tracker.createInstance<RealNContext>(_ctx, getState());
  enterRule(_localctx, 90, QuilParser::RuleRealN);
  size_t _la = 0;

  auto onExit = finally([=] { exitRule(); });
  try {
    enterOuterAlt(_localctx, 1);
    setState(428);
    _la = _input->LA(1);
    if (!(_la == QuilParser::INT

          || _la == QuilParser::FLOAT)) {
      _errHandler->recoverInline(this);
    } else {
      _errHandler->reportMatch(this);
      consume();
    }

  } catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool QuilParser::sempred(RuleContext *context, size_t ruleIndex,
                         size_t predicateIndex) {
  switch (ruleIndex) {
  case 39:
    return expressionSempred(dynamic_cast<ExpressionContext *>(context),
                             predicateIndex);

  default:
    break;
  }
  return true;
}

bool QuilParser::expressionSempred(ExpressionContext *_localctx,
                                   size_t predicateIndex) {
  switch (predicateIndex) {
  case 0:
    return precpred(_ctx, 8);
  case 1:
    return precpred(_ctx, 7);
  case 2:
    return precpred(_ctx, 6);

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

std::vector<std::string> QuilParser::_ruleNames = {"xaccsrc",
                                                   "xacckernel",
                                                   "typedparam",
                                                   "type",
                                                   "kernelcall",
                                                   "quil",
                                                   "allInstr",
                                                   "instr",
                                                   "gate",
                                                   "name",
                                                   "qubit",
                                                   "param",
                                                   "defGate",
                                                   "variable",
                                                   "matrix",
                                                   "matrixRow",
                                                   "defCircuit",
                                                   "qubitVariable",
                                                   "circuitQubit",
                                                   "circuitGate",
                                                   "circuitInstr",
                                                   "circuit",
                                                   "measure",
                                                   "addr",
                                                   "classicalBit",
                                                   "defLabel",
                                                   "label",
                                                   "halt",
                                                   "jump",
                                                   "jumpWhen",
                                                   "jumpUnless",
                                                   "resetState",
                                                   "wait",
                                                   "classicalUnary",
                                                   "classicalBinary",
                                                   "nop",
                                                   "include",
                                                   "pragma",
                                                   "pragma_name",
                                                   "expression",
                                                   "segment",
                                                   "function",
                                                   "sign",
                                                   "number",
                                                   "imaginaryN",
                                                   "realN"};

std::vector<std::string> QuilParser::_literalNames = {"",
                                                      "'__qpu__'",
                                                      "'AcceleratorBuffer'",
                                                      "'{'",
                                                      "'}'",
                                                      "'int'",
                                                      "'double'",
                                                      "'float'",
                                                      "'DEFGATE'",
                                                      "'DEFCIRCUIT'",
                                                      "'MEASURE'",
                                                      "'LABEL'",
                                                      "'HALT'",
                                                      "'JUMP'",
                                                      "'JUMP-WHEN'",
                                                      "'JUMP-UNLESS'",
                                                      "'RESET'",
                                                      "'WAIT'",
                                                      "'NOP'",
                                                      "'INCLUDE'",
                                                      "'PRAGMA'",
                                                      "'FALSE'",
                                                      "'TRUE'",
                                                      "'NOT'",
                                                      "'AND'",
                                                      "'OR'",
                                                      "'MOVE'",
                                                      "'EXCHANGE'",
                                                      "'pi'",
                                                      "'i'",
                                                      "'sin'",
                                                      "'cos'",
                                                      "'sqrt'",
                                                      "'exp'",
                                                      "'cis'",
                                                      "'+'",
                                                      "'-'",
                                                      "'*'",
                                                      "'/'",
                                                      "'^'",
                                                      "",
                                                      "",
                                                      "",
                                                      "",
                                                      "'.'",
                                                      "','",
                                                      "'('",
                                                      "')'",
                                                      "'['",
                                                      "']'",
                                                      "':'",
                                                      "'%'",
                                                      "'@'",
                                                      "'\"'",
                                                      "'_'",
                                                      "'    '",
                                                      "",
                                                      "",
                                                      "' '"};

std::vector<std::string> QuilParser::_symbolicNames = {
    "",           "",           "",         "",         "",
    "",           "",           "",         "DEFGATE",  "DEFCIRCUIT",
    "MEASURE",    "LABEL",      "HALT",     "JUMP",     "JUMPWHEN",
    "JUMPUNLESS", "RESET",      "WAIT",     "NOP",      "INCLUDE",
    "PRAGMA",     "FALSE",      "TRUE",     "NOT",      "AND",
    "OR",         "MOVE",       "EXCHANGE", "PI",       "I",
    "SIN",        "COS",        "SQRT",     "EXP",      "CIS",
    "PLUS",       "MINUS",      "TIMES",    "DIVIDE",   "POWER",
    "IDENTIFIER", "INT",        "FLOAT",    "STRING",   "PERIOD",
    "COMMA",      "LPAREN",     "RPAREN",   "LBRACKET", "RBRACKET",
    "COLON",      "PERCENTAGE", "AT",       "QUOTE",    "UNDERSCORE",
    "TAB",        "NEWLINE",    "COMMENT",  "SPACE",    "INVALID"};

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
      0x3,   0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964,
      0x3,   0x3d,   0x1b1,  0x4,    0x2,    0x9,    0x2,    0x4,    0x3,
      0x9,   0x3,    0x4,    0x4,    0x9,    0x4,    0x4,    0x5,    0x9,
      0x5,   0x4,    0x6,    0x9,    0x6,    0x4,    0x7,    0x9,    0x7,
      0x4,   0x8,    0x9,    0x8,    0x4,    0x9,    0x9,    0x9,    0x4,
      0xa,   0x9,    0xa,    0x4,    0xb,    0x9,    0xb,    0x4,    0xc,
      0x9,   0xc,    0x4,    0xd,    0x9,    0xd,    0x4,    0xe,    0x9,
      0xe,   0x4,    0xf,    0x9,    0xf,    0x4,    0x10,   0x9,    0x10,
      0x4,   0x11,   0x9,    0x11,   0x4,    0x12,   0x9,    0x12,   0x4,
      0x13,  0x9,    0x13,   0x4,    0x14,   0x9,    0x14,   0x4,    0x15,
      0x9,   0x15,   0x4,    0x16,   0x9,    0x16,   0x4,    0x17,   0x9,
      0x17,  0x4,    0x18,   0x9,    0x18,   0x4,    0x19,   0x9,    0x19,
      0x4,   0x1a,   0x9,    0x1a,   0x4,    0x1b,   0x9,    0x1b,   0x4,
      0x1c,  0x9,    0x1c,   0x4,    0x1d,   0x9,    0x1d,   0x4,    0x1e,
      0x9,   0x1e,   0x4,    0x1f,   0x9,    0x1f,   0x4,    0x20,   0x9,
      0x20,  0x4,    0x21,   0x9,    0x21,   0x4,    0x22,   0x9,    0x22,
      0x4,   0x23,   0x9,    0x23,   0x4,    0x24,   0x9,    0x24,   0x4,
      0x25,  0x9,    0x25,   0x4,    0x26,   0x9,    0x26,   0x4,    0x27,
      0x9,   0x27,   0x4,    0x28,   0x9,    0x28,   0x4,    0x29,   0x9,
      0x29,  0x4,    0x2a,   0x9,    0x2a,   0x4,    0x2b,   0x9,    0x2b,
      0x4,   0x2c,   0x9,    0x2c,   0x4,    0x2d,   0x9,    0x2d,   0x4,
      0x2e,  0x9,    0x2e,   0x4,    0x2f,   0x9,    0x2f,   0x3,    0x2,
      0x3,   0x2,    0x7,    0x2,    0x61,   0xa,    0x2,    0xc,    0x2,
      0xe,   0x2,    0x64,   0xb,    0x2,    0x7,    0x2,    0x66,   0xa,
      0x2,   0xc,    0x2,    0xe,    0x2,    0x69,   0xb,    0x2,    0x3,
      0x3,   0x3,    0x3,    0x3,    0x3,    0x3,    0x3,    0x3,    0x3,
      0x3,   0x3,    0x3,    0x3,    0x7,    0x3,    0x72,   0xa,    0x3,
      0xc,   0x3,    0xe,    0x3,    0x75,   0xb,    0x3,    0x3,    0x3,
      0x3,   0x3,    0x3,    0x3,    0x3,    0x3,    0x3,    0x3,    0x3,
      0x4,   0x3,    0x4,    0x3,    0x4,    0x3,    0x5,    0x3,    0x5,
      0x3,   0x6,    0x3,    0x6,    0x3,    0x6,    0x5,    0x6,    0x84,
      0xa,   0x6,    0x3,    0x6,    0x3,    0x6,    0x7,    0x6,    0x88,
      0xa,   0x6,    0xc,    0x6,    0xe,    0x6,    0x8b,   0xb,    0x6,
      0x3,   0x6,    0x3,    0x6,    0x3,    0x7,    0x5,    0x7,    0x90,
      0xa,   0x7,    0x3,    0x7,    0x6,    0x7,    0x93,   0xa,    0x7,
      0xd,   0x7,    0xe,    0x7,    0x94,   0x3,    0x7,    0x7,    0x7,
      0x98,  0xa,    0x7,    0xc,    0x7,    0xe,    0x7,    0x9b,   0xb,
      0x7,   0x3,    0x7,    0x7,    0x7,    0x9e,   0xa,    0x7,    0xc,
      0x7,   0xe,    0x7,    0xa1,   0xb,    0x7,    0x3,    0x8,    0x3,
      0x8,   0x3,    0x8,    0x3,    0x8,    0x5,    0x8,    0xa7,   0xa,
      0x8,   0x3,    0x9,    0x3,    0x9,    0x3,    0x9,    0x3,    0x9,
      0x3,   0x9,    0x3,    0x9,    0x3,    0x9,    0x3,    0x9,    0x3,
      0x9,   0x3,    0x9,    0x3,    0x9,    0x3,    0x9,    0x3,    0x9,
      0x3,   0x9,    0x5,    0x9,    0xb7,   0xa,    0x9,    0x3,    0xa,
      0x3,   0xa,    0x3,    0xa,    0x3,    0xa,    0x3,    0xa,    0x7,
      0xa,   0xbe,   0xa,    0xa,    0xc,    0xa,    0xe,    0xa,    0xc1,
      0xb,   0xa,    0x3,    0xa,    0x3,    0xa,    0x5,    0xa,    0xc5,
      0xa,   0xa,    0x3,    0xa,    0x6,    0xa,    0xc8,   0xa,    0xa,
      0xd,   0xa,    0xe,    0xa,    0xc9,   0x3,    0xb,    0x3,    0xb,
      0x3,   0xc,    0x3,    0xc,    0x3,    0xd,    0x3,    0xd,    0x3,
      0xe,   0x3,    0xe,    0x3,    0xe,    0x3,    0xe,    0x3,    0xe,
      0x3,   0xe,    0x7,    0xe,    0xd8,   0xa,    0xe,    0xc,    0xe,
      0xe,   0xe,    0xdb,   0xb,    0xe,    0x3,    0xe,    0x3,    0xe,
      0x5,   0xe,    0xdf,   0xa,    0xe,    0x3,    0xe,    0x3,    0xe,
      0x3,   0xe,    0x3,    0xe,    0x3,    0xf,    0x3,    0xf,    0x3,
      0xf,   0x3,    0x10,   0x3,    0x10,   0x3,    0x10,   0x7,    0x10,
      0xeb,  0xa,    0x10,   0xc,    0x10,   0xe,    0x10,   0xee,   0xb,
      0x10,  0x3,    0x10,   0x3,    0x10,   0x3,    0x11,   0x3,    0x11,
      0x3,   0x11,   0x3,    0x11,   0x7,    0x11,   0xf6,   0xa,    0x11,
      0xc,   0x11,   0xe,    0x11,   0xf9,   0xb,    0x11,   0x3,    0x12,
      0x3,   0x12,   0x3,    0x12,   0x3,    0x12,   0x3,    0x12,   0x3,
      0x12,  0x7,    0x12,   0x101,  0xa,    0x12,   0xc,    0x12,   0xe,
      0x12,  0x104,  0xb,    0x12,   0x3,    0x12,   0x3,    0x12,   0x5,
      0x12,  0x108,  0xa,    0x12,   0x3,    0x12,   0x7,    0x12,   0x10b,
      0xa,   0x12,   0xc,    0x12,   0xe,    0x12,   0x10e,  0xb,    0x12,
      0x3,   0x12,   0x3,    0x12,   0x3,    0x12,   0x3,    0x12,   0x3,
      0x13,  0x3,    0x13,   0x3,    0x14,   0x3,    0x14,   0x5,    0x14,
      0x118, 0xa,    0x14,   0x3,    0x15,   0x3,    0x15,   0x3,    0x15,
      0x3,   0x15,   0x3,    0x15,   0x7,    0x15,   0x11f,  0xa,    0x15,
      0xc,   0x15,   0xe,    0x15,   0x122,  0xb,    0x15,   0x3,    0x15,
      0x3,   0x15,   0x5,    0x15,   0x126,  0xa,    0x15,   0x3,    0x15,
      0x6,   0x15,   0x129,  0xa,    0x15,   0xd,    0x15,   0xe,    0x15,
      0x12a, 0x3,    0x16,   0x3,    0x16,   0x5,    0x16,   0x12f,  0xa,
      0x16,  0x3,    0x17,   0x3,    0x17,   0x3,    0x17,   0x3,    0x17,
      0x7,   0x17,   0x135,  0xa,    0x17,   0xc,    0x17,   0xe,    0x17,
      0x138, 0xb,    0x17,   0x3,    0x17,   0x3,    0x17,   0x3,    0x17,
      0x3,   0x18,   0x3,    0x18,   0x3,    0x18,   0x5,    0x18,   0x140,
      0xa,   0x18,   0x3,    0x19,   0x3,    0x19,   0x3,    0x19,   0x3,
      0x19,  0x3,    0x1a,   0x6,    0x1a,   0x147,  0xa,    0x1a,   0xd,
      0x1a,  0xe,    0x1a,   0x148,  0x3,    0x1b,   0x3,    0x1b,   0x3,
      0x1b,  0x3,    0x1c,   0x3,    0x1c,   0x3,    0x1c,   0x3,    0x1d,
      0x3,   0x1d,   0x3,    0x1e,   0x3,    0x1e,   0x3,    0x1e,   0x3,
      0x1f,  0x3,    0x1f,   0x3,    0x1f,   0x3,    0x1f,   0x3,    0x20,
      0x3,   0x20,   0x3,    0x20,   0x3,    0x20,   0x3,    0x21,   0x3,
      0x21,  0x3,    0x22,   0x3,    0x22,   0x3,    0x23,   0x3,    0x23,
      0x3,   0x23,   0x3,    0x24,   0x3,    0x24,   0x3,    0x24,   0x3,
      0x24,  0x3,    0x25,   0x3,    0x25,   0x3,    0x26,   0x3,    0x26,
      0x3,   0x26,   0x3,    0x27,   0x3,    0x27,   0x3,    0x27,   0x7,
      0x27,  0x171,  0xa,    0x27,   0xc,    0x27,   0xe,    0x27,   0x174,
      0xb,   0x27,   0x3,    0x27,   0x5,    0x27,   0x177,  0xa,    0x27,
      0x3,   0x28,   0x3,    0x28,   0x3,    0x29,   0x3,    0x29,   0x3,
      0x29,  0x3,    0x29,   0x3,    0x29,   0x3,    0x29,   0x3,    0x29,
      0x3,   0x29,   0x3,    0x29,   0x3,    0x29,   0x3,    0x29,   0x3,
      0x29,  0x3,    0x29,   0x3,    0x29,   0x3,    0x29,   0x3,    0x29,
      0x3,   0x29,   0x5,    0x29,   0x18c,  0xa,    0x29,   0x3,    0x29,
      0x3,   0x29,   0x3,    0x29,   0x3,    0x29,   0x3,    0x29,   0x3,
      0x29,  0x3,    0x29,   0x3,    0x29,   0x3,    0x29,   0x7,    0x29,
      0x197, 0xa,    0x29,   0xc,    0x29,   0xe,    0x29,   0x19a,  0xb,
      0x29,  0x3,    0x2a,   0x3,    0x2a,   0x3,    0x2a,   0x3,    0x2a,
      0x3,   0x2a,   0x3,    0x2a,   0x3,    0x2b,   0x3,    0x2b,   0x3,
      0x2c,  0x3,    0x2c,   0x3,    0x2d,   0x3,    0x2d,   0x3,    0x2d,
      0x3,   0x2d,   0x5,    0x2d,   0x1aa,  0xa,    0x2d,   0x3,    0x2e,
      0x3,   0x2e,   0x3,    0x2e,   0x3,    0x2f,   0x3,    0x2f,   0x3,
      0x2f,  0x2,    0x3,    0x50,   0x30,   0x2,    0x4,    0x6,    0x8,
      0xa,   0xc,    0xe,    0x10,   0x12,   0x14,   0x16,   0x18,   0x1a,
      0x1c,  0x1e,   0x20,   0x22,   0x24,   0x26,   0x28,   0x2a,   0x2c,
      0x2e,  0x30,   0x32,   0x34,   0x36,   0x38,   0x3a,   0x3c,   0x3e,
      0x40,  0x42,   0x44,   0x46,   0x48,   0x4a,   0x4c,   0x4e,   0x50,
      0x52,  0x54,   0x56,   0x58,   0x5a,   0x5c,   0x2,    0xa,    0x3,
      0x2,   0x7,    0x9,    0x3,    0x2,    0x17,   0x19,   0x3,    0x2,
      0x1a,  0x1d,   0x3,    0x2,    0x2a,   0x2b,   0x3,    0x2,    0x27,
      0x28,  0x3,    0x2,    0x25,   0x26,   0x3,    0x2,    0x20,   0x24,
      0x3,   0x2,    0x2b,   0x2c,   0x2,    0x1bb,  0x2,    0x67,   0x3,
      0x2,   0x2,    0x2,    0x4,    0x6a,   0x3,    0x2,    0x2,    0x2,
      0x6,   0x7b,   0x3,    0x2,    0x2,    0x2,    0x8,    0x7e,   0x3,
      0x2,   0x2,    0x2,    0xa,    0x80,   0x3,    0x2,    0x2,    0x2,
      0xc,   0x8f,   0x3,    0x2,    0x2,    0x2,    0xe,    0xa6,   0x3,
      0x2,   0x2,    0x2,    0x10,   0xb6,   0x3,    0x2,    0x2,    0x2,
      0x12,  0xb8,   0x3,    0x2,    0x2,    0x2,    0x14,   0xcb,   0x3,
      0x2,   0x2,    0x2,    0x16,   0xcd,   0x3,    0x2,    0x2,    0x2,
      0x18,  0xcf,   0x3,    0x2,    0x2,    0x2,    0x1a,   0xd1,   0x3,
      0x2,   0x2,    0x2,    0x1c,   0xe4,   0x3,    0x2,    0x2,    0x2,
      0x1e,  0xec,   0x3,    0x2,    0x2,    0x2,    0x20,   0xf1,   0x3,
      0x2,   0x2,    0x2,    0x22,   0xfa,   0x3,    0x2,    0x2,    0x2,
      0x24,  0x113,  0x3,    0x2,    0x2,    0x2,    0x26,   0x117,  0x3,
      0x2,   0x2,    0x2,    0x28,   0x119,  0x3,    0x2,    0x2,    0x2,
      0x2a,  0x12e,  0x3,    0x2,    0x2,    0x2,    0x2c,   0x136,  0x3,
      0x2,   0x2,    0x2,    0x2e,   0x13c,  0x3,    0x2,    0x2,    0x2,
      0x30,  0x141,  0x3,    0x2,    0x2,    0x2,    0x32,   0x146,  0x3,
      0x2,   0x2,    0x2,    0x34,   0x14a,  0x3,    0x2,    0x2,    0x2,
      0x36,  0x14d,  0x3,    0x2,    0x2,    0x2,    0x38,   0x150,  0x3,
      0x2,   0x2,    0x2,    0x3a,   0x152,  0x3,    0x2,    0x2,    0x2,
      0x3c,  0x155,  0x3,    0x2,    0x2,    0x2,    0x3e,   0x159,  0x3,
      0x2,   0x2,    0x2,    0x40,   0x15d,  0x3,    0x2,    0x2,    0x2,
      0x42,  0x15f,  0x3,    0x2,    0x2,    0x2,    0x44,   0x161,  0x3,
      0x2,   0x2,    0x2,    0x46,   0x164,  0x3,    0x2,    0x2,    0x2,
      0x48,  0x168,  0x3,    0x2,    0x2,    0x2,    0x4a,   0x16a,  0x3,
      0x2,   0x2,    0x2,    0x4c,   0x16d,  0x3,    0x2,    0x2,    0x2,
      0x4e,  0x178,  0x3,    0x2,    0x2,    0x2,    0x50,   0x18b,  0x3,
      0x2,   0x2,    0x2,    0x52,   0x19b,  0x3,    0x2,    0x2,    0x2,
      0x54,  0x1a1,  0x3,    0x2,    0x2,    0x2,    0x56,   0x1a3,  0x3,
      0x2,   0x2,    0x2,    0x58,   0x1a9,  0x3,    0x2,    0x2,    0x2,
      0x5a,  0x1ab,  0x3,    0x2,    0x2,    0x2,    0x5c,   0x1ae,  0x3,
      0x2,   0x2,    0x2,    0x5e,   0x62,   0x5,    0x4,    0x3,    0x2,
      0x5f,  0x61,   0x7,    0x3a,   0x2,    0x2,    0x60,   0x5f,   0x3,
      0x2,   0x2,    0x2,    0x61,   0x64,   0x3,    0x2,    0x2,    0x2,
      0x62,  0x60,   0x3,    0x2,    0x2,    0x2,    0x62,   0x63,   0x3,
      0x2,   0x2,    0x2,    0x63,   0x66,   0x3,    0x2,    0x2,    0x2,
      0x64,  0x62,   0x3,    0x2,    0x2,    0x2,    0x65,   0x5e,   0x3,
      0x2,   0x2,    0x2,    0x66,   0x69,   0x3,    0x2,    0x2,    0x2,
      0x67,  0x65,   0x3,    0x2,    0x2,    0x2,    0x67,   0x68,   0x3,
      0x2,   0x2,    0x2,    0x68,   0x3,    0x3,    0x2,    0x2,    0x2,
      0x69,  0x67,   0x3,    0x2,    0x2,    0x2,    0x6a,   0x6b,   0x7,
      0x3,   0x2,    0x2,    0x6b,   0x6c,   0x7,    0x2a,   0x2,    0x2,
      0x6c,  0x6d,   0x7,    0x30,   0x2,    0x2,    0x6d,   0x6e,   0x7,
      0x4,   0x2,    0x2,    0x6e,   0x73,   0x7,    0x2a,   0x2,    0x2,
      0x6f,  0x70,   0x7,    0x2f,   0x2,    0x2,    0x70,   0x72,   0x5,
      0x6,   0x4,    0x2,    0x71,   0x6f,   0x3,    0x2,    0x2,    0x2,
      0x72,  0x75,   0x3,    0x2,    0x2,    0x2,    0x73,   0x71,   0x3,
      0x2,   0x2,    0x2,    0x73,   0x74,   0x3,    0x2,    0x2,    0x2,
      0x74,  0x76,   0x3,    0x2,    0x2,    0x2,    0x75,   0x73,   0x3,
      0x2,   0x2,    0x2,    0x76,   0x77,   0x7,    0x31,   0x2,    0x2,
      0x77,  0x78,   0x7,    0x5,    0x2,    0x2,    0x78,   0x79,   0x5,
      0xc,   0x7,    0x2,    0x79,   0x7a,   0x7,    0x6,    0x2,    0x2,
      0x7a,  0x5,    0x3,    0x2,    0x2,    0x2,    0x7b,   0x7c,   0x5,
      0x8,   0x5,    0x2,    0x7c,   0x7d,   0x7,    0x2a,   0x2,    0x2,
      0x7d,  0x7,    0x3,    0x2,    0x2,    0x2,    0x7e,   0x7f,   0x9,
      0x2,   0x2,    0x2,    0x7f,   0x9,    0x3,    0x2,    0x2,    0x2,
      0x80,  0x81,   0x7,    0x2a,   0x2,    0x2,    0x81,   0x83,   0x7,
      0x30,  0x2,    0x2,    0x82,   0x84,   0x7,    0x2a,   0x2,    0x2,
      0x83,  0x82,   0x3,    0x2,    0x2,    0x2,    0x83,   0x84,   0x3,
      0x2,   0x2,    0x2,    0x84,   0x89,   0x3,    0x2,    0x2,    0x2,
      0x85,  0x86,   0x7,    0x2f,   0x2,    0x2,    0x86,   0x88,   0x7,
      0x2a,  0x2,    0x2,    0x87,   0x85,   0x3,    0x2,    0x2,    0x2,
      0x88,  0x8b,   0x3,    0x2,    0x2,    0x2,    0x89,   0x87,   0x3,
      0x2,   0x2,    0x2,    0x89,   0x8a,   0x3,    0x2,    0x2,    0x2,
      0x8a,  0x8c,   0x3,    0x2,    0x2,    0x2,    0x8b,   0x89,   0x3,
      0x2,   0x2,    0x2,    0x8c,   0x8d,   0x7,    0x31,   0x2,    0x2,
      0x8d,  0xb,    0x3,    0x2,    0x2,    0x2,    0x8e,   0x90,   0x5,
      0xe,   0x8,    0x2,    0x8f,   0x8e,   0x3,    0x2,    0x2,    0x2,
      0x8f,  0x90,   0x3,    0x2,    0x2,    0x2,    0x90,   0x99,   0x3,
      0x2,   0x2,    0x2,    0x91,   0x93,   0x7,    0x3a,   0x2,    0x2,
      0x92,  0x91,   0x3,    0x2,    0x2,    0x2,    0x93,   0x94,   0x3,
      0x2,   0x2,    0x2,    0x94,   0x92,   0x3,    0x2,    0x2,    0x2,
      0x94,  0x95,   0x3,    0x2,    0x2,    0x2,    0x95,   0x96,   0x3,
      0x2,   0x2,    0x2,    0x96,   0x98,   0x5,    0xe,    0x8,    0x2,
      0x97,  0x92,   0x3,    0x2,    0x2,    0x2,    0x98,   0x9b,   0x3,
      0x2,   0x2,    0x2,    0x99,   0x97,   0x3,    0x2,    0x2,    0x2,
      0x99,  0x9a,   0x3,    0x2,    0x2,    0x2,    0x9a,   0x9f,   0x3,
      0x2,   0x2,    0x2,    0x9b,   0x99,   0x3,    0x2,    0x2,    0x2,
      0x9c,  0x9e,   0x7,    0x3a,   0x2,    0x2,    0x9d,   0x9c,   0x3,
      0x2,   0x2,    0x2,    0x9e,   0xa1,   0x3,    0x2,    0x2,    0x2,
      0x9f,  0x9d,   0x3,    0x2,    0x2,    0x2,    0x9f,   0xa0,   0x3,
      0x2,   0x2,    0x2,    0xa0,   0xd,    0x3,    0x2,    0x2,    0x2,
      0xa1,  0x9f,   0x3,    0x2,    0x2,    0x2,    0xa2,   0xa7,   0x5,
      0x1a,  0xe,    0x2,    0xa3,   0xa7,   0x5,    0x22,   0x12,   0x2,
      0xa4,  0xa7,   0x5,    0x10,   0x9,    0x2,    0xa5,   0xa7,   0x5,
      0xa,   0x6,    0x2,    0xa6,   0xa2,   0x3,    0x2,    0x2,    0x2,
      0xa6,  0xa3,   0x3,    0x2,    0x2,    0x2,    0xa6,   0xa4,   0x3,
      0x2,   0x2,    0x2,    0xa6,   0xa5,   0x3,    0x2,    0x2,    0x2,
      0xa7,  0xf,    0x3,    0x2,    0x2,    0x2,    0xa8,   0xb7,   0x5,
      0x12,  0xa,    0x2,    0xa9,   0xb7,   0x5,    0x2e,   0x18,   0x2,
      0xaa,  0xb7,   0x5,    0x34,   0x1b,   0x2,    0xab,   0xb7,   0x5,
      0x38,  0x1d,   0x2,    0xac,   0xb7,   0x5,    0x3a,   0x1e,   0x2,
      0xad,  0xb7,   0x5,    0x3c,   0x1f,   0x2,    0xae,   0xb7,   0x5,
      0x3e,  0x20,   0x2,    0xaf,   0xb7,   0x5,    0x40,   0x21,   0x2,
      0xb0,  0xb7,   0x5,    0x42,   0x22,   0x2,    0xb1,   0xb7,   0x5,
      0x44,  0x23,   0x2,    0xb2,   0xb7,   0x5,    0x46,   0x24,   0x2,
      0xb3,  0xb7,   0x5,    0x48,   0x25,   0x2,    0xb4,   0xb7,   0x5,
      0x4a,  0x26,   0x2,    0xb5,   0xb7,   0x5,    0x4c,   0x27,   0x2,
      0xb6,  0xa8,   0x3,    0x2,    0x2,    0x2,    0xb6,   0xa9,   0x3,
      0x2,   0x2,    0x2,    0xb6,   0xaa,   0x3,    0x2,    0x2,    0x2,
      0xb6,  0xab,   0x3,    0x2,    0x2,    0x2,    0xb6,   0xac,   0x3,
      0x2,   0x2,    0x2,    0xb6,   0xad,   0x3,    0x2,    0x2,    0x2,
      0xb6,  0xae,   0x3,    0x2,    0x2,    0x2,    0xb6,   0xaf,   0x3,
      0x2,   0x2,    0x2,    0xb6,   0xb0,   0x3,    0x2,    0x2,    0x2,
      0xb6,  0xb1,   0x3,    0x2,    0x2,    0x2,    0xb6,   0xb2,   0x3,
      0x2,   0x2,    0x2,    0xb6,   0xb3,   0x3,    0x2,    0x2,    0x2,
      0xb6,  0xb4,   0x3,    0x2,    0x2,    0x2,    0xb6,   0xb5,   0x3,
      0x2,   0x2,    0x2,    0xb7,   0x11,   0x3,    0x2,    0x2,    0x2,
      0xb8,  0xc4,   0x5,    0x14,   0xb,    0x2,    0xb9,   0xba,   0x7,
      0x30,  0x2,    0x2,    0xba,   0xbf,   0x5,    0x18,   0xd,    0x2,
      0xbb,  0xbc,   0x7,    0x2f,   0x2,    0x2,    0xbc,   0xbe,   0x5,
      0x18,  0xd,    0x2,    0xbd,   0xbb,   0x3,    0x2,    0x2,    0x2,
      0xbe,  0xc1,   0x3,    0x2,    0x2,    0x2,    0xbf,   0xbd,   0x3,
      0x2,   0x2,    0x2,    0xbf,   0xc0,   0x3,    0x2,    0x2,    0x2,
      0xc0,  0xc2,   0x3,    0x2,    0x2,    0x2,    0xc1,   0xbf,   0x3,
      0x2,   0x2,    0x2,    0xc2,   0xc3,   0x7,    0x31,   0x2,    0x2,
      0xc3,  0xc5,   0x3,    0x2,    0x2,    0x2,    0xc4,   0xb9,   0x3,
      0x2,   0x2,    0x2,    0xc4,   0xc5,   0x3,    0x2,    0x2,    0x2,
      0xc5,  0xc7,   0x3,    0x2,    0x2,    0x2,    0xc6,   0xc8,   0x5,
      0x16,  0xc,    0x2,    0xc7,   0xc6,   0x3,    0x2,    0x2,    0x2,
      0xc8,  0xc9,   0x3,    0x2,    0x2,    0x2,    0xc9,   0xc7,   0x3,
      0x2,   0x2,    0x2,    0xc9,   0xca,   0x3,    0x2,    0x2,    0x2,
      0xca,  0x13,   0x3,    0x2,    0x2,    0x2,    0xcb,   0xcc,   0x7,
      0x2a,  0x2,    0x2,    0xcc,   0x15,   0x3,    0x2,    0x2,    0x2,
      0xcd,  0xce,   0x7,    0x2b,   0x2,    0x2,    0xce,   0x17,   0x3,
      0x2,   0x2,    0x2,    0xcf,   0xd0,   0x5,    0x50,   0x29,   0x2,
      0xd0,  0x19,   0x3,    0x2,    0x2,    0x2,    0xd1,   0xd2,   0x7,
      0xa,   0x2,    0x2,    0xd2,   0xde,   0x5,    0x14,   0xb,    0x2,
      0xd3,  0xd4,   0x7,    0x30,   0x2,    0x2,    0xd4,   0xd9,   0x5,
      0x1c,  0xf,    0x2,    0xd5,   0xd6,   0x7,    0x2f,   0x2,    0x2,
      0xd6,  0xd8,   0x5,    0x1c,   0xf,    0x2,    0xd7,   0xd5,   0x3,
      0x2,   0x2,    0x2,    0xd8,   0xdb,   0x3,    0x2,    0x2,    0x2,
      0xd9,  0xd7,   0x3,    0x2,    0x2,    0x2,    0xd9,   0xda,   0x3,
      0x2,   0x2,    0x2,    0xda,   0xdc,   0x3,    0x2,    0x2,    0x2,
      0xdb,  0xd9,   0x3,    0x2,    0x2,    0x2,    0xdc,   0xdd,   0x7,
      0x31,  0x2,    0x2,    0xdd,   0xdf,   0x3,    0x2,    0x2,    0x2,
      0xde,  0xd3,   0x3,    0x2,    0x2,    0x2,    0xde,   0xdf,   0x3,
      0x2,   0x2,    0x2,    0xdf,   0xe0,   0x3,    0x2,    0x2,    0x2,
      0xe0,  0xe1,   0x7,    0x34,   0x2,    0x2,    0xe1,   0xe2,   0x7,
      0x3a,  0x2,    0x2,    0xe2,   0xe3,   0x5,    0x1e,   0x10,   0x2,
      0xe3,  0x1b,   0x3,    0x2,    0x2,    0x2,    0xe4,   0xe5,   0x7,
      0x35,  0x2,    0x2,    0xe5,   0xe6,   0x7,    0x2a,   0x2,    0x2,
      0xe6,  0x1d,   0x3,    0x2,    0x2,    0x2,    0xe7,   0xe8,   0x5,
      0x20,  0x11,   0x2,    0xe8,   0xe9,   0x7,    0x3a,   0x2,    0x2,
      0xe9,  0xeb,   0x3,    0x2,    0x2,    0x2,    0xea,   0xe7,   0x3,
      0x2,   0x2,    0x2,    0xeb,   0xee,   0x3,    0x2,    0x2,    0x2,
      0xec,  0xea,   0x3,    0x2,    0x2,    0x2,    0xec,   0xed,   0x3,
      0x2,   0x2,    0x2,    0xed,   0xef,   0x3,    0x2,    0x2,    0x2,
      0xee,  0xec,   0x3,    0x2,    0x2,    0x2,    0xef,   0xf0,   0x5,
      0x20,  0x11,   0x2,    0xf0,   0x1f,   0x3,    0x2,    0x2,    0x2,
      0xf1,  0xf2,   0x7,    0x39,   0x2,    0x2,    0xf2,   0xf7,   0x5,
      0x50,  0x29,   0x2,    0xf3,   0xf4,   0x7,    0x2f,   0x2,    0x2,
      0xf4,  0xf6,   0x5,    0x50,   0x29,   0x2,    0xf5,   0xf3,   0x3,
      0x2,   0x2,    0x2,    0xf6,   0xf9,   0x3,    0x2,    0x2,    0x2,
      0xf7,  0xf5,   0x3,    0x2,    0x2,    0x2,    0xf7,   0xf8,   0x3,
      0x2,   0x2,    0x2,    0xf8,   0x21,   0x3,    0x2,    0x2,    0x2,
      0xf9,  0xf7,   0x3,    0x2,    0x2,    0x2,    0xfa,   0xfb,   0x7,
      0xb,   0x2,    0x2,    0xfb,   0x107,  0x5,    0x14,   0xb,    0x2,
      0xfc,  0xfd,   0x7,    0x30,   0x2,    0x2,    0xfd,   0x102,  0x5,
      0x1c,  0xf,    0x2,    0xfe,   0xff,   0x7,    0x2f,   0x2,    0x2,
      0xff,  0x101,  0x5,    0x1c,   0xf,    0x2,    0x100,  0xfe,   0x3,
      0x2,   0x2,    0x2,    0x101,  0x104,  0x3,    0x2,    0x2,    0x2,
      0x102, 0x100,  0x3,    0x2,    0x2,    0x2,    0x102,  0x103,  0x3,
      0x2,   0x2,    0x2,    0x103,  0x105,  0x3,    0x2,    0x2,    0x2,
      0x104, 0x102,  0x3,    0x2,    0x2,    0x2,    0x105,  0x106,  0x7,
      0x31,  0x2,    0x2,    0x106,  0x108,  0x3,    0x2,    0x2,    0x2,
      0x107, 0xfc,   0x3,    0x2,    0x2,    0x2,    0x107,  0x108,  0x3,
      0x2,   0x2,    0x2,    0x108,  0x10c,  0x3,    0x2,    0x2,    0x2,
      0x109, 0x10b,  0x5,    0x24,   0x13,   0x2,    0x10a,  0x109,  0x3,
      0x2,   0x2,    0x2,    0x10b,  0x10e,  0x3,    0x2,    0x2,    0x2,
      0x10c, 0x10a,  0x3,    0x2,    0x2,    0x2,    0x10c,  0x10d,  0x3,
      0x2,   0x2,    0x2,    0x10d,  0x10f,  0x3,    0x2,    0x2,    0x2,
      0x10e, 0x10c,  0x3,    0x2,    0x2,    0x2,    0x10f,  0x110,  0x7,
      0x34,  0x2,    0x2,    0x110,  0x111,  0x7,    0x3a,   0x2,    0x2,
      0x111, 0x112,  0x5,    0x2c,   0x17,   0x2,    0x112,  0x23,   0x3,
      0x2,   0x2,    0x2,    0x113,  0x114,  0x7,    0x2a,   0x2,    0x2,
      0x114, 0x25,   0x3,    0x2,    0x2,    0x2,    0x115,  0x118,  0x5,
      0x16,  0xc,    0x2,    0x116,  0x118,  0x5,    0x24,   0x13,   0x2,
      0x117, 0x115,  0x3,    0x2,    0x2,    0x2,    0x117,  0x116,  0x3,
      0x2,   0x2,    0x2,    0x118,  0x27,   0x3,    0x2,    0x2,    0x2,
      0x119, 0x125,  0x5,    0x14,   0xb,    0x2,    0x11a,  0x11b,  0x7,
      0x30,  0x2,    0x2,    0x11b,  0x120,  0x5,    0x18,   0xd,    0x2,
      0x11c, 0x11d,  0x7,    0x2f,   0x2,    0x2,    0x11d,  0x11f,  0x5,
      0x18,  0xd,    0x2,    0x11e,  0x11c,  0x3,    0x2,    0x2,    0x2,
      0x11f, 0x122,  0x3,    0x2,    0x2,    0x2,    0x120,  0x11e,  0x3,
      0x2,   0x2,    0x2,    0x120,  0x121,  0x3,    0x2,    0x2,    0x2,
      0x121, 0x123,  0x3,    0x2,    0x2,    0x2,    0x122,  0x120,  0x3,
      0x2,   0x2,    0x2,    0x123,  0x124,  0x7,    0x31,   0x2,    0x2,
      0x124, 0x126,  0x3,    0x2,    0x2,    0x2,    0x125,  0x11a,  0x3,
      0x2,   0x2,    0x2,    0x125,  0x126,  0x3,    0x2,    0x2,    0x2,
      0x126, 0x128,  0x3,    0x2,    0x2,    0x2,    0x127,  0x129,  0x5,
      0x26,  0x14,   0x2,    0x128,  0x127,  0x3,    0x2,    0x2,    0x2,
      0x129, 0x12a,  0x3,    0x2,    0x2,    0x2,    0x12a,  0x128,  0x3,
      0x2,   0x2,    0x2,    0x12a,  0x12b,  0x3,    0x2,    0x2,    0x2,
      0x12b, 0x29,   0x3,    0x2,    0x2,    0x2,    0x12c,  0x12f,  0x5,
      0x28,  0x15,   0x2,    0x12d,  0x12f,  0x5,    0x10,   0x9,    0x2,
      0x12e, 0x12c,  0x3,    0x2,    0x2,    0x2,    0x12e,  0x12d,  0x3,
      0x2,   0x2,    0x2,    0x12f,  0x2b,   0x3,    0x2,    0x2,    0x2,
      0x130, 0x131,  0x7,    0x39,   0x2,    0x2,    0x131,  0x132,  0x5,
      0x2a,  0x16,   0x2,    0x132,  0x133,  0x7,    0x3a,   0x2,    0x2,
      0x133, 0x135,  0x3,    0x2,    0x2,    0x2,    0x134,  0x130,  0x3,
      0x2,   0x2,    0x2,    0x135,  0x138,  0x3,    0x2,    0x2,    0x2,
      0x136, 0x134,  0x3,    0x2,    0x2,    0x2,    0x136,  0x137,  0x3,
      0x2,   0x2,    0x2,    0x137,  0x139,  0x3,    0x2,    0x2,    0x2,
      0x138, 0x136,  0x3,    0x2,    0x2,    0x2,    0x139,  0x13a,  0x7,
      0x39,  0x2,    0x2,    0x13a,  0x13b,  0x5,    0x2a,   0x16,   0x2,
      0x13b, 0x2d,   0x3,    0x2,    0x2,    0x2,    0x13c,  0x13d,  0x7,
      0xc,   0x2,    0x2,    0x13d,  0x13f,  0x5,    0x16,   0xc,    0x2,
      0x13e, 0x140,  0x5,    0x30,   0x19,   0x2,    0x13f,  0x13e,  0x3,
      0x2,   0x2,    0x2,    0x13f,  0x140,  0x3,    0x2,    0x2,    0x2,
      0x140, 0x2f,   0x3,    0x2,    0x2,    0x2,    0x141,  0x142,  0x7,
      0x32,  0x2,    0x2,    0x142,  0x143,  0x5,    0x32,   0x1a,   0x2,
      0x143, 0x144,  0x7,    0x33,   0x2,    0x2,    0x144,  0x31,   0x3,
      0x2,   0x2,    0x2,    0x145,  0x147,  0x7,    0x2b,   0x2,    0x2,
      0x146, 0x145,  0x3,    0x2,    0x2,    0x2,    0x147,  0x148,  0x3,
      0x2,   0x2,    0x2,    0x148,  0x146,  0x3,    0x2,    0x2,    0x2,
      0x148, 0x149,  0x3,    0x2,    0x2,    0x2,    0x149,  0x33,   0x3,
      0x2,   0x2,    0x2,    0x14a,  0x14b,  0x7,    0xd,    0x2,    0x2,
      0x14b, 0x14c,  0x5,    0x36,   0x1c,   0x2,    0x14c,  0x35,   0x3,
      0x2,   0x2,    0x2,    0x14d,  0x14e,  0x7,    0x36,   0x2,    0x2,
      0x14e, 0x14f,  0x7,    0x2a,   0x2,    0x2,    0x14f,  0x37,   0x3,
      0x2,   0x2,    0x2,    0x150,  0x151,  0x7,    0xe,    0x2,    0x2,
      0x151, 0x39,   0x3,    0x2,    0x2,    0x2,    0x152,  0x153,  0x7,
      0xf,   0x2,    0x2,    0x153,  0x154,  0x5,    0x36,   0x1c,   0x2,
      0x154, 0x3b,   0x3,    0x2,    0x2,    0x2,    0x155,  0x156,  0x7,
      0x10,  0x2,    0x2,    0x156,  0x157,  0x5,    0x36,   0x1c,   0x2,
      0x157, 0x158,  0x5,    0x30,   0x19,   0x2,    0x158,  0x3d,   0x3,
      0x2,   0x2,    0x2,    0x159,  0x15a,  0x7,    0x11,   0x2,    0x2,
      0x15a, 0x15b,  0x5,    0x36,   0x1c,   0x2,    0x15b,  0x15c,  0x5,
      0x30,  0x19,   0x2,    0x15c,  0x3f,   0x3,    0x2,    0x2,    0x2,
      0x15d, 0x15e,  0x7,    0x12,   0x2,    0x2,    0x15e,  0x41,   0x3,
      0x2,   0x2,    0x2,    0x15f,  0x160,  0x7,    0x13,   0x2,    0x2,
      0x160, 0x43,   0x3,    0x2,    0x2,    0x2,    0x161,  0x162,  0x9,
      0x3,   0x2,    0x2,    0x162,  0x163,  0x5,    0x30,   0x19,   0x2,
      0x163, 0x45,   0x3,    0x2,    0x2,    0x2,    0x164,  0x165,  0x9,
      0x4,   0x2,    0x2,    0x165,  0x166,  0x5,    0x30,   0x19,   0x2,
      0x166, 0x167,  0x5,    0x30,   0x19,   0x2,    0x167,  0x47,   0x3,
      0x2,   0x2,    0x2,    0x168,  0x169,  0x7,    0x14,   0x2,    0x2,
      0x169, 0x49,   0x3,    0x2,    0x2,    0x2,    0x16a,  0x16b,  0x7,
      0x15,  0x2,    0x2,    0x16b,  0x16c,  0x7,    0x2d,   0x2,    0x2,
      0x16c, 0x4b,   0x3,    0x2,    0x2,    0x2,    0x16d,  0x16e,  0x7,
      0x16,  0x2,    0x2,    0x16e,  0x172,  0x7,    0x2a,   0x2,    0x2,
      0x16f, 0x171,  0x5,    0x4e,   0x28,   0x2,    0x170,  0x16f,  0x3,
      0x2,   0x2,    0x2,    0x171,  0x174,  0x3,    0x2,    0x2,    0x2,
      0x172, 0x170,  0x3,    0x2,    0x2,    0x2,    0x172,  0x173,  0x3,
      0x2,   0x2,    0x2,    0x173,  0x176,  0x3,    0x2,    0x2,    0x2,
      0x174, 0x172,  0x3,    0x2,    0x2,    0x2,    0x175,  0x177,  0x7,
      0x2d,  0x2,    0x2,    0x176,  0x175,  0x3,    0x2,    0x2,    0x2,
      0x176, 0x177,  0x3,    0x2,    0x2,    0x2,    0x177,  0x4d,   0x3,
      0x2,   0x2,    0x2,    0x178,  0x179,  0x9,    0x5,    0x2,    0x2,
      0x179, 0x4f,   0x3,    0x2,    0x2,    0x2,    0x17a,  0x17b,  0x8,
      0x29,  0x1,    0x2,    0x17b,  0x17c,  0x7,    0x30,   0x2,    0x2,
      0x17c, 0x17d,  0x5,    0x50,   0x29,   0x2,    0x17d,  0x17e,  0x7,
      0x31,  0x2,    0x2,    0x17e,  0x18c,  0x3,    0x2,    0x2,    0x2,
      0x17f, 0x180,  0x5,    0x56,   0x2c,   0x2,    0x180,  0x181,  0x5,
      0x50,  0x29,   0xb,    0x181,  0x18c,  0x3,    0x2,    0x2,    0x2,
      0x182, 0x183,  0x5,    0x54,   0x2b,   0x2,    0x183,  0x184,  0x7,
      0x30,  0x2,    0x2,    0x184,  0x185,  0x5,    0x50,   0x29,   0x2,
      0x185, 0x186,  0x7,    0x31,   0x2,    0x2,    0x186,  0x18c,  0x3,
      0x2,   0x2,    0x2,    0x187,  0x18c,  0x5,    0x52,   0x2a,   0x2,
      0x188, 0x18c,  0x5,    0x58,   0x2d,   0x2,    0x189,  0x18c,  0x5,
      0x1c,  0xf,    0x2,    0x18a,  0x18c,  0x7,    0x2a,   0x2,    0x2,
      0x18b, 0x17a,  0x3,    0x2,    0x2,    0x2,    0x18b,  0x17f,  0x3,
      0x2,   0x2,    0x2,    0x18b,  0x182,  0x3,    0x2,    0x2,    0x2,
      0x18b, 0x187,  0x3,    0x2,    0x2,    0x2,    0x18b,  0x188,  0x3,
      0x2,   0x2,    0x2,    0x18b,  0x189,  0x3,    0x2,    0x2,    0x2,
      0x18b, 0x18a,  0x3,    0x2,    0x2,    0x2,    0x18c,  0x198,  0x3,
      0x2,   0x2,    0x2,    0x18d,  0x18e,  0xc,    0xa,    0x2,    0x2,
      0x18e, 0x18f,  0x7,    0x29,   0x2,    0x2,    0x18f,  0x197,  0x5,
      0x50,  0x29,   0xa,    0x190,  0x191,  0xc,    0x9,    0x2,    0x2,
      0x191, 0x192,  0x9,    0x6,    0x2,    0x2,    0x192,  0x197,  0x5,
      0x50,  0x29,   0xa,    0x193,  0x194,  0xc,    0x8,    0x2,    0x2,
      0x194, 0x195,  0x9,    0x7,    0x2,    0x2,    0x195,  0x197,  0x5,
      0x50,  0x29,   0x9,    0x196,  0x18d,  0x3,    0x2,    0x2,    0x2,
      0x196, 0x190,  0x3,    0x2,    0x2,    0x2,    0x196,  0x193,  0x3,
      0x2,   0x2,    0x2,    0x197,  0x19a,  0x3,    0x2,    0x2,    0x2,
      0x198, 0x196,  0x3,    0x2,    0x2,    0x2,    0x198,  0x199,  0x3,
      0x2,   0x2,    0x2,    0x199,  0x51,   0x3,    0x2,    0x2,    0x2,
      0x19a, 0x198,  0x3,    0x2,    0x2,    0x2,    0x19b,  0x19c,  0x7,
      0x32,  0x2,    0x2,    0x19c,  0x19d,  0x7,    0x2b,   0x2,    0x2,
      0x19d, 0x19e,  0x7,    0x26,   0x2,    0x2,    0x19e,  0x19f,  0x7,
      0x2b,  0x2,    0x2,    0x19f,  0x1a0,  0x7,    0x33,   0x2,    0x2,
      0x1a0, 0x53,   0x3,    0x2,    0x2,    0x2,    0x1a1,  0x1a2,  0x9,
      0x8,   0x2,    0x2,    0x1a2,  0x55,   0x3,    0x2,    0x2,    0x2,
      0x1a3, 0x1a4,  0x9,    0x7,    0x2,    0x2,    0x1a4,  0x57,   0x3,
      0x2,   0x2,    0x2,    0x1a5,  0x1aa,  0x5,    0x5c,   0x2f,   0x2,
      0x1a6, 0x1aa,  0x5,    0x5a,   0x2e,   0x2,    0x1a7,  0x1aa,  0x7,
      0x1f,  0x2,    0x2,    0x1a8,  0x1aa,  0x7,    0x1e,   0x2,    0x2,
      0x1a9, 0x1a5,  0x3,    0x2,    0x2,    0x2,    0x1a9,  0x1a6,  0x3,
      0x2,   0x2,    0x2,    0x1a9,  0x1a7,  0x3,    0x2,    0x2,    0x2,
      0x1a9, 0x1a8,  0x3,    0x2,    0x2,    0x2,    0x1aa,  0x59,   0x3,
      0x2,   0x2,    0x2,    0x1ab,  0x1ac,  0x5,    0x5c,   0x2f,   0x2,
      0x1ac, 0x1ad,  0x7,    0x1f,   0x2,    0x2,    0x1ad,  0x5b,   0x3,
      0x2,   0x2,    0x2,    0x1ae,  0x1af,  0x9,    0x9,    0x2,    0x2,
      0x1af, 0x5d,   0x3,    0x2,    0x2,    0x2,    0x25,   0x62,   0x67,
      0x73,  0x83,   0x89,   0x8f,   0x94,   0x99,   0x9f,   0xa6,   0xb6,
      0xbf,  0xc4,   0xc9,   0xd9,   0xde,   0xec,   0xf7,   0x102,  0x107,
      0x10c, 0x117,  0x120,  0x125,  0x12a,  0x12e,  0x136,  0x13f,  0x148,
      0x172, 0x176,  0x18b,  0x196,  0x198,  0x1a9,
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
