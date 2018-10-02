
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
    setState(41);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PyXACCIRParser::T__0) {
      setState(38);
      xacckernel();
      setState(43);
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
    setState(44);
    match(PyXACCIRParser::T__0);
    setState(45);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(46);
    match(PyXACCIRParser::T__1);
    setState(53);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__2)
      | (1ULL << PyXACCIRParser::T__6)
      | (1ULL << PyXACCIRParser::ID))) != 0)) {
      setState(48);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == PyXACCIRParser::T__2) {
        setState(47);
        match(PyXACCIRParser::T__2);
      }
      setState(50);
      param();
      setState(55);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(56);
    match(PyXACCIRParser::T__3);
    setState(57);
    match(PyXACCIRParser::T__4);
    setState(58);
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
    setState(60);
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
    setState(63); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(62);
      line();
      setState(65); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__5)
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
      | (1ULL << PyXACCIRParser::T__22)
      | (1ULL << PyXACCIRParser::T__23)
      | (1ULL << PyXACCIRParser::T__24)
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
    setState(73);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PyXACCIRParser::T__5:
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
      case PyXACCIRParser::T__21:
      case PyXACCIRParser::T__22:
      case PyXACCIRParser::T__23:
      case PyXACCIRParser::T__24: {
        enterOuterAlt(_localctx, 1);
        setState(68); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(67);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(70); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case PyXACCIRParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(72);
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
    setState(77);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
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
      case PyXACCIRParser::T__21:
      case PyXACCIRParser::T__22:
      case PyXACCIRParser::T__23:
      case PyXACCIRParser::T__24: {
        enterOuterAlt(_localctx, 1);
        setState(75);
        uop();
        break;
      }

      case PyXACCIRParser::T__5: {
        enterOuterAlt(_localctx, 2);
        setState(76);
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
    setState(79);
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
    setState(81);
    param();
    setState(84);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      setState(82);
      match(PyXACCIRParser::T__2);
      setState(83);
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
    setState(89);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case PyXACCIRParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(86);
        id();
        break;
      }

      case PyXACCIRParser::T__6: {
        enterOuterAlt(_localctx, 2);
        setState(87);
        match(PyXACCIRParser::T__6);
        setState(88);
        id();
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

PyXACCIRParser::UopContext::UopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::GateContext* PyXACCIRParser::UopContext::gate() {
  return getRuleContext<PyXACCIRParser::GateContext>(0);
}

PyXACCIRParser::ExplistContext* PyXACCIRParser::UopContext::explist() {
  return getRuleContext<PyXACCIRParser::ExplistContext>(0);
}

PyXACCIRParser::AllbitsOpContext* PyXACCIRParser::UopContext::allbitsOp() {
  return getRuleContext<PyXACCIRParser::AllbitsOpContext>(0);
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
    setState(97);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(91);
      dynamic_cast<UopContext *>(_localctx)->gatename = gate();
      setState(92);
      match(PyXACCIRParser::T__1);

      setState(93);
      explist();
      setState(94);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(96);
      allbitsOp();
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

//----------------- AllbitsOpContext ------------------------------------------------------------------

PyXACCIRParser::AllbitsOpContext::AllbitsOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

PyXACCIRParser::GateContext* PyXACCIRParser::AllbitsOpContext::gate() {
  return getRuleContext<PyXACCIRParser::GateContext>(0);
}

std::vector<tree::TerminalNode *> PyXACCIRParser::AllbitsOpContext::INT() {
  return getTokens(PyXACCIRParser::INT);
}

tree::TerminalNode* PyXACCIRParser::AllbitsOpContext::INT(size_t i) {
  return getToken(PyXACCIRParser::INT, i);
}


size_t PyXACCIRParser::AllbitsOpContext::getRuleIndex() const {
  return PyXACCIRParser::RuleAllbitsOp;
}

void PyXACCIRParser::AllbitsOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAllbitsOp(this);
}

void PyXACCIRParser::AllbitsOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAllbitsOp(this);
}

PyXACCIRParser::AllbitsOpContext* PyXACCIRParser::allbitsOp() {
  AllbitsOpContext *_localctx = _tracker.createInstance<AllbitsOpContext>(_ctx, getState());
  enterRule(_localctx, 20, PyXACCIRParser::RuleAllbitsOp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(111);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(99);
      dynamic_cast<AllbitsOpContext *>(_localctx)->gatename = gate();
      setState(100);
      match(PyXACCIRParser::T__1);
      setState(101);
      match(PyXACCIRParser::T__7);
      setState(102);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(104);
      dynamic_cast<AllbitsOpContext *>(_localctx)->gatename = gate();
      setState(105);
      match(PyXACCIRParser::T__1);
      setState(106);
      match(PyXACCIRParser::INT);
      setState(107);
      match(PyXACCIRParser::T__7);
      setState(108);
      match(PyXACCIRParser::INT);
      setState(109);
      match(PyXACCIRParser::T__3);
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
  enterRule(_localctx, 22, PyXACCIRParser::RuleGate);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__8)
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
      | (1ULL << PyXACCIRParser::T__22)
      | (1ULL << PyXACCIRParser::T__23)
      | (1ULL << PyXACCIRParser::T__24))) != 0))) {
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
  enterRule(_localctx, 24, PyXACCIRParser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    exp(0);
    setState(120);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == PyXACCIRParser::T__2) {
      setState(116);
      match(PyXACCIRParser::T__2);
      setState(117);
      exp(0);
      setState(122);
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

std::vector<PyXACCIRParser::CouplerContext *> PyXACCIRParser::ExpContext::coupler() {
  return getRuleContexts<PyXACCIRParser::CouplerContext>();
}

PyXACCIRParser::CouplerContext* PyXACCIRParser::ExpContext::coupler(size_t i) {
  return getRuleContext<PyXACCIRParser::CouplerContext>(i);
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
  size_t startState = 26;
  enterRecursionRule(_localctx, 26, PyXACCIRParser::RuleExp, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(154);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      setState(124);
      real();
      break;
    }

    case 2: {
      setState(125);
      match(PyXACCIRParser::INT);
      break;
    }

    case 3: {
      setState(126);
      match(PyXACCIRParser::T__25);
      break;
    }

    case 4: {
      setState(127);
      id();
      break;
    }

    case 5: {
      setState(128);
      match(PyXACCIRParser::T__27);
      setState(129);
      exp(6);
      break;
    }

    case 6: {
      setState(130);
      match(PyXACCIRParser::T__1);
      setState(131);
      exp(0);
      setState(132);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 7: {
      setState(134);
      unaryop();
      setState(135);
      match(PyXACCIRParser::T__1);
      setState(136);
      exp(0);
      setState(137);
      match(PyXACCIRParser::T__3);
      break;
    }

    case 8: {
      setState(139);
      id();
      setState(140);
      match(PyXACCIRParser::T__30);
      setState(141);
      exp(2);
      break;
    }

    case 9: {
      setState(143);
      match(PyXACCIRParser::T__31);
      setState(150);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == PyXACCIRParser::T__2

      || _la == PyXACCIRParser::T__33) {
        setState(145);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == PyXACCIRParser::T__2) {
          setState(144);
          match(PyXACCIRParser::T__2);
        }
        setState(147);
        coupler();
        setState(152);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(153);
      match(PyXACCIRParser::T__32);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(173);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(171);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(156);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(157);
          match(PyXACCIRParser::T__26);
          setState(158);
          exp(11);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(159);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(160);
          match(PyXACCIRParser::T__27);
          setState(161);
          exp(10);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(162);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(163);
          match(PyXACCIRParser::T__6);
          setState(164);
          exp(9);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(165);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(166);
          match(PyXACCIRParser::T__28);
          setState(167);
          exp(8);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(168);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(169);
          match(PyXACCIRParser::T__29);
          setState(170);
          exp(6);
          break;
        }

        } 
      }
      setState(175);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- CouplerContext ------------------------------------------------------------------

PyXACCIRParser::CouplerContext::CouplerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> PyXACCIRParser::CouplerContext::INT() {
  return getTokens(PyXACCIRParser::INT);
}

tree::TerminalNode* PyXACCIRParser::CouplerContext::INT(size_t i) {
  return getToken(PyXACCIRParser::INT, i);
}


size_t PyXACCIRParser::CouplerContext::getRuleIndex() const {
  return PyXACCIRParser::RuleCoupler;
}

void PyXACCIRParser::CouplerContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCoupler(this);
}

void PyXACCIRParser::CouplerContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<PyXACCIRListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCoupler(this);
}

PyXACCIRParser::CouplerContext* PyXACCIRParser::coupler() {
  CouplerContext *_localctx = _tracker.createInstance<CouplerContext>(_ctx, getState());
  enterRule(_localctx, 28, PyXACCIRParser::RuleCoupler);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(176);
    match(PyXACCIRParser::T__33);
    setState(177);
    match(PyXACCIRParser::INT);
    setState(178);
    match(PyXACCIRParser::T__2);
    setState(179);
    match(PyXACCIRParser::INT);
    setState(180);
    match(PyXACCIRParser::T__34);
   
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
  enterRule(_localctx, 30, PyXACCIRParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(182);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << PyXACCIRParser::T__35)
      | (1ULL << PyXACCIRParser::T__36)
      | (1ULL << PyXACCIRParser::T__37)
      | (1ULL << PyXACCIRParser::T__38)
      | (1ULL << PyXACCIRParser::T__39)
      | (1ULL << PyXACCIRParser::T__40))) != 0))) {
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
  enterRule(_localctx, 32, PyXACCIRParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(184);
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
  enterRule(_localctx, 34, PyXACCIRParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(186);
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
  enterRule(_localctx, 36, PyXACCIRParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(188);
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
    case 13: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool PyXACCIRParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 10);
    case 1: return precpred(_ctx, 9);
    case 2: return precpred(_ctx, 8);
    case 3: return precpred(_ctx, 7);
    case 4: return precpred(_ctx, 5);

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
  "paramlist", "param", "uop", "allbitsOp", "gate", "explist", "exp", "coupler", 
  "unaryop", "id", "real", "string"
};

std::vector<std::string> PyXACCIRParser::_literalNames = {
  "", "'def'", "'('", "','", "')'", "':'", "'return'", "'*'", "'...'", "'X'", 
  "'Rx'", "'Rz'", "'Ry'", "'Y'", "'Z'", "'CNOT'", "'CZ'", "'CX'", "'H'", 
  "'Swap'", "'Identity'", "'CPhase'", "'Measure'", "'MEASURE'", "'xacc'", 
  "'qmi'", "'pi'", "'+'", "'-'", "'/'", "'^'", "'='", "''['", "']''", "'['", 
  "']'", "'sin'", "'cos'", "'tan'", "'exp'", "'ln'", "'sqrt'"
};

std::vector<std::string> PyXACCIRParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "COMMENT", "ID", "REAL", "INT", "STRING", "WS", 
  "EOL"
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
    0x3, 0x32, 0xc1, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x3, 0x2, 0x7, 
    0x2, 0x2a, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x2d, 0xb, 0x2, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x33, 0xa, 0x3, 0x3, 0x3, 0x7, 0x3, 
    0x36, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x39, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x6, 0x5, 0x42, 0xa, 
    0x5, 0xd, 0x5, 0xe, 0x5, 0x43, 0x3, 0x6, 0x6, 0x6, 0x47, 0xa, 0x6, 0xd, 
    0x6, 0xe, 0x6, 0x48, 0x3, 0x6, 0x5, 0x6, 0x4c, 0xa, 0x6, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x50, 0xa, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 
    0x3, 0x9, 0x5, 0x9, 0x57, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x5, 
    0xa, 0x5c, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x5, 0xb, 0x64, 0xa, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 
    0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 
    0xc, 0x3, 0xc, 0x5, 0xc, 0x72, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x7, 0xe, 0x79, 0xa, 0xe, 0xc, 0xe, 0xe, 0xe, 0x7c, 
    0xb, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0xf, 0x5, 0xf, 0x94, 0xa, 0xf, 0x3, 0xf, 0x7, 0xf, 0x97, 
    0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 0x9a, 0xb, 0xf, 0x3, 0xf, 0x5, 0xf, 0x9d, 
    0xa, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0xf, 0x7, 0xf, 0xae, 0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 0xb1, 
    0xb, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 
    0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x2, 0x3, 0x1c, 0x15, 0x2, 0x4, 0x6, 
    0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 
    0x20, 0x22, 0x24, 0x26, 0x2, 0x4, 0x3, 0x2, 0xb, 0x1b, 0x3, 0x2, 0x26, 
    0x2b, 0x2, 0xc8, 0x2, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x4, 0x2e, 0x3, 0x2, 
    0x2, 0x2, 0x6, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x8, 0x41, 0x3, 0x2, 0x2, 0x2, 
    0xa, 0x4b, 0x3, 0x2, 0x2, 0x2, 0xc, 0x4f, 0x3, 0x2, 0x2, 0x2, 0xe, 0x51, 
    0x3, 0x2, 0x2, 0x2, 0x10, 0x53, 0x3, 0x2, 0x2, 0x2, 0x12, 0x5b, 0x3, 
    0x2, 0x2, 0x2, 0x14, 0x63, 0x3, 0x2, 0x2, 0x2, 0x16, 0x71, 0x3, 0x2, 
    0x2, 0x2, 0x18, 0x73, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x75, 0x3, 0x2, 0x2, 
    0x2, 0x1c, 0x9c, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xb2, 0x3, 0x2, 0x2, 0x2, 
    0x20, 0xb8, 0x3, 0x2, 0x2, 0x2, 0x22, 0xba, 0x3, 0x2, 0x2, 0x2, 0x24, 
    0xbc, 0x3, 0x2, 0x2, 0x2, 0x26, 0xbe, 0x3, 0x2, 0x2, 0x2, 0x28, 0x2a, 
    0x5, 0x4, 0x3, 0x2, 0x29, 0x28, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x2d, 0x3, 
    0x2, 0x2, 0x2, 0x2b, 0x29, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2c, 0x3, 0x2, 
    0x2, 0x2, 0x2c, 0x3, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x2b, 0x3, 0x2, 0x2, 
    0x2, 0x2e, 0x2f, 0x7, 0x3, 0x2, 0x2, 0x2f, 0x30, 0x5, 0x22, 0x12, 0x2, 
    0x30, 0x37, 0x7, 0x4, 0x2, 0x2, 0x31, 0x33, 0x7, 0x5, 0x2, 0x2, 0x32, 
    0x31, 0x3, 0x2, 0x2, 0x2, 0x32, 0x33, 0x3, 0x2, 0x2, 0x2, 0x33, 0x34, 
    0x3, 0x2, 0x2, 0x2, 0x34, 0x36, 0x5, 0x12, 0xa, 0x2, 0x35, 0x32, 0x3, 
    0x2, 0x2, 0x2, 0x36, 0x39, 0x3, 0x2, 0x2, 0x2, 0x37, 0x35, 0x3, 0x2, 
    0x2, 0x2, 0x37, 0x38, 0x3, 0x2, 0x2, 0x2, 0x38, 0x3a, 0x3, 0x2, 0x2, 
    0x2, 0x39, 0x37, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x3b, 0x7, 0x6, 0x2, 0x2, 
    0x3b, 0x3c, 0x7, 0x7, 0x2, 0x2, 0x3c, 0x3d, 0x5, 0x6, 0x4, 0x2, 0x3d, 
    0x5, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3f, 0x5, 0x8, 0x5, 0x2, 0x3f, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x40, 0x42, 0x5, 0xa, 0x6, 0x2, 0x41, 0x40, 0x3, 
    0x2, 0x2, 0x2, 0x42, 0x43, 0x3, 0x2, 0x2, 0x2, 0x43, 0x41, 0x3, 0x2, 
    0x2, 0x2, 0x43, 0x44, 0x3, 0x2, 0x2, 0x2, 0x44, 0x9, 0x3, 0x2, 0x2, 
    0x2, 0x45, 0x47, 0x5, 0xc, 0x7, 0x2, 0x46, 0x45, 0x3, 0x2, 0x2, 0x2, 
    0x47, 0x48, 0x3, 0x2, 0x2, 0x2, 0x48, 0x46, 0x3, 0x2, 0x2, 0x2, 0x48, 
    0x49, 0x3, 0x2, 0x2, 0x2, 0x49, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x4c, 
    0x5, 0xe, 0x8, 0x2, 0x4b, 0x46, 0x3, 0x2, 0x2, 0x2, 0x4b, 0x4a, 0x3, 
    0x2, 0x2, 0x2, 0x4c, 0xb, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x50, 0x5, 0x14, 
    0xb, 0x2, 0x4e, 0x50, 0x7, 0x8, 0x2, 0x2, 0x4f, 0x4d, 0x3, 0x2, 0x2, 
    0x2, 0x4f, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x50, 0xd, 0x3, 0x2, 0x2, 0x2, 
    0x51, 0x52, 0x7, 0x2c, 0x2, 0x2, 0x52, 0xf, 0x3, 0x2, 0x2, 0x2, 0x53, 
    0x56, 0x5, 0x12, 0xa, 0x2, 0x54, 0x55, 0x7, 0x5, 0x2, 0x2, 0x55, 0x57, 
    0x5, 0x10, 0x9, 0x2, 0x56, 0x54, 0x3, 0x2, 0x2, 0x2, 0x56, 0x57, 0x3, 
    0x2, 0x2, 0x2, 0x57, 0x11, 0x3, 0x2, 0x2, 0x2, 0x58, 0x5c, 0x5, 0x22, 
    0x12, 0x2, 0x59, 0x5a, 0x7, 0x9, 0x2, 0x2, 0x5a, 0x5c, 0x5, 0x22, 0x12, 
    0x2, 0x5b, 0x58, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x59, 0x3, 0x2, 0x2, 0x2, 
    0x5c, 0x13, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5e, 0x5, 0x18, 0xd, 0x2, 0x5e, 
    0x5f, 0x7, 0x4, 0x2, 0x2, 0x5f, 0x60, 0x5, 0x1a, 0xe, 0x2, 0x60, 0x61, 
    0x7, 0x6, 0x2, 0x2, 0x61, 0x64, 0x3, 0x2, 0x2, 0x2, 0x62, 0x64, 0x5, 
    0x16, 0xc, 0x2, 0x63, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x63, 0x62, 0x3, 0x2, 
    0x2, 0x2, 0x64, 0x15, 0x3, 0x2, 0x2, 0x2, 0x65, 0x66, 0x5, 0x18, 0xd, 
    0x2, 0x66, 0x67, 0x7, 0x4, 0x2, 0x2, 0x67, 0x68, 0x7, 0xa, 0x2, 0x2, 
    0x68, 0x69, 0x7, 0x6, 0x2, 0x2, 0x69, 0x72, 0x3, 0x2, 0x2, 0x2, 0x6a, 
    0x6b, 0x5, 0x18, 0xd, 0x2, 0x6b, 0x6c, 0x7, 0x4, 0x2, 0x2, 0x6c, 0x6d, 
    0x7, 0x2f, 0x2, 0x2, 0x6d, 0x6e, 0x7, 0xa, 0x2, 0x2, 0x6e, 0x6f, 0x7, 
    0x2f, 0x2, 0x2, 0x6f, 0x70, 0x7, 0x6, 0x2, 0x2, 0x70, 0x72, 0x3, 0x2, 
    0x2, 0x2, 0x71, 0x65, 0x3, 0x2, 0x2, 0x2, 0x71, 0x6a, 0x3, 0x2, 0x2, 
    0x2, 0x72, 0x17, 0x3, 0x2, 0x2, 0x2, 0x73, 0x74, 0x9, 0x2, 0x2, 0x2, 
    0x74, 0x19, 0x3, 0x2, 0x2, 0x2, 0x75, 0x7a, 0x5, 0x1c, 0xf, 0x2, 0x76, 
    0x77, 0x7, 0x5, 0x2, 0x2, 0x77, 0x79, 0x5, 0x1c, 0xf, 0x2, 0x78, 0x76, 
    0x3, 0x2, 0x2, 0x2, 0x79, 0x7c, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x78, 0x3, 
    0x2, 0x2, 0x2, 0x7a, 0x7b, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x1b, 0x3, 0x2, 
    0x2, 0x2, 0x7c, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7e, 0x8, 0xf, 0x1, 
    0x2, 0x7e, 0x9d, 0x5, 0x24, 0x13, 0x2, 0x7f, 0x9d, 0x7, 0x2f, 0x2, 0x2, 
    0x80, 0x9d, 0x7, 0x1c, 0x2, 0x2, 0x81, 0x9d, 0x5, 0x22, 0x12, 0x2, 0x82, 
    0x83, 0x7, 0x1e, 0x2, 0x2, 0x83, 0x9d, 0x5, 0x1c, 0xf, 0x8, 0x84, 0x85, 
    0x7, 0x4, 0x2, 0x2, 0x85, 0x86, 0x5, 0x1c, 0xf, 0x2, 0x86, 0x87, 0x7, 
    0x6, 0x2, 0x2, 0x87, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x88, 0x89, 0x5, 0x20, 
    0x11, 0x2, 0x89, 0x8a, 0x7, 0x4, 0x2, 0x2, 0x8a, 0x8b, 0x5, 0x1c, 0xf, 
    0x2, 0x8b, 0x8c, 0x7, 0x6, 0x2, 0x2, 0x8c, 0x9d, 0x3, 0x2, 0x2, 0x2, 
    0x8d, 0x8e, 0x5, 0x22, 0x12, 0x2, 0x8e, 0x8f, 0x7, 0x21, 0x2, 0x2, 0x8f, 
    0x90, 0x5, 0x1c, 0xf, 0x4, 0x90, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x91, 0x98, 
    0x7, 0x22, 0x2, 0x2, 0x92, 0x94, 0x7, 0x5, 0x2, 0x2, 0x93, 0x92, 0x3, 
    0x2, 0x2, 0x2, 0x93, 0x94, 0x3, 0x2, 0x2, 0x2, 0x94, 0x95, 0x3, 0x2, 
    0x2, 0x2, 0x95, 0x97, 0x5, 0x1e, 0x10, 0x2, 0x96, 0x93, 0x3, 0x2, 0x2, 
    0x2, 0x97, 0x9a, 0x3, 0x2, 0x2, 0x2, 0x98, 0x96, 0x3, 0x2, 0x2, 0x2, 
    0x98, 0x99, 0x3, 0x2, 0x2, 0x2, 0x99, 0x9b, 0x3, 0x2, 0x2, 0x2, 0x9a, 
    0x98, 0x3, 0x2, 0x2, 0x2, 0x9b, 0x9d, 0x7, 0x23, 0x2, 0x2, 0x9c, 0x7d, 
    0x3, 0x2, 0x2, 0x2, 0x9c, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x80, 0x3, 
    0x2, 0x2, 0x2, 0x9c, 0x81, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x82, 0x3, 0x2, 
    0x2, 0x2, 0x9c, 0x84, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x88, 0x3, 0x2, 0x2, 
    0x2, 0x9c, 0x8d, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x91, 0x3, 0x2, 0x2, 0x2, 
    0x9d, 0xaf, 0x3, 0x2, 0x2, 0x2, 0x9e, 0x9f, 0xc, 0xc, 0x2, 0x2, 0x9f, 
    0xa0, 0x7, 0x1d, 0x2, 0x2, 0xa0, 0xae, 0x5, 0x1c, 0xf, 0xd, 0xa1, 0xa2, 
    0xc, 0xb, 0x2, 0x2, 0xa2, 0xa3, 0x7, 0x1e, 0x2, 0x2, 0xa3, 0xae, 0x5, 
    0x1c, 0xf, 0xc, 0xa4, 0xa5, 0xc, 0xa, 0x2, 0x2, 0xa5, 0xa6, 0x7, 0x9, 
    0x2, 0x2, 0xa6, 0xae, 0x5, 0x1c, 0xf, 0xb, 0xa7, 0xa8, 0xc, 0x9, 0x2, 
    0x2, 0xa8, 0xa9, 0x7, 0x1f, 0x2, 0x2, 0xa9, 0xae, 0x5, 0x1c, 0xf, 0xa, 
    0xaa, 0xab, 0xc, 0x7, 0x2, 0x2, 0xab, 0xac, 0x7, 0x20, 0x2, 0x2, 0xac, 
    0xae, 0x5, 0x1c, 0xf, 0x8, 0xad, 0x9e, 0x3, 0x2, 0x2, 0x2, 0xad, 0xa1, 
    0x3, 0x2, 0x2, 0x2, 0xad, 0xa4, 0x3, 0x2, 0x2, 0x2, 0xad, 0xa7, 0x3, 
    0x2, 0x2, 0x2, 0xad, 0xaa, 0x3, 0x2, 0x2, 0x2, 0xae, 0xb1, 0x3, 0x2, 
    0x2, 0x2, 0xaf, 0xad, 0x3, 0x2, 0x2, 0x2, 0xaf, 0xb0, 0x3, 0x2, 0x2, 
    0x2, 0xb0, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xaf, 0x3, 0x2, 0x2, 0x2, 
    0xb2, 0xb3, 0x7, 0x24, 0x2, 0x2, 0xb3, 0xb4, 0x7, 0x2f, 0x2, 0x2, 0xb4, 
    0xb5, 0x7, 0x5, 0x2, 0x2, 0xb5, 0xb6, 0x7, 0x2f, 0x2, 0x2, 0xb6, 0xb7, 
    0x7, 0x25, 0x2, 0x2, 0xb7, 0x1f, 0x3, 0x2, 0x2, 0x2, 0xb8, 0xb9, 0x9, 
    0x3, 0x2, 0x2, 0xb9, 0x21, 0x3, 0x2, 0x2, 0x2, 0xba, 0xbb, 0x7, 0x2d, 
    0x2, 0x2, 0xbb, 0x23, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xbd, 0x7, 0x2e, 0x2, 
    0x2, 0xbd, 0x25, 0x3, 0x2, 0x2, 0x2, 0xbe, 0xbf, 0x7, 0x30, 0x2, 0x2, 
    0xbf, 0x27, 0x3, 0x2, 0x2, 0x2, 0x13, 0x2b, 0x32, 0x37, 0x43, 0x48, 
    0x4b, 0x4f, 0x56, 0x5b, 0x63, 0x71, 0x7a, 0x93, 0x98, 0x9c, 0xad, 0xaf, 
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
