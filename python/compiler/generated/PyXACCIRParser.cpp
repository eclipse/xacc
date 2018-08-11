
// Generated from PyXACCIR.g4 by ANTLR 4.7.1


#include "PyXACCIRListener.h"

#include "PyXACCIRParser.h"


using namespace antlrcpp;
using namespace pyxacc;
using namespace antlr4;

PyXACCIRParser::PyXACCIRParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

PyXACCIRParser::~PyXACCIRParser() {
  delete _interpreter;
}

std::string PyXACCIRParser::getGrammarFileName() const {
  return "PyXACCIR.g4";
}

const std::vector<std::string>& PyXACCIRParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& PyXACCIRParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

PyXACCIRParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PyXACCIRParser::XacckernelContext *> PyXACCIRParser::XaccsrcContext::xacckernel() {
  return getRuleContexts<PyXACCIRParser::XacckernelContext>();
}

PyXACCIRParser::XacckernelContext* PyXACCIRParser::XaccsrcContext::xacckernel(size_t i) {
  return getRuleContext<PyXACCIRParser::XacckernelContext>(i);
}


size_t PyXACCIRParser::XaccsrcContext::getRuleIndex() const {
  return PyXACCIRParser::RuleXaccsrc;
}

void PyXACCIRParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void PyXACCIRParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

PyXACCIRParser::XaccsrcContext* PyXACCIRParser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, PyXACCIRParser::RuleXaccsrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(37);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PyXACCIRParser::T__0) {
      setState(34);
      xacckernel();
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

//----------------- XacckernelContext ------------------------------------------------------------------

PyXACCIRParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::MainprogContext* PyXACCIRParser::XacckernelContext::mainprog() {
  return getRuleContext<PyXACCIRParser::MainprogContext>(0);
}

PyXACCIRParser::IdContext* PyXACCIRParser::XacckernelContext::id() {
  return getRuleContext<PyXACCIRParser::IdContext>(0);
}

std::vector<PyXACCIRParser::ParamContext *> PyXACCIRParser::XacckernelContext::param() {
  return getRuleContexts<PyXACCIRParser::ParamContext>();
}

PyXACCIRParser::ParamContext* PyXACCIRParser::XacckernelContext::param(size_t i) {
  return getRuleContext<PyXACCIRParser::ParamContext>(i);
}


size_t PyXACCIRParser::XacckernelContext::getRuleIndex() const {
  return PyXACCIRParser::RuleXacckernel;
}

void PyXACCIRParser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void PyXACCIRParser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

PyXACCIRParser::XacckernelContext* PyXACCIRParser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, PyXACCIRParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(40);
    match(PyXACCIRParser::T__0);
    setState(41);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(42);
    match(PyXACCIRParser::T__1);
    setState(49);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PyXACCIRParser::T__2

    || _la == PyXACCIRParser::ID) {
      setState(44);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == PyXACCIRParser::T__2) {
        setState(43);
        match(PyXACCIRParser::T__2);
      }
      setState(46);
      param();
      setState(51);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(52);
    match(PyXACCIRParser::T__3);
    setState(53);
    match(PyXACCIRParser::T__4);
    setState(54);
    mainprog();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MainprogContext ------------------------------------------------------------------

PyXACCIRParser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::ProgramContext* PyXACCIRParser::MainprogContext::program() {
  return getRuleContext<PyXACCIRParser::ProgramContext>(0);
}


size_t PyXACCIRParser::MainprogContext::getRuleIndex() const {
  return PyXACCIRParser::RuleMainprog;
}

void PyXACCIRParser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void PyXACCIRParser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

PyXACCIRParser::MainprogContext* PyXACCIRParser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 4, PyXACCIRParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56);
    program();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProgramContext ------------------------------------------------------------------

PyXACCIRParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PyXACCIRParser::LineContext *> PyXACCIRParser::ProgramContext::line() {
  return getRuleContexts<PyXACCIRParser::LineContext>();
}

PyXACCIRParser::LineContext* PyXACCIRParser::ProgramContext::line(size_t i) {
  return getRuleContext<PyXACCIRParser::LineContext>(i);
}


size_t PyXACCIRParser::ProgramContext::getRuleIndex() const {
  return PyXACCIRParser::RuleProgram;
}

void PyXACCIRParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void PyXACCIRParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

PyXACCIRParser::ProgramContext* PyXACCIRParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 6, PyXACCIRParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(59); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(58);
      line();
      setState(61); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__5)
      | (1ULL << PyXACCIRParser::T__6)
      | (1ULL << PyXACCIRParser::T__7)
      | (1ULL << PyXACCIRParser::T__8)
      | (1ULL << PyXACCIRParser::T__9)
      | (1ULL << PyXACCIRParser::T__10)
      | (1ULL << PyXACCIRParser::T__11)
      | (1ULL << PyXACCIRParser::T__12)
      | (1ULL << PyXACCIRParser::T__13)
      | (1ULL << PyXACCIRParser::T__14)
      | (1ULL << PyXACCIRParser::T__15)
      | (1ULL << PyXACCIRParser::T__16)
      | (1ULL << PyXACCIRParser::T__17)
      | (1ULL << PyXACCIRParser::T__18)
      | (1ULL << PyXACCIRParser::T__19)
      | (1ULL << PyXACCIRParser::T__20)
      | (1ULL << PyXACCIRParser::T__21)
      | (1ULL << PyXACCIRParser::COMMENT))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

PyXACCIRParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PyXACCIRParser::StatementContext *> PyXACCIRParser::LineContext::statement() {
  return getRuleContexts<PyXACCIRParser::StatementContext>();
}

PyXACCIRParser::StatementContext* PyXACCIRParser::LineContext::statement(size_t i) {
  return getRuleContext<PyXACCIRParser::StatementContext>(i);
}

PyXACCIRParser::CommentContext* PyXACCIRParser::LineContext::comment() {
  return getRuleContext<PyXACCIRParser::CommentContext>(0);
}


size_t PyXACCIRParser::LineContext::getRuleIndex() const {
  return PyXACCIRParser::RuleLine;
}

void PyXACCIRParser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void PyXACCIRParser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

PyXACCIRParser::LineContext* PyXACCIRParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 8, PyXACCIRParser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(69);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PyXACCIRParser::T__5:
      case PyXACCIRParser::T__6:
      case PyXACCIRParser::T__7:
      case PyXACCIRParser::T__8:
      case PyXACCIRParser::T__9:
      case PyXACCIRParser::T__10:
      case PyXACCIRParser::T__11:
      case PyXACCIRParser::T__12:
      case PyXACCIRParser::T__13:
      case PyXACCIRParser::T__14:
      case PyXACCIRParser::T__15:
      case PyXACCIRParser::T__16:
      case PyXACCIRParser::T__17:
      case PyXACCIRParser::T__18:
      case PyXACCIRParser::T__19:
      case PyXACCIRParser::T__20:
      case PyXACCIRParser::T__21: {
        enterOuterAlt(_localctx, 1);
        setState(64); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(63);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(66); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case PyXACCIRParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(68);
        comment();
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

PyXACCIRParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::UopContext* PyXACCIRParser::StatementContext::uop() {
  return getRuleContext<PyXACCIRParser::UopContext>(0);
}


size_t PyXACCIRParser::StatementContext::getRuleIndex() const {
  return PyXACCIRParser::RuleStatement;
}

void PyXACCIRParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void PyXACCIRParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

PyXACCIRParser::StatementContext* PyXACCIRParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 10, PyXACCIRParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(73);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PyXACCIRParser::T__6:
      case PyXACCIRParser::T__7:
      case PyXACCIRParser::T__8:
      case PyXACCIRParser::T__9:
      case PyXACCIRParser::T__10:
      case PyXACCIRParser::T__11:
      case PyXACCIRParser::T__12:
      case PyXACCIRParser::T__13:
      case PyXACCIRParser::T__14:
      case PyXACCIRParser::T__15:
      case PyXACCIRParser::T__16:
      case PyXACCIRParser::T__17:
      case PyXACCIRParser::T__18:
      case PyXACCIRParser::T__19:
      case PyXACCIRParser::T__20:
      case PyXACCIRParser::T__21: {
        enterOuterAlt(_localctx, 1);
        setState(71);
        uop();
        break;
      }

      case PyXACCIRParser::T__5: {
        enterOuterAlt(_localctx, 2);
        setState(72);
        match(PyXACCIRParser::T__5);
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

//----------------- CommentContext ------------------------------------------------------------------

PyXACCIRParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PyXACCIRParser::CommentContext::COMMENT() {
  return getToken(PyXACCIRParser::COMMENT, 0);
}


size_t PyXACCIRParser::CommentContext::getRuleIndex() const {
  return PyXACCIRParser::RuleComment;
}

void PyXACCIRParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void PyXACCIRParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

PyXACCIRParser::CommentContext* PyXACCIRParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 12, PyXACCIRParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    match(PyXACCIRParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamlistContext ------------------------------------------------------------------

PyXACCIRParser::ParamlistContext::ParamlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::ParamContext* PyXACCIRParser::ParamlistContext::param() {
  return getRuleContext<PyXACCIRParser::ParamContext>(0);
}

PyXACCIRParser::ParamlistContext* PyXACCIRParser::ParamlistContext::paramlist() {
  return getRuleContext<PyXACCIRParser::ParamlistContext>(0);
}


size_t PyXACCIRParser::ParamlistContext::getRuleIndex() const {
  return PyXACCIRParser::RuleParamlist;
}

void PyXACCIRParser::ParamlistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParamlist(this);
}

void PyXACCIRParser::ParamlistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParamlist(this);
}

PyXACCIRParser::ParamlistContext* PyXACCIRParser::paramlist() {
  ParamlistContext *_localctx = _tracker.createInstance<ParamlistContext>(_ctx, getState());
  enterRule(_localctx, 14, PyXACCIRParser::RuleParamlist);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(77);
    param();
    setState(80);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(78);
      match(PyXACCIRParser::T__2);
      setState(79);
      paramlist();
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

//----------------- ParamContext ------------------------------------------------------------------

PyXACCIRParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::IdContext* PyXACCIRParser::ParamContext::id() {
  return getRuleContext<PyXACCIRParser::IdContext>(0);
}


size_t PyXACCIRParser::ParamContext::getRuleIndex() const {
  return PyXACCIRParser::RuleParam;
}

void PyXACCIRParser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void PyXACCIRParser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}

PyXACCIRParser::ParamContext* PyXACCIRParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 16, PyXACCIRParser::RuleParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(82);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UopContext ------------------------------------------------------------------

PyXACCIRParser::UopContext::UopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::GateContext* PyXACCIRParser::UopContext::gate() {
  return getRuleContext<PyXACCIRParser::GateContext>(0);
}

PyXACCIRParser::ExplistContext* PyXACCIRParser::UopContext::explist() {
  return getRuleContext<PyXACCIRParser::ExplistContext>(0);
}


size_t PyXACCIRParser::UopContext::getRuleIndex() const {
  return PyXACCIRParser::RuleUop;
}

void PyXACCIRParser::UopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUop(this);
}

void PyXACCIRParser::UopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUop(this);
}

PyXACCIRParser::UopContext* PyXACCIRParser::uop() {
  UopContext *_localctx = _tracker.createInstance<UopContext>(_ctx, getState());
  enterRule(_localctx, 18, PyXACCIRParser::RuleUop);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(84);
    dynamic_cast<UopContext *>(_localctx)->gatename = gate();
    setState(85);
    match(PyXACCIRParser::T__1);

    setState(86);
    explist();
    setState(87);
    match(PyXACCIRParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GateContext ------------------------------------------------------------------

PyXACCIRParser::GateContext::GateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t PyXACCIRParser::GateContext::getRuleIndex() const {
  return PyXACCIRParser::RuleGate;
}

void PyXACCIRParser::GateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGate(this);
}

void PyXACCIRParser::GateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGate(this);
}

PyXACCIRParser::GateContext* PyXACCIRParser::gate() {
  GateContext *_localctx = _tracker.createInstance<GateContext>(_ctx, getState());
  enterRule(_localctx, 20, PyXACCIRParser::RuleGate);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(89);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__6)
      | (1ULL << PyXACCIRParser::T__7)
      | (1ULL << PyXACCIRParser::T__8)
      | (1ULL << PyXACCIRParser::T__9)
      | (1ULL << PyXACCIRParser::T__10)
      | (1ULL << PyXACCIRParser::T__11)
      | (1ULL << PyXACCIRParser::T__12)
      | (1ULL << PyXACCIRParser::T__13)
      | (1ULL << PyXACCIRParser::T__14)
      | (1ULL << PyXACCIRParser::T__15)
      | (1ULL << PyXACCIRParser::T__16)
      | (1ULL << PyXACCIRParser::T__17)
      | (1ULL << PyXACCIRParser::T__18)
      | (1ULL << PyXACCIRParser::T__19)
      | (1ULL << PyXACCIRParser::T__20)
      | (1ULL << PyXACCIRParser::T__21))) != 0))) {
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

//----------------- ExplistContext ------------------------------------------------------------------

PyXACCIRParser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<PyXACCIRParser::ExpContext *> PyXACCIRParser::ExplistContext::exp() {
  return getRuleContexts<PyXACCIRParser::ExpContext>();
}

PyXACCIRParser::ExpContext* PyXACCIRParser::ExplistContext::exp(size_t i) {
  return getRuleContext<PyXACCIRParser::ExpContext>(i);
}


size_t PyXACCIRParser::ExplistContext::getRuleIndex() const {
  return PyXACCIRParser::RuleExplist;
}

void PyXACCIRParser::ExplistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExplist(this);
}

void PyXACCIRParser::ExplistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExplist(this);
}

PyXACCIRParser::ExplistContext* PyXACCIRParser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 22, PyXACCIRParser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(91);
    exp(0);
    setState(96);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PyXACCIRParser::T__2) {
      setState(92);
      match(PyXACCIRParser::T__2);
      setState(93);
      exp(0);
      setState(98);
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

PyXACCIRParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::RealContext* PyXACCIRParser::ExpContext::real() {
  return getRuleContext<PyXACCIRParser::RealContext>(0);
}

tree::TerminalNode* PyXACCIRParser::ExpContext::INT() {
  return getToken(PyXACCIRParser::INT, 0);
}

PyXACCIRParser::IdContext* PyXACCIRParser::ExpContext::id() {
  return getRuleContext<PyXACCIRParser::IdContext>(0);
}

std::vector<PyXACCIRParser::ExpContext *> PyXACCIRParser::ExpContext::exp() {
  return getRuleContexts<PyXACCIRParser::ExpContext>();
}

PyXACCIRParser::ExpContext* PyXACCIRParser::ExpContext::exp(size_t i) {
  return getRuleContext<PyXACCIRParser::ExpContext>(i);
}

PyXACCIRParser::UnaryopContext* PyXACCIRParser::ExpContext::unaryop() {
  return getRuleContext<PyXACCIRParser::UnaryopContext>(0);
}


size_t PyXACCIRParser::ExpContext::getRuleIndex() const {
  return PyXACCIRParser::RuleExp;
}

void PyXACCIRParser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void PyXACCIRParser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


PyXACCIRParser::ExpContext* PyXACCIRParser::exp() {
   return exp(0);
}

PyXACCIRParser::ExpContext* PyXACCIRParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  PyXACCIRParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  PyXACCIRParser::ExpContext *previousContext = _localctx;
  size_t startState = 24;
  enterRecursionRule(_localctx, 24, PyXACCIRParser::RuleExp, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(119);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(100);
      real();
      break;
    }

    case 2: {
      setState(101);
      match(PyXACCIRParser::INT);
      break;
    }

    case 3: {
      setState(102);
      match(PyXACCIRParser::T__22);
      break;
    }

    case 4: {
      setState(103);
      id();
      break;
    }

    case 5: {
      setState(104);
      match(PyXACCIRParser::T__24);
      setState(105);
      exp(5);
      break;
    }

    case 6: {
      setState(106);
      match(PyXACCIRParser::T__1);
      setState(107);
      exp(0);
      setState(108);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 7: {
      setState(110);
      unaryop();
      setState(111);
      match(PyXACCIRParser::T__1);
      setState(112);
      exp(0);
      setState(113);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 8: {
      setState(115);
      id();
      setState(116);
      match(PyXACCIRParser::T__28);
      setState(117);
      exp(1);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(138);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(136);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(121);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(122);
          match(PyXACCIRParser::T__23);
          setState(123);
          exp(10);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(124);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(125);
          match(PyXACCIRParser::T__24);
          setState(126);
          exp(9);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(127);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(128);
          match(PyXACCIRParser::T__25);
          setState(129);
          exp(8);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(130);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(131);
          match(PyXACCIRParser::T__26);
          setState(132);
          exp(7);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(133);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(134);
          match(PyXACCIRParser::T__27);
          setState(135);
          exp(5);
          break;
        }

        } 
      }
      setState(140);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx);
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

PyXACCIRParser::UnaryopContext::UnaryopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t PyXACCIRParser::UnaryopContext::getRuleIndex() const {
  return PyXACCIRParser::RuleUnaryop;
}

void PyXACCIRParser::UnaryopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryop(this);
}

void PyXACCIRParser::UnaryopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryop(this);
}

PyXACCIRParser::UnaryopContext* PyXACCIRParser::unaryop() {
  UnaryopContext *_localctx = _tracker.createInstance<UnaryopContext>(_ctx, getState());
  enterRule(_localctx, 26, PyXACCIRParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(141);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__29)
      | (1ULL << PyXACCIRParser::T__30)
      | (1ULL << PyXACCIRParser::T__31)
      | (1ULL << PyXACCIRParser::T__32)
      | (1ULL << PyXACCIRParser::T__33)
      | (1ULL << PyXACCIRParser::T__34))) != 0))) {
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

//----------------- IdContext ------------------------------------------------------------------

PyXACCIRParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PyXACCIRParser::IdContext::ID() {
  return getToken(PyXACCIRParser::ID, 0);
}


size_t PyXACCIRParser::IdContext::getRuleIndex() const {
  return PyXACCIRParser::RuleId;
}

void PyXACCIRParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void PyXACCIRParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

PyXACCIRParser::IdContext* PyXACCIRParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 28, PyXACCIRParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(143);
    match(PyXACCIRParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

PyXACCIRParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PyXACCIRParser::RealContext::REAL() {
  return getToken(PyXACCIRParser::REAL, 0);
}


size_t PyXACCIRParser::RealContext::getRuleIndex() const {
  return PyXACCIRParser::RuleReal;
}

void PyXACCIRParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void PyXACCIRParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

PyXACCIRParser::RealContext* PyXACCIRParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 30, PyXACCIRParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(145);
    match(PyXACCIRParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

PyXACCIRParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* PyXACCIRParser::StringContext::STRING() {
  return getToken(PyXACCIRParser::STRING, 0);
}


size_t PyXACCIRParser::StringContext::getRuleIndex() const {
  return PyXACCIRParser::RuleString;
}

void PyXACCIRParser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void PyXACCIRParser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

PyXACCIRParser::StringContext* PyXACCIRParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 32, PyXACCIRParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(147);
    match(PyXACCIRParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool PyXACCIRParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 12: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool PyXACCIRParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 9);
    case 1: return precpred(_ctx, 8);
    case 2: return precpred(_ctx, 7);
    case 3: return precpred(_ctx, 6);
    case 4: return precpred(_ctx, 4);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> PyXACCIRParser::_decisionToDFA;
atn::PredictionContextCache PyXACCIRParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN PyXACCIRParser::_atn;
std::vector<uint16_t> PyXACCIRParser::_serializedATN;

std::vector<std::string> PyXACCIRParser::_ruleNames = {
  "xaccsrc", "xacckernel", "mainprog", "program", "line", "statement", "comment", 
  "paramlist", "param", "uop", "gate", "explist", "exp", "unaryop", "id", 
  "real", "string"
};

std::vector<std::string> PyXACCIRParser::_literalNames = {
  "", "'def'", "'('", "','", "')'", "':'", "'return'", "'X'", "'Rx'", "'Rz'", 
  "'Ry'", "'Y'", "'Z'", "'CNOT'", "'CZ'", "'CX'", "'H'", "'Swap'", "'Identity'", 
  "'CPhase'", "'Measure'", "'MEASURE'", "'xacc'", "'pi'", "'+'", "'-'", 
  "'*'", "'/'", "'^'", "'='", "'sin'", "'cos'", "'tan'", "'exp'", "'ln'", 
  "'sqrt'"
};

std::vector<std::string> PyXACCIRParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "COMMENT", "ID", "REAL", "INT", "STRING", "WS", "EOL"
};

dfa::Vocabulary PyXACCIRParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> PyXACCIRParser::_tokenNames;

PyXACCIRParser::Initializer::Initializer() {
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
    0x3, 0x2c, 0x98, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x3, 0x2, 0x7, 0x2, 0x26, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x29, 
    0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x2f, 0xa, 
    0x3, 0x3, 0x3, 0x7, 0x3, 0x32, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x35, 0xb, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 
    0x5, 0x6, 0x5, 0x3e, 0xa, 0x5, 0xd, 0x5, 0xe, 0x5, 0x3f, 0x3, 0x6, 0x6, 
    0x6, 0x43, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x44, 0x3, 0x6, 0x5, 0x6, 0x48, 
    0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x4c, 0xa, 0x7, 0x3, 0x8, 0x3, 
    0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 0x53, 0xa, 0x9, 0x3, 0xa, 
    0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x7, 0xd, 0x61, 0xa, 0xd, 0xc, 
    0xd, 0xe, 0xd, 0x64, 0xb, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x5, 0xe, 0x7a, 0xa, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x7, 0xe, 0x8b, 
    0xa, 0xe, 0xc, 0xe, 0xe, 0xe, 0x8e, 0xb, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 
    0x10, 0x3, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 
    0x2, 0x3, 0x1a, 0x13, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 
    0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x2, 0x4, 0x3, 0x2, 
    0x9, 0x18, 0x3, 0x2, 0x20, 0x25, 0x2, 0x9b, 0x2, 0x27, 0x3, 0x2, 0x2, 
    0x2, 0x4, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x6, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x8, 
    0x3d, 0x3, 0x2, 0x2, 0x2, 0xa, 0x47, 0x3, 0x2, 0x2, 0x2, 0xc, 0x4b, 
    0x3, 0x2, 0x2, 0x2, 0xe, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x10, 0x4f, 0x3, 
    0x2, 0x2, 0x2, 0x12, 0x54, 0x3, 0x2, 0x2, 0x2, 0x14, 0x56, 0x3, 0x2, 
    0x2, 0x2, 0x16, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x18, 0x5d, 0x3, 0x2, 0x2, 
    0x2, 0x1a, 0x79, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x8f, 0x3, 0x2, 0x2, 0x2, 
    0x1e, 0x91, 0x3, 0x2, 0x2, 0x2, 0x20, 0x93, 0x3, 0x2, 0x2, 0x2, 0x22, 
    0x95, 0x3, 0x2, 0x2, 0x2, 0x24, 0x26, 0x5, 0x4, 0x3, 0x2, 0x25, 0x24, 
    0x3, 0x2, 0x2, 0x2, 0x26, 0x29, 0x3, 0x2, 0x2, 0x2, 0x27, 0x25, 0x3, 
    0x2, 0x2, 0x2, 0x27, 0x28, 0x3, 0x2, 0x2, 0x2, 0x28, 0x3, 0x3, 0x2, 
    0x2, 0x2, 0x29, 0x27, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x2b, 0x7, 0x3, 0x2, 
    0x2, 0x2b, 0x2c, 0x5, 0x1e, 0x10, 0x2, 0x2c, 0x33, 0x7, 0x4, 0x2, 0x2, 
    0x2d, 0x2f, 0x7, 0x5, 0x2, 0x2, 0x2e, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x2e, 
    0x2f, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x30, 0x3, 0x2, 0x2, 0x2, 0x30, 0x32, 
    0x5, 0x12, 0xa, 0x2, 0x31, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x32, 0x35, 0x3, 
    0x2, 0x2, 0x2, 0x33, 0x31, 0x3, 0x2, 0x2, 0x2, 0x33, 0x34, 0x3, 0x2, 
    0x2, 0x2, 0x34, 0x36, 0x3, 0x2, 0x2, 0x2, 0x35, 0x33, 0x3, 0x2, 0x2, 
    0x2, 0x36, 0x37, 0x7, 0x6, 0x2, 0x2, 0x37, 0x38, 0x7, 0x7, 0x2, 0x2, 
    0x38, 0x39, 0x5, 0x6, 0x4, 0x2, 0x39, 0x5, 0x3, 0x2, 0x2, 0x2, 0x3a, 
    0x3b, 0x5, 0x8, 0x5, 0x2, 0x3b, 0x7, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3e, 
    0x5, 0xa, 0x6, 0x2, 0x3d, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3f, 0x3, 
    0x2, 0x2, 0x2, 0x3f, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3f, 0x40, 0x3, 0x2, 
    0x2, 0x2, 0x40, 0x9, 0x3, 0x2, 0x2, 0x2, 0x41, 0x43, 0x5, 0xc, 0x7, 
    0x2, 0x42, 0x41, 0x3, 0x2, 0x2, 0x2, 0x43, 0x44, 0x3, 0x2, 0x2, 0x2, 
    0x44, 0x42, 0x3, 0x2, 0x2, 0x2, 0x44, 0x45, 0x3, 0x2, 0x2, 0x2, 0x45, 
    0x48, 0x3, 0x2, 0x2, 0x2, 0x46, 0x48, 0x5, 0xe, 0x8, 0x2, 0x47, 0x42, 
    0x3, 0x2, 0x2, 0x2, 0x47, 0x46, 0x3, 0x2, 0x2, 0x2, 0x48, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x49, 0x4c, 0x5, 0x14, 0xb, 0x2, 0x4a, 0x4c, 0x7, 0x8, 
    0x2, 0x2, 0x4b, 0x49, 0x3, 0x2, 0x2, 0x2, 0x4b, 0x4a, 0x3, 0x2, 0x2, 
    0x2, 0x4c, 0xd, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4e, 0x7, 0x26, 0x2, 0x2, 
    0x4e, 0xf, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x52, 0x5, 0x12, 0xa, 0x2, 0x50, 
    0x51, 0x7, 0x5, 0x2, 0x2, 0x51, 0x53, 0x5, 0x10, 0x9, 0x2, 0x52, 0x50, 
    0x3, 0x2, 0x2, 0x2, 0x52, 0x53, 0x3, 0x2, 0x2, 0x2, 0x53, 0x11, 0x3, 
    0x2, 0x2, 0x2, 0x54, 0x55, 0x5, 0x1e, 0x10, 0x2, 0x55, 0x13, 0x3, 0x2, 
    0x2, 0x2, 0x56, 0x57, 0x5, 0x16, 0xc, 0x2, 0x57, 0x58, 0x7, 0x4, 0x2, 
    0x2, 0x58, 0x59, 0x5, 0x18, 0xd, 0x2, 0x59, 0x5a, 0x7, 0x6, 0x2, 0x2, 
    0x5a, 0x15, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5c, 0x9, 0x2, 0x2, 0x2, 0x5c, 
    0x17, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x62, 0x5, 0x1a, 0xe, 0x2, 0x5e, 0x5f, 
    0x7, 0x5, 0x2, 0x2, 0x5f, 0x61, 0x5, 0x1a, 0xe, 0x2, 0x60, 0x5e, 0x3, 
    0x2, 0x2, 0x2, 0x61, 0x64, 0x3, 0x2, 0x2, 0x2, 0x62, 0x60, 0x3, 0x2, 
    0x2, 0x2, 0x62, 0x63, 0x3, 0x2, 0x2, 0x2, 0x63, 0x19, 0x3, 0x2, 0x2, 
    0x2, 0x64, 0x62, 0x3, 0x2, 0x2, 0x2, 0x65, 0x66, 0x8, 0xe, 0x1, 0x2, 
    0x66, 0x7a, 0x5, 0x20, 0x11, 0x2, 0x67, 0x7a, 0x7, 0x29, 0x2, 0x2, 0x68, 
    0x7a, 0x7, 0x19, 0x2, 0x2, 0x69, 0x7a, 0x5, 0x1e, 0x10, 0x2, 0x6a, 0x6b, 
    0x7, 0x1b, 0x2, 0x2, 0x6b, 0x7a, 0x5, 0x1a, 0xe, 0x7, 0x6c, 0x6d, 0x7, 
    0x4, 0x2, 0x2, 0x6d, 0x6e, 0x5, 0x1a, 0xe, 0x2, 0x6e, 0x6f, 0x7, 0x6, 
    0x2, 0x2, 0x6f, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x70, 0x71, 0x5, 0x1c, 0xf, 
    0x2, 0x71, 0x72, 0x7, 0x4, 0x2, 0x2, 0x72, 0x73, 0x5, 0x1a, 0xe, 0x2, 
    0x73, 0x74, 0x7, 0x6, 0x2, 0x2, 0x74, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x75, 
    0x76, 0x5, 0x1e, 0x10, 0x2, 0x76, 0x77, 0x7, 0x1f, 0x2, 0x2, 0x77, 0x78, 
    0x5, 0x1a, 0xe, 0x3, 0x78, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x79, 0x65, 0x3, 
    0x2, 0x2, 0x2, 0x79, 0x67, 0x3, 0x2, 0x2, 0x2, 0x79, 0x68, 0x3, 0x2, 
    0x2, 0x2, 0x79, 0x69, 0x3, 0x2, 0x2, 0x2, 0x79, 0x6a, 0x3, 0x2, 0x2, 
    0x2, 0x79, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x79, 0x70, 0x3, 0x2, 0x2, 0x2, 
    0x79, 0x75, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x7b, 
    0x7c, 0xc, 0xb, 0x2, 0x2, 0x7c, 0x7d, 0x7, 0x1a, 0x2, 0x2, 0x7d, 0x8b, 
    0x5, 0x1a, 0xe, 0xc, 0x7e, 0x7f, 0xc, 0xa, 0x2, 0x2, 0x7f, 0x80, 0x7, 
    0x1b, 0x2, 0x2, 0x80, 0x8b, 0x5, 0x1a, 0xe, 0xb, 0x81, 0x82, 0xc, 0x9, 
    0x2, 0x2, 0x82, 0x83, 0x7, 0x1c, 0x2, 0x2, 0x83, 0x8b, 0x5, 0x1a, 0xe, 
    0xa, 0x84, 0x85, 0xc, 0x8, 0x2, 0x2, 0x85, 0x86, 0x7, 0x1d, 0x2, 0x2, 
    0x86, 0x8b, 0x5, 0x1a, 0xe, 0x9, 0x87, 0x88, 0xc, 0x6, 0x2, 0x2, 0x88, 
    0x89, 0x7, 0x1e, 0x2, 0x2, 0x89, 0x8b, 0x5, 0x1a, 0xe, 0x7, 0x8a, 0x7b, 
    0x3, 0x2, 0x2, 0x2, 0x8a, 0x7e, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x81, 0x3, 
    0x2, 0x2, 0x2, 0x8a, 0x84, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x87, 0x3, 0x2, 
    0x2, 0x2, 0x8b, 0x8e, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8a, 0x3, 0x2, 0x2, 
    0x2, 0x8c, 0x8d, 0x3, 0x2, 0x2, 0x2, 0x8d, 0x1b, 0x3, 0x2, 0x2, 0x2, 
    0x8e, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x8f, 0x90, 0x9, 0x3, 0x2, 0x2, 0x90, 
    0x1d, 0x3, 0x2, 0x2, 0x2, 0x91, 0x92, 0x7, 0x27, 0x2, 0x2, 0x92, 0x1f, 
    0x3, 0x2, 0x2, 0x2, 0x93, 0x94, 0x7, 0x28, 0x2, 0x2, 0x94, 0x21, 0x3, 
    0x2, 0x2, 0x2, 0x95, 0x96, 0x7, 0x2a, 0x2, 0x2, 0x96, 0x23, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x27, 0x2e, 0x33, 0x3f, 0x44, 0x47, 0x4b, 0x52, 0x62, 
    0x79, 0x8a, 0x8c, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

PyXACCIRParser::Initializer PyXACCIRParser::_init;
