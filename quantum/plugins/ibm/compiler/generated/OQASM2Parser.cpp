
// Generated from OQASM2.g4 by ANTLR 4.7.1


#include "OQASM2Listener.h"

#include "OQASM2Parser.h"


using namespace antlrcpp;
using namespace oqasm;
using namespace antlr4;

OQASM2Parser::OQASM2Parser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

OQASM2Parser::~OQASM2Parser() {
  delete _interpreter;
}

std::string OQASM2Parser::getGrammarFileName() const {
  return "OQASM2.g4";
}

const std::vector<std::string>& OQASM2Parser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& OQASM2Parser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

OQASM2Parser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::XacckernelContext *> OQASM2Parser::XaccsrcContext::xacckernel() {
  return getRuleContexts<OQASM2Parser::XacckernelContext>();
}

OQASM2Parser::XacckernelContext* OQASM2Parser::XaccsrcContext::xacckernel(size_t i) {
  return getRuleContext<OQASM2Parser::XacckernelContext>(i);
}


size_t OQASM2Parser::XaccsrcContext::getRuleIndex() const {
  return OQASM2Parser::RuleXaccsrc;
}

void OQASM2Parser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void OQASM2Parser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

OQASM2Parser::XaccsrcContext* OQASM2Parser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, OQASM2Parser::RuleXaccsrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(93);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::T__0) {
      setState(90);
      xacckernel();
      setState(95);
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

//----------------- XacckernelContext ------------------------------------------------------------------

OQASM2Parser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::MainprogContext* OQASM2Parser::XacckernelContext::mainprog() {
  return getRuleContext<OQASM2Parser::MainprogContext>(0);
}

std::vector<OQASM2Parser::IdContext *> OQASM2Parser::XacckernelContext::id() {
  return getRuleContexts<OQASM2Parser::IdContext>();
}

OQASM2Parser::IdContext* OQASM2Parser::XacckernelContext::id(size_t i) {
  return getRuleContext<OQASM2Parser::IdContext>(i);
}

std::vector<OQASM2Parser::TypedparamContext *> OQASM2Parser::XacckernelContext::typedparam() {
  return getRuleContexts<OQASM2Parser::TypedparamContext>();
}

OQASM2Parser::TypedparamContext* OQASM2Parser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<OQASM2Parser::TypedparamContext>(i);
}


size_t OQASM2Parser::XacckernelContext::getRuleIndex() const {
  return OQASM2Parser::RuleXacckernel;
}

void OQASM2Parser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void OQASM2Parser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

OQASM2Parser::XacckernelContext* OQASM2Parser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, OQASM2Parser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(96);
    match(OQASM2Parser::T__0);
    setState(97);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(98);
    match(OQASM2Parser::T__1);
    setState(99);
    match(OQASM2Parser::T__2);
    setState(100);
    dynamic_cast<XacckernelContext *>(_localctx)->acceleratorbuffer = id();
    setState(105);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::T__3) {
      setState(101);
      match(OQASM2Parser::T__3);
      setState(102);
      typedparam();
      setState(107);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(108);
    match(OQASM2Parser::T__4);
    setState(109);
    match(OQASM2Parser::T__5);
    setState(110);
    mainprog();
    setState(111);
    match(OQASM2Parser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypedparamContext ------------------------------------------------------------------

OQASM2Parser::TypedparamContext::TypedparamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::TypeContext* OQASM2Parser::TypedparamContext::type() {
  return getRuleContext<OQASM2Parser::TypeContext>(0);
}

OQASM2Parser::ParamContext* OQASM2Parser::TypedparamContext::param() {
  return getRuleContext<OQASM2Parser::ParamContext>(0);
}


size_t OQASM2Parser::TypedparamContext::getRuleIndex() const {
  return OQASM2Parser::RuleTypedparam;
}

void OQASM2Parser::TypedparamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypedparam(this);
}

void OQASM2Parser::TypedparamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypedparam(this);
}

OQASM2Parser::TypedparamContext* OQASM2Parser::typedparam() {
  TypedparamContext *_localctx = _tracker.createInstance<TypedparamContext>(_ctx, getState());
  enterRule(_localctx, 4, OQASM2Parser::RuleTypedparam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    type();
    setState(114);
    param();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

OQASM2Parser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OQASM2Parser::TypeContext::getRuleIndex() const {
  return OQASM2Parser::RuleType;
}

void OQASM2Parser::TypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType(this);
}

void OQASM2Parser::TypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType(this);
}

OQASM2Parser::TypeContext* OQASM2Parser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 6, OQASM2Parser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(116);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << OQASM2Parser::T__7)
      | (1ULL << OQASM2Parser::T__8)
      | (1ULL << OQASM2Parser::T__9))) != 0))) {
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

//----------------- KernelcallContext ------------------------------------------------------------------

OQASM2Parser::KernelcallContext::KernelcallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::KernelcallContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

std::vector<OQASM2Parser::ParamContext *> OQASM2Parser::KernelcallContext::param() {
  return getRuleContexts<OQASM2Parser::ParamContext>();
}

OQASM2Parser::ParamContext* OQASM2Parser::KernelcallContext::param(size_t i) {
  return getRuleContext<OQASM2Parser::ParamContext>(i);
}


size_t OQASM2Parser::KernelcallContext::getRuleIndex() const {
  return OQASM2Parser::RuleKernelcall;
}

void OQASM2Parser::KernelcallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKernelcall(this);
}

void OQASM2Parser::KernelcallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKernelcall(this);
}

OQASM2Parser::KernelcallContext* OQASM2Parser::kernelcall() {
  KernelcallContext *_localctx = _tracker.createInstance<KernelcallContext>(_ctx, getState());
  enterRule(_localctx, 8, OQASM2Parser::RuleKernelcall);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(118);
    dynamic_cast<KernelcallContext *>(_localctx)->kernelname = id();
    setState(119);
    match(OQASM2Parser::T__1);
    setState(121);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == OQASM2Parser::ID) {
      setState(120);
      param();
    }
    setState(127);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::T__3) {
      setState(123);
      match(OQASM2Parser::T__3);
      setState(124);
      param();
      setState(129);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(130);
    match(OQASM2Parser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MainprogContext ------------------------------------------------------------------

OQASM2Parser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::MainprogContext::OPENQASM() {
  return getToken(OQASM2Parser::OPENQASM, 0);
}

OQASM2Parser::RealContext* OQASM2Parser::MainprogContext::real() {
  return getRuleContext<OQASM2Parser::RealContext>(0);
}

std::vector<OQASM2Parser::CommentContext *> OQASM2Parser::MainprogContext::comment() {
  return getRuleContexts<OQASM2Parser::CommentContext>();
}

OQASM2Parser::CommentContext* OQASM2Parser::MainprogContext::comment(size_t i) {
  return getRuleContext<OQASM2Parser::CommentContext>(i);
}

OQASM2Parser::ProgramContext* OQASM2Parser::MainprogContext::program() {
  return getRuleContext<OQASM2Parser::ProgramContext>(0);
}


size_t OQASM2Parser::MainprogContext::getRuleIndex() const {
  return OQASM2Parser::RuleMainprog;
}

void OQASM2Parser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void OQASM2Parser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

OQASM2Parser::MainprogContext* OQASM2Parser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 10, OQASM2Parser::RuleMainprog);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(135);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::COMMENT) {
      setState(132);
      comment();
      setState(137);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(138);
    match(OQASM2Parser::OPENQASM);
    setState(139);
    real();
    setState(140);
    match(OQASM2Parser::T__10);
    setState(142);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << OQASM2Parser::T__11)
      | (1ULL << OQASM2Parser::T__14)
      | (1ULL << OQASM2Parser::T__15)
      | (1ULL << OQASM2Parser::T__16)
      | (1ULL << OQASM2Parser::COMMENT)
      | (1ULL << OQASM2Parser::QREG)
      | (1ULL << OQASM2Parser::CREG)
      | (1ULL << OQASM2Parser::GATE)
      | (1ULL << OQASM2Parser::MEASURE)
      | (1ULL << OQASM2Parser::RESET)
      | (1ULL << OQASM2Parser::BARRIER)
      | (1ULL << OQASM2Parser::OPAQUE)
      | (1ULL << OQASM2Parser::ID))) != 0)) {
      setState(141);
      program();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProgramContext ------------------------------------------------------------------

OQASM2Parser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::LineContext *> OQASM2Parser::ProgramContext::line() {
  return getRuleContexts<OQASM2Parser::LineContext>();
}

OQASM2Parser::LineContext* OQASM2Parser::ProgramContext::line(size_t i) {
  return getRuleContext<OQASM2Parser::LineContext>(i);
}


size_t OQASM2Parser::ProgramContext::getRuleIndex() const {
  return OQASM2Parser::RuleProgram;
}

void OQASM2Parser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void OQASM2Parser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

OQASM2Parser::ProgramContext* OQASM2Parser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 12, OQASM2Parser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(145); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(144);
      line();
      setState(147); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << OQASM2Parser::T__11)
      | (1ULL << OQASM2Parser::T__14)
      | (1ULL << OQASM2Parser::T__15)
      | (1ULL << OQASM2Parser::T__16)
      | (1ULL << OQASM2Parser::COMMENT)
      | (1ULL << OQASM2Parser::QREG)
      | (1ULL << OQASM2Parser::CREG)
      | (1ULL << OQASM2Parser::GATE)
      | (1ULL << OQASM2Parser::MEASURE)
      | (1ULL << OQASM2Parser::RESET)
      | (1ULL << OQASM2Parser::BARRIER)
      | (1ULL << OQASM2Parser::OPAQUE)
      | (1ULL << OQASM2Parser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

OQASM2Parser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::StatementContext *> OQASM2Parser::LineContext::statement() {
  return getRuleContexts<OQASM2Parser::StatementContext>();
}

OQASM2Parser::StatementContext* OQASM2Parser::LineContext::statement(size_t i) {
  return getRuleContext<OQASM2Parser::StatementContext>(i);
}

OQASM2Parser::CommentContext* OQASM2Parser::LineContext::comment() {
  return getRuleContext<OQASM2Parser::CommentContext>(0);
}

std::vector<OQASM2Parser::IncludeContext *> OQASM2Parser::LineContext::include() {
  return getRuleContexts<OQASM2Parser::IncludeContext>();
}

OQASM2Parser::IncludeContext* OQASM2Parser::LineContext::include(size_t i) {
  return getRuleContext<OQASM2Parser::IncludeContext>(i);
}


size_t OQASM2Parser::LineContext::getRuleIndex() const {
  return OQASM2Parser::RuleLine;
}

void OQASM2Parser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void OQASM2Parser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

OQASM2Parser::LineContext* OQASM2Parser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 14, OQASM2Parser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(160);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OQASM2Parser::T__14:
      case OQASM2Parser::T__15:
      case OQASM2Parser::T__16:
      case OQASM2Parser::QREG:
      case OQASM2Parser::CREG:
      case OQASM2Parser::GATE:
      case OQASM2Parser::MEASURE:
      case OQASM2Parser::RESET:
      case OQASM2Parser::BARRIER:
      case OQASM2Parser::OPAQUE:
      case OQASM2Parser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(150); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(149);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(152); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case OQASM2Parser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(154);
        comment();
        break;
      }

      case OQASM2Parser::T__11: {
        enterOuterAlt(_localctx, 3);
        setState(156); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(155);
                  include();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(158); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
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

//----------------- StatementContext ------------------------------------------------------------------

OQASM2Parser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::RegdeclContext* OQASM2Parser::StatementContext::regdecl() {
  return getRuleContext<OQASM2Parser::RegdeclContext>(0);
}

OQASM2Parser::GatedeclContext* OQASM2Parser::StatementContext::gatedecl() {
  return getRuleContext<OQASM2Parser::GatedeclContext>(0);
}

OQASM2Parser::OpaqueContext* OQASM2Parser::StatementContext::opaque() {
  return getRuleContext<OQASM2Parser::OpaqueContext>(0);
}

OQASM2Parser::QopContext* OQASM2Parser::StatementContext::qop() {
  return getRuleContext<OQASM2Parser::QopContext>(0);
}

OQASM2Parser::ConditionalContext* OQASM2Parser::StatementContext::conditional() {
  return getRuleContext<OQASM2Parser::ConditionalContext>(0);
}

OQASM2Parser::KernelcallContext* OQASM2Parser::StatementContext::kernelcall() {
  return getRuleContext<OQASM2Parser::KernelcallContext>(0);
}


size_t OQASM2Parser::StatementContext::getRuleIndex() const {
  return OQASM2Parser::RuleStatement;
}

void OQASM2Parser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void OQASM2Parser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

OQASM2Parser::StatementContext* OQASM2Parser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 16, OQASM2Parser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(178);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(162);
      regdecl();
      setState(163);
      match(OQASM2Parser::T__10);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(165);
      gatedecl();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(166);
      opaque();
      setState(167);
      match(OQASM2Parser::T__10);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(169);
      qop();
      setState(170);
      match(OQASM2Parser::T__10);
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(172);
      conditional();
      setState(173);
      match(OQASM2Parser::T__10);
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(175);
      kernelcall();
      setState(176);
      match(OQASM2Parser::T__10);
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

OQASM2Parser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::CommentContext::COMMENT() {
  return getToken(OQASM2Parser::COMMENT, 0);
}


size_t OQASM2Parser::CommentContext::getRuleIndex() const {
  return OQASM2Parser::RuleComment;
}

void OQASM2Parser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void OQASM2Parser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

OQASM2Parser::CommentContext* OQASM2Parser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 18, OQASM2Parser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(180);
    match(OQASM2Parser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext ------------------------------------------------------------------

OQASM2Parser::IncludeContext::IncludeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::FilenameContext* OQASM2Parser::IncludeContext::filename() {
  return getRuleContext<OQASM2Parser::FilenameContext>(0);
}


size_t OQASM2Parser::IncludeContext::getRuleIndex() const {
  return OQASM2Parser::RuleInclude;
}

void OQASM2Parser::IncludeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInclude(this);
}

void OQASM2Parser::IncludeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInclude(this);
}

OQASM2Parser::IncludeContext* OQASM2Parser::include() {
  IncludeContext *_localctx = _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 20, OQASM2Parser::RuleInclude);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(182);
    match(OQASM2Parser::T__11);
    setState(183);
    filename();
    setState(184);
    match(OQASM2Parser::T__10);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FilenameContext ------------------------------------------------------------------

OQASM2Parser::FilenameContext::FilenameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::StringContext* OQASM2Parser::FilenameContext::string() {
  return getRuleContext<OQASM2Parser::StringContext>(0);
}


size_t OQASM2Parser::FilenameContext::getRuleIndex() const {
  return OQASM2Parser::RuleFilename;
}

void OQASM2Parser::FilenameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFilename(this);
}

void OQASM2Parser::FilenameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFilename(this);
}

OQASM2Parser::FilenameContext* OQASM2Parser::filename() {
  FilenameContext *_localctx = _tracker.createInstance<FilenameContext>(_ctx, getState());
  enterRule(_localctx, 22, OQASM2Parser::RuleFilename);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(186);
    string();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RegdeclContext ------------------------------------------------------------------

OQASM2Parser::RegdeclContext::RegdeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::QregContext* OQASM2Parser::RegdeclContext::qreg() {
  return getRuleContext<OQASM2Parser::QregContext>(0);
}

OQASM2Parser::QregisterContext* OQASM2Parser::RegdeclContext::qregister() {
  return getRuleContext<OQASM2Parser::QregisterContext>(0);
}

OQASM2Parser::CregContext* OQASM2Parser::RegdeclContext::creg() {
  return getRuleContext<OQASM2Parser::CregContext>(0);
}

OQASM2Parser::CregisterContext* OQASM2Parser::RegdeclContext::cregister() {
  return getRuleContext<OQASM2Parser::CregisterContext>(0);
}


size_t OQASM2Parser::RegdeclContext::getRuleIndex() const {
  return OQASM2Parser::RuleRegdecl;
}

void OQASM2Parser::RegdeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRegdecl(this);
}

void OQASM2Parser::RegdeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRegdecl(this);
}

OQASM2Parser::RegdeclContext* OQASM2Parser::regdecl() {
  RegdeclContext *_localctx = _tracker.createInstance<RegdeclContext>(_ctx, getState());
  enterRule(_localctx, 24, OQASM2Parser::RuleRegdecl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(194);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OQASM2Parser::QREG: {
        enterOuterAlt(_localctx, 1);
        setState(188);
        qreg();
        setState(189);
        qregister();
        break;
      }

      case OQASM2Parser::CREG: {
        enterOuterAlt(_localctx, 2);
        setState(191);
        creg();
        setState(192);
        cregister();
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

//----------------- QregisterContext ------------------------------------------------------------------

OQASM2Parser::QregisterContext::QregisterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::QregisterContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

OQASM2Parser::RegistersizeContext* OQASM2Parser::QregisterContext::registersize() {
  return getRuleContext<OQASM2Parser::RegistersizeContext>(0);
}


size_t OQASM2Parser::QregisterContext::getRuleIndex() const {
  return OQASM2Parser::RuleQregister;
}

void OQASM2Parser::QregisterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQregister(this);
}

void OQASM2Parser::QregisterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQregister(this);
}

OQASM2Parser::QregisterContext* OQASM2Parser::qregister() {
  QregisterContext *_localctx = _tracker.createInstance<QregisterContext>(_ctx, getState());
  enterRule(_localctx, 26, OQASM2Parser::RuleQregister);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(202);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(196);
      id();
      setState(197);
      match(OQASM2Parser::T__12);
      setState(198);
      registersize();
      setState(199);
      match(OQASM2Parser::T__13);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(201);
      id();
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

//----------------- CregisterContext ------------------------------------------------------------------

OQASM2Parser::CregisterContext::CregisterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::CregisterContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

OQASM2Parser::RegistersizeContext* OQASM2Parser::CregisterContext::registersize() {
  return getRuleContext<OQASM2Parser::RegistersizeContext>(0);
}


size_t OQASM2Parser::CregisterContext::getRuleIndex() const {
  return OQASM2Parser::RuleCregister;
}

void OQASM2Parser::CregisterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCregister(this);
}

void OQASM2Parser::CregisterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCregister(this);
}

OQASM2Parser::CregisterContext* OQASM2Parser::cregister() {
  CregisterContext *_localctx = _tracker.createInstance<CregisterContext>(_ctx, getState());
  enterRule(_localctx, 28, OQASM2Parser::RuleCregister);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(210);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(204);
      id();
      setState(205);
      match(OQASM2Parser::T__12);
      setState(206);
      registersize();
      setState(207);
      match(OQASM2Parser::T__13);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(209);
      id();
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

//----------------- RegistersizeContext ------------------------------------------------------------------

OQASM2Parser::RegistersizeContext::RegistersizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::RegistersizeContext::INT() {
  return getToken(OQASM2Parser::INT, 0);
}


size_t OQASM2Parser::RegistersizeContext::getRuleIndex() const {
  return OQASM2Parser::RuleRegistersize;
}

void OQASM2Parser::RegistersizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRegistersize(this);
}

void OQASM2Parser::RegistersizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRegistersize(this);
}

OQASM2Parser::RegistersizeContext* OQASM2Parser::registersize() {
  RegistersizeContext *_localctx = _tracker.createInstance<RegistersizeContext>(_ctx, getState());
  enterRule(_localctx, 30, OQASM2Parser::RuleRegistersize);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(212);
    match(OQASM2Parser::INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GatedeclContext ------------------------------------------------------------------

OQASM2Parser::GatedeclContext::GatedeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::GateContext* OQASM2Parser::GatedeclContext::gate() {
  return getRuleContext<OQASM2Parser::GateContext>(0);
}

OQASM2Parser::GatenameContext* OQASM2Parser::GatedeclContext::gatename() {
  return getRuleContext<OQASM2Parser::GatenameContext>(0);
}

OQASM2Parser::GatearglistContext* OQASM2Parser::GatedeclContext::gatearglist() {
  return getRuleContext<OQASM2Parser::GatearglistContext>(0);
}

OQASM2Parser::GatebodyContext* OQASM2Parser::GatedeclContext::gatebody() {
  return getRuleContext<OQASM2Parser::GatebodyContext>(0);
}


size_t OQASM2Parser::GatedeclContext::getRuleIndex() const {
  return OQASM2Parser::RuleGatedecl;
}

void OQASM2Parser::GatedeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatedecl(this);
}

void OQASM2Parser::GatedeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatedecl(this);
}

OQASM2Parser::GatedeclContext* OQASM2Parser::gatedecl() {
  GatedeclContext *_localctx = _tracker.createInstance<GatedeclContext>(_ctx, getState());
  enterRule(_localctx, 32, OQASM2Parser::RuleGatedecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(214);
    gate();
    setState(215);
    gatename();
    setState(216);
    gatearglist();
    setState(218);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == OQASM2Parser::T__5) {
      setState(217);
      gatebody();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GatenameContext ------------------------------------------------------------------

OQASM2Parser::GatenameContext::GatenameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::GatenameContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

OQASM2Parser::ParamlistContext* OQASM2Parser::GatenameContext::paramlist() {
  return getRuleContext<OQASM2Parser::ParamlistContext>(0);
}


size_t OQASM2Parser::GatenameContext::getRuleIndex() const {
  return OQASM2Parser::RuleGatename;
}

void OQASM2Parser::GatenameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatename(this);
}

void OQASM2Parser::GatenameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatename(this);
}

OQASM2Parser::GatenameContext* OQASM2Parser::gatename() {
  GatenameContext *_localctx = _tracker.createInstance<GatenameContext>(_ctx, getState());
  enterRule(_localctx, 34, OQASM2Parser::RuleGatename);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(228);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(220);
      id();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(221);
      id();
      setState(222);
      match(OQASM2Parser::T__1);
      setState(224);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == OQASM2Parser::ID) {
        setState(223);
        paramlist();
      }
      setState(226);
      match(OQASM2Parser::T__4);
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

//----------------- GatearglistContext ------------------------------------------------------------------

OQASM2Parser::GatearglistContext::GatearglistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::GateargContext *> OQASM2Parser::GatearglistContext::gatearg() {
  return getRuleContexts<OQASM2Parser::GateargContext>();
}

OQASM2Parser::GateargContext* OQASM2Parser::GatearglistContext::gatearg(size_t i) {
  return getRuleContext<OQASM2Parser::GateargContext>(i);
}


size_t OQASM2Parser::GatearglistContext::getRuleIndex() const {
  return OQASM2Parser::RuleGatearglist;
}

void OQASM2Parser::GatearglistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatearglist(this);
}

void OQASM2Parser::GatearglistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatearglist(this);
}

OQASM2Parser::GatearglistContext* OQASM2Parser::gatearglist() {
  GatearglistContext *_localctx = _tracker.createInstance<GatearglistContext>(_ctx, getState());
  enterRule(_localctx, 36, OQASM2Parser::RuleGatearglist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(230);
    gatearg();
    setState(235);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::T__3) {
      setState(231);
      match(OQASM2Parser::T__3);
      setState(232);
      gatearg();
      setState(237);
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

//----------------- GateargContext ------------------------------------------------------------------

OQASM2Parser::GateargContext::GateargContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::GateargContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

tree::TerminalNode* OQASM2Parser::GateargContext::INT() {
  return getToken(OQASM2Parser::INT, 0);
}


size_t OQASM2Parser::GateargContext::getRuleIndex() const {
  return OQASM2Parser::RuleGatearg;
}

void OQASM2Parser::GateargContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatearg(this);
}

void OQASM2Parser::GateargContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatearg(this);
}

OQASM2Parser::GateargContext* OQASM2Parser::gatearg() {
  GateargContext *_localctx = _tracker.createInstance<GateargContext>(_ctx, getState());
  enterRule(_localctx, 38, OQASM2Parser::RuleGatearg);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(244);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(238);
      id();
      setState(239);
      match(OQASM2Parser::T__12);
      setState(240);
      match(OQASM2Parser::INT);
      setState(241);
      match(OQASM2Parser::T__13);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(243);
      id();
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

//----------------- GatebodyContext ------------------------------------------------------------------

OQASM2Parser::GatebodyContext::GatebodyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::GateprogContext* OQASM2Parser::GatebodyContext::gateprog() {
  return getRuleContext<OQASM2Parser::GateprogContext>(0);
}


size_t OQASM2Parser::GatebodyContext::getRuleIndex() const {
  return OQASM2Parser::RuleGatebody;
}

void OQASM2Parser::GatebodyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGatebody(this);
}

void OQASM2Parser::GatebodyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGatebody(this);
}

OQASM2Parser::GatebodyContext* OQASM2Parser::gatebody() {
  GatebodyContext *_localctx = _tracker.createInstance<GatebodyContext>(_ctx, getState());
  enterRule(_localctx, 40, OQASM2Parser::RuleGatebody);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(246);
    match(OQASM2Parser::T__5);
    setState(247);
    gateprog();
    setState(248);
    match(OQASM2Parser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GateprogContext ------------------------------------------------------------------

OQASM2Parser::GateprogContext::GateprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::GatelineContext *> OQASM2Parser::GateprogContext::gateline() {
  return getRuleContexts<OQASM2Parser::GatelineContext>();
}

OQASM2Parser::GatelineContext* OQASM2Parser::GateprogContext::gateline(size_t i) {
  return getRuleContext<OQASM2Parser::GatelineContext>(i);
}


size_t OQASM2Parser::GateprogContext::getRuleIndex() const {
  return OQASM2Parser::RuleGateprog;
}

void OQASM2Parser::GateprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGateprog(this);
}

void OQASM2Parser::GateprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGateprog(this);
}

OQASM2Parser::GateprogContext* OQASM2Parser::gateprog() {
  GateprogContext *_localctx = _tracker.createInstance<GateprogContext>(_ctx, getState());
  enterRule(_localctx, 42, OQASM2Parser::RuleGateprog);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(253);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << OQASM2Parser::T__14)
      | (1ULL << OQASM2Parser::T__15)
      | (1ULL << OQASM2Parser::ID))) != 0)) {
      setState(250);
      gateline();
      setState(255);
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

//----------------- GatelineContext ------------------------------------------------------------------

OQASM2Parser::GatelineContext::GatelineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::UopContext* OQASM2Parser::GatelineContext::uop() {
  return getRuleContext<OQASM2Parser::UopContext>(0);
}


size_t OQASM2Parser::GatelineContext::getRuleIndex() const {
  return OQASM2Parser::RuleGateline;
}

void OQASM2Parser::GatelineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGateline(this);
}

void OQASM2Parser::GatelineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGateline(this);
}

OQASM2Parser::GatelineContext* OQASM2Parser::gateline() {
  GatelineContext *_localctx = _tracker.createInstance<GatelineContext>(_ctx, getState());
  enterRule(_localctx, 44, OQASM2Parser::RuleGateline);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(256);
    uop();
    setState(257);
    match(OQASM2Parser::T__10);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamlistContext ------------------------------------------------------------------

OQASM2Parser::ParamlistContext::ParamlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::ParamContext* OQASM2Parser::ParamlistContext::param() {
  return getRuleContext<OQASM2Parser::ParamContext>(0);
}

OQASM2Parser::ParamlistContext* OQASM2Parser::ParamlistContext::paramlist() {
  return getRuleContext<OQASM2Parser::ParamlistContext>(0);
}


size_t OQASM2Parser::ParamlistContext::getRuleIndex() const {
  return OQASM2Parser::RuleParamlist;
}

void OQASM2Parser::ParamlistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParamlist(this);
}

void OQASM2Parser::ParamlistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParamlist(this);
}

OQASM2Parser::ParamlistContext* OQASM2Parser::paramlist() {
  ParamlistContext *_localctx = _tracker.createInstance<ParamlistContext>(_ctx, getState());
  enterRule(_localctx, 46, OQASM2Parser::RuleParamlist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(259);
    param();
    setState(262);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == OQASM2Parser::T__3) {
      setState(260);
      match(OQASM2Parser::T__3);
      setState(261);
      paramlist();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext ------------------------------------------------------------------

OQASM2Parser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::ParamContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}


size_t OQASM2Parser::ParamContext::getRuleIndex() const {
  return OQASM2Parser::RuleParam;
}

void OQASM2Parser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void OQASM2Parser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}

OQASM2Parser::ParamContext* OQASM2Parser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 48, OQASM2Parser::RuleParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(264);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OpaqueContext ------------------------------------------------------------------

OQASM2Parser::OpaqueContext::OpaqueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::OpaqueContext::OPAQUE() {
  return getToken(OQASM2Parser::OPAQUE, 0);
}

OQASM2Parser::OpaquenameContext* OQASM2Parser::OpaqueContext::opaquename() {
  return getRuleContext<OQASM2Parser::OpaquenameContext>(0);
}

OQASM2Parser::OpaquearglistContext* OQASM2Parser::OpaqueContext::opaquearglist() {
  return getRuleContext<OQASM2Parser::OpaquearglistContext>(0);
}


size_t OQASM2Parser::OpaqueContext::getRuleIndex() const {
  return OQASM2Parser::RuleOpaque;
}

void OQASM2Parser::OpaqueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOpaque(this);
}

void OQASM2Parser::OpaqueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOpaque(this);
}

OQASM2Parser::OpaqueContext* OQASM2Parser::opaque() {
  OpaqueContext *_localctx = _tracker.createInstance<OpaqueContext>(_ctx, getState());
  enterRule(_localctx, 50, OQASM2Parser::RuleOpaque);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(266);
    match(OQASM2Parser::OPAQUE);
    setState(267);
    opaquename();
    setState(268);
    opaquearglist();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OpaquenameContext ------------------------------------------------------------------

OQASM2Parser::OpaquenameContext::OpaquenameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::OpaquenameContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

OQASM2Parser::ParamlistContext* OQASM2Parser::OpaquenameContext::paramlist() {
  return getRuleContext<OQASM2Parser::ParamlistContext>(0);
}


size_t OQASM2Parser::OpaquenameContext::getRuleIndex() const {
  return OQASM2Parser::RuleOpaquename;
}

void OQASM2Parser::OpaquenameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOpaquename(this);
}

void OQASM2Parser::OpaquenameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOpaquename(this);
}

OQASM2Parser::OpaquenameContext* OQASM2Parser::opaquename() {
  OpaquenameContext *_localctx = _tracker.createInstance<OpaquenameContext>(_ctx, getState());
  enterRule(_localctx, 52, OQASM2Parser::RuleOpaquename);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(278);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(270);
      id();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(271);
      id();
      setState(272);
      match(OQASM2Parser::T__1);
      setState(274);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == OQASM2Parser::ID) {
        setState(273);
        paramlist();
      }
      setState(276);
      match(OQASM2Parser::T__4);
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

//----------------- OpaquearglistContext ------------------------------------------------------------------

OQASM2Parser::OpaquearglistContext::OpaquearglistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::OpaqueargContext* OQASM2Parser::OpaquearglistContext::opaquearg() {
  return getRuleContext<OQASM2Parser::OpaqueargContext>(0);
}

OQASM2Parser::OpaquearglistContext* OQASM2Parser::OpaquearglistContext::opaquearglist() {
  return getRuleContext<OQASM2Parser::OpaquearglistContext>(0);
}


size_t OQASM2Parser::OpaquearglistContext::getRuleIndex() const {
  return OQASM2Parser::RuleOpaquearglist;
}

void OQASM2Parser::OpaquearglistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOpaquearglist(this);
}

void OQASM2Parser::OpaquearglistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOpaquearglist(this);
}

OQASM2Parser::OpaquearglistContext* OQASM2Parser::opaquearglist() {
  OpaquearglistContext *_localctx = _tracker.createInstance<OpaquearglistContext>(_ctx, getState());
  enterRule(_localctx, 54, OQASM2Parser::RuleOpaquearglist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(280);
    opaquearg();
    setState(283);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == OQASM2Parser::T__3) {
      setState(281);
      match(OQASM2Parser::T__3);
      setState(282);
      opaquearglist();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OpaqueargContext ------------------------------------------------------------------

OQASM2Parser::OpaqueargContext::OpaqueargContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::OpaqueargContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

tree::TerminalNode* OQASM2Parser::OpaqueargContext::INT() {
  return getToken(OQASM2Parser::INT, 0);
}


size_t OQASM2Parser::OpaqueargContext::getRuleIndex() const {
  return OQASM2Parser::RuleOpaquearg;
}

void OQASM2Parser::OpaqueargContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOpaquearg(this);
}

void OQASM2Parser::OpaqueargContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOpaquearg(this);
}

OQASM2Parser::OpaqueargContext* OQASM2Parser::opaquearg() {
  OpaqueargContext *_localctx = _tracker.createInstance<OpaqueargContext>(_ctx, getState());
  enterRule(_localctx, 56, OQASM2Parser::RuleOpaquearg);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(285);
    id();
    setState(286);
    match(OQASM2Parser::T__12);
    setState(287);
    match(OQASM2Parser::INT);
    setState(288);
    match(OQASM2Parser::T__13);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QopContext ------------------------------------------------------------------

OQASM2Parser::QopContext::QopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::UopContext* OQASM2Parser::QopContext::uop() {
  return getRuleContext<OQASM2Parser::UopContext>(0);
}

OQASM2Parser::ReesetContext* OQASM2Parser::QopContext::reeset() {
  return getRuleContext<OQASM2Parser::ReesetContext>(0);
}

OQASM2Parser::MeasureContext* OQASM2Parser::QopContext::measure() {
  return getRuleContext<OQASM2Parser::MeasureContext>(0);
}

OQASM2Parser::BarrierContext* OQASM2Parser::QopContext::barrier() {
  return getRuleContext<OQASM2Parser::BarrierContext>(0);
}


size_t OQASM2Parser::QopContext::getRuleIndex() const {
  return OQASM2Parser::RuleQop;
}

void OQASM2Parser::QopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQop(this);
}

void OQASM2Parser::QopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQop(this);
}

OQASM2Parser::QopContext* OQASM2Parser::qop() {
  QopContext *_localctx = _tracker.createInstance<QopContext>(_ctx, getState());
  enterRule(_localctx, 58, OQASM2Parser::RuleQop);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(294);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OQASM2Parser::T__14:
      case OQASM2Parser::T__15:
      case OQASM2Parser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(290);
        uop();
        break;
      }

      case OQASM2Parser::RESET: {
        enterOuterAlt(_localctx, 2);
        setState(291);
        reeset();
        break;
      }

      case OQASM2Parser::MEASURE: {
        enterOuterAlt(_localctx, 3);
        setState(292);
        measure();
        break;
      }

      case OQASM2Parser::BARRIER: {
        enterOuterAlt(_localctx, 4);
        setState(293);
        barrier();
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

//----------------- UopContext ------------------------------------------------------------------

OQASM2Parser::UopContext::UopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OQASM2Parser::UopContext::getRuleIndex() const {
  return OQASM2Parser::RuleUop;
}

void OQASM2Parser::UopContext::copyFrom(UopContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- UContext ------------------------------------------------------------------

OQASM2Parser::GateargContext* OQASM2Parser::UContext::gatearg() {
  return getRuleContext<OQASM2Parser::GateargContext>(0);
}

OQASM2Parser::ExplistContext* OQASM2Parser::UContext::explist() {
  return getRuleContext<OQASM2Parser::ExplistContext>(0);
}

OQASM2Parser::UContext::UContext(UopContext *ctx) { copyFrom(ctx); }

void OQASM2Parser::UContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterU(this);
}
void OQASM2Parser::UContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitU(this);
}
//----------------- CXContext ------------------------------------------------------------------

std::vector<OQASM2Parser::GateargContext *> OQASM2Parser::CXContext::gatearg() {
  return getRuleContexts<OQASM2Parser::GateargContext>();
}

OQASM2Parser::GateargContext* OQASM2Parser::CXContext::gatearg(size_t i) {
  return getRuleContext<OQASM2Parser::GateargContext>(i);
}

OQASM2Parser::CXContext::CXContext(UopContext *ctx) { copyFrom(ctx); }

void OQASM2Parser::CXContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCX(this);
}
void OQASM2Parser::CXContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCX(this);
}
//----------------- UserDefGateContext ------------------------------------------------------------------

OQASM2Parser::GatenameContext* OQASM2Parser::UserDefGateContext::gatename() {
  return getRuleContext<OQASM2Parser::GatenameContext>(0);
}

OQASM2Parser::GatearglistContext* OQASM2Parser::UserDefGateContext::gatearglist() {
  return getRuleContext<OQASM2Parser::GatearglistContext>(0);
}

OQASM2Parser::ExplistContext* OQASM2Parser::UserDefGateContext::explist() {
  return getRuleContext<OQASM2Parser::ExplistContext>(0);
}

OQASM2Parser::UserDefGateContext::UserDefGateContext(UopContext *ctx) { copyFrom(ctx); }

void OQASM2Parser::UserDefGateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUserDefGate(this);
}
void OQASM2Parser::UserDefGateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUserDefGate(this);
}
OQASM2Parser::UopContext* OQASM2Parser::uop() {
  UopContext *_localctx = _tracker.createInstance<UopContext>(_ctx, getState());
  enterRule(_localctx, 60, OQASM2Parser::RuleUop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(317);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OQASM2Parser::T__14: {
        _localctx = dynamic_cast<UopContext *>(_tracker.createInstance<OQASM2Parser::UContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(296);
        match(OQASM2Parser::T__14);
        setState(297);
        match(OQASM2Parser::T__1);
        setState(299);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << OQASM2Parser::T__1)
          | (1ULL << OQASM2Parser::T__18)
          | (1ULL << OQASM2Parser::T__20)
          | (1ULL << OQASM2Parser::T__24)
          | (1ULL << OQASM2Parser::T__25)
          | (1ULL << OQASM2Parser::T__26)
          | (1ULL << OQASM2Parser::T__27)
          | (1ULL << OQASM2Parser::T__28)
          | (1ULL << OQASM2Parser::T__29)
          | (1ULL << OQASM2Parser::ID)
          | (1ULL << OQASM2Parser::REAL)
          | (1ULL << OQASM2Parser::INT))) != 0)) {
          setState(298);
          explist();
        }
        setState(301);
        match(OQASM2Parser::T__4);
        setState(302);
        gatearg();
        break;
      }

      case OQASM2Parser::T__15: {
        _localctx = dynamic_cast<UopContext *>(_tracker.createInstance<OQASM2Parser::CXContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(303);
        match(OQASM2Parser::T__15);
        setState(304);
        gatearg();
        setState(305);
        match(OQASM2Parser::T__3);
        setState(306);
        gatearg();
        break;
      }

      case OQASM2Parser::ID: {
        _localctx = dynamic_cast<UopContext *>(_tracker.createInstance<OQASM2Parser::UserDefGateContext>(_localctx));
        enterOuterAlt(_localctx, 3);
        setState(308);
        gatename();
        setState(313);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == OQASM2Parser::T__1) {
          setState(309);
          match(OQASM2Parser::T__1);
          setState(310);
          explist();
          setState(311);
          match(OQASM2Parser::T__4);
        }
        setState(315);
        gatearglist();
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

//----------------- ConditionalContext ------------------------------------------------------------------

OQASM2Parser::ConditionalContext::ConditionalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::IdContext* OQASM2Parser::ConditionalContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

tree::TerminalNode* OQASM2Parser::ConditionalContext::INT() {
  return getToken(OQASM2Parser::INT, 0);
}

OQASM2Parser::ActionContext* OQASM2Parser::ConditionalContext::action() {
  return getRuleContext<OQASM2Parser::ActionContext>(0);
}


size_t OQASM2Parser::ConditionalContext::getRuleIndex() const {
  return OQASM2Parser::RuleConditional;
}

void OQASM2Parser::ConditionalContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConditional(this);
}

void OQASM2Parser::ConditionalContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConditional(this);
}

OQASM2Parser::ConditionalContext* OQASM2Parser::conditional() {
  ConditionalContext *_localctx = _tracker.createInstance<ConditionalContext>(_ctx, getState());
  enterRule(_localctx, 62, OQASM2Parser::RuleConditional);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(319);
    match(OQASM2Parser::T__16);
    setState(320);
    match(OQASM2Parser::T__1);
    setState(321);
    id();
    setState(322);
    match(OQASM2Parser::T__17);
    setState(323);
    match(OQASM2Parser::INT);
    setState(324);
    match(OQASM2Parser::T__4);
    setState(325);
    action();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ActionContext ------------------------------------------------------------------

OQASM2Parser::ActionContext::ActionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::QopContext* OQASM2Parser::ActionContext::qop() {
  return getRuleContext<OQASM2Parser::QopContext>(0);
}


size_t OQASM2Parser::ActionContext::getRuleIndex() const {
  return OQASM2Parser::RuleAction;
}

void OQASM2Parser::ActionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAction(this);
}

void OQASM2Parser::ActionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAction(this);
}

OQASM2Parser::ActionContext* OQASM2Parser::action() {
  ActionContext *_localctx = _tracker.createInstance<ActionContext>(_ctx, getState());
  enterRule(_localctx, 64, OQASM2Parser::RuleAction);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(327);
    qop();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExplistContext ------------------------------------------------------------------

OQASM2Parser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OQASM2Parser::ExpContext *> OQASM2Parser::ExplistContext::exp() {
  return getRuleContexts<OQASM2Parser::ExpContext>();
}

OQASM2Parser::ExpContext* OQASM2Parser::ExplistContext::exp(size_t i) {
  return getRuleContext<OQASM2Parser::ExpContext>(i);
}


size_t OQASM2Parser::ExplistContext::getRuleIndex() const {
  return OQASM2Parser::RuleExplist;
}

void OQASM2Parser::ExplistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExplist(this);
}

void OQASM2Parser::ExplistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExplist(this);
}

OQASM2Parser::ExplistContext* OQASM2Parser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 66, OQASM2Parser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(329);
    exp(0);
    setState(334);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OQASM2Parser::T__3) {
      setState(330);
      match(OQASM2Parser::T__3);
      setState(331);
      exp(0);
      setState(336);
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

//----------------- ExpContext ------------------------------------------------------------------

OQASM2Parser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OQASM2Parser::RealContext* OQASM2Parser::ExpContext::real() {
  return getRuleContext<OQASM2Parser::RealContext>(0);
}

tree::TerminalNode* OQASM2Parser::ExpContext::INT() {
  return getToken(OQASM2Parser::INT, 0);
}

OQASM2Parser::IdContext* OQASM2Parser::ExpContext::id() {
  return getRuleContext<OQASM2Parser::IdContext>(0);
}

std::vector<OQASM2Parser::ExpContext *> OQASM2Parser::ExpContext::exp() {
  return getRuleContexts<OQASM2Parser::ExpContext>();
}

OQASM2Parser::ExpContext* OQASM2Parser::ExpContext::exp(size_t i) {
  return getRuleContext<OQASM2Parser::ExpContext>(i);
}

OQASM2Parser::UnaryopContext* OQASM2Parser::ExpContext::unaryop() {
  return getRuleContext<OQASM2Parser::UnaryopContext>(0);
}


size_t OQASM2Parser::ExpContext::getRuleIndex() const {
  return OQASM2Parser::RuleExp;
}

void OQASM2Parser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void OQASM2Parser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


OQASM2Parser::ExpContext* OQASM2Parser::exp() {
   return exp(0);
}

OQASM2Parser::ExpContext* OQASM2Parser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  OQASM2Parser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  OQASM2Parser::ExpContext *previousContext = _localctx;
  size_t startState = 68;
  enterRecursionRule(_localctx, 68, OQASM2Parser::RuleExp, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(353);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OQASM2Parser::REAL: {
        setState(338);
        real();
        break;
      }

      case OQASM2Parser::INT: {
        setState(339);
        match(OQASM2Parser::INT);
        break;
      }

      case OQASM2Parser::T__18: {
        setState(340);
        match(OQASM2Parser::T__18);
        break;
      }

      case OQASM2Parser::ID: {
        setState(341);
        id();
        break;
      }

      case OQASM2Parser::T__20: {
        setState(342);
        match(OQASM2Parser::T__20);
        setState(343);
        exp(4);
        break;
      }

      case OQASM2Parser::T__1: {
        setState(344);
        match(OQASM2Parser::T__1);
        setState(345);
        exp(0);
        setState(346);
        match(OQASM2Parser::T__4);
        break;
      }

      case OQASM2Parser::T__24:
      case OQASM2Parser::T__25:
      case OQASM2Parser::T__26:
      case OQASM2Parser::T__27:
      case OQASM2Parser::T__28:
      case OQASM2Parser::T__29: {
        setState(348);
        unaryop();
        setState(349);
        match(OQASM2Parser::T__1);
        setState(350);
        exp(0);
        setState(351);
        match(OQASM2Parser::T__4);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(372);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(370);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 30, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(355);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(356);
          match(OQASM2Parser::T__19);
          setState(357);
          exp(9);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(358);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(359);
          match(OQASM2Parser::T__20);
          setState(360);
          exp(8);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(361);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(362);
          match(OQASM2Parser::T__21);
          setState(363);
          exp(7);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(364);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(365);
          match(OQASM2Parser::T__22);
          setState(366);
          exp(6);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(367);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(368);
          match(OQASM2Parser::T__23);
          setState(369);
          exp(4);
          break;
        }

        } 
      }
      setState(374);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 31, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- UnaryopContext ------------------------------------------------------------------

OQASM2Parser::UnaryopContext::UnaryopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OQASM2Parser::UnaryopContext::getRuleIndex() const {
  return OQASM2Parser::RuleUnaryop;
}

void OQASM2Parser::UnaryopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryop(this);
}

void OQASM2Parser::UnaryopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryop(this);
}

OQASM2Parser::UnaryopContext* OQASM2Parser::unaryop() {
  UnaryopContext *_localctx = _tracker.createInstance<UnaryopContext>(_ctx, getState());
  enterRule(_localctx, 70, OQASM2Parser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(375);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << OQASM2Parser::T__24)
      | (1ULL << OQASM2Parser::T__25)
      | (1ULL << OQASM2Parser::T__26)
      | (1ULL << OQASM2Parser::T__27)
      | (1ULL << OQASM2Parser::T__28)
      | (1ULL << OQASM2Parser::T__29))) != 0))) {
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

//----------------- QregContext ------------------------------------------------------------------

OQASM2Parser::QregContext::QregContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::QregContext::QREG() {
  return getToken(OQASM2Parser::QREG, 0);
}


size_t OQASM2Parser::QregContext::getRuleIndex() const {
  return OQASM2Parser::RuleQreg;
}

void OQASM2Parser::QregContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQreg(this);
}

void OQASM2Parser::QregContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQreg(this);
}

OQASM2Parser::QregContext* OQASM2Parser::qreg() {
  QregContext *_localctx = _tracker.createInstance<QregContext>(_ctx, getState());
  enterRule(_localctx, 72, OQASM2Parser::RuleQreg);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(377);
    match(OQASM2Parser::QREG);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CregContext ------------------------------------------------------------------

OQASM2Parser::CregContext::CregContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::CregContext::CREG() {
  return getToken(OQASM2Parser::CREG, 0);
}


size_t OQASM2Parser::CregContext::getRuleIndex() const {
  return OQASM2Parser::RuleCreg;
}

void OQASM2Parser::CregContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCreg(this);
}

void OQASM2Parser::CregContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCreg(this);
}

OQASM2Parser::CregContext* OQASM2Parser::creg() {
  CregContext *_localctx = _tracker.createInstance<CregContext>(_ctx, getState());
  enterRule(_localctx, 74, OQASM2Parser::RuleCreg);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(379);
    match(OQASM2Parser::CREG);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GateContext ------------------------------------------------------------------

OQASM2Parser::GateContext::GateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::GateContext::GATE() {
  return getToken(OQASM2Parser::GATE, 0);
}


size_t OQASM2Parser::GateContext::getRuleIndex() const {
  return OQASM2Parser::RuleGate;
}

void OQASM2Parser::GateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGate(this);
}

void OQASM2Parser::GateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGate(this);
}

OQASM2Parser::GateContext* OQASM2Parser::gate() {
  GateContext *_localctx = _tracker.createInstance<GateContext>(_ctx, getState());
  enterRule(_localctx, 76, OQASM2Parser::RuleGate);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(381);
    match(OQASM2Parser::GATE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MeasureContext ------------------------------------------------------------------

OQASM2Parser::MeasureContext::MeasureContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::MeasureContext::MEASURE() {
  return getToken(OQASM2Parser::MEASURE, 0);
}

OQASM2Parser::QregisterContext* OQASM2Parser::MeasureContext::qregister() {
  return getRuleContext<OQASM2Parser::QregisterContext>(0);
}

OQASM2Parser::CregisterContext* OQASM2Parser::MeasureContext::cregister() {
  return getRuleContext<OQASM2Parser::CregisterContext>(0);
}

std::vector<OQASM2Parser::GateargContext *> OQASM2Parser::MeasureContext::gatearg() {
  return getRuleContexts<OQASM2Parser::GateargContext>();
}

OQASM2Parser::GateargContext* OQASM2Parser::MeasureContext::gatearg(size_t i) {
  return getRuleContext<OQASM2Parser::GateargContext>(i);
}


size_t OQASM2Parser::MeasureContext::getRuleIndex() const {
  return OQASM2Parser::RuleMeasure;
}

void OQASM2Parser::MeasureContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMeasure(this);
}

void OQASM2Parser::MeasureContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMeasure(this);
}

OQASM2Parser::MeasureContext* OQASM2Parser::measure() {
  MeasureContext *_localctx = _tracker.createInstance<MeasureContext>(_ctx, getState());
  enterRule(_localctx, 78, OQASM2Parser::RuleMeasure);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(383);
    match(OQASM2Parser::MEASURE);
    setState(386);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 32, _ctx)) {
    case 1: {
      setState(384);
      dynamic_cast<MeasureContext *>(_localctx)->qbit = gatearg();
      break;
    }

    case 2: {
      setState(385);
      qregister();
      break;
    }

    }
    setState(388);
    match(OQASM2Parser::T__30);
    setState(391);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      setState(389);
      dynamic_cast<MeasureContext *>(_localctx)->cbit = gatearg();
      break;
    }

    case 2: {
      setState(390);
      cregister();
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

//----------------- ReesetContext ------------------------------------------------------------------

OQASM2Parser::ReesetContext::ReesetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::ReesetContext::RESET() {
  return getToken(OQASM2Parser::RESET, 0);
}

OQASM2Parser::GateargContext* OQASM2Parser::ReesetContext::gatearg() {
  return getRuleContext<OQASM2Parser::GateargContext>(0);
}

OQASM2Parser::QregisterContext* OQASM2Parser::ReesetContext::qregister() {
  return getRuleContext<OQASM2Parser::QregisterContext>(0);
}

OQASM2Parser::CregisterContext* OQASM2Parser::ReesetContext::cregister() {
  return getRuleContext<OQASM2Parser::CregisterContext>(0);
}


size_t OQASM2Parser::ReesetContext::getRuleIndex() const {
  return OQASM2Parser::RuleReeset;
}

void OQASM2Parser::ReesetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReeset(this);
}

void OQASM2Parser::ReesetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReeset(this);
}

OQASM2Parser::ReesetContext* OQASM2Parser::reeset() {
  ReesetContext *_localctx = _tracker.createInstance<ReesetContext>(_ctx, getState());
  enterRule(_localctx, 80, OQASM2Parser::RuleReeset);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(393);
    match(OQASM2Parser::RESET);
    setState(397);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 34, _ctx)) {
    case 1: {
      setState(394);
      gatearg();
      break;
    }

    case 2: {
      setState(395);
      qregister();
      break;
    }

    case 3: {
      setState(396);
      cregister();
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

//----------------- BarrierContext ------------------------------------------------------------------

OQASM2Parser::BarrierContext::BarrierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::BarrierContext::BARRIER() {
  return getToken(OQASM2Parser::BARRIER, 0);
}

OQASM2Parser::GatearglistContext* OQASM2Parser::BarrierContext::gatearglist() {
  return getRuleContext<OQASM2Parser::GatearglistContext>(0);
}


size_t OQASM2Parser::BarrierContext::getRuleIndex() const {
  return OQASM2Parser::RuleBarrier;
}

void OQASM2Parser::BarrierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBarrier(this);
}

void OQASM2Parser::BarrierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBarrier(this);
}

OQASM2Parser::BarrierContext* OQASM2Parser::barrier() {
  BarrierContext *_localctx = _tracker.createInstance<BarrierContext>(_ctx, getState());
  enterRule(_localctx, 82, OQASM2Parser::RuleBarrier);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(399);
    match(OQASM2Parser::BARRIER);
    setState(400);
    gatearglist();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdContext ------------------------------------------------------------------

OQASM2Parser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::IdContext::ID() {
  return getToken(OQASM2Parser::ID, 0);
}


size_t OQASM2Parser::IdContext::getRuleIndex() const {
  return OQASM2Parser::RuleId;
}

void OQASM2Parser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void OQASM2Parser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

OQASM2Parser::IdContext* OQASM2Parser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 84, OQASM2Parser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(402);
    match(OQASM2Parser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

OQASM2Parser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::RealContext::REAL() {
  return getToken(OQASM2Parser::REAL, 0);
}


size_t OQASM2Parser::RealContext::getRuleIndex() const {
  return OQASM2Parser::RuleReal;
}

void OQASM2Parser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void OQASM2Parser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

OQASM2Parser::RealContext* OQASM2Parser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 86, OQASM2Parser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(404);
    match(OQASM2Parser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

OQASM2Parser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OQASM2Parser::StringContext::STRING() {
  return getToken(OQASM2Parser::STRING, 0);
}


size_t OQASM2Parser::StringContext::getRuleIndex() const {
  return OQASM2Parser::RuleString;
}

void OQASM2Parser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void OQASM2Parser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OQASM2Listener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

OQASM2Parser::StringContext* OQASM2Parser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 88, OQASM2Parser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(406);
    match(OQASM2Parser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool OQASM2Parser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 34: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool OQASM2Parser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 8);
    case 1: return precpred(_ctx, 7);
    case 2: return precpred(_ctx, 6);
    case 3: return precpred(_ctx, 5);
    case 4: return precpred(_ctx, 3);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> OQASM2Parser::_decisionToDFA;
atn::PredictionContextCache OQASM2Parser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN OQASM2Parser::_atn;
std::vector<uint16_t> OQASM2Parser::_serializedATN;

std::vector<std::string> OQASM2Parser::_ruleNames = {
  "xaccsrc", "xacckernel", "typedparam", "type", "kernelcall", "mainprog", 
  "program", "line", "statement", "comment", "include", "filename", "regdecl", 
  "qregister", "cregister", "registersize", "gatedecl", "gatename", "gatearglist", 
  "gatearg", "gatebody", "gateprog", "gateline", "paramlist", "param", "opaque", 
  "opaquename", "opaquearglist", "opaquearg", "qop", "uop", "conditional", 
  "action", "explist", "exp", "unaryop", "qreg", "creg", "gate", "measure", 
  "reeset", "barrier", "id", "real", "string"
};

std::vector<std::string> OQASM2Parser::_literalNames = {
  "", "'__qpu__'", "'('", "'AcceleratorBuffer'", "','", "')'", "'{'", "'}'", 
  "'int'", "'double'", "'float'", "';'", "'include'", "'['", "']'", "'U'", 
  "'CX'", "'if'", "'=='", "'pi'", "'+'", "'-'", "'*'", "'/'", "'^'", "'sin'", 
  "'cos'", "'tan'", "'exp'", "'ln'", "'sqrt'", "'->'", "", "'qreg'", "'creg'", 
  "'gate'", "'measure'", "'reset'", "'barrier'", "", "'opaque'"
};

std::vector<std::string> OQASM2Parser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "COMMENT", "QREG", 
  "CREG", "GATE", "MEASURE", "RESET", "BARRIER", "OPENQASM", "OPAQUE", "ID", 
  "REAL", "INT", "STRING", "WS", "EOL"
};

dfa::Vocabulary OQASM2Parser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> OQASM2Parser::_tokenNames;

OQASM2Parser::Initializer::Initializer() {
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
    0x3, 0x30, 0x19b, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
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
    0x2c, 0x9, 0x2c, 0x4, 0x2d, 0x9, 0x2d, 0x4, 0x2e, 0x9, 0x2e, 0x3, 0x2, 
    0x7, 0x2, 0x5e, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x61, 0xb, 0x2, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 
    0x6a, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x6d, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 0x7c, 0xa, 0x6, 0x3, 
    0x6, 0x3, 0x6, 0x7, 0x6, 0x80, 0xa, 0x6, 0xc, 0x6, 0xe, 0x6, 0x83, 0xb, 
    0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x7, 0x7, 0x88, 0xa, 0x7, 0xc, 0x7, 
    0xe, 0x7, 0x8b, 0xb, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 
    0x7, 0x91, 0xa, 0x7, 0x3, 0x8, 0x6, 0x8, 0x94, 0xa, 0x8, 0xd, 0x8, 0xe, 
    0x8, 0x95, 0x3, 0x9, 0x6, 0x9, 0x99, 0xa, 0x9, 0xd, 0x9, 0xe, 0x9, 0x9a, 
    0x3, 0x9, 0x3, 0x9, 0x6, 0x9, 0x9f, 0xa, 0x9, 0xd, 0x9, 0xe, 0x9, 0xa0, 
    0x5, 0x9, 0xa3, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 0xb5, 0xa, 0xa, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xd, 
    0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x5, 0xe, 0xc5, 0xa, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 
    0xf, 0x3, 0xf, 0x5, 0xf, 0xcd, 0xa, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 
    0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x5, 0x10, 0xd5, 0xa, 0x10, 0x3, 
    0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x5, 0x12, 
    0xdd, 0xa, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 
    0xe3, 0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xe7, 0xa, 0x13, 0x3, 
    0x14, 0x3, 0x14, 0x3, 0x14, 0x7, 0x14, 0xec, 0xa, 0x14, 0xc, 0x14, 0xe, 
    0x14, 0xef, 0xb, 0x14, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 
    0x15, 0x3, 0x15, 0x5, 0x15, 0xf7, 0xa, 0x15, 0x3, 0x16, 0x3, 0x16, 0x3, 
    0x16, 0x3, 0x16, 0x3, 0x17, 0x7, 0x17, 0xfe, 0xa, 0x17, 0xc, 0x17, 0xe, 
    0x17, 0x101, 0xb, 0x17, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x109, 0xa, 0x19, 0x3, 0x1a, 0x3, 0x1a, 
    0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 0x3, 
    0x1c, 0x3, 0x1c, 0x5, 0x1c, 0x115, 0xa, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 
    0x5, 0x1c, 0x119, 0xa, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x5, 0x1d, 
    0x11e, 0xa, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 
    0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x5, 0x1f, 0x129, 0xa, 0x1f, 
    0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x5, 0x20, 0x12e, 0xa, 0x20, 0x3, 0x20, 
    0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 
    0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x5, 0x20, 0x13c, 
    0xa, 0x20, 0x3, 0x20, 0x3, 0x20, 0x5, 0x20, 0x140, 0xa, 0x20, 0x3, 0x21, 
    0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 
    0x21, 0x3, 0x22, 0x3, 0x22, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x7, 0x23, 
    0x14f, 0xa, 0x23, 0xc, 0x23, 0xe, 0x23, 0x152, 0xb, 0x23, 0x3, 0x24, 
    0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 
    0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 
    0x3, 0x24, 0x3, 0x24, 0x5, 0x24, 0x164, 0xa, 0x24, 0x3, 0x24, 0x3, 0x24, 
    0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 
    0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 
    0x7, 0x24, 0x175, 0xa, 0x24, 0xc, 0x24, 0xe, 0x24, 0x178, 0xb, 0x24, 
    0x3, 0x25, 0x3, 0x25, 0x3, 0x26, 0x3, 0x26, 0x3, 0x27, 0x3, 0x27, 0x3, 
    0x28, 0x3, 0x28, 0x3, 0x29, 0x3, 0x29, 0x3, 0x29, 0x5, 0x29, 0x185, 
    0xa, 0x29, 0x3, 0x29, 0x3, 0x29, 0x3, 0x29, 0x5, 0x29, 0x18a, 0xa, 0x29, 
    0x3, 0x2a, 0x3, 0x2a, 0x3, 0x2a, 0x3, 0x2a, 0x5, 0x2a, 0x190, 0xa, 0x2a, 
    0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2d, 0x3, 
    0x2d, 0x3, 0x2e, 0x3, 0x2e, 0x3, 0x2e, 0x2, 0x3, 0x46, 0x2f, 0x2, 0x4, 
    0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 
    0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 
    0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 
    0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x2, 0x4, 0x3, 0x2, 0xa, 0xc, 0x3, 
    0x2, 0x1b, 0x20, 0x2, 0x1a1, 0x2, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x4, 0x62, 
    0x3, 0x2, 0x2, 0x2, 0x6, 0x73, 0x3, 0x2, 0x2, 0x2, 0x8, 0x76, 0x3, 0x2, 
    0x2, 0x2, 0xa, 0x78, 0x3, 0x2, 0x2, 0x2, 0xc, 0x89, 0x3, 0x2, 0x2, 0x2, 
    0xe, 0x93, 0x3, 0x2, 0x2, 0x2, 0x10, 0xa2, 0x3, 0x2, 0x2, 0x2, 0x12, 
    0xb4, 0x3, 0x2, 0x2, 0x2, 0x14, 0xb6, 0x3, 0x2, 0x2, 0x2, 0x16, 0xb8, 
    0x3, 0x2, 0x2, 0x2, 0x18, 0xbc, 0x3, 0x2, 0x2, 0x2, 0x1a, 0xc4, 0x3, 
    0x2, 0x2, 0x2, 0x1c, 0xcc, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xd4, 0x3, 0x2, 
    0x2, 0x2, 0x20, 0xd6, 0x3, 0x2, 0x2, 0x2, 0x22, 0xd8, 0x3, 0x2, 0x2, 
    0x2, 0x24, 0xe6, 0x3, 0x2, 0x2, 0x2, 0x26, 0xe8, 0x3, 0x2, 0x2, 0x2, 
    0x28, 0xf6, 0x3, 0x2, 0x2, 0x2, 0x2a, 0xf8, 0x3, 0x2, 0x2, 0x2, 0x2c, 
    0xff, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x102, 0x3, 0x2, 0x2, 0x2, 0x30, 0x105, 
    0x3, 0x2, 0x2, 0x2, 0x32, 0x10a, 0x3, 0x2, 0x2, 0x2, 0x34, 0x10c, 0x3, 
    0x2, 0x2, 0x2, 0x36, 0x118, 0x3, 0x2, 0x2, 0x2, 0x38, 0x11a, 0x3, 0x2, 
    0x2, 0x2, 0x3a, 0x11f, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x128, 0x3, 0x2, 0x2, 
    0x2, 0x3e, 0x13f, 0x3, 0x2, 0x2, 0x2, 0x40, 0x141, 0x3, 0x2, 0x2, 0x2, 
    0x42, 0x149, 0x3, 0x2, 0x2, 0x2, 0x44, 0x14b, 0x3, 0x2, 0x2, 0x2, 0x46, 
    0x163, 0x3, 0x2, 0x2, 0x2, 0x48, 0x179, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x17b, 
    0x3, 0x2, 0x2, 0x2, 0x4c, 0x17d, 0x3, 0x2, 0x2, 0x2, 0x4e, 0x17f, 0x3, 
    0x2, 0x2, 0x2, 0x50, 0x181, 0x3, 0x2, 0x2, 0x2, 0x52, 0x18b, 0x3, 0x2, 
    0x2, 0x2, 0x54, 0x191, 0x3, 0x2, 0x2, 0x2, 0x56, 0x194, 0x3, 0x2, 0x2, 
    0x2, 0x58, 0x196, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x198, 0x3, 0x2, 0x2, 0x2, 
    0x5c, 0x5e, 0x5, 0x4, 0x3, 0x2, 0x5d, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x5e, 
    0x61, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x60, 
    0x3, 0x2, 0x2, 0x2, 0x60, 0x3, 0x3, 0x2, 0x2, 0x2, 0x61, 0x5f, 0x3, 
    0x2, 0x2, 0x2, 0x62, 0x63, 0x7, 0x3, 0x2, 0x2, 0x63, 0x64, 0x5, 0x56, 
    0x2c, 0x2, 0x64, 0x65, 0x7, 0x4, 0x2, 0x2, 0x65, 0x66, 0x7, 0x5, 0x2, 
    0x2, 0x66, 0x6b, 0x5, 0x56, 0x2c, 0x2, 0x67, 0x68, 0x7, 0x6, 0x2, 0x2, 
    0x68, 0x6a, 0x5, 0x6, 0x4, 0x2, 0x69, 0x67, 0x3, 0x2, 0x2, 0x2, 0x6a, 
    0x6d, 0x3, 0x2, 0x2, 0x2, 0x6b, 0x69, 0x3, 0x2, 0x2, 0x2, 0x6b, 0x6c, 
    0x3, 0x2, 0x2, 0x2, 0x6c, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x6d, 0x6b, 0x3, 
    0x2, 0x2, 0x2, 0x6e, 0x6f, 0x7, 0x7, 0x2, 0x2, 0x6f, 0x70, 0x7, 0x8, 
    0x2, 0x2, 0x70, 0x71, 0x5, 0xc, 0x7, 0x2, 0x71, 0x72, 0x7, 0x9, 0x2, 
    0x2, 0x72, 0x5, 0x3, 0x2, 0x2, 0x2, 0x73, 0x74, 0x5, 0x8, 0x5, 0x2, 
    0x74, 0x75, 0x5, 0x32, 0x1a, 0x2, 0x75, 0x7, 0x3, 0x2, 0x2, 0x2, 0x76, 
    0x77, 0x9, 0x2, 0x2, 0x2, 0x77, 0x9, 0x3, 0x2, 0x2, 0x2, 0x78, 0x79, 
    0x5, 0x56, 0x2c, 0x2, 0x79, 0x7b, 0x7, 0x4, 0x2, 0x2, 0x7a, 0x7c, 0x5, 
    0x32, 0x1a, 0x2, 0x7b, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x7c, 0x3, 0x2, 
    0x2, 0x2, 0x7c, 0x81, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7e, 0x7, 0x6, 0x2, 
    0x2, 0x7e, 0x80, 0x5, 0x32, 0x1a, 0x2, 0x7f, 0x7d, 0x3, 0x2, 0x2, 0x2, 
    0x80, 0x83, 0x3, 0x2, 0x2, 0x2, 0x81, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x81, 
    0x82, 0x3, 0x2, 0x2, 0x2, 0x82, 0x84, 0x3, 0x2, 0x2, 0x2, 0x83, 0x81, 
    0x3, 0x2, 0x2, 0x2, 0x84, 0x85, 0x7, 0x7, 0x2, 0x2, 0x85, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x86, 0x88, 0x5, 0x14, 0xb, 0x2, 0x87, 0x86, 0x3, 0x2, 
    0x2, 0x2, 0x88, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x89, 0x87, 0x3, 0x2, 0x2, 
    0x2, 0x89, 0x8a, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x8c, 0x3, 0x2, 0x2, 0x2, 
    0x8b, 0x89, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8d, 0x7, 0x29, 0x2, 0x2, 0x8d, 
    0x8e, 0x5, 0x58, 0x2d, 0x2, 0x8e, 0x90, 0x7, 0xd, 0x2, 0x2, 0x8f, 0x91, 
    0x5, 0xe, 0x8, 0x2, 0x90, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 0x3, 
    0x2, 0x2, 0x2, 0x91, 0xd, 0x3, 0x2, 0x2, 0x2, 0x92, 0x94, 0x5, 0x10, 
    0x9, 0x2, 0x93, 0x92, 0x3, 0x2, 0x2, 0x2, 0x94, 0x95, 0x3, 0x2, 0x2, 
    0x2, 0x95, 0x93, 0x3, 0x2, 0x2, 0x2, 0x95, 0x96, 0x3, 0x2, 0x2, 0x2, 
    0x96, 0xf, 0x3, 0x2, 0x2, 0x2, 0x97, 0x99, 0x5, 0x12, 0xa, 0x2, 0x98, 
    0x97, 0x3, 0x2, 0x2, 0x2, 0x99, 0x9a, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x98, 
    0x3, 0x2, 0x2, 0x2, 0x9a, 0x9b, 0x3, 0x2, 0x2, 0x2, 0x9b, 0xa3, 0x3, 
    0x2, 0x2, 0x2, 0x9c, 0xa3, 0x5, 0x14, 0xb, 0x2, 0x9d, 0x9f, 0x5, 0x16, 
    0xc, 0x2, 0x9e, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x9f, 0xa0, 0x3, 0x2, 0x2, 
    0x2, 0xa0, 0x9e, 0x3, 0x2, 0x2, 0x2, 0xa0, 0xa1, 0x3, 0x2, 0x2, 0x2, 
    0xa1, 0xa3, 0x3, 0x2, 0x2, 0x2, 0xa2, 0x98, 0x3, 0x2, 0x2, 0x2, 0xa2, 
    0x9c, 0x3, 0x2, 0x2, 0x2, 0xa2, 0x9e, 0x3, 0x2, 0x2, 0x2, 0xa3, 0x11, 
    0x3, 0x2, 0x2, 0x2, 0xa4, 0xa5, 0x5, 0x1a, 0xe, 0x2, 0xa5, 0xa6, 0x7, 
    0xd, 0x2, 0x2, 0xa6, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xb5, 0x5, 0x22, 
    0x12, 0x2, 0xa8, 0xa9, 0x5, 0x34, 0x1b, 0x2, 0xa9, 0xaa, 0x7, 0xd, 0x2, 
    0x2, 0xaa, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xab, 0xac, 0x5, 0x3c, 0x1f, 0x2, 
    0xac, 0xad, 0x7, 0xd, 0x2, 0x2, 0xad, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xae, 
    0xaf, 0x5, 0x40, 0x21, 0x2, 0xaf, 0xb0, 0x7, 0xd, 0x2, 0x2, 0xb0, 0xb5, 
    0x3, 0x2, 0x2, 0x2, 0xb1, 0xb2, 0x5, 0xa, 0x6, 0x2, 0xb2, 0xb3, 0x7, 
    0xd, 0x2, 0x2, 0xb3, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xb4, 0xa4, 0x3, 0x2, 
    0x2, 0x2, 0xb4, 0xa7, 0x3, 0x2, 0x2, 0x2, 0xb4, 0xa8, 0x3, 0x2, 0x2, 
    0x2, 0xb4, 0xab, 0x3, 0x2, 0x2, 0x2, 0xb4, 0xae, 0x3, 0x2, 0x2, 0x2, 
    0xb4, 0xb1, 0x3, 0x2, 0x2, 0x2, 0xb5, 0x13, 0x3, 0x2, 0x2, 0x2, 0xb6, 
    0xb7, 0x7, 0x22, 0x2, 0x2, 0xb7, 0x15, 0x3, 0x2, 0x2, 0x2, 0xb8, 0xb9, 
    0x7, 0xe, 0x2, 0x2, 0xb9, 0xba, 0x5, 0x18, 0xd, 0x2, 0xba, 0xbb, 0x7, 
    0xd, 0x2, 0x2, 0xbb, 0x17, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xbd, 0x5, 0x5a, 
    0x2e, 0x2, 0xbd, 0x19, 0x3, 0x2, 0x2, 0x2, 0xbe, 0xbf, 0x5, 0x4a, 0x26, 
    0x2, 0xbf, 0xc0, 0x5, 0x1c, 0xf, 0x2, 0xc0, 0xc5, 0x3, 0x2, 0x2, 0x2, 
    0xc1, 0xc2, 0x5, 0x4c, 0x27, 0x2, 0xc2, 0xc3, 0x5, 0x1e, 0x10, 0x2, 
    0xc3, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xc4, 0xbe, 0x3, 0x2, 0x2, 0x2, 0xc4, 
    0xc1, 0x3, 0x2, 0x2, 0x2, 0xc5, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xc6, 0xc7, 
    0x5, 0x56, 0x2c, 0x2, 0xc7, 0xc8, 0x7, 0xf, 0x2, 0x2, 0xc8, 0xc9, 0x5, 
    0x20, 0x11, 0x2, 0xc9, 0xca, 0x7, 0x10, 0x2, 0x2, 0xca, 0xcd, 0x3, 0x2, 
    0x2, 0x2, 0xcb, 0xcd, 0x5, 0x56, 0x2c, 0x2, 0xcc, 0xc6, 0x3, 0x2, 0x2, 
    0x2, 0xcc, 0xcb, 0x3, 0x2, 0x2, 0x2, 0xcd, 0x1d, 0x3, 0x2, 0x2, 0x2, 
    0xce, 0xcf, 0x5, 0x56, 0x2c, 0x2, 0xcf, 0xd0, 0x7, 0xf, 0x2, 0x2, 0xd0, 
    0xd1, 0x5, 0x20, 0x11, 0x2, 0xd1, 0xd2, 0x7, 0x10, 0x2, 0x2, 0xd2, 0xd5, 
    0x3, 0x2, 0x2, 0x2, 0xd3, 0xd5, 0x5, 0x56, 0x2c, 0x2, 0xd4, 0xce, 0x3, 
    0x2, 0x2, 0x2, 0xd4, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd5, 0x1f, 0x3, 0x2, 
    0x2, 0x2, 0xd6, 0xd7, 0x7, 0x2d, 0x2, 0x2, 0xd7, 0x21, 0x3, 0x2, 0x2, 
    0x2, 0xd8, 0xd9, 0x5, 0x4e, 0x28, 0x2, 0xd9, 0xda, 0x5, 0x24, 0x13, 
    0x2, 0xda, 0xdc, 0x5, 0x26, 0x14, 0x2, 0xdb, 0xdd, 0x5, 0x2a, 0x16, 
    0x2, 0xdc, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xdc, 0xdd, 0x3, 0x2, 0x2, 0x2, 
    0xdd, 0x23, 0x3, 0x2, 0x2, 0x2, 0xde, 0xe7, 0x5, 0x56, 0x2c, 0x2, 0xdf, 
    0xe0, 0x5, 0x56, 0x2c, 0x2, 0xe0, 0xe2, 0x7, 0x4, 0x2, 0x2, 0xe1, 0xe3, 
    0x5, 0x30, 0x19, 0x2, 0xe2, 0xe1, 0x3, 0x2, 0x2, 0x2, 0xe2, 0xe3, 0x3, 
    0x2, 0x2, 0x2, 0xe3, 0xe4, 0x3, 0x2, 0x2, 0x2, 0xe4, 0xe5, 0x7, 0x7, 
    0x2, 0x2, 0xe5, 0xe7, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xde, 0x3, 0x2, 0x2, 
    0x2, 0xe6, 0xdf, 0x3, 0x2, 0x2, 0x2, 0xe7, 0x25, 0x3, 0x2, 0x2, 0x2, 
    0xe8, 0xed, 0x5, 0x28, 0x15, 0x2, 0xe9, 0xea, 0x7, 0x6, 0x2, 0x2, 0xea, 
    0xec, 0x5, 0x28, 0x15, 0x2, 0xeb, 0xe9, 0x3, 0x2, 0x2, 0x2, 0xec, 0xef, 
    0x3, 0x2, 0x2, 0x2, 0xed, 0xeb, 0x3, 0x2, 0x2, 0x2, 0xed, 0xee, 0x3, 
    0x2, 0x2, 0x2, 0xee, 0x27, 0x3, 0x2, 0x2, 0x2, 0xef, 0xed, 0x3, 0x2, 
    0x2, 0x2, 0xf0, 0xf1, 0x5, 0x56, 0x2c, 0x2, 0xf1, 0xf2, 0x7, 0xf, 0x2, 
    0x2, 0xf2, 0xf3, 0x7, 0x2d, 0x2, 0x2, 0xf3, 0xf4, 0x7, 0x10, 0x2, 0x2, 
    0xf4, 0xf7, 0x3, 0x2, 0x2, 0x2, 0xf5, 0xf7, 0x5, 0x56, 0x2c, 0x2, 0xf6, 
    0xf0, 0x3, 0x2, 0x2, 0x2, 0xf6, 0xf5, 0x3, 0x2, 0x2, 0x2, 0xf7, 0x29, 
    0x3, 0x2, 0x2, 0x2, 0xf8, 0xf9, 0x7, 0x8, 0x2, 0x2, 0xf9, 0xfa, 0x5, 
    0x2c, 0x17, 0x2, 0xfa, 0xfb, 0x7, 0x9, 0x2, 0x2, 0xfb, 0x2b, 0x3, 0x2, 
    0x2, 0x2, 0xfc, 0xfe, 0x5, 0x2e, 0x18, 0x2, 0xfd, 0xfc, 0x3, 0x2, 0x2, 
    0x2, 0xfe, 0x101, 0x3, 0x2, 0x2, 0x2, 0xff, 0xfd, 0x3, 0x2, 0x2, 0x2, 
    0xff, 0x100, 0x3, 0x2, 0x2, 0x2, 0x100, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x101, 
    0xff, 0x3, 0x2, 0x2, 0x2, 0x102, 0x103, 0x5, 0x3e, 0x20, 0x2, 0x103, 
    0x104, 0x7, 0xd, 0x2, 0x2, 0x104, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x105, 0x108, 
    0x5, 0x32, 0x1a, 0x2, 0x106, 0x107, 0x7, 0x6, 0x2, 0x2, 0x107, 0x109, 
    0x5, 0x30, 0x19, 0x2, 0x108, 0x106, 0x3, 0x2, 0x2, 0x2, 0x108, 0x109, 
    0x3, 0x2, 0x2, 0x2, 0x109, 0x31, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10b, 0x5, 
    0x56, 0x2c, 0x2, 0x10b, 0x33, 0x3, 0x2, 0x2, 0x2, 0x10c, 0x10d, 0x7, 
    0x2a, 0x2, 0x2, 0x10d, 0x10e, 0x5, 0x36, 0x1c, 0x2, 0x10e, 0x10f, 0x5, 
    0x38, 0x1d, 0x2, 0x10f, 0x35, 0x3, 0x2, 0x2, 0x2, 0x110, 0x119, 0x5, 
    0x56, 0x2c, 0x2, 0x111, 0x112, 0x5, 0x56, 0x2c, 0x2, 0x112, 0x114, 0x7, 
    0x4, 0x2, 0x2, 0x113, 0x115, 0x5, 0x30, 0x19, 0x2, 0x114, 0x113, 0x3, 
    0x2, 0x2, 0x2, 0x114, 0x115, 0x3, 0x2, 0x2, 0x2, 0x115, 0x116, 0x3, 
    0x2, 0x2, 0x2, 0x116, 0x117, 0x7, 0x7, 0x2, 0x2, 0x117, 0x119, 0x3, 
    0x2, 0x2, 0x2, 0x118, 0x110, 0x3, 0x2, 0x2, 0x2, 0x118, 0x111, 0x3, 
    0x2, 0x2, 0x2, 0x119, 0x37, 0x3, 0x2, 0x2, 0x2, 0x11a, 0x11d, 0x5, 0x3a, 
    0x1e, 0x2, 0x11b, 0x11c, 0x7, 0x6, 0x2, 0x2, 0x11c, 0x11e, 0x5, 0x38, 
    0x1d, 0x2, 0x11d, 0x11b, 0x3, 0x2, 0x2, 0x2, 0x11d, 0x11e, 0x3, 0x2, 
    0x2, 0x2, 0x11e, 0x39, 0x3, 0x2, 0x2, 0x2, 0x11f, 0x120, 0x5, 0x56, 
    0x2c, 0x2, 0x120, 0x121, 0x7, 0xf, 0x2, 0x2, 0x121, 0x122, 0x7, 0x2d, 
    0x2, 0x2, 0x122, 0x123, 0x7, 0x10, 0x2, 0x2, 0x123, 0x3b, 0x3, 0x2, 
    0x2, 0x2, 0x124, 0x129, 0x5, 0x3e, 0x20, 0x2, 0x125, 0x129, 0x5, 0x52, 
    0x2a, 0x2, 0x126, 0x129, 0x5, 0x50, 0x29, 0x2, 0x127, 0x129, 0x5, 0x54, 
    0x2b, 0x2, 0x128, 0x124, 0x3, 0x2, 0x2, 0x2, 0x128, 0x125, 0x3, 0x2, 
    0x2, 0x2, 0x128, 0x126, 0x3, 0x2, 0x2, 0x2, 0x128, 0x127, 0x3, 0x2, 
    0x2, 0x2, 0x129, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x12a, 0x12b, 0x7, 0x11, 
    0x2, 0x2, 0x12b, 0x12d, 0x7, 0x4, 0x2, 0x2, 0x12c, 0x12e, 0x5, 0x44, 
    0x23, 0x2, 0x12d, 0x12c, 0x3, 0x2, 0x2, 0x2, 0x12d, 0x12e, 0x3, 0x2, 
    0x2, 0x2, 0x12e, 0x12f, 0x3, 0x2, 0x2, 0x2, 0x12f, 0x130, 0x7, 0x7, 
    0x2, 0x2, 0x130, 0x140, 0x5, 0x28, 0x15, 0x2, 0x131, 0x132, 0x7, 0x12, 
    0x2, 0x2, 0x132, 0x133, 0x5, 0x28, 0x15, 0x2, 0x133, 0x134, 0x7, 0x6, 
    0x2, 0x2, 0x134, 0x135, 0x5, 0x28, 0x15, 0x2, 0x135, 0x140, 0x3, 0x2, 
    0x2, 0x2, 0x136, 0x13b, 0x5, 0x24, 0x13, 0x2, 0x137, 0x138, 0x7, 0x4, 
    0x2, 0x2, 0x138, 0x139, 0x5, 0x44, 0x23, 0x2, 0x139, 0x13a, 0x7, 0x7, 
    0x2, 0x2, 0x13a, 0x13c, 0x3, 0x2, 0x2, 0x2, 0x13b, 0x137, 0x3, 0x2, 
    0x2, 0x2, 0x13b, 0x13c, 0x3, 0x2, 0x2, 0x2, 0x13c, 0x13d, 0x3, 0x2, 
    0x2, 0x2, 0x13d, 0x13e, 0x5, 0x26, 0x14, 0x2, 0x13e, 0x140, 0x3, 0x2, 
    0x2, 0x2, 0x13f, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x13f, 0x131, 0x3, 0x2, 
    0x2, 0x2, 0x13f, 0x136, 0x3, 0x2, 0x2, 0x2, 0x140, 0x3f, 0x3, 0x2, 0x2, 
    0x2, 0x141, 0x142, 0x7, 0x13, 0x2, 0x2, 0x142, 0x143, 0x7, 0x4, 0x2, 
    0x2, 0x143, 0x144, 0x5, 0x56, 0x2c, 0x2, 0x144, 0x145, 0x7, 0x14, 0x2, 
    0x2, 0x145, 0x146, 0x7, 0x2d, 0x2, 0x2, 0x146, 0x147, 0x7, 0x7, 0x2, 
    0x2, 0x147, 0x148, 0x5, 0x42, 0x22, 0x2, 0x148, 0x41, 0x3, 0x2, 0x2, 
    0x2, 0x149, 0x14a, 0x5, 0x3c, 0x1f, 0x2, 0x14a, 0x43, 0x3, 0x2, 0x2, 
    0x2, 0x14b, 0x150, 0x5, 0x46, 0x24, 0x2, 0x14c, 0x14d, 0x7, 0x6, 0x2, 
    0x2, 0x14d, 0x14f, 0x5, 0x46, 0x24, 0x2, 0x14e, 0x14c, 0x3, 0x2, 0x2, 
    0x2, 0x14f, 0x152, 0x3, 0x2, 0x2, 0x2, 0x150, 0x14e, 0x3, 0x2, 0x2, 
    0x2, 0x150, 0x151, 0x3, 0x2, 0x2, 0x2, 0x151, 0x45, 0x3, 0x2, 0x2, 0x2, 
    0x152, 0x150, 0x3, 0x2, 0x2, 0x2, 0x153, 0x154, 0x8, 0x24, 0x1, 0x2, 
    0x154, 0x164, 0x5, 0x58, 0x2d, 0x2, 0x155, 0x164, 0x7, 0x2d, 0x2, 0x2, 
    0x156, 0x164, 0x7, 0x15, 0x2, 0x2, 0x157, 0x164, 0x5, 0x56, 0x2c, 0x2, 
    0x158, 0x159, 0x7, 0x17, 0x2, 0x2, 0x159, 0x164, 0x5, 0x46, 0x24, 0x6, 
    0x15a, 0x15b, 0x7, 0x4, 0x2, 0x2, 0x15b, 0x15c, 0x5, 0x46, 0x24, 0x2, 
    0x15c, 0x15d, 0x7, 0x7, 0x2, 0x2, 0x15d, 0x164, 0x3, 0x2, 0x2, 0x2, 
    0x15e, 0x15f, 0x5, 0x48, 0x25, 0x2, 0x15f, 0x160, 0x7, 0x4, 0x2, 0x2, 
    0x160, 0x161, 0x5, 0x46, 0x24, 0x2, 0x161, 0x162, 0x7, 0x7, 0x2, 0x2, 
    0x162, 0x164, 0x3, 0x2, 0x2, 0x2, 0x163, 0x153, 0x3, 0x2, 0x2, 0x2, 
    0x163, 0x155, 0x3, 0x2, 0x2, 0x2, 0x163, 0x156, 0x3, 0x2, 0x2, 0x2, 
    0x163, 0x157, 0x3, 0x2, 0x2, 0x2, 0x163, 0x158, 0x3, 0x2, 0x2, 0x2, 
    0x163, 0x15a, 0x3, 0x2, 0x2, 0x2, 0x163, 0x15e, 0x3, 0x2, 0x2, 0x2, 
    0x164, 0x176, 0x3, 0x2, 0x2, 0x2, 0x165, 0x166, 0xc, 0xa, 0x2, 0x2, 
    0x166, 0x167, 0x7, 0x16, 0x2, 0x2, 0x167, 0x175, 0x5, 0x46, 0x24, 0xb, 
    0x168, 0x169, 0xc, 0x9, 0x2, 0x2, 0x169, 0x16a, 0x7, 0x17, 0x2, 0x2, 
    0x16a, 0x175, 0x5, 0x46, 0x24, 0xa, 0x16b, 0x16c, 0xc, 0x8, 0x2, 0x2, 
    0x16c, 0x16d, 0x7, 0x18, 0x2, 0x2, 0x16d, 0x175, 0x5, 0x46, 0x24, 0x9, 
    0x16e, 0x16f, 0xc, 0x7, 0x2, 0x2, 0x16f, 0x170, 0x7, 0x19, 0x2, 0x2, 
    0x170, 0x175, 0x5, 0x46, 0x24, 0x8, 0x171, 0x172, 0xc, 0x5, 0x2, 0x2, 
    0x172, 0x173, 0x7, 0x1a, 0x2, 0x2, 0x173, 0x175, 0x5, 0x46, 0x24, 0x6, 
    0x174, 0x165, 0x3, 0x2, 0x2, 0x2, 0x174, 0x168, 0x3, 0x2, 0x2, 0x2, 
    0x174, 0x16b, 0x3, 0x2, 0x2, 0x2, 0x174, 0x16e, 0x3, 0x2, 0x2, 0x2, 
    0x174, 0x171, 0x3, 0x2, 0x2, 0x2, 0x175, 0x178, 0x3, 0x2, 0x2, 0x2, 
    0x176, 0x174, 0x3, 0x2, 0x2, 0x2, 0x176, 0x177, 0x3, 0x2, 0x2, 0x2, 
    0x177, 0x47, 0x3, 0x2, 0x2, 0x2, 0x178, 0x176, 0x3, 0x2, 0x2, 0x2, 0x179, 
    0x17a, 0x9, 0x3, 0x2, 0x2, 0x17a, 0x49, 0x3, 0x2, 0x2, 0x2, 0x17b, 0x17c, 
    0x7, 0x23, 0x2, 0x2, 0x17c, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x17d, 0x17e, 
    0x7, 0x24, 0x2, 0x2, 0x17e, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x17f, 0x180, 
    0x7, 0x25, 0x2, 0x2, 0x180, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x181, 0x184, 
    0x7, 0x26, 0x2, 0x2, 0x182, 0x185, 0x5, 0x28, 0x15, 0x2, 0x183, 0x185, 
    0x5, 0x1c, 0xf, 0x2, 0x184, 0x182, 0x3, 0x2, 0x2, 0x2, 0x184, 0x183, 
    0x3, 0x2, 0x2, 0x2, 0x185, 0x186, 0x3, 0x2, 0x2, 0x2, 0x186, 0x189, 
    0x7, 0x21, 0x2, 0x2, 0x187, 0x18a, 0x5, 0x28, 0x15, 0x2, 0x188, 0x18a, 
    0x5, 0x1e, 0x10, 0x2, 0x189, 0x187, 0x3, 0x2, 0x2, 0x2, 0x189, 0x188, 
    0x3, 0x2, 0x2, 0x2, 0x18a, 0x51, 0x3, 0x2, 0x2, 0x2, 0x18b, 0x18f, 0x7, 
    0x27, 0x2, 0x2, 0x18c, 0x190, 0x5, 0x28, 0x15, 0x2, 0x18d, 0x190, 0x5, 
    0x1c, 0xf, 0x2, 0x18e, 0x190, 0x5, 0x1e, 0x10, 0x2, 0x18f, 0x18c, 0x3, 
    0x2, 0x2, 0x2, 0x18f, 0x18d, 0x3, 0x2, 0x2, 0x2, 0x18f, 0x18e, 0x3, 
    0x2, 0x2, 0x2, 0x190, 0x53, 0x3, 0x2, 0x2, 0x2, 0x191, 0x192, 0x7, 0x28, 
    0x2, 0x2, 0x192, 0x193, 0x5, 0x26, 0x14, 0x2, 0x193, 0x55, 0x3, 0x2, 
    0x2, 0x2, 0x194, 0x195, 0x7, 0x2b, 0x2, 0x2, 0x195, 0x57, 0x3, 0x2, 
    0x2, 0x2, 0x196, 0x197, 0x7, 0x2c, 0x2, 0x2, 0x197, 0x59, 0x3, 0x2, 
    0x2, 0x2, 0x198, 0x199, 0x7, 0x2e, 0x2, 0x2, 0x199, 0x5b, 0x3, 0x2, 
    0x2, 0x2, 0x25, 0x5f, 0x6b, 0x7b, 0x81, 0x89, 0x90, 0x95, 0x9a, 0xa0, 
    0xa2, 0xb4, 0xc4, 0xcc, 0xd4, 0xdc, 0xe2, 0xe6, 0xed, 0xf6, 0xff, 0x108, 
    0x114, 0x118, 0x11d, 0x128, 0x12d, 0x13b, 0x13f, 0x150, 0x163, 0x174, 
    0x176, 0x184, 0x189, 0x18f, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

OQASM2Parser::Initializer OQASM2Parser::_init;
