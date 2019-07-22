
// Generated from XACCLang.g4 by ANTLR 4.7.2


#include "XACCLangListener.h"

#include "XACCLangParser.h"


using namespace antlrcpp;
using namespace xacclang;
using namespace antlr4;

XACCLangParser::XACCLangParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

XACCLangParser::~XACCLangParser() {
  delete _interpreter;
}

std::string XACCLangParser::getGrammarFileName() const {
  return "XACCLang.g4";
}

const std::vector<std::string>& XACCLangParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& XACCLangParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

XACCLangParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<XACCLangParser::XacckernelContext *> XACCLangParser::XaccsrcContext::xacckernel() {
  return getRuleContexts<XACCLangParser::XacckernelContext>();
}

XACCLangParser::XacckernelContext* XACCLangParser::XaccsrcContext::xacckernel(size_t i) {
  return getRuleContext<XACCLangParser::XacckernelContext>(i);
}


size_t XACCLangParser::XaccsrcContext::getRuleIndex() const {
  return XACCLangParser::RuleXaccsrc;
}

void XACCLangParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void XACCLangParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

XACCLangParser::XaccsrcContext* XACCLangParser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, XACCLangParser::RuleXaccsrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(47);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == XACCLangParser::T__0) {
      setState(44);
      xacckernel();
      setState(49);
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

XACCLangParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::MainprogContext* XACCLangParser::XacckernelContext::mainprog() {
  return getRuleContext<XACCLangParser::MainprogContext>(0);
}

std::vector<XACCLangParser::IdContext *> XACCLangParser::XacckernelContext::id() {
  return getRuleContexts<XACCLangParser::IdContext>();
}

XACCLangParser::IdContext* XACCLangParser::XacckernelContext::id(size_t i) {
  return getRuleContext<XACCLangParser::IdContext>(i);
}

std::vector<XACCLangParser::TypedparamContext *> XACCLangParser::XacckernelContext::typedparam() {
  return getRuleContexts<XACCLangParser::TypedparamContext>();
}

XACCLangParser::TypedparamContext* XACCLangParser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<XACCLangParser::TypedparamContext>(i);
}


size_t XACCLangParser::XacckernelContext::getRuleIndex() const {
  return XACCLangParser::RuleXacckernel;
}

void XACCLangParser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void XACCLangParser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

XACCLangParser::XacckernelContext* XACCLangParser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, XACCLangParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(50);
    match(XACCLangParser::T__0);
    setState(51);
    match(XACCLangParser::T__1);
    setState(52);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(53);
    match(XACCLangParser::T__2);
    setState(54);
    match(XACCLangParser::T__3);
    setState(55);
    dynamic_cast<XacckernelContext *>(_localctx)->acceleratorbuffer = id();
    setState(60);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == XACCLangParser::T__4) {
      setState(56);
      match(XACCLangParser::T__4);
      setState(57);
      typedparam();
      setState(62);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(63);
    match(XACCLangParser::T__5);
    setState(64);
    match(XACCLangParser::T__6);
    setState(65);
    mainprog();
    setState(66);
    match(XACCLangParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypedparamContext ------------------------------------------------------------------

XACCLangParser::TypedparamContext::TypedparamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::TypeContext* XACCLangParser::TypedparamContext::type() {
  return getRuleContext<XACCLangParser::TypeContext>(0);
}

XACCLangParser::ParamContext* XACCLangParser::TypedparamContext::param() {
  return getRuleContext<XACCLangParser::ParamContext>(0);
}


size_t XACCLangParser::TypedparamContext::getRuleIndex() const {
  return XACCLangParser::RuleTypedparam;
}

void XACCLangParser::TypedparamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypedparam(this);
}

void XACCLangParser::TypedparamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypedparam(this);
}

XACCLangParser::TypedparamContext* XACCLangParser::typedparam() {
  TypedparamContext *_localctx = _tracker.createInstance<TypedparamContext>(_ctx, getState());
  enterRule(_localctx, 4, XACCLangParser::RuleTypedparam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(68);
    type();
    setState(69);
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

XACCLangParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t XACCLangParser::TypeContext::getRuleIndex() const {
  return XACCLangParser::RuleType;
}

void XACCLangParser::TypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType(this);
}

void XACCLangParser::TypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType(this);
}

XACCLangParser::TypeContext* XACCLangParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 6, XACCLangParser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(71);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << XACCLangParser::T__8)
      | (1ULL << XACCLangParser::T__9)
      | (1ULL << XACCLangParser::T__10))) != 0))) {
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

XACCLangParser::KernelcallContext::KernelcallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::IdContext* XACCLangParser::KernelcallContext::id() {
  return getRuleContext<XACCLangParser::IdContext>(0);
}

std::vector<XACCLangParser::ParamContext *> XACCLangParser::KernelcallContext::param() {
  return getRuleContexts<XACCLangParser::ParamContext>();
}

XACCLangParser::ParamContext* XACCLangParser::KernelcallContext::param(size_t i) {
  return getRuleContext<XACCLangParser::ParamContext>(i);
}


size_t XACCLangParser::KernelcallContext::getRuleIndex() const {
  return XACCLangParser::RuleKernelcall;
}

void XACCLangParser::KernelcallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKernelcall(this);
}

void XACCLangParser::KernelcallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKernelcall(this);
}

XACCLangParser::KernelcallContext* XACCLangParser::kernelcall() {
  KernelcallContext *_localctx = _tracker.createInstance<KernelcallContext>(_ctx, getState());
  enterRule(_localctx, 8, XACCLangParser::RuleKernelcall);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(73);
    dynamic_cast<KernelcallContext *>(_localctx)->kernelname = id();
    setState(74);
    match(XACCLangParser::T__2);
    setState(76);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == XACCLangParser::T__13

    || _la == XACCLangParser::ID) {
      setState(75);
      param();
    }
    setState(82);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == XACCLangParser::T__4) {
      setState(78);
      match(XACCLangParser::T__4);
      setState(79);
      param();
      setState(84);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(85);
    match(XACCLangParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MainprogContext ------------------------------------------------------------------

XACCLangParser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::ProgramContext* XACCLangParser::MainprogContext::program() {
  return getRuleContext<XACCLangParser::ProgramContext>(0);
}


size_t XACCLangParser::MainprogContext::getRuleIndex() const {
  return XACCLangParser::RuleMainprog;
}

void XACCLangParser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void XACCLangParser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

XACCLangParser::MainprogContext* XACCLangParser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 10, XACCLangParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(87);
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

XACCLangParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<XACCLangParser::LineContext *> XACCLangParser::ProgramContext::line() {
  return getRuleContexts<XACCLangParser::LineContext>();
}

XACCLangParser::LineContext* XACCLangParser::ProgramContext::line(size_t i) {
  return getRuleContext<XACCLangParser::LineContext>(i);
}


size_t XACCLangParser::ProgramContext::getRuleIndex() const {
  return XACCLangParser::RuleProgram;
}

void XACCLangParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void XACCLangParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

XACCLangParser::ProgramContext* XACCLangParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 12, XACCLangParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(90); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(89);
      line();
      setState(92); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << XACCLangParser::T__12)
      | (1ULL << XACCLangParser::COMMENT)
      | (1ULL << XACCLangParser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

XACCLangParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<XACCLangParser::StatementContext *> XACCLangParser::LineContext::statement() {
  return getRuleContexts<XACCLangParser::StatementContext>();
}

XACCLangParser::StatementContext* XACCLangParser::LineContext::statement(size_t i) {
  return getRuleContext<XACCLangParser::StatementContext>(i);
}

XACCLangParser::CommentContext* XACCLangParser::LineContext::comment() {
  return getRuleContext<XACCLangParser::CommentContext>(0);
}


size_t XACCLangParser::LineContext::getRuleIndex() const {
  return XACCLangParser::RuleLine;
}

void XACCLangParser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void XACCLangParser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

XACCLangParser::LineContext* XACCLangParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 14, XACCLangParser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(100);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case XACCLangParser::T__12:
      case XACCLangParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(95); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(94);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(97); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case XACCLangParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(99);
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

XACCLangParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::UopContext* XACCLangParser::StatementContext::uop() {
  return getRuleContext<XACCLangParser::UopContext>(0);
}


size_t XACCLangParser::StatementContext::getRuleIndex() const {
  return XACCLangParser::RuleStatement;
}

void XACCLangParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void XACCLangParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

XACCLangParser::StatementContext* XACCLangParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 16, XACCLangParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(107);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case XACCLangParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(102);
        uop();
        setState(103);
        match(XACCLangParser::T__11);
        break;
      }

      case XACCLangParser::T__12: {
        enterOuterAlt(_localctx, 2);
        setState(105);
        match(XACCLangParser::T__12);
        setState(106);
        match(XACCLangParser::T__11);
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

XACCLangParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* XACCLangParser::CommentContext::COMMENT() {
  return getToken(XACCLangParser::COMMENT, 0);
}


size_t XACCLangParser::CommentContext::getRuleIndex() const {
  return XACCLangParser::RuleComment;
}

void XACCLangParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void XACCLangParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

XACCLangParser::CommentContext* XACCLangParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 18, XACCLangParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(109);
    match(XACCLangParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamlistContext ------------------------------------------------------------------

XACCLangParser::ParamlistContext::ParamlistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::ParamContext* XACCLangParser::ParamlistContext::param() {
  return getRuleContext<XACCLangParser::ParamContext>(0);
}

XACCLangParser::ParamlistContext* XACCLangParser::ParamlistContext::paramlist() {
  return getRuleContext<XACCLangParser::ParamlistContext>(0);
}


size_t XACCLangParser::ParamlistContext::getRuleIndex() const {
  return XACCLangParser::RuleParamlist;
}

void XACCLangParser::ParamlistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParamlist(this);
}

void XACCLangParser::ParamlistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParamlist(this);
}

XACCLangParser::ParamlistContext* XACCLangParser::paramlist() {
  ParamlistContext *_localctx = _tracker.createInstance<ParamlistContext>(_ctx, getState());
  enterRule(_localctx, 20, XACCLangParser::RuleParamlist);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(111);
    param();
    setState(114);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(112);
      match(XACCLangParser::T__4);
      setState(113);
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

XACCLangParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::IdContext* XACCLangParser::ParamContext::id() {
  return getRuleContext<XACCLangParser::IdContext>(0);
}


size_t XACCLangParser::ParamContext::getRuleIndex() const {
  return XACCLangParser::RuleParam;
}

void XACCLangParser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void XACCLangParser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}

XACCLangParser::ParamContext* XACCLangParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 22, XACCLangParser::RuleParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(119);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case XACCLangParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(116);
        id();
        break;
      }

      case XACCLangParser::T__13: {
        enterOuterAlt(_localctx, 2);
        setState(117);
        match(XACCLangParser::T__13);
        setState(118);
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

XACCLangParser::UopContext::UopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::GateContext* XACCLangParser::UopContext::gate() {
  return getRuleContext<XACCLangParser::GateContext>(0);
}

XACCLangParser::ExplistContext* XACCLangParser::UopContext::explist() {
  return getRuleContext<XACCLangParser::ExplistContext>(0);
}


size_t XACCLangParser::UopContext::getRuleIndex() const {
  return XACCLangParser::RuleUop;
}

void XACCLangParser::UopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUop(this);
}

void XACCLangParser::UopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUop(this);
}

XACCLangParser::UopContext* XACCLangParser::uop() {
  UopContext *_localctx = _tracker.createInstance<UopContext>(_ctx, getState());
  enterRule(_localctx, 24, XACCLangParser::RuleUop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    dynamic_cast<UopContext *>(_localctx)->gatename = gate();
    setState(122);
    match(XACCLangParser::T__2);
    setState(124);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << XACCLangParser::T__2)
      | (1ULL << XACCLangParser::T__14)
      | (1ULL << XACCLangParser::T__16)
      | (1ULL << XACCLangParser::T__20)
      | (1ULL << XACCLangParser::T__24)
      | (1ULL << XACCLangParser::T__25)
      | (1ULL << XACCLangParser::T__26)
      | (1ULL << XACCLangParser::T__27)
      | (1ULL << XACCLangParser::T__28)
      | (1ULL << XACCLangParser::T__29)
      | (1ULL << XACCLangParser::ID)
      | (1ULL << XACCLangParser::REAL)
      | (1ULL << XACCLangParser::INT))) != 0)) {
      setState(123);
      explist();
    }
    setState(126);
    match(XACCLangParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GateContext ------------------------------------------------------------------

XACCLangParser::GateContext::GateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::IdContext* XACCLangParser::GateContext::id() {
  return getRuleContext<XACCLangParser::IdContext>(0);
}


size_t XACCLangParser::GateContext::getRuleIndex() const {
  return XACCLangParser::RuleGate;
}

void XACCLangParser::GateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterGate(this);
}

void XACCLangParser::GateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitGate(this);
}

XACCLangParser::GateContext* XACCLangParser::gate() {
  GateContext *_localctx = _tracker.createInstance<GateContext>(_ctx, getState());
  enterRule(_localctx, 26, XACCLangParser::RuleGate);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(128);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExplistContext ------------------------------------------------------------------

XACCLangParser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<XACCLangParser::ExpContext *> XACCLangParser::ExplistContext::exp() {
  return getRuleContexts<XACCLangParser::ExpContext>();
}

XACCLangParser::ExpContext* XACCLangParser::ExplistContext::exp(size_t i) {
  return getRuleContext<XACCLangParser::ExpContext>(i);
}


size_t XACCLangParser::ExplistContext::getRuleIndex() const {
  return XACCLangParser::RuleExplist;
}

void XACCLangParser::ExplistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExplist(this);
}

void XACCLangParser::ExplistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExplist(this);
}

XACCLangParser::ExplistContext* XACCLangParser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 28, XACCLangParser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(130);
    exp(0);
    setState(135);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == XACCLangParser::T__4) {
      setState(131);
      match(XACCLangParser::T__4);
      setState(132);
      exp(0);
      setState(137);
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

XACCLangParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::RealContext* XACCLangParser::ExpContext::real() {
  return getRuleContext<XACCLangParser::RealContext>(0);
}

tree::TerminalNode* XACCLangParser::ExpContext::INT() {
  return getToken(XACCLangParser::INT, 0);
}

XACCLangParser::IdContext* XACCLangParser::ExpContext::id() {
  return getRuleContext<XACCLangParser::IdContext>(0);
}

std::vector<XACCLangParser::ExpContext *> XACCLangParser::ExpContext::exp() {
  return getRuleContexts<XACCLangParser::ExpContext>();
}

XACCLangParser::ExpContext* XACCLangParser::ExpContext::exp(size_t i) {
  return getRuleContext<XACCLangParser::ExpContext>(i);
}

XACCLangParser::UnaryopContext* XACCLangParser::ExpContext::unaryop() {
  return getRuleContext<XACCLangParser::UnaryopContext>(0);
}

XACCLangParser::KeyContext* XACCLangParser::ExpContext::key() {
  return getRuleContext<XACCLangParser::KeyContext>(0);
}

std::vector<XACCLangParser::CouplerContext *> XACCLangParser::ExpContext::coupler() {
  return getRuleContexts<XACCLangParser::CouplerContext>();
}

XACCLangParser::CouplerContext* XACCLangParser::ExpContext::coupler(size_t i) {
  return getRuleContext<XACCLangParser::CouplerContext>(i);
}


size_t XACCLangParser::ExpContext::getRuleIndex() const {
  return XACCLangParser::RuleExp;
}

void XACCLangParser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void XACCLangParser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


XACCLangParser::ExpContext* XACCLangParser::exp() {
   return exp(0);
}

XACCLangParser::ExpContext* XACCLangParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  XACCLangParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  XACCLangParser::ExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 30;
  enterRecursionRule(_localctx, 30, XACCLangParser::RuleExp, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(169);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx)) {
    case 1: {
      setState(139);
      real();
      break;
    }

    case 2: {
      setState(140);
      match(XACCLangParser::INT);
      break;
    }

    case 3: {
      setState(141);
      match(XACCLangParser::T__14);
      break;
    }

    case 4: {
      setState(142);
      id();
      break;
    }

    case 5: {
      setState(143);
      match(XACCLangParser::T__16);
      setState(144);
      exp(6);
      break;
    }

    case 6: {
      setState(145);
      match(XACCLangParser::T__2);
      setState(146);
      exp(0);
      setState(147);
      match(XACCLangParser::T__5);
      break;
    }

    case 7: {
      setState(149);
      unaryop();
      setState(150);
      match(XACCLangParser::T__2);
      setState(151);
      exp(0);
      setState(152);
      match(XACCLangParser::T__5);
      break;
    }

    case 8: {
      setState(154);
      key();
      setState(155);
      match(XACCLangParser::T__19);
      setState(156);
      exp(2);
      break;
    }

    case 9: {
      setState(158);
      match(XACCLangParser::T__20);
      setState(165);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == XACCLangParser::T__4

      || _la == XACCLangParser::T__22) {
        setState(160);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == XACCLangParser::T__4) {
          setState(159);
          match(XACCLangParser::T__4);
        }
        setState(162);
        coupler();
        setState(167);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(168);
      match(XACCLangParser::T__21);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(188);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(186);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(171);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(172);
          match(XACCLangParser::T__15);
          setState(173);
          exp(11);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(174);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(175);
          match(XACCLangParser::T__16);
          setState(176);
          exp(10);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(177);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(178);
          match(XACCLangParser::T__13);
          setState(179);
          exp(9);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(180);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(181);
          match(XACCLangParser::T__17);
          setState(182);
          exp(8);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(183);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(184);
          match(XACCLangParser::T__18);
          setState(185);
          exp(6);
          break;
        }

        } 
      }
      setState(190);
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

//----------------- KeyContext ------------------------------------------------------------------

XACCLangParser::KeyContext::KeyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

XACCLangParser::IdContext* XACCLangParser::KeyContext::id() {
  return getRuleContext<XACCLangParser::IdContext>(0);
}


size_t XACCLangParser::KeyContext::getRuleIndex() const {
  return XACCLangParser::RuleKey;
}

void XACCLangParser::KeyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKey(this);
}

void XACCLangParser::KeyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKey(this);
}

XACCLangParser::KeyContext* XACCLangParser::key() {
  KeyContext *_localctx = _tracker.createInstance<KeyContext>(_ctx, getState());
  enterRule(_localctx, 32, XACCLangParser::RuleKey);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(191);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CouplerContext ------------------------------------------------------------------

XACCLangParser::CouplerContext::CouplerContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> XACCLangParser::CouplerContext::INT() {
  return getTokens(XACCLangParser::INT);
}

tree::TerminalNode* XACCLangParser::CouplerContext::INT(size_t i) {
  return getToken(XACCLangParser::INT, i);
}


size_t XACCLangParser::CouplerContext::getRuleIndex() const {
  return XACCLangParser::RuleCoupler;
}

void XACCLangParser::CouplerContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCoupler(this);
}

void XACCLangParser::CouplerContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCoupler(this);
}

XACCLangParser::CouplerContext* XACCLangParser::coupler() {
  CouplerContext *_localctx = _tracker.createInstance<CouplerContext>(_ctx, getState());
  enterRule(_localctx, 34, XACCLangParser::RuleCoupler);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(193);
    match(XACCLangParser::T__22);
    setState(194);
    match(XACCLangParser::INT);
    setState(195);
    match(XACCLangParser::T__4);
    setState(196);
    match(XACCLangParser::INT);
    setState(197);
    match(XACCLangParser::T__23);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryopContext ------------------------------------------------------------------

XACCLangParser::UnaryopContext::UnaryopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t XACCLangParser::UnaryopContext::getRuleIndex() const {
  return XACCLangParser::RuleUnaryop;
}

void XACCLangParser::UnaryopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryop(this);
}

void XACCLangParser::UnaryopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryop(this);
}

XACCLangParser::UnaryopContext* XACCLangParser::unaryop() {
  UnaryopContext *_localctx = _tracker.createInstance<UnaryopContext>(_ctx, getState());
  enterRule(_localctx, 36, XACCLangParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(199);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << XACCLangParser::T__24)
      | (1ULL << XACCLangParser::T__25)
      | (1ULL << XACCLangParser::T__26)
      | (1ULL << XACCLangParser::T__27)
      | (1ULL << XACCLangParser::T__28)
      | (1ULL << XACCLangParser::T__29))) != 0))) {
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

XACCLangParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* XACCLangParser::IdContext::ID() {
  return getToken(XACCLangParser::ID, 0);
}


size_t XACCLangParser::IdContext::getRuleIndex() const {
  return XACCLangParser::RuleId;
}

void XACCLangParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void XACCLangParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

XACCLangParser::IdContext* XACCLangParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 38, XACCLangParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(201);
    match(XACCLangParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

XACCLangParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* XACCLangParser::RealContext::REAL() {
  return getToken(XACCLangParser::REAL, 0);
}


size_t XACCLangParser::RealContext::getRuleIndex() const {
  return XACCLangParser::RuleReal;
}

void XACCLangParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void XACCLangParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

XACCLangParser::RealContext* XACCLangParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 40, XACCLangParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(203);
    match(XACCLangParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

XACCLangParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* XACCLangParser::StringContext::STRING() {
  return getToken(XACCLangParser::STRING, 0);
}


size_t XACCLangParser::StringContext::getRuleIndex() const {
  return XACCLangParser::RuleString;
}

void XACCLangParser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void XACCLangParser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<XACCLangListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

XACCLangParser::StringContext* XACCLangParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 42, XACCLangParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(205);
    match(XACCLangParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool XACCLangParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 15: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool XACCLangParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
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
std::vector<dfa::DFA> XACCLangParser::_decisionToDFA;
atn::PredictionContextCache XACCLangParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN XACCLangParser::_atn;
std::vector<uint16_t> XACCLangParser::_serializedATN;

std::vector<std::string> XACCLangParser::_ruleNames = {
  "xaccsrc", "xacckernel", "typedparam", "type", "kernelcall", "mainprog", 
  "program", "line", "statement", "comment", "paramlist", "param", "uop", 
  "gate", "explist", "exp", "key", "coupler", "unaryop", "id", "real", "string"
};

std::vector<std::string> XACCLangParser::_literalNames = {
  "", "'__qpu__'", "'void'", "'('", "'qbit'", "','", "')'", "'{'", "'}'", 
  "'int'", "'double'", "'float'", "';'", "'return'", "'*'", "'pi'", "'+'", 
  "'-'", "'/'", "'^'", "'='", "''['", "']''", "'['", "']'", "'sin'", "'cos'", 
  "'tan'", "'exp'", "'ln'", "'sqrt'"
};

std::vector<std::string> XACCLangParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "COMMENT", "ID", "REAL", 
  "INT", "STRING", "WS", "EOL"
};

dfa::Vocabulary XACCLangParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> XACCLangParser::_tokenNames;

XACCLangParser::Initializer::Initializer() {
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
    0x3, 0x27, 0xd2, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 0x9, 
    0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x3, 0x2, 0x7, 0x2, 
    0x30, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x33, 0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 
    0x3d, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x40, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 0x4f, 0xa, 0x6, 0x3, 
    0x6, 0x3, 0x6, 0x7, 0x6, 0x53, 0xa, 0x6, 0xc, 0x6, 0xe, 0x6, 0x56, 0xb, 
    0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 0x6, 0x8, 0x5d, 
    0xa, 0x8, 0xd, 0x8, 0xe, 0x8, 0x5e, 0x3, 0x9, 0x6, 0x9, 0x62, 0xa, 0x9, 
    0xd, 0x9, 0xe, 0x9, 0x63, 0x3, 0x9, 0x5, 0x9, 0x67, 0xa, 0x9, 0x3, 0xa, 
    0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 0x6e, 0xa, 0xa, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 0x75, 0xa, 0xc, 
    0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x5, 0xd, 0x7a, 0xa, 0xd, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xe, 0x5, 0xe, 0x7f, 0xa, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0x88, 0xa, 0x10, 
    0xc, 0x10, 0xe, 0x10, 0x8b, 0xb, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 
    0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x5, 
    0x11, 0xa3, 0xa, 0x11, 0x3, 0x11, 0x7, 0x11, 0xa6, 0xa, 0x11, 0xc, 0x11, 
    0xe, 0x11, 0xa9, 0xb, 0x11, 0x3, 0x11, 0x5, 0x11, 0xac, 0xa, 0x11, 0x3, 
    0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 
    0x11, 0x3, 0x11, 0x7, 0x11, 0xbd, 0xa, 0x11, 0xc, 0x11, 0xe, 0x11, 0xc0, 
    0xb, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x15, 0x3, 0x15, 
    0x3, 0x16, 0x3, 0x16, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x2, 0x3, 0x20, 
    0x18, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 
    0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2, 0x4, 
    0x3, 0x2, 0xb, 0xd, 0x3, 0x2, 0x1b, 0x20, 0x2, 0xd6, 0x2, 0x31, 0x3, 
    0x2, 0x2, 0x2, 0x4, 0x34, 0x3, 0x2, 0x2, 0x2, 0x6, 0x46, 0x3, 0x2, 0x2, 
    0x2, 0x8, 0x49, 0x3, 0x2, 0x2, 0x2, 0xa, 0x4b, 0x3, 0x2, 0x2, 0x2, 0xc, 
    0x59, 0x3, 0x2, 0x2, 0x2, 0xe, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x10, 0x66, 
    0x3, 0x2, 0x2, 0x2, 0x12, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x14, 0x6f, 0x3, 
    0x2, 0x2, 0x2, 0x16, 0x71, 0x3, 0x2, 0x2, 0x2, 0x18, 0x79, 0x3, 0x2, 
    0x2, 0x2, 0x1a, 0x7b, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x82, 0x3, 0x2, 0x2, 
    0x2, 0x1e, 0x84, 0x3, 0x2, 0x2, 0x2, 0x20, 0xab, 0x3, 0x2, 0x2, 0x2, 
    0x22, 0xc1, 0x3, 0x2, 0x2, 0x2, 0x24, 0xc3, 0x3, 0x2, 0x2, 0x2, 0x26, 
    0xc9, 0x3, 0x2, 0x2, 0x2, 0x28, 0xcb, 0x3, 0x2, 0x2, 0x2, 0x2a, 0xcd, 
    0x3, 0x2, 0x2, 0x2, 0x2c, 0xcf, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x30, 0x5, 
    0x4, 0x3, 0x2, 0x2f, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x30, 0x33, 0x3, 0x2, 
    0x2, 0x2, 0x31, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x31, 0x32, 0x3, 0x2, 0x2, 
    0x2, 0x32, 0x3, 0x3, 0x2, 0x2, 0x2, 0x33, 0x31, 0x3, 0x2, 0x2, 0x2, 
    0x34, 0x35, 0x7, 0x3, 0x2, 0x2, 0x35, 0x36, 0x7, 0x4, 0x2, 0x2, 0x36, 
    0x37, 0x5, 0x28, 0x15, 0x2, 0x37, 0x38, 0x7, 0x5, 0x2, 0x2, 0x38, 0x39, 
    0x7, 0x6, 0x2, 0x2, 0x39, 0x3e, 0x5, 0x28, 0x15, 0x2, 0x3a, 0x3b, 0x7, 
    0x7, 0x2, 0x2, 0x3b, 0x3d, 0x5, 0x6, 0x4, 0x2, 0x3c, 0x3a, 0x3, 0x2, 
    0x2, 0x2, 0x3d, 0x40, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3c, 0x3, 0x2, 0x2, 
    0x2, 0x3e, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x3f, 0x41, 0x3, 0x2, 0x2, 0x2, 
    0x40, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x41, 0x42, 0x7, 0x8, 0x2, 0x2, 0x42, 
    0x43, 0x7, 0x9, 0x2, 0x2, 0x43, 0x44, 0x5, 0xc, 0x7, 0x2, 0x44, 0x45, 
    0x7, 0xa, 0x2, 0x2, 0x45, 0x5, 0x3, 0x2, 0x2, 0x2, 0x46, 0x47, 0x5, 
    0x8, 0x5, 0x2, 0x47, 0x48, 0x5, 0x18, 0xd, 0x2, 0x48, 0x7, 0x3, 0x2, 
    0x2, 0x2, 0x49, 0x4a, 0x9, 0x2, 0x2, 0x2, 0x4a, 0x9, 0x3, 0x2, 0x2, 
    0x2, 0x4b, 0x4c, 0x5, 0x28, 0x15, 0x2, 0x4c, 0x4e, 0x7, 0x5, 0x2, 0x2, 
    0x4d, 0x4f, 0x5, 0x18, 0xd, 0x2, 0x4e, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x4e, 
    0x4f, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x54, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 
    0x7, 0x7, 0x2, 0x2, 0x51, 0x53, 0x5, 0x18, 0xd, 0x2, 0x52, 0x50, 0x3, 
    0x2, 0x2, 0x2, 0x53, 0x56, 0x3, 0x2, 0x2, 0x2, 0x54, 0x52, 0x3, 0x2, 
    0x2, 0x2, 0x54, 0x55, 0x3, 0x2, 0x2, 0x2, 0x55, 0x57, 0x3, 0x2, 0x2, 
    0x2, 0x56, 0x54, 0x3, 0x2, 0x2, 0x2, 0x57, 0x58, 0x7, 0x8, 0x2, 0x2, 
    0x58, 0xb, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5a, 0x5, 0xe, 0x8, 0x2, 0x5a, 
    0xd, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5d, 0x5, 0x10, 0x9, 0x2, 0x5c, 0x5b, 
    0x3, 0x2, 0x2, 0x2, 0x5d, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x5c, 0x3, 
    0x2, 0x2, 0x2, 0x5e, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x5f, 0xf, 0x3, 0x2, 
    0x2, 0x2, 0x60, 0x62, 0x5, 0x12, 0xa, 0x2, 0x61, 0x60, 0x3, 0x2, 0x2, 
    0x2, 0x62, 0x63, 0x3, 0x2, 0x2, 0x2, 0x63, 0x61, 0x3, 0x2, 0x2, 0x2, 
    0x63, 0x64, 0x3, 0x2, 0x2, 0x2, 0x64, 0x67, 0x3, 0x2, 0x2, 0x2, 0x65, 
    0x67, 0x5, 0x14, 0xb, 0x2, 0x66, 0x61, 0x3, 0x2, 0x2, 0x2, 0x66, 0x65, 
    0x3, 0x2, 0x2, 0x2, 0x67, 0x11, 0x3, 0x2, 0x2, 0x2, 0x68, 0x69, 0x5, 
    0x1a, 0xe, 0x2, 0x69, 0x6a, 0x7, 0xe, 0x2, 0x2, 0x6a, 0x6e, 0x3, 0x2, 
    0x2, 0x2, 0x6b, 0x6c, 0x7, 0xf, 0x2, 0x2, 0x6c, 0x6e, 0x7, 0xe, 0x2, 
    0x2, 0x6d, 0x68, 0x3, 0x2, 0x2, 0x2, 0x6d, 0x6b, 0x3, 0x2, 0x2, 0x2, 
    0x6e, 0x13, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x70, 0x7, 0x21, 0x2, 0x2, 0x70, 
    0x15, 0x3, 0x2, 0x2, 0x2, 0x71, 0x74, 0x5, 0x18, 0xd, 0x2, 0x72, 0x73, 
    0x7, 0x7, 0x2, 0x2, 0x73, 0x75, 0x5, 0x16, 0xc, 0x2, 0x74, 0x72, 0x3, 
    0x2, 0x2, 0x2, 0x74, 0x75, 0x3, 0x2, 0x2, 0x2, 0x75, 0x17, 0x3, 0x2, 
    0x2, 0x2, 0x76, 0x7a, 0x5, 0x28, 0x15, 0x2, 0x77, 0x78, 0x7, 0x10, 0x2, 
    0x2, 0x78, 0x7a, 0x5, 0x28, 0x15, 0x2, 0x79, 0x76, 0x3, 0x2, 0x2, 0x2, 
    0x79, 0x77, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x19, 0x3, 0x2, 0x2, 0x2, 0x7b, 
    0x7c, 0x5, 0x1c, 0xf, 0x2, 0x7c, 0x7e, 0x7, 0x5, 0x2, 0x2, 0x7d, 0x7f, 
    0x5, 0x1e, 0x10, 0x2, 0x7e, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x7e, 0x7f, 0x3, 
    0x2, 0x2, 0x2, 0x7f, 0x80, 0x3, 0x2, 0x2, 0x2, 0x80, 0x81, 0x7, 0x8, 
    0x2, 0x2, 0x81, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x82, 0x83, 0x5, 0x28, 0x15, 
    0x2, 0x83, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x84, 0x89, 0x5, 0x20, 0x11, 0x2, 
    0x85, 0x86, 0x7, 0x7, 0x2, 0x2, 0x86, 0x88, 0x5, 0x20, 0x11, 0x2, 0x87, 
    0x85, 0x3, 0x2, 0x2, 0x2, 0x88, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x89, 0x87, 
    0x3, 0x2, 0x2, 0x2, 0x89, 0x8a, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x1f, 0x3, 
    0x2, 0x2, 0x2, 0x8b, 0x89, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8d, 0x8, 0x11, 
    0x1, 0x2, 0x8d, 0xac, 0x5, 0x2a, 0x16, 0x2, 0x8e, 0xac, 0x7, 0x24, 0x2, 
    0x2, 0x8f, 0xac, 0x7, 0x11, 0x2, 0x2, 0x90, 0xac, 0x5, 0x28, 0x15, 0x2, 
    0x91, 0x92, 0x7, 0x13, 0x2, 0x2, 0x92, 0xac, 0x5, 0x20, 0x11, 0x8, 0x93, 
    0x94, 0x7, 0x5, 0x2, 0x2, 0x94, 0x95, 0x5, 0x20, 0x11, 0x2, 0x95, 0x96, 
    0x7, 0x8, 0x2, 0x2, 0x96, 0xac, 0x3, 0x2, 0x2, 0x2, 0x97, 0x98, 0x5, 
    0x26, 0x14, 0x2, 0x98, 0x99, 0x7, 0x5, 0x2, 0x2, 0x99, 0x9a, 0x5, 0x20, 
    0x11, 0x2, 0x9a, 0x9b, 0x7, 0x8, 0x2, 0x2, 0x9b, 0xac, 0x3, 0x2, 0x2, 
    0x2, 0x9c, 0x9d, 0x5, 0x22, 0x12, 0x2, 0x9d, 0x9e, 0x7, 0x16, 0x2, 0x2, 
    0x9e, 0x9f, 0x5, 0x20, 0x11, 0x4, 0x9f, 0xac, 0x3, 0x2, 0x2, 0x2, 0xa0, 
    0xa7, 0x7, 0x17, 0x2, 0x2, 0xa1, 0xa3, 0x7, 0x7, 0x2, 0x2, 0xa2, 0xa1, 
    0x3, 0x2, 0x2, 0x2, 0xa2, 0xa3, 0x3, 0x2, 0x2, 0x2, 0xa3, 0xa4, 0x3, 
    0x2, 0x2, 0x2, 0xa4, 0xa6, 0x5, 0x24, 0x13, 0x2, 0xa5, 0xa2, 0x3, 0x2, 
    0x2, 0x2, 0xa6, 0xa9, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa5, 0x3, 0x2, 0x2, 
    0x2, 0xa7, 0xa8, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xaa, 0x3, 0x2, 0x2, 0x2, 
    0xa9, 0xa7, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xac, 0x7, 0x18, 0x2, 0x2, 0xab, 
    0x8c, 0x3, 0x2, 0x2, 0x2, 0xab, 0x8e, 0x3, 0x2, 0x2, 0x2, 0xab, 0x8f, 
    0x3, 0x2, 0x2, 0x2, 0xab, 0x90, 0x3, 0x2, 0x2, 0x2, 0xab, 0x91, 0x3, 
    0x2, 0x2, 0x2, 0xab, 0x93, 0x3, 0x2, 0x2, 0x2, 0xab, 0x97, 0x3, 0x2, 
    0x2, 0x2, 0xab, 0x9c, 0x3, 0x2, 0x2, 0x2, 0xab, 0xa0, 0x3, 0x2, 0x2, 
    0x2, 0xac, 0xbe, 0x3, 0x2, 0x2, 0x2, 0xad, 0xae, 0xc, 0xc, 0x2, 0x2, 
    0xae, 0xaf, 0x7, 0x12, 0x2, 0x2, 0xaf, 0xbd, 0x5, 0x20, 0x11, 0xd, 0xb0, 
    0xb1, 0xc, 0xb, 0x2, 0x2, 0xb1, 0xb2, 0x7, 0x13, 0x2, 0x2, 0xb2, 0xbd, 
    0x5, 0x20, 0x11, 0xc, 0xb3, 0xb4, 0xc, 0xa, 0x2, 0x2, 0xb4, 0xb5, 0x7, 
    0x10, 0x2, 0x2, 0xb5, 0xbd, 0x5, 0x20, 0x11, 0xb, 0xb6, 0xb7, 0xc, 0x9, 
    0x2, 0x2, 0xb7, 0xb8, 0x7, 0x14, 0x2, 0x2, 0xb8, 0xbd, 0x5, 0x20, 0x11, 
    0xa, 0xb9, 0xba, 0xc, 0x7, 0x2, 0x2, 0xba, 0xbb, 0x7, 0x15, 0x2, 0x2, 
    0xbb, 0xbd, 0x5, 0x20, 0x11, 0x8, 0xbc, 0xad, 0x3, 0x2, 0x2, 0x2, 0xbc, 
    0xb0, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xb6, 
    0x3, 0x2, 0x2, 0x2, 0xbc, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xbd, 0xc0, 0x3, 
    0x2, 0x2, 0x2, 0xbe, 0xbc, 0x3, 0x2, 0x2, 0x2, 0xbe, 0xbf, 0x3, 0x2, 
    0x2, 0x2, 0xbf, 0x21, 0x3, 0x2, 0x2, 0x2, 0xc0, 0xbe, 0x3, 0x2, 0x2, 
    0x2, 0xc1, 0xc2, 0x5, 0x28, 0x15, 0x2, 0xc2, 0x23, 0x3, 0x2, 0x2, 0x2, 
    0xc3, 0xc4, 0x7, 0x19, 0x2, 0x2, 0xc4, 0xc5, 0x7, 0x24, 0x2, 0x2, 0xc5, 
    0xc6, 0x7, 0x7, 0x2, 0x2, 0xc6, 0xc7, 0x7, 0x24, 0x2, 0x2, 0xc7, 0xc8, 
    0x7, 0x1a, 0x2, 0x2, 0xc8, 0x25, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xca, 0x9, 
    0x3, 0x2, 0x2, 0xca, 0x27, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 0x7, 0x22, 
    0x2, 0x2, 0xcc, 0x29, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xce, 0x7, 0x23, 0x2, 
    0x2, 0xce, 0x2b, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xd0, 0x7, 0x25, 0x2, 0x2, 
    0xd0, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x13, 0x31, 0x3e, 0x4e, 0x54, 0x5e, 
    0x63, 0x66, 0x6d, 0x74, 0x79, 0x7e, 0x89, 0xa2, 0xa7, 0xab, 0xbc, 0xbe, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

XACCLangParser::Initializer XACCLangParser::_init;
