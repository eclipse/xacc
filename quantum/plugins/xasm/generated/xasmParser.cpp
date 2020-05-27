
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
    setState(56);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case xasmParser::T__0: {
        enterOuterAlt(_localctx, 1);
        setState(54);
        xacckernel();
        break;
      }

      case xasmParser::T__7: {
        enterOuterAlt(_localctx, 2);
        setState(55);
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

std::vector<xasmParser::TypedparamContext *> xasmParser::XacckernelContext::typedparam() {
  return getRuleContexts<xasmParser::TypedparamContext>();
}

xasmParser::TypedparamContext* xasmParser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<xasmParser::TypedparamContext>(i);
}

xasmParser::IdContext* xasmParser::XacckernelContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

xasmParser::MainprogContext* xasmParser::XacckernelContext::mainprog() {
  return getRuleContext<xasmParser::MainprogContext>(0);
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
    setState(58);
    match(xasmParser::T__0);
    setState(59);
    match(xasmParser::T__1);
    setState(60);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(61);
    match(xasmParser::T__2);
    setState(62);
    typedparam();
    setState(67);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__3) {
      setState(63);
      match(xasmParser::T__3);
      setState(64);
      typedparam();
      setState(69);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(70);
    match(xasmParser::T__4);
    setState(71);
    match(xasmParser::T__5);
    setState(73);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__19)
      | (1ULL << xasmParser::T__20)
      | (1ULL << xasmParser::T__25)
      | (1ULL << xasmParser::COMMENT)
      | (1ULL << xasmParser::ID))) != 0)) {
      setState(72);
      mainprog();
    }
    setState(75);
    match(xasmParser::T__6);
   
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

std::vector<xasmParser::TypedparamContext *> xasmParser::XacclambdaContext::typedparam() {
  return getRuleContexts<xasmParser::TypedparamContext>();
}

xasmParser::TypedparamContext* xasmParser::XacclambdaContext::typedparam(size_t i) {
  return getRuleContext<xasmParser::TypedparamContext>(i);
}

xasmParser::MainprogContext* xasmParser::XacclambdaContext::mainprog() {
  return getRuleContext<xasmParser::MainprogContext>(0);
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
    setState(77);
    match(xasmParser::T__7);
    setState(79);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__8

    || _la == xasmParser::T__9) {
      setState(78);
      _la = _input->LA(1);
      if (!(_la == xasmParser::T__8

      || _la == xasmParser::T__9)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(81);
    match(xasmParser::T__10);
    setState(82);
    match(xasmParser::T__2);
    setState(83);
    typedparam();
    setState(88);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__3) {
      setState(84);
      match(xasmParser::T__3);
      setState(85);
      typedparam();
      setState(90);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(91);
    match(xasmParser::T__4);
    setState(92);
    match(xasmParser::T__5);
    setState(94);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__19)
      | (1ULL << xasmParser::T__20)
      | (1ULL << xasmParser::T__25)
      | (1ULL << xasmParser::COMMENT)
      | (1ULL << xasmParser::ID))) != 0)) {
      setState(93);
      mainprog();
    }
    setState(96);
    match(xasmParser::T__6);
   
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

xasmParser::Variable_param_nameContext* xasmParser::TypedparamContext::variable_param_name() {
  return getRuleContext<xasmParser::Variable_param_nameContext>(0);
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
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(98);
    type();
    setState(100);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__8

    || _la == xasmParser::T__11) {
      setState(99);
      _la = _input->LA(1);
      if (!(_la == xasmParser::T__8

      || _la == xasmParser::T__11)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
    }
    setState(102);
    variable_param_name();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Variable_param_nameContext ------------------------------------------------------------------

xasmParser::Variable_param_nameContext::Variable_param_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::IdContext* xasmParser::Variable_param_nameContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}


size_t xasmParser::Variable_param_nameContext::getRuleIndex() const {
  return xasmParser::RuleVariable_param_name;
}

void xasmParser::Variable_param_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVariable_param_name(this);
}

void xasmParser::Variable_param_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVariable_param_name(this);
}

xasmParser::Variable_param_nameContext* xasmParser::variable_param_name() {
  Variable_param_nameContext *_localctx = _tracker.createInstance<Variable_param_nameContext>(_ctx, getState());
  enterRule(_localctx, 8, xasmParser::RuleVariable_param_name);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(104);
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

std::vector<xasmParser::IdContext *> xasmParser::TypeContext::id() {
  return getRuleContexts<xasmParser::IdContext>();
}

xasmParser::IdContext* xasmParser::TypeContext::id(size_t i) {
  return getRuleContext<xasmParser::IdContext>(i);
}

xasmParser::TypeContext* xasmParser::TypeContext::type() {
  return getRuleContext<xasmParser::TypeContext>(0);
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
  enterRule(_localctx, 10, xasmParser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(136);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(106);
      match(xasmParser::T__12);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(107);
      match(xasmParser::T__13);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(108);
      id();
      setState(117);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == xasmParser::T__14) {
        setState(109);
        match(xasmParser::T__14);
        setState(110);
        id();
        setState(113);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == xasmParser::T__3) {
          setState(111);
          match(xasmParser::T__3);
          setState(112);
          id();
        }
        setState(115);
        match(xasmParser::T__15);
      }
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(119);
      id();
      setState(120);
      match(xasmParser::T__16);
      setState(121);
      id();
      setState(130);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == xasmParser::T__14) {
        setState(122);
        match(xasmParser::T__14);
        setState(123);
        id();
        setState(126);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == xasmParser::T__3) {
          setState(124);
          match(xasmParser::T__3);
          setState(125);
          id();
        }
        setState(128);
        match(xasmParser::T__15);
      }
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(132);
      match(xasmParser::T__17);
      setState(133);
      type();
      setState(134);
      match(xasmParser::T__15);
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
  enterRule(_localctx, 12, xasmParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(138);
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
  enterRule(_localctx, 14, xasmParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(141); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(140);
      line();
      setState(143); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__19)
      | (1ULL << xasmParser::T__20)
      | (1ULL << xasmParser::T__25)
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
  enterRule(_localctx, 16, xasmParser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(151);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case xasmParser::T__19:
      case xasmParser::T__20:
      case xasmParser::T__25:
      case xasmParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(146); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(145);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(148); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case xasmParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(150);
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

xasmParser::Composite_generatorContext* xasmParser::StatementContext::composite_generator() {
  return getRuleContext<xasmParser::Composite_generatorContext>(0);
}

xasmParser::ForstmtContext* xasmParser::StatementContext::forstmt() {
  return getRuleContext<xasmParser::ForstmtContext>(0);
}

xasmParser::IfstmtContext* xasmParser::StatementContext::ifstmt() {
  return getRuleContext<xasmParser::IfstmtContext>(0);
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
  enterRule(_localctx, 18, xasmParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(163);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(153);
      instruction();
      setState(154);
      match(xasmParser::T__18);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(156);
      composite_generator();
      setState(157);
      match(xasmParser::T__18);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(159);
      forstmt();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(160);
      ifstmt();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(161);
      match(xasmParser::T__19);
      setState(162);
      match(xasmParser::T__18);
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
  enterRule(_localctx, 20, xasmParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(165);
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

xasmParser::ProgramContext* xasmParser::ForstmtContext::program() {
  return getRuleContext<xasmParser::ProgramContext>(0);
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
  enterRule(_localctx, 22, xasmParser::RuleForstmt);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(167);
    match(xasmParser::T__20);
    setState(168);
    match(xasmParser::T__2);
    setState(169);
    _la = _input->LA(1);
    if (!(_la == xasmParser::T__12

    || _la == xasmParser::T__13)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(170);
    dynamic_cast<ForstmtContext *>(_localctx)->varname = id();
    setState(171);
    match(xasmParser::T__9);
    setState(172);
    dynamic_cast<ForstmtContext *>(_localctx)->start = match(xasmParser::INT);
    setState(173);
    match(xasmParser::T__18);
    setState(174);
    id();
    setState(175);
    dynamic_cast<ForstmtContext *>(_localctx)->comparator = _input->LT(1);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__14)
      | (1ULL << xasmParser::T__15)
      | (1ULL << xasmParser::T__21)
      | (1ULL << xasmParser::T__22))) != 0))) {
      dynamic_cast<ForstmtContext *>(_localctx)->comparator = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(176);
    dynamic_cast<ForstmtContext *>(_localctx)->end = match(xasmParser::INT);
    setState(177);
    match(xasmParser::T__18);
    setState(178);
    id();
    setState(179);
    dynamic_cast<ForstmtContext *>(_localctx)->inc_or_dec = _input->LT(1);
    _la = _input->LA(1);
    if (!(_la == xasmParser::T__23

    || _la == xasmParser::T__24)) {
      dynamic_cast<ForstmtContext *>(_localctx)->inc_or_dec = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(180);
    match(xasmParser::T__4);
    setState(181);
    match(xasmParser::T__5);
    setState(182);
    dynamic_cast<ForstmtContext *>(_localctx)->forScope = program();
    setState(183);
    match(xasmParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IfstmtContext ------------------------------------------------------------------

xasmParser::IfstmtContext::IfstmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::ProgramContext* xasmParser::IfstmtContext::program() {
  return getRuleContext<xasmParser::ProgramContext>(0);
}

xasmParser::IdContext* xasmParser::IfstmtContext::id() {
  return getRuleContext<xasmParser::IdContext>(0);
}

tree::TerminalNode* xasmParser::IfstmtContext::INT() {
  return getToken(xasmParser::INT, 0);
}


size_t xasmParser::IfstmtContext::getRuleIndex() const {
  return xasmParser::RuleIfstmt;
}

void xasmParser::IfstmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIfstmt(this);
}

void xasmParser::IfstmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIfstmt(this);
}

xasmParser::IfstmtContext* xasmParser::ifstmt() {
  IfstmtContext *_localctx = _tracker.createInstance<IfstmtContext>(_ctx, getState());
  enterRule(_localctx, 24, xasmParser::RuleIfstmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(185);
    match(xasmParser::T__25);
    setState(186);
    match(xasmParser::T__2);
    setState(187);
    dynamic_cast<IfstmtContext *>(_localctx)->buffer_name = id();
    setState(188);
    match(xasmParser::T__7);
    setState(189);
    dynamic_cast<IfstmtContext *>(_localctx)->idx = match(xasmParser::INT);
    setState(190);
    match(xasmParser::T__10);
    setState(191);
    match(xasmParser::T__4);
    setState(192);
    match(xasmParser::T__5);
    setState(193);
    program();
    setState(194);
    match(xasmParser::T__6);
   
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

xasmParser::BufferListContext* xasmParser::InstructionContext::bufferList() {
  return getRuleContext<xasmParser::BufferListContext>(0);
}

xasmParser::ParamListContext* xasmParser::InstructionContext::paramList() {
  return getRuleContext<xasmParser::ParamListContext>(0);
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
  enterRule(_localctx, 26, xasmParser::RuleInstruction);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(196);
    dynamic_cast<InstructionContext *>(_localctx)->inst_name = id();
    setState(197);
    match(xasmParser::T__2);
    setState(198);
    dynamic_cast<InstructionContext *>(_localctx)->buffer_list = bufferList();
    setState(201);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__3) {
      setState(199);
      match(xasmParser::T__3);
      setState(200);
      dynamic_cast<InstructionContext *>(_localctx)->param_list = paramList();
    }
    setState(203);
    match(xasmParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BufferListContext ------------------------------------------------------------------

xasmParser::BufferListContext::BufferListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::BufferIndexContext *> xasmParser::BufferListContext::bufferIndex() {
  return getRuleContexts<xasmParser::BufferIndexContext>();
}

xasmParser::BufferIndexContext* xasmParser::BufferListContext::bufferIndex(size_t i) {
  return getRuleContext<xasmParser::BufferIndexContext>(i);
}


size_t xasmParser::BufferListContext::getRuleIndex() const {
  return xasmParser::RuleBufferList;
}

void xasmParser::BufferListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBufferList(this);
}

void xasmParser::BufferListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBufferList(this);
}

xasmParser::BufferListContext* xasmParser::bufferList() {
  BufferListContext *_localctx = _tracker.createInstance<BufferListContext>(_ctx, getState());
  enterRule(_localctx, 28, xasmParser::RuleBufferList);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(205);
    bufferIndex();
    setState(208);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
    case 1: {
      setState(206);
      match(xasmParser::T__3);
      setState(207);
      bufferIndex();
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

//----------------- ParamListContext ------------------------------------------------------------------

xasmParser::ParamListContext::ParamListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::ParameterContext *> xasmParser::ParamListContext::parameter() {
  return getRuleContexts<xasmParser::ParameterContext>();
}

xasmParser::ParameterContext* xasmParser::ParamListContext::parameter(size_t i) {
  return getRuleContext<xasmParser::ParameterContext>(i);
}


size_t xasmParser::ParamListContext::getRuleIndex() const {
  return xasmParser::RuleParamList;
}

void xasmParser::ParamListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParamList(this);
}

void xasmParser::ParamListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParamList(this);
}

xasmParser::ParamListContext* xasmParser::paramList() {
  ParamListContext *_localctx = _tracker.createInstance<ParamListContext>(_ctx, getState());
  enterRule(_localctx, 30, xasmParser::RuleParamList);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(210);
    parameter();
    setState(215);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(211);
        match(xasmParser::T__3);
        setState(212);
        parameter(); 
      }
      setState(217);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterContext ------------------------------------------------------------------

xasmParser::ParameterContext::ParameterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

xasmParser::ExpContext* xasmParser::ParameterContext::exp() {
  return getRuleContext<xasmParser::ExpContext>(0);
}


size_t xasmParser::ParameterContext::getRuleIndex() const {
  return xasmParser::RuleParameter;
}

void xasmParser::ParameterContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParameter(this);
}

void xasmParser::ParameterContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParameter(this);
}

xasmParser::ParameterContext* xasmParser::parameter() {
  ParameterContext *_localctx = _tracker.createInstance<ParameterContext>(_ctx, getState());
  enterRule(_localctx, 32, xasmParser::RuleParameter);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(218);
    exp(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Composite_generatorContext ------------------------------------------------------------------

xasmParser::Composite_generatorContext::Composite_generatorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<xasmParser::IdContext *> xasmParser::Composite_generatorContext::id() {
  return getRuleContexts<xasmParser::IdContext>();
}

xasmParser::IdContext* xasmParser::Composite_generatorContext::id(size_t i) {
  return getRuleContext<xasmParser::IdContext>(i);
}

xasmParser::ParamListContext* xasmParser::Composite_generatorContext::paramList() {
  return getRuleContext<xasmParser::ParamListContext>(0);
}

xasmParser::OptionsMapContext* xasmParser::Composite_generatorContext::optionsMap() {
  return getRuleContext<xasmParser::OptionsMapContext>(0);
}


size_t xasmParser::Composite_generatorContext::getRuleIndex() const {
  return xasmParser::RuleComposite_generator;
}

void xasmParser::Composite_generatorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComposite_generator(this);
}

void xasmParser::Composite_generatorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<xasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComposite_generator(this);
}

xasmParser::Composite_generatorContext* xasmParser::composite_generator() {
  Composite_generatorContext *_localctx = _tracker.createInstance<Composite_generatorContext>(_ctx, getState());
  enterRule(_localctx, 34, xasmParser::RuleComposite_generator);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(220);
    dynamic_cast<Composite_generatorContext *>(_localctx)->composite_name = id();
    setState(221);
    match(xasmParser::T__2);
    setState(222);
    dynamic_cast<Composite_generatorContext *>(_localctx)->buffer_name = id();
    setState(225);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      setState(223);
      match(xasmParser::T__3);
      setState(224);
      dynamic_cast<Composite_generatorContext *>(_localctx)->composite_params = paramList();
      break;
    }

    }
    setState(229);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == xasmParser::T__3) {
      setState(227);
      match(xasmParser::T__3);
      setState(228);
      dynamic_cast<Composite_generatorContext *>(_localctx)->composite_options = optionsMap();
    }
    setState(231);
    match(xasmParser::T__4);
   
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
  enterRule(_localctx, 36, xasmParser::RuleBufferIndex);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(233);
    dynamic_cast<BufferIndexContext *>(_localctx)->buffer_name = id();

    setState(234);
    match(xasmParser::T__7);
    setState(235);
    dynamic_cast<BufferIndexContext *>(_localctx)->idx = exp(0);
    setState(236);
    match(xasmParser::T__10);
   
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
  enterRule(_localctx, 38, xasmParser::RuleOptionsMap);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(238);
    match(xasmParser::T__5);
    setState(239);
    optionsType();
    setState(244);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__3) {
      setState(240);
      match(xasmParser::T__3);
      setState(241);
      optionsType();
      setState(246);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(247);
    match(xasmParser::T__6);
   
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
  enterRule(_localctx, 40, xasmParser::RuleOptionsType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(249);
    match(xasmParser::T__5);
    setState(250);
    dynamic_cast<OptionsTypeContext *>(_localctx)->key = string();
    setState(251);
    match(xasmParser::T__3);
    setState(252);
    dynamic_cast<OptionsTypeContext *>(_localctx)->value = exp(0);
    setState(253);
    match(xasmParser::T__6);
   
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
  enterRule(_localctx, 42, xasmParser::RuleExplist);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(255);
    exp(0);
    setState(260);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == xasmParser::T__3) {
      setState(256);
      match(xasmParser::T__3);
      setState(257);
      exp(0);
      setState(262);
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

tree::TerminalNode* xasmParser::ExpContext::ID() {
  return getToken(xasmParser::ID, 0);
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
  size_t startState = 44;
  enterRecursionRule(_localctx, 44, xasmParser::RuleExp, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(285);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(264);
      id();
      break;
    }

    case 2: {
      setState(265);
      match(xasmParser::T__27);
      setState(266);
      exp(9);
      break;
    }

    case 3: {
      setState(267);
      match(xasmParser::T__2);
      setState(268);
      exp(0);
      setState(269);
      match(xasmParser::T__4);
      break;
    }

    case 4: {
      setState(271);
      unaryop();
      setState(272);
      match(xasmParser::T__2);
      setState(273);
      exp(0);
      setState(274);
      match(xasmParser::T__4);
      break;
    }

    case 5: {
      setState(276);
      string();
      break;
    }

    case 6: {
      setState(277);
      real();
      break;
    }

    case 7: {
      setState(278);
      match(xasmParser::INT);
      break;
    }

    case 8: {
      setState(279);
      match(xasmParser::T__30);
      break;
    }

    case 9: {
      setState(280);
      dynamic_cast<ExpContext *>(_localctx)->var_name = id();
      setState(281);
      match(xasmParser::T__7);
      setState(282);
      dynamic_cast<ExpContext *>(_localctx)->idx = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == xasmParser::ID

      || _la == xasmParser::INT)) {
        dynamic_cast<ExpContext *>(_localctx)->idx = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(283);
      match(xasmParser::T__10);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(304);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(302);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 24, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(287);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(288);
          match(xasmParser::T__26);
          setState(289);
          exp(14);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(290);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(291);
          match(xasmParser::T__27);
          setState(292);
          exp(13);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(293);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(294);
          match(xasmParser::T__11);
          setState(295);
          exp(12);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(296);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(297);
          match(xasmParser::T__28);
          setState(298);
          exp(11);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(299);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(300);
          match(xasmParser::T__29);
          setState(301);
          exp(9);
          break;
        }

        } 
      }
      setState(306);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
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
  enterRule(_localctx, 46, xasmParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(307);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << xasmParser::T__31)
      | (1ULL << xasmParser::T__32)
      | (1ULL << xasmParser::T__33)
      | (1ULL << xasmParser::T__34)
      | (1ULL << xasmParser::T__35)
      | (1ULL << xasmParser::T__36))) != 0))) {
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
  enterRule(_localctx, 48, xasmParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(309);
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
  enterRule(_localctx, 50, xasmParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(311);
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
  enterRule(_localctx, 52, xasmParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(313);
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
    case 22: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

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
  "xaccsrc", "xacckernel", "xacclambda", "typedparam", "variable_param_name", 
  "type", "mainprog", "program", "line", "statement", "comment", "forstmt", 
  "ifstmt", "instruction", "bufferList", "paramList", "parameter", "composite_generator", 
  "bufferIndex", "optionsMap", "optionsType", "explist", "exp", "unaryop", 
  "id", "real", "string"
};

std::vector<std::string> xasmParser::_literalNames = {
  "", "'__qpu__'", "'void'", "'('", "','", "')'", "'{'", "'}'", "'['", "'&'", 
  "'='", "']'", "'*'", "'auto'", "'int'", "'<'", "'>'", "'::'", "'std::shared_ptr<'", 
  "';'", "'return'", "'for'", "'<='", "'>='", "'++'", "'--'", "'if'", "'+'", 
  "'-'", "'/'", "'^'", "'pi'", "'sin'", "'cos'", "'tan'", "'exp'", "'ln'", 
  "'sqrt'"
};

std::vector<std::string> xasmParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "COMMENT", "ID", "REAL", "INT", "STRING", "WS", "EOL"
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
    0x3, 0x2e, 0x13e, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 
    0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 
    0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 
    0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 
    0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x4, 0x18, 0x9, 
    0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 
    0x4, 0x1c, 0x9, 0x1c, 0x3, 0x2, 0x3, 0x2, 0x5, 0x2, 0x3b, 0xa, 0x2, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x7, 0x3, 0x44, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x47, 0xb, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x4c, 0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x4, 0x3, 0x4, 0x5, 0x4, 0x52, 0xa, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x7, 0x4, 0x59, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x5c, 
    0xb, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x61, 0xa, 0x4, 0x3, 
    0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x67, 0xa, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 
    0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x74, 0xa, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x78, 0xa, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 
    0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x5, 0x7, 0x81, 0xa, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x85, 0xa, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 
    0x5, 0x7, 0x8b, 0xa, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x6, 0x9, 0x90, 
    0xa, 0x9, 0xd, 0x9, 0xe, 0x9, 0x91, 0x3, 0xa, 0x6, 0xa, 0x95, 0xa, 0xa, 
    0xd, 0xa, 0xe, 0xa, 0x96, 0x3, 0xa, 0x5, 0xa, 0x9a, 0xa, 0xa, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 0xa6, 0xa, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 
    0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 
    0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 
    0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x5, 0xf, 0xcc, 
    0xa, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x5, 
    0x10, 0xd3, 0xa, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x7, 0x11, 0xd8, 
    0xa, 0x11, 0xc, 0x11, 0xe, 0x11, 0xdb, 0xb, 0x11, 0x3, 0x12, 0x3, 0x12, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xe4, 
    0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xe8, 0xa, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 
    0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x7, 0x15, 0xf5, 0xa, 0x15, 0xc, 
    0x15, 0xe, 0x15, 0xf8, 0xb, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 
    0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x17, 0x3, 0x17, 
    0x3, 0x17, 0x7, 0x17, 0x105, 0xa, 0x17, 0xc, 0x17, 0xe, 0x17, 0x108, 
    0xb, 0x17, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 
    0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 
    0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 
    0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x5, 0x18, 0x120, 0xa, 0x18, 
    0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 
    0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 
    0x3, 0x18, 0x3, 0x18, 0x7, 0x18, 0x131, 0xa, 0x18, 0xc, 0x18, 0xe, 0x18, 
    0x134, 0xb, 0x18, 0x3, 0x19, 0x3, 0x19, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1b, 
    0x3, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x2, 0x3, 0x2e, 0x1d, 0x2, 
    0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 
    0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 
    0x36, 0x2, 0x9, 0x3, 0x2, 0xb, 0xc, 0x4, 0x2, 0xb, 0xb, 0xe, 0xe, 0x3, 
    0x2, 0xf, 0x10, 0x4, 0x2, 0x11, 0x12, 0x18, 0x19, 0x3, 0x2, 0x1a, 0x1b, 
    0x4, 0x2, 0x29, 0x29, 0x2b, 0x2b, 0x3, 0x2, 0x22, 0x27, 0x2, 0x14c, 
    0x2, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x4, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x6, 0x4f, 
    0x3, 0x2, 0x2, 0x2, 0x8, 0x64, 0x3, 0x2, 0x2, 0x2, 0xa, 0x6a, 0x3, 0x2, 
    0x2, 0x2, 0xc, 0x8a, 0x3, 0x2, 0x2, 0x2, 0xe, 0x8c, 0x3, 0x2, 0x2, 0x2, 
    0x10, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x12, 0x99, 0x3, 0x2, 0x2, 0x2, 0x14, 
    0xa5, 0x3, 0x2, 0x2, 0x2, 0x16, 0xa7, 0x3, 0x2, 0x2, 0x2, 0x18, 0xa9, 
    0x3, 0x2, 0x2, 0x2, 0x1a, 0xbb, 0x3, 0x2, 0x2, 0x2, 0x1c, 0xc6, 0x3, 
    0x2, 0x2, 0x2, 0x1e, 0xcf, 0x3, 0x2, 0x2, 0x2, 0x20, 0xd4, 0x3, 0x2, 
    0x2, 0x2, 0x22, 0xdc, 0x3, 0x2, 0x2, 0x2, 0x24, 0xde, 0x3, 0x2, 0x2, 
    0x2, 0x26, 0xeb, 0x3, 0x2, 0x2, 0x2, 0x28, 0xf0, 0x3, 0x2, 0x2, 0x2, 
    0x2a, 0xfb, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x101, 0x3, 0x2, 0x2, 0x2, 0x2e, 
    0x11f, 0x3, 0x2, 0x2, 0x2, 0x30, 0x135, 0x3, 0x2, 0x2, 0x2, 0x32, 0x137, 
    0x3, 0x2, 0x2, 0x2, 0x34, 0x139, 0x3, 0x2, 0x2, 0x2, 0x36, 0x13b, 0x3, 
    0x2, 0x2, 0x2, 0x38, 0x3b, 0x5, 0x4, 0x3, 0x2, 0x39, 0x3b, 0x5, 0x6, 
    0x4, 0x2, 0x3a, 0x38, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x39, 0x3, 0x2, 0x2, 
    0x2, 0x3b, 0x3, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3d, 0x7, 0x3, 0x2, 0x2, 
    0x3d, 0x3e, 0x7, 0x4, 0x2, 0x2, 0x3e, 0x3f, 0x5, 0x32, 0x1a, 0x2, 0x3f, 
    0x40, 0x7, 0x5, 0x2, 0x2, 0x40, 0x45, 0x5, 0x8, 0x5, 0x2, 0x41, 0x42, 
    0x7, 0x6, 0x2, 0x2, 0x42, 0x44, 0x5, 0x8, 0x5, 0x2, 0x43, 0x41, 0x3, 
    0x2, 0x2, 0x2, 0x44, 0x47, 0x3, 0x2, 0x2, 0x2, 0x45, 0x43, 0x3, 0x2, 
    0x2, 0x2, 0x45, 0x46, 0x3, 0x2, 0x2, 0x2, 0x46, 0x48, 0x3, 0x2, 0x2, 
    0x2, 0x47, 0x45, 0x3, 0x2, 0x2, 0x2, 0x48, 0x49, 0x7, 0x7, 0x2, 0x2, 
    0x49, 0x4b, 0x7, 0x8, 0x2, 0x2, 0x4a, 0x4c, 0x5, 0xe, 0x8, 0x2, 0x4b, 
    0x4a, 0x3, 0x2, 0x2, 0x2, 0x4b, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4d, 
    0x3, 0x2, 0x2, 0x2, 0x4d, 0x4e, 0x7, 0x9, 0x2, 0x2, 0x4e, 0x5, 0x3, 
    0x2, 0x2, 0x2, 0x4f, 0x51, 0x7, 0xa, 0x2, 0x2, 0x50, 0x52, 0x9, 0x2, 
    0x2, 0x2, 0x51, 0x50, 0x3, 0x2, 0x2, 0x2, 0x51, 0x52, 0x3, 0x2, 0x2, 
    0x2, 0x52, 0x53, 0x3, 0x2, 0x2, 0x2, 0x53, 0x54, 0x7, 0xd, 0x2, 0x2, 
    0x54, 0x55, 0x7, 0x5, 0x2, 0x2, 0x55, 0x5a, 0x5, 0x8, 0x5, 0x2, 0x56, 
    0x57, 0x7, 0x6, 0x2, 0x2, 0x57, 0x59, 0x5, 0x8, 0x5, 0x2, 0x58, 0x56, 
    0x3, 0x2, 0x2, 0x2, 0x59, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x58, 0x3, 
    0x2, 0x2, 0x2, 0x5a, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5d, 0x3, 0x2, 
    0x2, 0x2, 0x5c, 0x5a, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5e, 0x7, 0x7, 0x2, 
    0x2, 0x5e, 0x60, 0x7, 0x8, 0x2, 0x2, 0x5f, 0x61, 0x5, 0xe, 0x8, 0x2, 
    0x60, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x60, 0x61, 0x3, 0x2, 0x2, 0x2, 0x61, 
    0x62, 0x3, 0x2, 0x2, 0x2, 0x62, 0x63, 0x7, 0x9, 0x2, 0x2, 0x63, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x64, 0x66, 0x5, 0xc, 0x7, 0x2, 0x65, 0x67, 0x9, 
    0x3, 0x2, 0x2, 0x66, 0x65, 0x3, 0x2, 0x2, 0x2, 0x66, 0x67, 0x3, 0x2, 
    0x2, 0x2, 0x67, 0x68, 0x3, 0x2, 0x2, 0x2, 0x68, 0x69, 0x5, 0xa, 0x6, 
    0x2, 0x69, 0x9, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x6b, 0x5, 0x32, 0x1a, 0x2, 
    0x6b, 0xb, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x8b, 0x7, 0xf, 0x2, 0x2, 0x6d, 
    0x8b, 0x7, 0x10, 0x2, 0x2, 0x6e, 0x77, 0x5, 0x32, 0x1a, 0x2, 0x6f, 0x70, 
    0x7, 0x11, 0x2, 0x2, 0x70, 0x73, 0x5, 0x32, 0x1a, 0x2, 0x71, 0x72, 0x7, 
    0x6, 0x2, 0x2, 0x72, 0x74, 0x5, 0x32, 0x1a, 0x2, 0x73, 0x71, 0x3, 0x2, 
    0x2, 0x2, 0x73, 0x74, 0x3, 0x2, 0x2, 0x2, 0x74, 0x75, 0x3, 0x2, 0x2, 
    0x2, 0x75, 0x76, 0x7, 0x12, 0x2, 0x2, 0x76, 0x78, 0x3, 0x2, 0x2, 0x2, 
    0x77, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x77, 0x78, 0x3, 0x2, 0x2, 0x2, 0x78, 
    0x8b, 0x3, 0x2, 0x2, 0x2, 0x79, 0x7a, 0x5, 0x32, 0x1a, 0x2, 0x7a, 0x7b, 
    0x7, 0x13, 0x2, 0x2, 0x7b, 0x84, 0x5, 0x32, 0x1a, 0x2, 0x7c, 0x7d, 0x7, 
    0x11, 0x2, 0x2, 0x7d, 0x80, 0x5, 0x32, 0x1a, 0x2, 0x7e, 0x7f, 0x7, 0x6, 
    0x2, 0x2, 0x7f, 0x81, 0x5, 0x32, 0x1a, 0x2, 0x80, 0x7e, 0x3, 0x2, 0x2, 
    0x2, 0x80, 0x81, 0x3, 0x2, 0x2, 0x2, 0x81, 0x82, 0x3, 0x2, 0x2, 0x2, 
    0x82, 0x83, 0x7, 0x12, 0x2, 0x2, 0x83, 0x85, 0x3, 0x2, 0x2, 0x2, 0x84, 
    0x7c, 0x3, 0x2, 0x2, 0x2, 0x84, 0x85, 0x3, 0x2, 0x2, 0x2, 0x85, 0x8b, 
    0x3, 0x2, 0x2, 0x2, 0x86, 0x87, 0x7, 0x14, 0x2, 0x2, 0x87, 0x88, 0x5, 
    0xc, 0x7, 0x2, 0x88, 0x89, 0x7, 0x12, 0x2, 0x2, 0x89, 0x8b, 0x3, 0x2, 
    0x2, 0x2, 0x8a, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x6d, 0x3, 0x2, 0x2, 
    0x2, 0x8a, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x79, 0x3, 0x2, 0x2, 0x2, 
    0x8a, 0x86, 0x3, 0x2, 0x2, 0x2, 0x8b, 0xd, 0x3, 0x2, 0x2, 0x2, 0x8c, 
    0x8d, 0x5, 0x10, 0x9, 0x2, 0x8d, 0xf, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x90, 
    0x5, 0x12, 0xa, 0x2, 0x8f, 0x8e, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 0x3, 
    0x2, 0x2, 0x2, 0x91, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x91, 0x92, 0x3, 0x2, 
    0x2, 0x2, 0x92, 0x11, 0x3, 0x2, 0x2, 0x2, 0x93, 0x95, 0x5, 0x14, 0xb, 
    0x2, 0x94, 0x93, 0x3, 0x2, 0x2, 0x2, 0x95, 0x96, 0x3, 0x2, 0x2, 0x2, 
    0x96, 0x94, 0x3, 0x2, 0x2, 0x2, 0x96, 0x97, 0x3, 0x2, 0x2, 0x2, 0x97, 
    0x9a, 0x3, 0x2, 0x2, 0x2, 0x98, 0x9a, 0x5, 0x16, 0xc, 0x2, 0x99, 0x94, 
    0x3, 0x2, 0x2, 0x2, 0x99, 0x98, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x13, 0x3, 
    0x2, 0x2, 0x2, 0x9b, 0x9c, 0x5, 0x1c, 0xf, 0x2, 0x9c, 0x9d, 0x7, 0x15, 
    0x2, 0x2, 0x9d, 0xa6, 0x3, 0x2, 0x2, 0x2, 0x9e, 0x9f, 0x5, 0x24, 0x13, 
    0x2, 0x9f, 0xa0, 0x7, 0x15, 0x2, 0x2, 0xa0, 0xa6, 0x3, 0x2, 0x2, 0x2, 
    0xa1, 0xa6, 0x5, 0x18, 0xd, 0x2, 0xa2, 0xa6, 0x5, 0x1a, 0xe, 0x2, 0xa3, 
    0xa4, 0x7, 0x16, 0x2, 0x2, 0xa4, 0xa6, 0x7, 0x15, 0x2, 0x2, 0xa5, 0x9b, 
    0x3, 0x2, 0x2, 0x2, 0xa5, 0x9e, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa1, 0x3, 
    0x2, 0x2, 0x2, 0xa5, 0xa2, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa3, 0x3, 0x2, 
    0x2, 0x2, 0xa6, 0x15, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa8, 0x7, 0x28, 0x2, 
    0x2, 0xa8, 0x17, 0x3, 0x2, 0x2, 0x2, 0xa9, 0xaa, 0x7, 0x17, 0x2, 0x2, 
    0xaa, 0xab, 0x7, 0x5, 0x2, 0x2, 0xab, 0xac, 0x9, 0x4, 0x2, 0x2, 0xac, 
    0xad, 0x5, 0x32, 0x1a, 0x2, 0xad, 0xae, 0x7, 0xc, 0x2, 0x2, 0xae, 0xaf, 
    0x7, 0x2b, 0x2, 0x2, 0xaf, 0xb0, 0x7, 0x15, 0x2, 0x2, 0xb0, 0xb1, 0x5, 
    0x32, 0x1a, 0x2, 0xb1, 0xb2, 0x9, 0x5, 0x2, 0x2, 0xb2, 0xb3, 0x7, 0x2b, 
    0x2, 0x2, 0xb3, 0xb4, 0x7, 0x15, 0x2, 0x2, 0xb4, 0xb5, 0x5, 0x32, 0x1a, 
    0x2, 0xb5, 0xb6, 0x9, 0x6, 0x2, 0x2, 0xb6, 0xb7, 0x7, 0x7, 0x2, 0x2, 
    0xb7, 0xb8, 0x7, 0x8, 0x2, 0x2, 0xb8, 0xb9, 0x5, 0x10, 0x9, 0x2, 0xb9, 
    0xba, 0x7, 0x9, 0x2, 0x2, 0xba, 0x19, 0x3, 0x2, 0x2, 0x2, 0xbb, 0xbc, 
    0x7, 0x1c, 0x2, 0x2, 0xbc, 0xbd, 0x7, 0x5, 0x2, 0x2, 0xbd, 0xbe, 0x5, 
    0x32, 0x1a, 0x2, 0xbe, 0xbf, 0x7, 0xa, 0x2, 0x2, 0xbf, 0xc0, 0x7, 0x2b, 
    0x2, 0x2, 0xc0, 0xc1, 0x7, 0xd, 0x2, 0x2, 0xc1, 0xc2, 0x7, 0x7, 0x2, 
    0x2, 0xc2, 0xc3, 0x7, 0x8, 0x2, 0x2, 0xc3, 0xc4, 0x5, 0x10, 0x9, 0x2, 
    0xc4, 0xc5, 0x7, 0x9, 0x2, 0x2, 0xc5, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xc6, 
    0xc7, 0x5, 0x32, 0x1a, 0x2, 0xc7, 0xc8, 0x7, 0x5, 0x2, 0x2, 0xc8, 0xcb, 
    0x5, 0x1e, 0x10, 0x2, 0xc9, 0xca, 0x7, 0x6, 0x2, 0x2, 0xca, 0xcc, 0x5, 
    0x20, 0x11, 0x2, 0xcb, 0xc9, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 0x3, 0x2, 
    0x2, 0x2, 0xcc, 0xcd, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xce, 0x7, 0x7, 0x2, 
    0x2, 0xce, 0x1d, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xd2, 0x5, 0x26, 0x14, 0x2, 
    0xd0, 0xd1, 0x7, 0x6, 0x2, 0x2, 0xd1, 0xd3, 0x5, 0x26, 0x14, 0x2, 0xd2, 
    0xd0, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd3, 0x1f, 
    0x3, 0x2, 0x2, 0x2, 0xd4, 0xd9, 0x5, 0x22, 0x12, 0x2, 0xd5, 0xd6, 0x7, 
    0x6, 0x2, 0x2, 0xd6, 0xd8, 0x5, 0x22, 0x12, 0x2, 0xd7, 0xd5, 0x3, 0x2, 
    0x2, 0x2, 0xd8, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xd9, 0xd7, 0x3, 0x2, 0x2, 
    0x2, 0xd9, 0xda, 0x3, 0x2, 0x2, 0x2, 0xda, 0x21, 0x3, 0x2, 0x2, 0x2, 
    0xdb, 0xd9, 0x3, 0x2, 0x2, 0x2, 0xdc, 0xdd, 0x5, 0x2e, 0x18, 0x2, 0xdd, 
    0x23, 0x3, 0x2, 0x2, 0x2, 0xde, 0xdf, 0x5, 0x32, 0x1a, 0x2, 0xdf, 0xe0, 
    0x7, 0x5, 0x2, 0x2, 0xe0, 0xe3, 0x5, 0x32, 0x1a, 0x2, 0xe1, 0xe2, 0x7, 
    0x6, 0x2, 0x2, 0xe2, 0xe4, 0x5, 0x20, 0x11, 0x2, 0xe3, 0xe1, 0x3, 0x2, 
    0x2, 0x2, 0xe3, 0xe4, 0x3, 0x2, 0x2, 0x2, 0xe4, 0xe7, 0x3, 0x2, 0x2, 
    0x2, 0xe5, 0xe6, 0x7, 0x6, 0x2, 0x2, 0xe6, 0xe8, 0x5, 0x28, 0x15, 0x2, 
    0xe7, 0xe5, 0x3, 0x2, 0x2, 0x2, 0xe7, 0xe8, 0x3, 0x2, 0x2, 0x2, 0xe8, 
    0xe9, 0x3, 0x2, 0x2, 0x2, 0xe9, 0xea, 0x7, 0x7, 0x2, 0x2, 0xea, 0x25, 
    0x3, 0x2, 0x2, 0x2, 0xeb, 0xec, 0x5, 0x32, 0x1a, 0x2, 0xec, 0xed, 0x7, 
    0xa, 0x2, 0x2, 0xed, 0xee, 0x5, 0x2e, 0x18, 0x2, 0xee, 0xef, 0x7, 0xd, 
    0x2, 0x2, 0xef, 0x27, 0x3, 0x2, 0x2, 0x2, 0xf0, 0xf1, 0x7, 0x8, 0x2, 
    0x2, 0xf1, 0xf6, 0x5, 0x2a, 0x16, 0x2, 0xf2, 0xf3, 0x7, 0x6, 0x2, 0x2, 
    0xf3, 0xf5, 0x5, 0x2a, 0x16, 0x2, 0xf4, 0xf2, 0x3, 0x2, 0x2, 0x2, 0xf5, 
    0xf8, 0x3, 0x2, 0x2, 0x2, 0xf6, 0xf4, 0x3, 0x2, 0x2, 0x2, 0xf6, 0xf7, 
    0x3, 0x2, 0x2, 0x2, 0xf7, 0xf9, 0x3, 0x2, 0x2, 0x2, 0xf8, 0xf6, 0x3, 
    0x2, 0x2, 0x2, 0xf9, 0xfa, 0x7, 0x9, 0x2, 0x2, 0xfa, 0x29, 0x3, 0x2, 
    0x2, 0x2, 0xfb, 0xfc, 0x7, 0x8, 0x2, 0x2, 0xfc, 0xfd, 0x5, 0x36, 0x1c, 
    0x2, 0xfd, 0xfe, 0x7, 0x6, 0x2, 0x2, 0xfe, 0xff, 0x5, 0x2e, 0x18, 0x2, 
    0xff, 0x100, 0x7, 0x9, 0x2, 0x2, 0x100, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x101, 
    0x106, 0x5, 0x2e, 0x18, 0x2, 0x102, 0x103, 0x7, 0x6, 0x2, 0x2, 0x103, 
    0x105, 0x5, 0x2e, 0x18, 0x2, 0x104, 0x102, 0x3, 0x2, 0x2, 0x2, 0x105, 
    0x108, 0x3, 0x2, 0x2, 0x2, 0x106, 0x104, 0x3, 0x2, 0x2, 0x2, 0x106, 
    0x107, 0x3, 0x2, 0x2, 0x2, 0x107, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x108, 0x106, 
    0x3, 0x2, 0x2, 0x2, 0x109, 0x10a, 0x8, 0x18, 0x1, 0x2, 0x10a, 0x120, 
    0x5, 0x32, 0x1a, 0x2, 0x10b, 0x10c, 0x7, 0x1e, 0x2, 0x2, 0x10c, 0x120, 
    0x5, 0x2e, 0x18, 0xb, 0x10d, 0x10e, 0x7, 0x5, 0x2, 0x2, 0x10e, 0x10f, 
    0x5, 0x2e, 0x18, 0x2, 0x10f, 0x110, 0x7, 0x7, 0x2, 0x2, 0x110, 0x120, 
    0x3, 0x2, 0x2, 0x2, 0x111, 0x112, 0x5, 0x30, 0x19, 0x2, 0x112, 0x113, 
    0x7, 0x5, 0x2, 0x2, 0x113, 0x114, 0x5, 0x2e, 0x18, 0x2, 0x114, 0x115, 
    0x7, 0x7, 0x2, 0x2, 0x115, 0x120, 0x3, 0x2, 0x2, 0x2, 0x116, 0x120, 
    0x5, 0x36, 0x1c, 0x2, 0x117, 0x120, 0x5, 0x34, 0x1b, 0x2, 0x118, 0x120, 
    0x7, 0x2b, 0x2, 0x2, 0x119, 0x120, 0x7, 0x21, 0x2, 0x2, 0x11a, 0x11b, 
    0x5, 0x32, 0x1a, 0x2, 0x11b, 0x11c, 0x7, 0xa, 0x2, 0x2, 0x11c, 0x11d, 
    0x9, 0x7, 0x2, 0x2, 0x11d, 0x11e, 0x7, 0xd, 0x2, 0x2, 0x11e, 0x120, 
    0x3, 0x2, 0x2, 0x2, 0x11f, 0x109, 0x3, 0x2, 0x2, 0x2, 0x11f, 0x10b, 
    0x3, 0x2, 0x2, 0x2, 0x11f, 0x10d, 0x3, 0x2, 0x2, 0x2, 0x11f, 0x111, 
    0x3, 0x2, 0x2, 0x2, 0x11f, 0x116, 0x3, 0x2, 0x2, 0x2, 0x11f, 0x117, 
    0x3, 0x2, 0x2, 0x2, 0x11f, 0x118, 0x3, 0x2, 0x2, 0x2, 0x11f, 0x119, 
    0x3, 0x2, 0x2, 0x2, 0x11f, 0x11a, 0x3, 0x2, 0x2, 0x2, 0x120, 0x132, 
    0x3, 0x2, 0x2, 0x2, 0x121, 0x122, 0xc, 0xf, 0x2, 0x2, 0x122, 0x123, 
    0x7, 0x1d, 0x2, 0x2, 0x123, 0x131, 0x5, 0x2e, 0x18, 0x10, 0x124, 0x125, 
    0xc, 0xe, 0x2, 0x2, 0x125, 0x126, 0x7, 0x1e, 0x2, 0x2, 0x126, 0x131, 
    0x5, 0x2e, 0x18, 0xf, 0x127, 0x128, 0xc, 0xd, 0x2, 0x2, 0x128, 0x129, 
    0x7, 0xe, 0x2, 0x2, 0x129, 0x131, 0x5, 0x2e, 0x18, 0xe, 0x12a, 0x12b, 
    0xc, 0xc, 0x2, 0x2, 0x12b, 0x12c, 0x7, 0x1f, 0x2, 0x2, 0x12c, 0x131, 
    0x5, 0x2e, 0x18, 0xd, 0x12d, 0x12e, 0xc, 0xa, 0x2, 0x2, 0x12e, 0x12f, 
    0x7, 0x20, 0x2, 0x2, 0x12f, 0x131, 0x5, 0x2e, 0x18, 0xb, 0x130, 0x121, 
    0x3, 0x2, 0x2, 0x2, 0x130, 0x124, 0x3, 0x2, 0x2, 0x2, 0x130, 0x127, 
    0x3, 0x2, 0x2, 0x2, 0x130, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x130, 0x12d, 
    0x3, 0x2, 0x2, 0x2, 0x131, 0x134, 0x3, 0x2, 0x2, 0x2, 0x132, 0x130, 
    0x3, 0x2, 0x2, 0x2, 0x132, 0x133, 0x3, 0x2, 0x2, 0x2, 0x133, 0x2f, 0x3, 
    0x2, 0x2, 0x2, 0x134, 0x132, 0x3, 0x2, 0x2, 0x2, 0x135, 0x136, 0x9, 
    0x8, 0x2, 0x2, 0x136, 0x31, 0x3, 0x2, 0x2, 0x2, 0x137, 0x138, 0x7, 0x29, 
    0x2, 0x2, 0x138, 0x33, 0x3, 0x2, 0x2, 0x2, 0x139, 0x13a, 0x7, 0x2a, 
    0x2, 0x2, 0x13a, 0x35, 0x3, 0x2, 0x2, 0x2, 0x13b, 0x13c, 0x7, 0x2c, 
    0x2, 0x2, 0x13c, 0x37, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x3a, 0x45, 0x4b, 0x51, 
    0x5a, 0x60, 0x66, 0x73, 0x77, 0x80, 0x84, 0x8a, 0x91, 0x96, 0x99, 0xa5, 
    0xcb, 0xd2, 0xd9, 0xe3, 0xe7, 0xf6, 0x106, 0x11f, 0x130, 0x132, 
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
