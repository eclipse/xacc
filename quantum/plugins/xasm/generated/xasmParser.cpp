
// Generated from xasm.g4 by ANTLR 4.7.2


#include "xasmListener.h"

#include "xasmParser.h"


using namespace antlrcpp;
using namespace xasm;
using namespace antlr4;

xasmParser::xasmParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

xasmParser::~xasmParser() {
  delete _interpreter;
}

std::string xasmParser::getGrammarFileName() const {
  return "xasm.g4";
}

const std::vector<std::string>& xasmParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& xasmParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

xasmParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::XacckernelContext* xasmParser::XaccsrcContext::xacckernel() {
  return getRuleContext<xasmParser::XacckernelContext>(0);
}

xasmParser::XacclambdaContext* xasmParser::XaccsrcContext::xacclambda() {
  return getRuleContext<xasmParser::XacclambdaContext>(0);
}


size_t xasmParser::XaccsrcContext::getRuleIndex() const {
  return xasmParser::RuleXaccsrc;
}

void xasmParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void xasmParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

xasmParser::XaccsrcContext* xasmParser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, xasmParser::RuleXaccsrc);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(46);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case xasmParser::T__0: {
        enterOuterAlt(_localctx, 1);
        setState(44);
        xacckernel();
        break;
      }

      case xasmParser::T__8: {
        enterOuterAlt(_localctx, 2);
        setState(45);
        xacclambda();
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

//----------------- XacckernelContext ------------------------------------------------------------------

xasmParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::MainprogContext* xasmParser::XacckernelContext::mainprog() {
  return getRuleContext<xasmParser::MainprogContext>(0);
}

std::vector<xasmParser::IdContext *> xasmParser::XacckernelContext::id() {
  return getRuleContexts<xasmParser::IdContext>();
}

xasmParser::IdContext* xasmParser::XacckernelContext::id(size_t i) {
  return getRuleContext<xasmParser::IdContext>(i);
}

std::vector<xasmParser::TypedparamContext *> xasmParser::XacckernelContext::typedparam() {
  return getRuleContexts<xasmParser::TypedparamContext>();
}

xasmParser::TypedparamContext* xasmParser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<xasmParser::TypedparamContext>(i);
}


size_t xasmParser::XacckernelContext::getRuleIndex() const {
  return xasmParser::RuleXacckernel;
}

void xasmParser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void xasmParser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

xasmParser::XacckernelContext* xasmParser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, xasmParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(48);
    match(xasmParser::T__0);
    setState(49);
    match(xasmParser::T__1);
    setState(50);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(51);
    match(xasmParser::T__2);
    setState(52);
    match(xasmParser::T__3);
    setState(53);
    dynamic_cast<XacckernelContext *>(_localctx)->acceleratorbuffer = id();
    setState(58);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__4) {
      setState(54);
      match(xasmParser::T__4);
      setState(55);
      typedparam();
      setState(60);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(61);
    match(xasmParser::T__5);
    setState(62);
    match(xasmParser::T__6);
    setState(63);
    mainprog();
    setState(64);
    match(xasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- XacclambdaContext ------------------------------------------------------------------

xasmParser::XacclambdaContext::XacclambdaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::MainprogContext* xasmParser::XacclambdaContext::mainprog() {
  return getRuleContext<xasmParser::MainprogContext>(0);
}

xasmParser::IdContext* xasmParser::XacclambdaContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

std::vector<xasmParser::TypedparamContext *> xasmParser::XacclambdaContext::typedparam() {
  return getRuleContexts<xasmParser::TypedparamContext>();
}

xasmParser::TypedparamContext* xasmParser::XacclambdaContext::typedparam(size_t i) {
  return getRuleContext<xasmParser::TypedparamContext>(i);
}


size_t xasmParser::XacclambdaContext::getRuleIndex() const {
  return xasmParser::RuleXacclambda;
}

void xasmParser::XacclambdaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacclambda(this);
}

void xasmParser::XacclambdaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacclambda(this);
}

xasmParser::XacclambdaContext* xasmParser::xacclambda() {
  XacclambdaContext *_localctx = _tracker.createInstance<XacclambdaContext>(_ctx, getState());
  enterRule(_localctx, 4, xasmParser::RuleXacclambda);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(66);
    match(xasmParser::T__8);
    setState(68);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__9

    || _la == xasmParser::T__10) {
      setState(67);
      _la = _input->LA(1);
      if (!(_la == xasmParser::T__9

      || _la == xasmParser::T__10)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(70);
    match(xasmParser::T__11);
    setState(71);
    match(xasmParser::T__2);
    setState(72);
    match(xasmParser::T__3);
    setState(73);
    dynamic_cast<XacclambdaContext *>(_localctx)->acceleratorbuffer = id();
    setState(78);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__4) {
      setState(74);
      match(xasmParser::T__4);
      setState(75);
      typedparam();
      setState(80);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(81);
    match(xasmParser::T__5);
    setState(82);
    match(xasmParser::T__6);
    setState(83);
    mainprog();
    setState(84);
    match(xasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypedparamContext ------------------------------------------------------------------

xasmParser::TypedparamContext::TypedparamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::TypeContext* xasmParser::TypedparamContext::type() {
  return getRuleContext<xasmParser::TypeContext>(0);
}

xasmParser::IdContext* xasmParser::TypedparamContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}


size_t xasmParser::TypedparamContext::getRuleIndex() const {
  return xasmParser::RuleTypedparam;
}

void xasmParser::TypedparamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypedparam(this);
}

void xasmParser::TypedparamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypedparam(this);
}

xasmParser::TypedparamContext* xasmParser::typedparam() {
  TypedparamContext *_localctx = _tracker.createInstance<TypedparamContext>(_ctx, getState());
  enterRule(_localctx, 6, xasmParser::RuleTypedparam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(86);
    type();
    setState(87);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

xasmParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t xasmParser::TypeContext::getRuleIndex() const {
  return xasmParser::RuleType;
}

void xasmParser::TypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType(this);
}

void xasmParser::TypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType(this);
}

xasmParser::TypeContext* xasmParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 8, xasmParser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(89);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__12)
      | (1ULL << xasmParser::T__13)
      | (1ULL << xasmParser::T__14)
      | (1ULL << xasmParser::T__15))) != 0))) {
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

//----------------- MainprogContext ------------------------------------------------------------------

xasmParser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::ProgramContext* xasmParser::MainprogContext::program() {
  return getRuleContext<xasmParser::ProgramContext>(0);
}


size_t xasmParser::MainprogContext::getRuleIndex() const {
  return xasmParser::RuleMainprog;
}

void xasmParser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void xasmParser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

xasmParser::MainprogContext* xasmParser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 10, xasmParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(91);
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

xasmParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::LineContext *> xasmParser::ProgramContext::line() {
  return getRuleContexts<xasmParser::LineContext>();
}

xasmParser::LineContext* xasmParser::ProgramContext::line(size_t i) {
  return getRuleContext<xasmParser::LineContext>(i);
}


size_t xasmParser::ProgramContext::getRuleIndex() const {
  return xasmParser::RuleProgram;
}

void xasmParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void xasmParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

xasmParser::ProgramContext* xasmParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 12, xasmParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(94); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(93);
      line();
      setState(96); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__17)
      | (1ULL << xasmParser::T__18)
      | (1ULL << xasmParser::COMMENT)
      | (1ULL << xasmParser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

xasmParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::StatementContext *> xasmParser::LineContext::statement() {
  return getRuleContexts<xasmParser::StatementContext>();
}

xasmParser::StatementContext* xasmParser::LineContext::statement(size_t i) {
  return getRuleContext<xasmParser::StatementContext>(i);
}

xasmParser::CommentContext* xasmParser::LineContext::comment() {
  return getRuleContext<xasmParser::CommentContext>(0);
}


size_t xasmParser::LineContext::getRuleIndex() const {
  return xasmParser::RuleLine;
}

void xasmParser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void xasmParser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

xasmParser::LineContext* xasmParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 14, xasmParser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(104);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case xasmParser::T__17:
      case xasmParser::T__18:
      case xasmParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(99); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(98);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(101); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case xasmParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(103);
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

xasmParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::InstructionContext* xasmParser::StatementContext::instruction() {
  return getRuleContext<xasmParser::InstructionContext>(0);
}

xasmParser::ForstmtContext* xasmParser::StatementContext::forstmt() {
  return getRuleContext<xasmParser::ForstmtContext>(0);
}


size_t xasmParser::StatementContext::getRuleIndex() const {
  return xasmParser::RuleStatement;
}

void xasmParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void xasmParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

xasmParser::StatementContext* xasmParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 16, xasmParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(112);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case xasmParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(106);
        instruction();
        setState(107);
        match(xasmParser::T__16);
        break;
      }

      case xasmParser::T__18: {
        enterOuterAlt(_localctx, 2);
        setState(109);
        forstmt();
        break;
      }

      case xasmParser::T__17: {
        enterOuterAlt(_localctx, 3);
        setState(110);
        match(xasmParser::T__17);
        setState(111);
        match(xasmParser::T__16);
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

xasmParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* xasmParser::CommentContext::COMMENT() {
  return getToken(xasmParser::COMMENT, 0);
}


size_t xasmParser::CommentContext::getRuleIndex() const {
  return xasmParser::RuleComment;
}

void xasmParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void xasmParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

xasmParser::CommentContext* xasmParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 18, xasmParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(114);
    match(xasmParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ForstmtContext ------------------------------------------------------------------

xasmParser::ForstmtContext::ForstmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::IdContext *> xasmParser::ForstmtContext::id() {
  return getRuleContexts<xasmParser::IdContext>();
}

xasmParser::IdContext* xasmParser::ForstmtContext::id(size_t i) {
  return getRuleContext<xasmParser::IdContext>(i);
}

std::vector<tree::TerminalNode *> xasmParser::ForstmtContext::INT() {
  return getTokens(xasmParser::INT);
}

tree::TerminalNode* xasmParser::ForstmtContext::INT(size_t i) {
  return getToken(xasmParser::INT, i);
}

std::vector<xasmParser::StatementContext *> xasmParser::ForstmtContext::statement() {
  return getRuleContexts<xasmParser::StatementContext>();
}

xasmParser::StatementContext* xasmParser::ForstmtContext::statement(size_t i) {
  return getRuleContext<xasmParser::StatementContext>(i);
}


size_t xasmParser::ForstmtContext::getRuleIndex() const {
  return xasmParser::RuleForstmt;
}

void xasmParser::ForstmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForstmt(this);
}

void xasmParser::ForstmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForstmt(this);
}

xasmParser::ForstmtContext* xasmParser::forstmt() {
  ForstmtContext *_localctx = _tracker.createInstance<ForstmtContext>(_ctx, getState());
  enterRule(_localctx, 20, xasmParser::RuleForstmt);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(116);
    match(xasmParser::T__18);
    setState(117);
    match(xasmParser::T__2);
    setState(118);
    _la = _input->LA(1);
    if (!(_la == xasmParser::T__12

    || _la == xasmParser::T__19)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(119);
    dynamic_cast<ForstmtContext *>(_localctx)->varname = id();
    setState(120);
    match(xasmParser::T__10);
    setState(121);
    dynamic_cast<ForstmtContext *>(_localctx)->start = match(xasmParser::INT);
    setState(122);
    match(xasmParser::T__16);
    setState(123);
    id();
    setState(124);
    dynamic_cast<ForstmtContext *>(_localctx)->comparator = _input->LT(1);
    _la = _input->LA(1);
    if (!(_la == xasmParser::T__20

    || _la == xasmParser::T__21)) {
      dynamic_cast<ForstmtContext *>(_localctx)->comparator = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(125);
    dynamic_cast<ForstmtContext *>(_localctx)->end = match(xasmParser::INT);
    setState(126);
    match(xasmParser::T__16);
    setState(127);
    id();
    setState(128);
    dynamic_cast<ForstmtContext *>(_localctx)->inc_or_dec = _input->LT(1);
    _la = _input->LA(1);
    if (!(_la == xasmParser::T__22

    || _la == xasmParser::T__23)) {
      dynamic_cast<ForstmtContext *>(_localctx)->inc_or_dec = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(129);
    match(xasmParser::T__5);
    setState(130);
    match(xasmParser::T__6);
    setState(132); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(131);
      dynamic_cast<ForstmtContext *>(_localctx)->forScope = statement();
      setState(134); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__17)
      | (1ULL << xasmParser::T__18)
      | (1ULL << xasmParser::ID))) != 0));
    setState(136);
    match(xasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstructionContext ------------------------------------------------------------------

xasmParser::InstructionContext::InstructionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::IdContext* xasmParser::InstructionContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

xasmParser::ExplistContext* xasmParser::InstructionContext::explist() {
  return getRuleContext<xasmParser::ExplistContext>(0);
}

xasmParser::OptionsMapContext* xasmParser::InstructionContext::optionsMap() {
  return getRuleContext<xasmParser::OptionsMapContext>(0);
}


size_t xasmParser::InstructionContext::getRuleIndex() const {
  return xasmParser::RuleInstruction;
}

void xasmParser::InstructionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInstruction(this);
}

void xasmParser::InstructionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInstruction(this);
}

xasmParser::InstructionContext* xasmParser::instruction() {
  InstructionContext *_localctx = _tracker.createInstance<InstructionContext>(_ctx, getState());
  enterRule(_localctx, 22, xasmParser::RuleInstruction);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(138);
    dynamic_cast<InstructionContext *>(_localctx)->inst_name = id();
    setState(139);
    match(xasmParser::T__2);

    setState(140);
    dynamic_cast<InstructionContext *>(_localctx)->bits_and_params = explist();
    setState(143);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__4) {
      setState(141);
      match(xasmParser::T__4);
      setState(142);
      dynamic_cast<InstructionContext *>(_localctx)->options = optionsMap();
    }
    setState(145);
    match(xasmParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BufferIndexContext ------------------------------------------------------------------

xasmParser::BufferIndexContext::BufferIndexContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::IdContext* xasmParser::BufferIndexContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

xasmParser::ExpContext* xasmParser::BufferIndexContext::exp() {
  return getRuleContext<xasmParser::ExpContext>(0);
}


size_t xasmParser::BufferIndexContext::getRuleIndex() const {
  return xasmParser::RuleBufferIndex;
}

void xasmParser::BufferIndexContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBufferIndex(this);
}

void xasmParser::BufferIndexContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBufferIndex(this);
}

xasmParser::BufferIndexContext* xasmParser::bufferIndex() {
  BufferIndexContext *_localctx = _tracker.createInstance<BufferIndexContext>(_ctx, getState());
  enterRule(_localctx, 24, xasmParser::RuleBufferIndex);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(147);
    id();

    setState(148);
    match(xasmParser::T__8);
    setState(149);
    exp(0);
    setState(150);
    match(xasmParser::T__11);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitsOrParamTypeContext ------------------------------------------------------------------

xasmParser::BitsOrParamTypeContext::BitsOrParamTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::BufferIndexContext *> xasmParser::BitsOrParamTypeContext::bufferIndex() {
  return getRuleContexts<xasmParser::BufferIndexContext>();
}

xasmParser::BufferIndexContext* xasmParser::BitsOrParamTypeContext::bufferIndex(size_t i) {
  return getRuleContext<xasmParser::BufferIndexContext>(i);
}

std::vector<xasmParser::ExpContext *> xasmParser::BitsOrParamTypeContext::exp() {
  return getRuleContexts<xasmParser::ExpContext>();
}

xasmParser::ExpContext* xasmParser::BitsOrParamTypeContext::exp(size_t i) {
  return getRuleContext<xasmParser::ExpContext>(i);
}


size_t xasmParser::BitsOrParamTypeContext::getRuleIndex() const {
  return xasmParser::RuleBitsOrParamType;
}

void xasmParser::BitsOrParamTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitsOrParamType(this);
}

void xasmParser::BitsOrParamTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitsOrParamType(this);
}

xasmParser::BitsOrParamTypeContext* xasmParser::bitsOrParamType() {
  BitsOrParamTypeContext *_localctx = _tracker.createInstance<BitsOrParamTypeContext>(_ctx, getState());
  enterRule(_localctx, 26, xasmParser::RuleBitsOrParamType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(166);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(152);
      bufferIndex();
      setState(157);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == xasmParser::T__4) {
        setState(153);
        match(xasmParser::T__4);
        setState(154);
        bufferIndex();
        setState(159);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(163);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << xasmParser::T__2)
        | (1ULL << xasmParser::T__25)
        | (1ULL << xasmParser::T__29)
        | (1ULL << xasmParser::T__30)
        | (1ULL << xasmParser::T__31)
        | (1ULL << xasmParser::T__32)
        | (1ULL << xasmParser::T__33)
        | (1ULL << xasmParser::T__34)
        | (1ULL << xasmParser::T__35)
        | (1ULL << xasmParser::ID)
        | (1ULL << xasmParser::REAL)
        | (1ULL << xasmParser::INT)
        | (1ULL << xasmParser::STRING))) != 0)) {
        setState(160);
        exp(0);
        setState(165);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
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

//----------------- OptionsMapContext ------------------------------------------------------------------

xasmParser::OptionsMapContext::OptionsMapContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::OptionsTypeContext *> xasmParser::OptionsMapContext::optionsType() {
  return getRuleContexts<xasmParser::OptionsTypeContext>();
}

xasmParser::OptionsTypeContext* xasmParser::OptionsMapContext::optionsType(size_t i) {
  return getRuleContext<xasmParser::OptionsTypeContext>(i);
}


size_t xasmParser::OptionsMapContext::getRuleIndex() const {
  return xasmParser::RuleOptionsMap;
}

void xasmParser::OptionsMapContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOptionsMap(this);
}

void xasmParser::OptionsMapContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOptionsMap(this);
}

xasmParser::OptionsMapContext* xasmParser::optionsMap() {
  OptionsMapContext *_localctx = _tracker.createInstance<OptionsMapContext>(_ctx, getState());
  enterRule(_localctx, 28, xasmParser::RuleOptionsMap);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(168);
    match(xasmParser::T__6);
    setState(169);
    optionsType();
    setState(174);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__4) {
      setState(170);
      match(xasmParser::T__4);
      setState(171);
      optionsType();
      setState(176);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(177);
    match(xasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OptionsTypeContext ------------------------------------------------------------------

xasmParser::OptionsTypeContext::OptionsTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::StringContext* xasmParser::OptionsTypeContext::string() {
  return getRuleContext<xasmParser::StringContext>(0);
}

xasmParser::ExpContext* xasmParser::OptionsTypeContext::exp() {
  return getRuleContext<xasmParser::ExpContext>(0);
}


size_t xasmParser::OptionsTypeContext::getRuleIndex() const {
  return xasmParser::RuleOptionsType;
}

void xasmParser::OptionsTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOptionsType(this);
}

void xasmParser::OptionsTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOptionsType(this);
}

xasmParser::OptionsTypeContext* xasmParser::optionsType() {
  OptionsTypeContext *_localctx = _tracker.createInstance<OptionsTypeContext>(_ctx, getState());
  enterRule(_localctx, 30, xasmParser::RuleOptionsType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(179);
    match(xasmParser::T__6);
    setState(180);
    dynamic_cast<OptionsTypeContext *>(_localctx)->key = string();
    setState(181);
    match(xasmParser::T__4);
    setState(182);
    dynamic_cast<OptionsTypeContext *>(_localctx)->value = exp(0);
    setState(183);
    match(xasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExplistContext ------------------------------------------------------------------

xasmParser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::ExpContext *> xasmParser::ExplistContext::exp() {
  return getRuleContexts<xasmParser::ExpContext>();
}

xasmParser::ExpContext* xasmParser::ExplistContext::exp(size_t i) {
  return getRuleContext<xasmParser::ExpContext>(i);
}


size_t xasmParser::ExplistContext::getRuleIndex() const {
  return xasmParser::RuleExplist;
}

void xasmParser::ExplistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExplist(this);
}

void xasmParser::ExplistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExplist(this);
}

xasmParser::ExplistContext* xasmParser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 32, xasmParser::RuleExplist);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(185);
    exp(0);
    setState(190);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(186);
        match(xasmParser::T__4);
        setState(187);
        exp(0); 
      }
      setState(192);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
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

xasmParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::IdContext* xasmParser::ExpContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

std::vector<xasmParser::ExpContext *> xasmParser::ExpContext::exp() {
  return getRuleContexts<xasmParser::ExpContext>();
}

xasmParser::ExpContext* xasmParser::ExpContext::exp(size_t i) {
  return getRuleContext<xasmParser::ExpContext>(i);
}

xasmParser::UnaryopContext* xasmParser::ExpContext::unaryop() {
  return getRuleContext<xasmParser::UnaryopContext>(0);
}

xasmParser::StringContext* xasmParser::ExpContext::string() {
  return getRuleContext<xasmParser::StringContext>(0);
}

xasmParser::RealContext* xasmParser::ExpContext::real() {
  return getRuleContext<xasmParser::RealContext>(0);
}

tree::TerminalNode* xasmParser::ExpContext::INT() {
  return getToken(xasmParser::INT, 0);
}

xasmParser::BufferIndexContext* xasmParser::ExpContext::bufferIndex() {
  return getRuleContext<xasmParser::BufferIndexContext>(0);
}


size_t xasmParser::ExpContext::getRuleIndex() const {
  return xasmParser::RuleExp;
}

void xasmParser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void xasmParser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


xasmParser::ExpContext* xasmParser::exp() {
   return exp(0);
}

xasmParser::ExpContext* xasmParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  xasmParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  xasmParser::ExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 34;
  enterRecursionRule(_localctx, 34, xasmParser::RuleExp, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(211);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      setState(194);
      id();
      break;
    }

    case 2: {
      setState(195);
      match(xasmParser::T__25);
      setState(196);
      exp(9);
      break;
    }

    case 3: {
      setState(197);
      match(xasmParser::T__2);
      setState(198);
      exp(0);
      setState(199);
      match(xasmParser::T__5);
      break;
    }

    case 4: {
      setState(201);
      unaryop();
      setState(202);
      match(xasmParser::T__2);
      setState(203);
      exp(0);
      setState(204);
      match(xasmParser::T__5);
      break;
    }

    case 5: {
      setState(206);
      string();
      break;
    }

    case 6: {
      setState(207);
      real();
      break;
    }

    case 7: {
      setState(208);
      match(xasmParser::INT);
      break;
    }

    case 8: {
      setState(209);
      match(xasmParser::T__29);
      break;
    }

    case 9: {
      setState(210);
      bufferIndex();
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(230);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(228);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(213);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(214);
          match(xasmParser::T__24);
          setState(215);
          exp(14);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(216);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(217);
          match(xasmParser::T__25);
          setState(218);
          exp(13);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(219);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(220);
          match(xasmParser::T__26);
          setState(221);
          exp(12);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(222);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(223);
          match(xasmParser::T__27);
          setState(224);
          exp(11);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(225);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(226);
          match(xasmParser::T__28);
          setState(227);
          exp(9);
          break;
        }

        } 
      }
      setState(232);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx);
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

xasmParser::UnaryopContext::UnaryopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t xasmParser::UnaryopContext::getRuleIndex() const {
  return xasmParser::RuleUnaryop;
}

void xasmParser::UnaryopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryop(this);
}

void xasmParser::UnaryopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryop(this);
}

xasmParser::UnaryopContext* xasmParser::unaryop() {
  UnaryopContext *_localctx = _tracker.createInstance<UnaryopContext>(_ctx, getState());
  enterRule(_localctx, 36, xasmParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(233);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__30)
      | (1ULL << xasmParser::T__31)
      | (1ULL << xasmParser::T__32)
      | (1ULL << xasmParser::T__33)
      | (1ULL << xasmParser::T__34)
      | (1ULL << xasmParser::T__35))) != 0))) {
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

xasmParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* xasmParser::IdContext::ID() {
  return getToken(xasmParser::ID, 0);
}


size_t xasmParser::IdContext::getRuleIndex() const {
  return xasmParser::RuleId;
}

void xasmParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void xasmParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

xasmParser::IdContext* xasmParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 38, xasmParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(235);
    match(xasmParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

xasmParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* xasmParser::RealContext::REAL() {
  return getToken(xasmParser::REAL, 0);
}


size_t xasmParser::RealContext::getRuleIndex() const {
  return xasmParser::RuleReal;
}

void xasmParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void xasmParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

xasmParser::RealContext* xasmParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 40, xasmParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(237);
    match(xasmParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

xasmParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* xasmParser::StringContext::STRING() {
  return getToken(xasmParser::STRING, 0);
}


size_t xasmParser::StringContext::getRuleIndex() const {
  return xasmParser::RuleString;
}

void xasmParser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void xasmParser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

xasmParser::StringContext* xasmParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 42, xasmParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(239);
    match(xasmParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool xasmParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 17: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool xasmParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 13);
    case 1: return precpred(_ctx, 12);
    case 2: return precpred(_ctx, 11);
    case 3: return precpred(_ctx, 10);
    case 4: return precpred(_ctx, 8);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> xasmParser::_decisionToDFA;
atn::PredictionContextCache xasmParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN xasmParser::_atn;
std::vector<uint16_t> xasmParser::_serializedATN;

std::vector<std::string> xasmParser::_ruleNames = {
  "xaccsrc", "xacckernel", "xacclambda", "typedparam", "type", "mainprog", 
  "program", "line", "statement", "comment", "forstmt", "instruction", "bufferIndex", 
  "bitsOrParamType", "optionsMap", "optionsType", "explist", "exp", "unaryop", 
  "id", "real", "string"
};

std::vector<std::string> xasmParser::_literalNames = {
  "", "'__qpu__'", "'void'", "'('", "'qbit'", "','", "')'", "'{'", "'}'", 
  "'['", "'&'", "'='", "']'", "'int'", "'double'", "'float'", "'std::vector<double>'", 
  "';'", "'return'", "'for'", "'auto'", "'<'", "'>'", "'++'", "'--'", "'+'", 
  "'-'", "'*'", "'/'", "'^'", "'pi'", "'sin'", "'cos'", "'tan'", "'exp'", 
  "'ln'", "'sqrt'"
};

std::vector<std::string> xasmParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "COMMENT", "ID", "REAL", "INT", "STRING", "WS", "EOL"
};

dfa::Vocabulary xasmParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> xasmParser::_tokenNames;

xasmParser::Initializer::Initializer() {
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
    0x3, 0x2d, 0xf4, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 0x9, 
    0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x3, 0x2, 0x3, 0x2, 
    0x5, 0x2, 0x31, 0xa, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x3b, 0xa, 0x3, 0xc, 0x3, 
    0xe, 0x3, 0x3e, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x47, 0xa, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x7, 0x4, 0x4f, 0xa, 0x4, 0xc, 
    0x4, 0xe, 0x4, 0x52, 0xb, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 
    0x3, 0x7, 0x3, 0x8, 0x6, 0x8, 0x61, 0xa, 0x8, 0xd, 0x8, 0xe, 0x8, 0x62, 
    0x3, 0x9, 0x6, 0x9, 0x66, 0xa, 0x9, 0xd, 0x9, 0xe, 0x9, 0x67, 0x3, 0x9, 
    0x5, 0x9, 0x6b, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x5, 0xa, 0x73, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 
    0x3, 0xc, 0x6, 0xc, 0x87, 0xa, 0xc, 0xd, 0xc, 0xe, 0xc, 0x88, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x5, 0xd, 
    0x92, 0xa, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x7, 0xf, 0x9e, 0xa, 0xf, 
    0xc, 0xf, 0xe, 0xf, 0xa1, 0xb, 0xf, 0x3, 0xf, 0x7, 0xf, 0xa4, 0xa, 0xf, 
    0xc, 0xf, 0xe, 0xf, 0xa7, 0xb, 0xf, 0x5, 0xf, 0xa9, 0xa, 0xf, 0x3, 0x10, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0xaf, 0xa, 0x10, 0xc, 0x10, 
    0xe, 0x10, 0xb2, 0xb, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 
    0x12, 0x7, 0x12, 0xbf, 0xa, 0x12, 0xc, 0x12, 0xe, 0x12, 0xc2, 0xb, 0x12, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xd6, 
    0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x7, 0x13, 0xe7, 0xa, 0x13, 0xc, 0x13, 
    0xe, 0x13, 0xea, 0xb, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x15, 0x3, 0x15, 
    0x3, 0x16, 0x3, 0x16, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x2, 0x3, 0x24, 
    0x18, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 
    0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2, 0x8, 
    0x3, 0x2, 0xc, 0xd, 0x3, 0x2, 0xf, 0x12, 0x4, 0x2, 0xf, 0xf, 0x16, 0x16, 
    0x3, 0x2, 0x17, 0x18, 0x3, 0x2, 0x19, 0x1a, 0x3, 0x2, 0x21, 0x26, 0x2, 
    0xfa, 0x2, 0x30, 0x3, 0x2, 0x2, 0x2, 0x4, 0x32, 0x3, 0x2, 0x2, 0x2, 
    0x6, 0x44, 0x3, 0x2, 0x2, 0x2, 0x8, 0x58, 0x3, 0x2, 0x2, 0x2, 0xa, 0x5b, 
    0x3, 0x2, 0x2, 0x2, 0xc, 0x5d, 0x3, 0x2, 0x2, 0x2, 0xe, 0x60, 0x3, 0x2, 
    0x2, 0x2, 0x10, 0x6a, 0x3, 0x2, 0x2, 0x2, 0x12, 0x72, 0x3, 0x2, 0x2, 
    0x2, 0x14, 0x74, 0x3, 0x2, 0x2, 0x2, 0x16, 0x76, 0x3, 0x2, 0x2, 0x2, 
    0x18, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x95, 0x3, 0x2, 0x2, 0x2, 0x1c, 
    0xa8, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xaa, 0x3, 0x2, 0x2, 0x2, 0x20, 0xb5, 
    0x3, 0x2, 0x2, 0x2, 0x22, 0xbb, 0x3, 0x2, 0x2, 0x2, 0x24, 0xd5, 0x3, 
    0x2, 0x2, 0x2, 0x26, 0xeb, 0x3, 0x2, 0x2, 0x2, 0x28, 0xed, 0x3, 0x2, 
    0x2, 0x2, 0x2a, 0xef, 0x3, 0x2, 0x2, 0x2, 0x2c, 0xf1, 0x3, 0x2, 0x2, 
    0x2, 0x2e, 0x31, 0x5, 0x4, 0x3, 0x2, 0x2f, 0x31, 0x5, 0x6, 0x4, 0x2, 
    0x30, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x30, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x31, 
    0x3, 0x3, 0x2, 0x2, 0x2, 0x32, 0x33, 0x7, 0x3, 0x2, 0x2, 0x33, 0x34, 
    0x7, 0x4, 0x2, 0x2, 0x34, 0x35, 0x5, 0x28, 0x15, 0x2, 0x35, 0x36, 0x7, 
    0x5, 0x2, 0x2, 0x36, 0x37, 0x7, 0x6, 0x2, 0x2, 0x37, 0x3c, 0x5, 0x28, 
    0x15, 0x2, 0x38, 0x39, 0x7, 0x7, 0x2, 0x2, 0x39, 0x3b, 0x5, 0x8, 0x5, 
    0x2, 0x3a, 0x38, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x3e, 0x3, 0x2, 0x2, 0x2, 
    0x3c, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3d, 
    0x3f, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3f, 0x40, 
    0x7, 0x8, 0x2, 0x2, 0x40, 0x41, 0x7, 0x9, 0x2, 0x2, 0x41, 0x42, 0x5, 
    0xc, 0x7, 0x2, 0x42, 0x43, 0x7, 0xa, 0x2, 0x2, 0x43, 0x5, 0x3, 0x2, 
    0x2, 0x2, 0x44, 0x46, 0x7, 0xb, 0x2, 0x2, 0x45, 0x47, 0x9, 0x2, 0x2, 
    0x2, 0x46, 0x45, 0x3, 0x2, 0x2, 0x2, 0x46, 0x47, 0x3, 0x2, 0x2, 0x2, 
    0x47, 0x48, 0x3, 0x2, 0x2, 0x2, 0x48, 0x49, 0x7, 0xe, 0x2, 0x2, 0x49, 
    0x4a, 0x7, 0x5, 0x2, 0x2, 0x4a, 0x4b, 0x7, 0x6, 0x2, 0x2, 0x4b, 0x50, 
    0x5, 0x28, 0x15, 0x2, 0x4c, 0x4d, 0x7, 0x7, 0x2, 0x2, 0x4d, 0x4f, 0x5, 
    0x8, 0x5, 0x2, 0x4e, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x52, 0x3, 0x2, 
    0x2, 0x2, 0x50, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x3, 0x2, 0x2, 
    0x2, 0x51, 0x53, 0x3, 0x2, 0x2, 0x2, 0x52, 0x50, 0x3, 0x2, 0x2, 0x2, 
    0x53, 0x54, 0x7, 0x8, 0x2, 0x2, 0x54, 0x55, 0x7, 0x9, 0x2, 0x2, 0x55, 
    0x56, 0x5, 0xc, 0x7, 0x2, 0x56, 0x57, 0x7, 0xa, 0x2, 0x2, 0x57, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x58, 0x59, 0x5, 0xa, 0x6, 0x2, 0x59, 0x5a, 0x5, 
    0x28, 0x15, 0x2, 0x5a, 0x9, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5c, 0x9, 0x3, 
    0x2, 0x2, 0x5c, 0xb, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5e, 0x5, 0xe, 0x8, 
    0x2, 0x5e, 0xd, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x61, 0x5, 0x10, 0x9, 0x2, 
    0x60, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x61, 0x62, 0x3, 0x2, 0x2, 0x2, 0x62, 
    0x60, 0x3, 0x2, 0x2, 0x2, 0x62, 0x63, 0x3, 0x2, 0x2, 0x2, 0x63, 0xf, 
    0x3, 0x2, 0x2, 0x2, 0x64, 0x66, 0x5, 0x12, 0xa, 0x2, 0x65, 0x64, 0x3, 
    0x2, 0x2, 0x2, 0x66, 0x67, 0x3, 0x2, 0x2, 0x2, 0x67, 0x65, 0x3, 0x2, 
    0x2, 0x2, 0x67, 0x68, 0x3, 0x2, 0x2, 0x2, 0x68, 0x6b, 0x3, 0x2, 0x2, 
    0x2, 0x69, 0x6b, 0x5, 0x14, 0xb, 0x2, 0x6a, 0x65, 0x3, 0x2, 0x2, 0x2, 
    0x6a, 0x69, 0x3, 0x2, 0x2, 0x2, 0x6b, 0x11, 0x3, 0x2, 0x2, 0x2, 0x6c, 
    0x6d, 0x5, 0x18, 0xd, 0x2, 0x6d, 0x6e, 0x7, 0x13, 0x2, 0x2, 0x6e, 0x73, 
    0x3, 0x2, 0x2, 0x2, 0x6f, 0x73, 0x5, 0x16, 0xc, 0x2, 0x70, 0x71, 0x7, 
    0x14, 0x2, 0x2, 0x71, 0x73, 0x7, 0x13, 0x2, 0x2, 0x72, 0x6c, 0x3, 0x2, 
    0x2, 0x2, 0x72, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x72, 0x70, 0x3, 0x2, 0x2, 
    0x2, 0x73, 0x13, 0x3, 0x2, 0x2, 0x2, 0x74, 0x75, 0x7, 0x27, 0x2, 0x2, 
    0x75, 0x15, 0x3, 0x2, 0x2, 0x2, 0x76, 0x77, 0x7, 0x15, 0x2, 0x2, 0x77, 
    0x78, 0x7, 0x5, 0x2, 0x2, 0x78, 0x79, 0x9, 0x4, 0x2, 0x2, 0x79, 0x7a, 
    0x5, 0x28, 0x15, 0x2, 0x7a, 0x7b, 0x7, 0xd, 0x2, 0x2, 0x7b, 0x7c, 0x7, 
    0x2a, 0x2, 0x2, 0x7c, 0x7d, 0x7, 0x13, 0x2, 0x2, 0x7d, 0x7e, 0x5, 0x28, 
    0x15, 0x2, 0x7e, 0x7f, 0x9, 0x5, 0x2, 0x2, 0x7f, 0x80, 0x7, 0x2a, 0x2, 
    0x2, 0x80, 0x81, 0x7, 0x13, 0x2, 0x2, 0x81, 0x82, 0x5, 0x28, 0x15, 0x2, 
    0x82, 0x83, 0x9, 0x6, 0x2, 0x2, 0x83, 0x84, 0x7, 0x8, 0x2, 0x2, 0x84, 
    0x86, 0x7, 0x9, 0x2, 0x2, 0x85, 0x87, 0x5, 0x12, 0xa, 0x2, 0x86, 0x85, 
    0x3, 0x2, 0x2, 0x2, 0x87, 0x88, 0x3, 0x2, 0x2, 0x2, 0x88, 0x86, 0x3, 
    0x2, 0x2, 0x2, 0x88, 0x89, 0x3, 0x2, 0x2, 0x2, 0x89, 0x8a, 0x3, 0x2, 
    0x2, 0x2, 0x8a, 0x8b, 0x7, 0xa, 0x2, 0x2, 0x8b, 0x17, 0x3, 0x2, 0x2, 
    0x2, 0x8c, 0x8d, 0x5, 0x28, 0x15, 0x2, 0x8d, 0x8e, 0x7, 0x5, 0x2, 0x2, 
    0x8e, 0x91, 0x5, 0x22, 0x12, 0x2, 0x8f, 0x90, 0x7, 0x7, 0x2, 0x2, 0x90, 
    0x92, 0x5, 0x1e, 0x10, 0x2, 0x91, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x91, 0x92, 
    0x3, 0x2, 0x2, 0x2, 0x92, 0x93, 0x3, 0x2, 0x2, 0x2, 0x93, 0x94, 0x7, 
    0x8, 0x2, 0x2, 0x94, 0x19, 0x3, 0x2, 0x2, 0x2, 0x95, 0x96, 0x5, 0x28, 
    0x15, 0x2, 0x96, 0x97, 0x7, 0xb, 0x2, 0x2, 0x97, 0x98, 0x5, 0x24, 0x13, 
    0x2, 0x98, 0x99, 0x7, 0xe, 0x2, 0x2, 0x99, 0x1b, 0x3, 0x2, 0x2, 0x2, 
    0x9a, 0x9f, 0x5, 0x1a, 0xe, 0x2, 0x9b, 0x9c, 0x7, 0x7, 0x2, 0x2, 0x9c, 
    0x9e, 0x5, 0x1a, 0xe, 0x2, 0x9d, 0x9b, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xa1, 
    0x3, 0x2, 0x2, 0x2, 0x9f, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x9f, 0xa0, 0x3, 
    0x2, 0x2, 0x2, 0xa0, 0xa9, 0x3, 0x2, 0x2, 0x2, 0xa1, 0x9f, 0x3, 0x2, 
    0x2, 0x2, 0xa2, 0xa4, 0x5, 0x24, 0x13, 0x2, 0xa3, 0xa2, 0x3, 0x2, 0x2, 
    0x2, 0xa4, 0xa7, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa3, 0x3, 0x2, 0x2, 0x2, 
    0xa5, 0xa6, 0x3, 0x2, 0x2, 0x2, 0xa6, 0xa9, 0x3, 0x2, 0x2, 0x2, 0xa7, 
    0xa5, 0x3, 0x2, 0x2, 0x2, 0xa8, 0x9a, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xa5, 
    0x3, 0x2, 0x2, 0x2, 0xa9, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xab, 0x7, 
    0x9, 0x2, 0x2, 0xab, 0xb0, 0x5, 0x20, 0x11, 0x2, 0xac, 0xad, 0x7, 0x7, 
    0x2, 0x2, 0xad, 0xaf, 0x5, 0x20, 0x11, 0x2, 0xae, 0xac, 0x3, 0x2, 0x2, 
    0x2, 0xaf, 0xb2, 0x3, 0x2, 0x2, 0x2, 0xb0, 0xae, 0x3, 0x2, 0x2, 0x2, 
    0xb0, 0xb1, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xb2, 
    0xb0, 0x3, 0x2, 0x2, 0x2, 0xb3, 0xb4, 0x7, 0xa, 0x2, 0x2, 0xb4, 0x1f, 
    0x3, 0x2, 0x2, 0x2, 0xb5, 0xb6, 0x7, 0x9, 0x2, 0x2, 0xb6, 0xb7, 0x5, 
    0x2c, 0x17, 0x2, 0xb7, 0xb8, 0x7, 0x7, 0x2, 0x2, 0xb8, 0xb9, 0x5, 0x24, 
    0x13, 0x2, 0xb9, 0xba, 0x7, 0xa, 0x2, 0x2, 0xba, 0x21, 0x3, 0x2, 0x2, 
    0x2, 0xbb, 0xc0, 0x5, 0x24, 0x13, 0x2, 0xbc, 0xbd, 0x7, 0x7, 0x2, 0x2, 
    0xbd, 0xbf, 0x5, 0x24, 0x13, 0x2, 0xbe, 0xbc, 0x3, 0x2, 0x2, 0x2, 0xbf, 
    0xc2, 0x3, 0x2, 0x2, 0x2, 0xc0, 0xbe, 0x3, 0x2, 0x2, 0x2, 0xc0, 0xc1, 
    0x3, 0x2, 0x2, 0x2, 0xc1, 0x23, 0x3, 0x2, 0x2, 0x2, 0xc2, 0xc0, 0x3, 
    0x2, 0x2, 0x2, 0xc3, 0xc4, 0x8, 0x13, 0x1, 0x2, 0xc4, 0xd6, 0x5, 0x28, 
    0x15, 0x2, 0xc5, 0xc6, 0x7, 0x1c, 0x2, 0x2, 0xc6, 0xd6, 0x5, 0x24, 0x13, 
    0xb, 0xc7, 0xc8, 0x7, 0x5, 0x2, 0x2, 0xc8, 0xc9, 0x5, 0x24, 0x13, 0x2, 
    0xc9, 0xca, 0x7, 0x8, 0x2, 0x2, 0xca, 0xd6, 0x3, 0x2, 0x2, 0x2, 0xcb, 
    0xcc, 0x5, 0x26, 0x14, 0x2, 0xcc, 0xcd, 0x7, 0x5, 0x2, 0x2, 0xcd, 0xce, 
    0x5, 0x24, 0x13, 0x2, 0xce, 0xcf, 0x7, 0x8, 0x2, 0x2, 0xcf, 0xd6, 0x3, 
    0x2, 0x2, 0x2, 0xd0, 0xd6, 0x5, 0x2c, 0x17, 0x2, 0xd1, 0xd6, 0x5, 0x2a, 
    0x16, 0x2, 0xd2, 0xd6, 0x7, 0x2a, 0x2, 0x2, 0xd3, 0xd6, 0x7, 0x20, 0x2, 
    0x2, 0xd4, 0xd6, 0x5, 0x1a, 0xe, 0x2, 0xd5, 0xc3, 0x3, 0x2, 0x2, 0x2, 
    0xd5, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xd5, 0xc7, 0x3, 0x2, 0x2, 0x2, 0xd5, 
    0xcb, 0x3, 0x2, 0x2, 0x2, 0xd5, 0xd0, 0x3, 0x2, 0x2, 0x2, 0xd5, 0xd1, 
    0x3, 0x2, 0x2, 0x2, 0xd5, 0xd2, 0x3, 0x2, 0x2, 0x2, 0xd5, 0xd3, 0x3, 
    0x2, 0x2, 0x2, 0xd5, 0xd4, 0x3, 0x2, 0x2, 0x2, 0xd6, 0xe8, 0x3, 0x2, 
    0x2, 0x2, 0xd7, 0xd8, 0xc, 0xf, 0x2, 0x2, 0xd8, 0xd9, 0x7, 0x1b, 0x2, 
    0x2, 0xd9, 0xe7, 0x5, 0x24, 0x13, 0x10, 0xda, 0xdb, 0xc, 0xe, 0x2, 0x2, 
    0xdb, 0xdc, 0x7, 0x1c, 0x2, 0x2, 0xdc, 0xe7, 0x5, 0x24, 0x13, 0xf, 0xdd, 
    0xde, 0xc, 0xd, 0x2, 0x2, 0xde, 0xdf, 0x7, 0x1d, 0x2, 0x2, 0xdf, 0xe7, 
    0x5, 0x24, 0x13, 0xe, 0xe0, 0xe1, 0xc, 0xc, 0x2, 0x2, 0xe1, 0xe2, 0x7, 
    0x1e, 0x2, 0x2, 0xe2, 0xe7, 0x5, 0x24, 0x13, 0xd, 0xe3, 0xe4, 0xc, 0xa, 
    0x2, 0x2, 0xe4, 0xe5, 0x7, 0x1f, 0x2, 0x2, 0xe5, 0xe7, 0x5, 0x24, 0x13, 
    0xb, 0xe6, 0xd7, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xda, 0x3, 0x2, 0x2, 0x2, 
    0xe6, 0xdd, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xe0, 0x3, 0x2, 0x2, 0x2, 0xe6, 
    0xe3, 0x3, 0x2, 0x2, 0x2, 0xe7, 0xea, 0x3, 0x2, 0x2, 0x2, 0xe8, 0xe6, 
    0x3, 0x2, 0x2, 0x2, 0xe8, 0xe9, 0x3, 0x2, 0x2, 0x2, 0xe9, 0x25, 0x3, 
    0x2, 0x2, 0x2, 0xea, 0xe8, 0x3, 0x2, 0x2, 0x2, 0xeb, 0xec, 0x9, 0x7, 
    0x2, 0x2, 0xec, 0x27, 0x3, 0x2, 0x2, 0x2, 0xed, 0xee, 0x7, 0x28, 0x2, 
    0x2, 0xee, 0x29, 0x3, 0x2, 0x2, 0x2, 0xef, 0xf0, 0x7, 0x29, 0x2, 0x2, 
    0xf0, 0x2b, 0x3, 0x2, 0x2, 0x2, 0xf1, 0xf2, 0x7, 0x2b, 0x2, 0x2, 0xf2, 
    0x2d, 0x3, 0x2, 0x2, 0x2, 0x14, 0x30, 0x3c, 0x46, 0x50, 0x62, 0x67, 
    0x6a, 0x72, 0x88, 0x91, 0x9f, 0xa5, 0xa8, 0xb0, 0xc0, 0xd5, 0xe6, 0xe8, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

xasmParser::Initializer xasmParser::_init;
