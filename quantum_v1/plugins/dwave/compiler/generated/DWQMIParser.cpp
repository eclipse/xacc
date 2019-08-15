
// Generated from DWQMI.g4 by ANTLR 4.7.1


#include "DWQMIListener.h"

#include "DWQMIParser.h"


using namespace antlrcpp;
using namespace dwqmi;
using namespace antlr4;

DWQMIParser::DWQMIParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

DWQMIParser::~DWQMIParser() {
  delete _interpreter;
}

std::string DWQMIParser::getGrammarFileName() const {
  return "DWQMI.g4";
}

const std::vector<std::string>& DWQMIParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& DWQMIParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

DWQMIParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DWQMIParser::XacckernelContext *> DWQMIParser::XaccsrcContext::xacckernel() {
  return getRuleContexts<DWQMIParser::XacckernelContext>();
}

DWQMIParser::XacckernelContext* DWQMIParser::XaccsrcContext::xacckernel(size_t i) {
  return getRuleContext<DWQMIParser::XacckernelContext>(i);
}


size_t DWQMIParser::XaccsrcContext::getRuleIndex() const {
  return DWQMIParser::RuleXaccsrc;
}

void DWQMIParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void DWQMIParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

DWQMIParser::XaccsrcContext* DWQMIParser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, DWQMIParser::RuleXaccsrc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(45);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == DWQMIParser::T__0) {
      setState(42);
      xacckernel();
      setState(47);
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

DWQMIParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::MainprogContext* DWQMIParser::XacckernelContext::mainprog() {
  return getRuleContext<DWQMIParser::MainprogContext>(0);
}

std::vector<DWQMIParser::IdContext *> DWQMIParser::XacckernelContext::id() {
  return getRuleContexts<DWQMIParser::IdContext>();
}

DWQMIParser::IdContext* DWQMIParser::XacckernelContext::id(size_t i) {
  return getRuleContext<DWQMIParser::IdContext>(i);
}

std::vector<DWQMIParser::TypedparamContext *> DWQMIParser::XacckernelContext::typedparam() {
  return getRuleContexts<DWQMIParser::TypedparamContext>();
}

DWQMIParser::TypedparamContext* DWQMIParser::XacckernelContext::typedparam(size_t i) {
  return getRuleContext<DWQMIParser::TypedparamContext>(i);
}


size_t DWQMIParser::XacckernelContext::getRuleIndex() const {
  return DWQMIParser::RuleXacckernel;
}

void DWQMIParser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void DWQMIParser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

DWQMIParser::XacckernelContext* DWQMIParser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, DWQMIParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(48);
    match(DWQMIParser::T__0);
    setState(49);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(50);
    match(DWQMIParser::T__1);
    setState(51);
    match(DWQMIParser::T__2);
    setState(52);
    dynamic_cast<XacckernelContext *>(_localctx)->acceleratorbuffer = id();
    setState(57);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == DWQMIParser::T__3) {
      setState(53);
      match(DWQMIParser::T__3);
      setState(54);
      typedparam();
      setState(59);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(60);
    match(DWQMIParser::T__4);
    setState(61);
    match(DWQMIParser::T__5);
    setState(62);
    mainprog();
    setState(63);
    match(DWQMIParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- KernelcallContext ------------------------------------------------------------------

DWQMIParser::KernelcallContext::KernelcallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DWQMIParser::IdContext *> DWQMIParser::KernelcallContext::id() {
  return getRuleContexts<DWQMIParser::IdContext>();
}

DWQMIParser::IdContext* DWQMIParser::KernelcallContext::id(size_t i) {
  return getRuleContext<DWQMIParser::IdContext>(i);
}


size_t DWQMIParser::KernelcallContext::getRuleIndex() const {
  return DWQMIParser::RuleKernelcall;
}

void DWQMIParser::KernelcallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKernelcall(this);
}

void DWQMIParser::KernelcallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKernelcall(this);
}

DWQMIParser::KernelcallContext* DWQMIParser::kernelcall() {
  KernelcallContext *_localctx = _tracker.createInstance<KernelcallContext>(_ctx, getState());
  enterRule(_localctx, 4, DWQMIParser::RuleKernelcall);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(65);
    dynamic_cast<KernelcallContext *>(_localctx)->kernelname = id();
    setState(66);
    match(DWQMIParser::T__1);
    setState(68);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DWQMIParser::ID) {
      setState(67);
      id();
    }
    setState(74);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == DWQMIParser::T__3) {
      setState(70);
      match(DWQMIParser::T__3);
      setState(71);
      id();
      setState(76);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(77);
    match(DWQMIParser::T__4);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypedparamContext ------------------------------------------------------------------

DWQMIParser::TypedparamContext::TypedparamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::TypedparamContext::TYPE() {
  return getToken(DWQMIParser::TYPE, 0);
}

DWQMIParser::IdContext* DWQMIParser::TypedparamContext::id() {
  return getRuleContext<DWQMIParser::IdContext>(0);
}


size_t DWQMIParser::TypedparamContext::getRuleIndex() const {
  return DWQMIParser::RuleTypedparam;
}

void DWQMIParser::TypedparamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTypedparam(this);
}

void DWQMIParser::TypedparamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTypedparam(this);
}

DWQMIParser::TypedparamContext* DWQMIParser::typedparam() {
  TypedparamContext *_localctx = _tracker.createInstance<TypedparamContext>(_ctx, getState());
  enterRule(_localctx, 6, DWQMIParser::RuleTypedparam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(79);
    match(DWQMIParser::TYPE);
    setState(80);
    id();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MainprogContext ------------------------------------------------------------------

DWQMIParser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::ProgramContext* DWQMIParser::MainprogContext::program() {
  return getRuleContext<DWQMIParser::ProgramContext>(0);
}


size_t DWQMIParser::MainprogContext::getRuleIndex() const {
  return DWQMIParser::RuleMainprog;
}

void DWQMIParser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void DWQMIParser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

DWQMIParser::MainprogContext* DWQMIParser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 8, DWQMIParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(82);
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

DWQMIParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<DWQMIParser::LineContext *> DWQMIParser::ProgramContext::line() {
  return getRuleContexts<DWQMIParser::LineContext>();
}

DWQMIParser::LineContext* DWQMIParser::ProgramContext::line(size_t i) {
  return getRuleContext<DWQMIParser::LineContext>(i);
}


size_t DWQMIParser::ProgramContext::getRuleIndex() const {
  return DWQMIParser::RuleProgram;
}

void DWQMIParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void DWQMIParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

DWQMIParser::ProgramContext* DWQMIParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 10, DWQMIParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(85); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(84);
      line();
      setState(87); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << DWQMIParser::COMMENT)
      | (1ULL << DWQMIParser::ANNEAL)
      | (1ULL << DWQMIParser::ID)
      | (1ULL << DWQMIParser::INT)
      | (1ULL << DWQMIParser::EOL))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

DWQMIParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::LineContext::EOL() {
  return getToken(DWQMIParser::EOL, 0);
}

std::vector<DWQMIParser::StatementContext *> DWQMIParser::LineContext::statement() {
  return getRuleContexts<DWQMIParser::StatementContext>();
}

DWQMIParser::StatementContext* DWQMIParser::LineContext::statement(size_t i) {
  return getRuleContext<DWQMIParser::StatementContext>(i);
}

DWQMIParser::CommentContext* DWQMIParser::LineContext::comment() {
  return getRuleContext<DWQMIParser::CommentContext>(0);
}


size_t DWQMIParser::LineContext::getRuleIndex() const {
  return DWQMIParser::RuleLine;
}

void DWQMIParser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void DWQMIParser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

DWQMIParser::LineContext* DWQMIParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 12, DWQMIParser::RuleLine);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(98);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ANNEAL:
      case DWQMIParser::ID:
      case DWQMIParser::INT: {
        enterOuterAlt(_localctx, 1);
        setState(90); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(89);
          statement();
          setState(92); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << DWQMIParser::ANNEAL)
          | (1ULL << DWQMIParser::ID)
          | (1ULL << DWQMIParser::INT))) != 0));
        setState(94);
        match(DWQMIParser::EOL);
        break;
      }

      case DWQMIParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(96);
        comment();
        break;
      }

      case DWQMIParser::EOL: {
        enterOuterAlt(_localctx, 3);
        setState(97);
        match(DWQMIParser::EOL);
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

DWQMIParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::AnnealdeclContext* DWQMIParser::StatementContext::annealdecl() {
  return getRuleContext<DWQMIParser::AnnealdeclContext>(0);
}

DWQMIParser::InstContext* DWQMIParser::StatementContext::inst() {
  return getRuleContext<DWQMIParser::InstContext>(0);
}

DWQMIParser::KernelcallContext* DWQMIParser::StatementContext::kernelcall() {
  return getRuleContext<DWQMIParser::KernelcallContext>(0);
}


size_t DWQMIParser::StatementContext::getRuleIndex() const {
  return DWQMIParser::RuleStatement;
}

void DWQMIParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void DWQMIParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

DWQMIParser::StatementContext* DWQMIParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 14, DWQMIParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(109);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ANNEAL: {
        enterOuterAlt(_localctx, 1);
        setState(100);
        annealdecl();
        setState(101);
        match(DWQMIParser::T__7);
        break;
      }

      case DWQMIParser::INT: {
        enterOuterAlt(_localctx, 2);
        setState(103);
        inst();
        setState(104);
        match(DWQMIParser::T__7);
        break;
      }

      case DWQMIParser::ID: {
        enterOuterAlt(_localctx, 3);
        setState(106);
        kernelcall();
        setState(107);
        match(DWQMIParser::T__7);
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

DWQMIParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::CommentContext::COMMENT() {
  return getToken(DWQMIParser::COMMENT, 0);
}


size_t DWQMIParser::CommentContext::getRuleIndex() const {
  return DWQMIParser::RuleComment;
}

void DWQMIParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void DWQMIParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

DWQMIParser::CommentContext* DWQMIParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 16, DWQMIParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(111);
    match(DWQMIParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstContext ------------------------------------------------------------------

DWQMIParser::InstContext::InstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> DWQMIParser::InstContext::INT() {
  return getTokens(DWQMIParser::INT);
}

tree::TerminalNode* DWQMIParser::InstContext::INT(size_t i) {
  return getToken(DWQMIParser::INT, i);
}

DWQMIParser::IdContext* DWQMIParser::InstContext::id() {
  return getRuleContext<DWQMIParser::IdContext>(0);
}

DWQMIParser::RealContext* DWQMIParser::InstContext::real() {
  return getRuleContext<DWQMIParser::RealContext>(0);
}


size_t DWQMIParser::InstContext::getRuleIndex() const {
  return DWQMIParser::RuleInst;
}

void DWQMIParser::InstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInst(this);
}

void DWQMIParser::InstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInst(this);
}

DWQMIParser::InstContext* DWQMIParser::inst() {
  InstContext *_localctx = _tracker.createInstance<InstContext>(_ctx, getState());
  enterRule(_localctx, 18, DWQMIParser::RuleInst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    match(DWQMIParser::INT);
    setState(114);
    match(DWQMIParser::INT);
    setState(118);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ID: {
        setState(115);
        id();
        break;
      }

      case DWQMIParser::REAL: {
        setState(116);
        real();
        break;
      }

      case DWQMIParser::INT: {
        setState(117);
        match(DWQMIParser::INT);
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

//----------------- AnnealdeclContext ------------------------------------------------------------------

DWQMIParser::AnnealdeclContext::AnnealdeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::AnnealContext* DWQMIParser::AnnealdeclContext::anneal() {
  return getRuleContext<DWQMIParser::AnnealContext>(0);
}

DWQMIParser::TaContext* DWQMIParser::AnnealdeclContext::ta() {
  return getRuleContext<DWQMIParser::TaContext>(0);
}

DWQMIParser::TpContext* DWQMIParser::AnnealdeclContext::tp() {
  return getRuleContext<DWQMIParser::TpContext>(0);
}

DWQMIParser::TqContext* DWQMIParser::AnnealdeclContext::tq() {
  return getRuleContext<DWQMIParser::TqContext>(0);
}

DWQMIParser::DirectionContext* DWQMIParser::AnnealdeclContext::direction() {
  return getRuleContext<DWQMIParser::DirectionContext>(0);
}


size_t DWQMIParser::AnnealdeclContext::getRuleIndex() const {
  return DWQMIParser::RuleAnnealdecl;
}

void DWQMIParser::AnnealdeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnnealdecl(this);
}

void DWQMIParser::AnnealdeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnnealdecl(this);
}

DWQMIParser::AnnealdeclContext* DWQMIParser::annealdecl() {
  AnnealdeclContext *_localctx = _tracker.createInstance<AnnealdeclContext>(_ctx, getState());
  enterRule(_localctx, 20, DWQMIParser::RuleAnnealdecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(120);
    anneal();
    setState(121);
    ta();
    setState(122);
    tp();
    setState(123);
    tq();
    setState(125);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == DWQMIParser::T__8

    || _la == DWQMIParser::T__9) {
      setState(124);
      direction();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TaContext ------------------------------------------------------------------

DWQMIParser::TaContext::TaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::IdContext* DWQMIParser::TaContext::id() {
  return getRuleContext<DWQMIParser::IdContext>(0);
}

DWQMIParser::RealContext* DWQMIParser::TaContext::real() {
  return getRuleContext<DWQMIParser::RealContext>(0);
}

tree::TerminalNode* DWQMIParser::TaContext::INT() {
  return getToken(DWQMIParser::INT, 0);
}


size_t DWQMIParser::TaContext::getRuleIndex() const {
  return DWQMIParser::RuleTa;
}

void DWQMIParser::TaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTa(this);
}

void DWQMIParser::TaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTa(this);
}

DWQMIParser::TaContext* DWQMIParser::ta() {
  TaContext *_localctx = _tracker.createInstance<TaContext>(_ctx, getState());
  enterRule(_localctx, 22, DWQMIParser::RuleTa);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(130);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(127);
        id();
        break;
      }

      case DWQMIParser::REAL: {
        enterOuterAlt(_localctx, 2);
        setState(128);
        real();
        break;
      }

      case DWQMIParser::INT: {
        enterOuterAlt(_localctx, 3);
        setState(129);
        match(DWQMIParser::INT);
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

//----------------- TpContext ------------------------------------------------------------------

DWQMIParser::TpContext::TpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::IdContext* DWQMIParser::TpContext::id() {
  return getRuleContext<DWQMIParser::IdContext>(0);
}

DWQMIParser::RealContext* DWQMIParser::TpContext::real() {
  return getRuleContext<DWQMIParser::RealContext>(0);
}

tree::TerminalNode* DWQMIParser::TpContext::INT() {
  return getToken(DWQMIParser::INT, 0);
}


size_t DWQMIParser::TpContext::getRuleIndex() const {
  return DWQMIParser::RuleTp;
}

void DWQMIParser::TpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTp(this);
}

void DWQMIParser::TpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTp(this);
}

DWQMIParser::TpContext* DWQMIParser::tp() {
  TpContext *_localctx = _tracker.createInstance<TpContext>(_ctx, getState());
  enterRule(_localctx, 24, DWQMIParser::RuleTp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(135);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(132);
        id();
        break;
      }

      case DWQMIParser::REAL: {
        enterOuterAlt(_localctx, 2);
        setState(133);
        real();
        break;
      }

      case DWQMIParser::INT: {
        enterOuterAlt(_localctx, 3);
        setState(134);
        match(DWQMIParser::INT);
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

//----------------- TqContext ------------------------------------------------------------------

DWQMIParser::TqContext::TqContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::IdContext* DWQMIParser::TqContext::id() {
  return getRuleContext<DWQMIParser::IdContext>(0);
}

DWQMIParser::RealContext* DWQMIParser::TqContext::real() {
  return getRuleContext<DWQMIParser::RealContext>(0);
}

tree::TerminalNode* DWQMIParser::TqContext::INT() {
  return getToken(DWQMIParser::INT, 0);
}


size_t DWQMIParser::TqContext::getRuleIndex() const {
  return DWQMIParser::RuleTq;
}

void DWQMIParser::TqContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTq(this);
}

void DWQMIParser::TqContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTq(this);
}

DWQMIParser::TqContext* DWQMIParser::tq() {
  TqContext *_localctx = _tracker.createInstance<TqContext>(_ctx, getState());
  enterRule(_localctx, 26, DWQMIParser::RuleTq);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(140);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(137);
        id();
        break;
      }

      case DWQMIParser::REAL: {
        enterOuterAlt(_localctx, 2);
        setState(138);
        real();
        break;
      }

      case DWQMIParser::INT: {
        enterOuterAlt(_localctx, 3);
        setState(139);
        match(DWQMIParser::INT);
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

//----------------- AnnealContext ------------------------------------------------------------------

DWQMIParser::AnnealContext::AnnealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::AnnealContext::ANNEAL() {
  return getToken(DWQMIParser::ANNEAL, 0);
}


size_t DWQMIParser::AnnealContext::getRuleIndex() const {
  return DWQMIParser::RuleAnneal;
}

void DWQMIParser::AnnealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAnneal(this);
}

void DWQMIParser::AnnealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAnneal(this);
}

DWQMIParser::AnnealContext* DWQMIParser::anneal() {
  AnnealContext *_localctx = _tracker.createInstance<AnnealContext>(_ctx, getState());
  enterRule(_localctx, 28, DWQMIParser::RuleAnneal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(142);
    match(DWQMIParser::ANNEAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdContext ------------------------------------------------------------------

DWQMIParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::IdContext::ID() {
  return getToken(DWQMIParser::ID, 0);
}


size_t DWQMIParser::IdContext::getRuleIndex() const {
  return DWQMIParser::RuleId;
}

void DWQMIParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void DWQMIParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

DWQMIParser::IdContext* DWQMIParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 30, DWQMIParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(144);
    match(DWQMIParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

DWQMIParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::RealContext::REAL() {
  return getToken(DWQMIParser::REAL, 0);
}


size_t DWQMIParser::RealContext::getRuleIndex() const {
  return DWQMIParser::RuleReal;
}

void DWQMIParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void DWQMIParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

DWQMIParser::RealContext* DWQMIParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 32, DWQMIParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(146);
    match(DWQMIParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

DWQMIParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* DWQMIParser::StringContext::STRING() {
  return getToken(DWQMIParser::STRING, 0);
}


size_t DWQMIParser::StringContext::getRuleIndex() const {
  return DWQMIParser::RuleString;
}

void DWQMIParser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void DWQMIParser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

DWQMIParser::StringContext* DWQMIParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 34, DWQMIParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(148);
    match(DWQMIParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DirectionContext ------------------------------------------------------------------

DWQMIParser::DirectionContext::DirectionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

DWQMIParser::ForwardContext* DWQMIParser::DirectionContext::forward() {
  return getRuleContext<DWQMIParser::ForwardContext>(0);
}

DWQMIParser::ReverseContext* DWQMIParser::DirectionContext::reverse() {
  return getRuleContext<DWQMIParser::ReverseContext>(0);
}


size_t DWQMIParser::DirectionContext::getRuleIndex() const {
  return DWQMIParser::RuleDirection;
}

void DWQMIParser::DirectionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDirection(this);
}

void DWQMIParser::DirectionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDirection(this);
}

DWQMIParser::DirectionContext* DWQMIParser::direction() {
  DirectionContext *_localctx = _tracker.createInstance<DirectionContext>(_ctx, getState());
  enterRule(_localctx, 36, DWQMIParser::RuleDirection);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(152);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case DWQMIParser::T__8: {
        enterOuterAlt(_localctx, 1);
        setState(150);
        forward();
        break;
      }

      case DWQMIParser::T__9: {
        enterOuterAlt(_localctx, 2);
        setState(151);
        reverse();
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

//----------------- ForwardContext ------------------------------------------------------------------

DWQMIParser::ForwardContext::ForwardContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t DWQMIParser::ForwardContext::getRuleIndex() const {
  return DWQMIParser::RuleForward;
}

void DWQMIParser::ForwardContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForward(this);
}

void DWQMIParser::ForwardContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForward(this);
}

DWQMIParser::ForwardContext* DWQMIParser::forward() {
  ForwardContext *_localctx = _tracker.createInstance<ForwardContext>(_ctx, getState());
  enterRule(_localctx, 38, DWQMIParser::RuleForward);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(154);
    match(DWQMIParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ReverseContext ------------------------------------------------------------------

DWQMIParser::ReverseContext::ReverseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t DWQMIParser::ReverseContext::getRuleIndex() const {
  return DWQMIParser::RuleReverse;
}

void DWQMIParser::ReverseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReverse(this);
}

void DWQMIParser::ReverseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<DWQMIListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReverse(this);
}

DWQMIParser::ReverseContext* DWQMIParser::reverse() {
  ReverseContext *_localctx = _tracker.createInstance<ReverseContext>(_ctx, getState());
  enterRule(_localctx, 40, DWQMIParser::RuleReverse);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(156);
    match(DWQMIParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> DWQMIParser::_decisionToDFA;
atn::PredictionContextCache DWQMIParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN DWQMIParser::_atn;
std::vector<uint16_t> DWQMIParser::_serializedATN;

std::vector<std::string> DWQMIParser::_ruleNames = {
  "xaccsrc", "xacckernel", "kernelcall", "typedparam", "mainprog", "program", 
  "line", "statement", "comment", "inst", "annealdecl", "ta", "tp", "tq", 
  "anneal", "id", "real", "string", "direction", "forward", "reverse"
};

std::vector<std::string> DWQMIParser::_literalNames = {
  "", "'__qpu__'", "'('", "'AcceleratorBuffer'", "','", "')'", "'{'", "'}'", 
  "';'", "'forward'", "'reverse'", "", "", "'anneal'", "", "", "'-'"
};

std::vector<std::string> DWQMIParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "TYPE", "COMMENT", "ANNEAL", 
  "ID", "REAL", "MINUS", "INT", "STRING", "EOL", "WS"
};

dfa::Vocabulary DWQMIParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> DWQMIParser::_tokenNames;

DWQMIParser::Initializer::Initializer() {
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
    0x3, 0x16, 0xa1, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 0x9, 
    0x15, 0x4, 0x16, 0x9, 0x16, 0x3, 0x2, 0x7, 0x2, 0x2e, 0xa, 0x2, 0xc, 
    0x2, 0xe, 0x2, 0x31, 0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x3a, 0xa, 0x3, 0xc, 0x3, 0xe, 
    0x3, 0x3d, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x47, 0xa, 0x4, 0x3, 0x4, 0x3, 
    0x4, 0x7, 0x4, 0x4b, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x4e, 0xb, 0x4, 0x3, 
    0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 
    0x7, 0x6, 0x7, 0x58, 0xa, 0x7, 0xd, 0x7, 0xe, 0x7, 0x59, 0x3, 0x8, 0x6, 
    0x8, 0x5d, 0xa, 0x8, 0xd, 0x8, 0xe, 0x8, 0x5e, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x8, 0x3, 0x8, 0x5, 0x8, 0x65, 0xa, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 
    0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 
    0x70, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x5, 0xb, 0x79, 0xa, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 0x80, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 
    0xd, 0x5, 0xd, 0x85, 0xa, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x5, 0xe, 
    0x8a, 0xa, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x5, 0xf, 0x8f, 0xa, 0xf, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x9b, 0xa, 0x14, 0x3, 
    0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x2, 0x2, 0x17, 0x2, 
    0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 
    0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2, 0x2, 0x2, 0x9f, 0x2, 
    0x2f, 0x3, 0x2, 0x2, 0x2, 0x4, 0x32, 0x3, 0x2, 0x2, 0x2, 0x6, 0x43, 
    0x3, 0x2, 0x2, 0x2, 0x8, 0x51, 0x3, 0x2, 0x2, 0x2, 0xa, 0x54, 0x3, 0x2, 
    0x2, 0x2, 0xc, 0x57, 0x3, 0x2, 0x2, 0x2, 0xe, 0x64, 0x3, 0x2, 0x2, 0x2, 
    0x10, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x12, 0x71, 0x3, 0x2, 0x2, 0x2, 0x14, 
    0x73, 0x3, 0x2, 0x2, 0x2, 0x16, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x18, 0x84, 
    0x3, 0x2, 0x2, 0x2, 0x1a, 0x89, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x8e, 0x3, 
    0x2, 0x2, 0x2, 0x1e, 0x90, 0x3, 0x2, 0x2, 0x2, 0x20, 0x92, 0x3, 0x2, 
    0x2, 0x2, 0x22, 0x94, 0x3, 0x2, 0x2, 0x2, 0x24, 0x96, 0x3, 0x2, 0x2, 
    0x2, 0x26, 0x9a, 0x3, 0x2, 0x2, 0x2, 0x28, 0x9c, 0x3, 0x2, 0x2, 0x2, 
    0x2a, 0x9e, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x2e, 0x5, 0x4, 0x3, 0x2, 0x2d, 
    0x2c, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x31, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x2d, 
    0x3, 0x2, 0x2, 0x2, 0x2f, 0x30, 0x3, 0x2, 0x2, 0x2, 0x30, 0x3, 0x3, 
    0x2, 0x2, 0x2, 0x31, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x32, 0x33, 0x7, 0x3, 
    0x2, 0x2, 0x33, 0x34, 0x5, 0x20, 0x11, 0x2, 0x34, 0x35, 0x7, 0x4, 0x2, 
    0x2, 0x35, 0x36, 0x7, 0x5, 0x2, 0x2, 0x36, 0x3b, 0x5, 0x20, 0x11, 0x2, 
    0x37, 0x38, 0x7, 0x6, 0x2, 0x2, 0x38, 0x3a, 0x5, 0x8, 0x5, 0x2, 0x39, 
    0x37, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x39, 
    0x3, 0x2, 0x2, 0x2, 0x3b, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3e, 0x3, 
    0x2, 0x2, 0x2, 0x3d, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3f, 0x7, 0x7, 
    0x2, 0x2, 0x3f, 0x40, 0x7, 0x8, 0x2, 0x2, 0x40, 0x41, 0x5, 0xa, 0x6, 
    0x2, 0x41, 0x42, 0x7, 0x9, 0x2, 0x2, 0x42, 0x5, 0x3, 0x2, 0x2, 0x2, 
    0x43, 0x44, 0x5, 0x20, 0x11, 0x2, 0x44, 0x46, 0x7, 0x4, 0x2, 0x2, 0x45, 
    0x47, 0x5, 0x20, 0x11, 0x2, 0x46, 0x45, 0x3, 0x2, 0x2, 0x2, 0x46, 0x47, 
    0x3, 0x2, 0x2, 0x2, 0x47, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x48, 0x49, 0x7, 
    0x6, 0x2, 0x2, 0x49, 0x4b, 0x5, 0x20, 0x11, 0x2, 0x4a, 0x48, 0x3, 0x2, 
    0x2, 0x2, 0x4b, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4a, 0x3, 0x2, 0x2, 
    0x2, 0x4c, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4f, 0x3, 0x2, 0x2, 0x2, 
    0x4e, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4f, 0x50, 0x7, 0x7, 0x2, 0x2, 0x50, 
    0x7, 0x3, 0x2, 0x2, 0x2, 0x51, 0x52, 0x7, 0xd, 0x2, 0x2, 0x52, 0x53, 
    0x5, 0x20, 0x11, 0x2, 0x53, 0x9, 0x3, 0x2, 0x2, 0x2, 0x54, 0x55, 0x5, 
    0xc, 0x7, 0x2, 0x55, 0xb, 0x3, 0x2, 0x2, 0x2, 0x56, 0x58, 0x5, 0xe, 
    0x8, 0x2, 0x57, 0x56, 0x3, 0x2, 0x2, 0x2, 0x58, 0x59, 0x3, 0x2, 0x2, 
    0x2, 0x59, 0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5a, 0x3, 0x2, 0x2, 0x2, 
    0x5a, 0xd, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5d, 0x5, 0x10, 0x9, 0x2, 0x5c, 
    0x5b, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x5c, 
    0x3, 0x2, 0x2, 0x2, 0x5e, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x60, 0x3, 
    0x2, 0x2, 0x2, 0x60, 0x61, 0x7, 0x15, 0x2, 0x2, 0x61, 0x65, 0x3, 0x2, 
    0x2, 0x2, 0x62, 0x65, 0x5, 0x12, 0xa, 0x2, 0x63, 0x65, 0x7, 0x15, 0x2, 
    0x2, 0x64, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x64, 0x62, 0x3, 0x2, 0x2, 0x2, 
    0x64, 0x63, 0x3, 0x2, 0x2, 0x2, 0x65, 0xf, 0x3, 0x2, 0x2, 0x2, 0x66, 
    0x67, 0x5, 0x16, 0xc, 0x2, 0x67, 0x68, 0x7, 0xa, 0x2, 0x2, 0x68, 0x70, 
    0x3, 0x2, 0x2, 0x2, 0x69, 0x6a, 0x5, 0x14, 0xb, 0x2, 0x6a, 0x6b, 0x7, 
    0xa, 0x2, 0x2, 0x6b, 0x70, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x6d, 0x5, 0x6, 
    0x4, 0x2, 0x6d, 0x6e, 0x7, 0xa, 0x2, 0x2, 0x6e, 0x70, 0x3, 0x2, 0x2, 
    0x2, 0x6f, 0x66, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x69, 0x3, 0x2, 0x2, 0x2, 
    0x6f, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x70, 0x11, 0x3, 0x2, 0x2, 0x2, 0x71, 
    0x72, 0x7, 0xe, 0x2, 0x2, 0x72, 0x13, 0x3, 0x2, 0x2, 0x2, 0x73, 0x74, 
    0x7, 0x13, 0x2, 0x2, 0x74, 0x78, 0x7, 0x13, 0x2, 0x2, 0x75, 0x79, 0x5, 
    0x20, 0x11, 0x2, 0x76, 0x79, 0x5, 0x22, 0x12, 0x2, 0x77, 0x79, 0x7, 
    0x13, 0x2, 0x2, 0x78, 0x75, 0x3, 0x2, 0x2, 0x2, 0x78, 0x76, 0x3, 0x2, 
    0x2, 0x2, 0x78, 0x77, 0x3, 0x2, 0x2, 0x2, 0x79, 0x15, 0x3, 0x2, 0x2, 
    0x2, 0x7a, 0x7b, 0x5, 0x1e, 0x10, 0x2, 0x7b, 0x7c, 0x5, 0x18, 0xd, 0x2, 
    0x7c, 0x7d, 0x5, 0x1a, 0xe, 0x2, 0x7d, 0x7f, 0x5, 0x1c, 0xf, 0x2, 0x7e, 
    0x80, 0x5, 0x26, 0x14, 0x2, 0x7f, 0x7e, 0x3, 0x2, 0x2, 0x2, 0x7f, 0x80, 
    0x3, 0x2, 0x2, 0x2, 0x80, 0x17, 0x3, 0x2, 0x2, 0x2, 0x81, 0x85, 0x5, 
    0x20, 0x11, 0x2, 0x82, 0x85, 0x5, 0x22, 0x12, 0x2, 0x83, 0x85, 0x7, 
    0x13, 0x2, 0x2, 0x84, 0x81, 0x3, 0x2, 0x2, 0x2, 0x84, 0x82, 0x3, 0x2, 
    0x2, 0x2, 0x84, 0x83, 0x3, 0x2, 0x2, 0x2, 0x85, 0x19, 0x3, 0x2, 0x2, 
    0x2, 0x86, 0x8a, 0x5, 0x20, 0x11, 0x2, 0x87, 0x8a, 0x5, 0x22, 0x12, 
    0x2, 0x88, 0x8a, 0x7, 0x13, 0x2, 0x2, 0x89, 0x86, 0x3, 0x2, 0x2, 0x2, 
    0x89, 0x87, 0x3, 0x2, 0x2, 0x2, 0x89, 0x88, 0x3, 0x2, 0x2, 0x2, 0x8a, 
    0x1b, 0x3, 0x2, 0x2, 0x2, 0x8b, 0x8f, 0x5, 0x20, 0x11, 0x2, 0x8c, 0x8f, 
    0x5, 0x22, 0x12, 0x2, 0x8d, 0x8f, 0x7, 0x13, 0x2, 0x2, 0x8e, 0x8b, 0x3, 
    0x2, 0x2, 0x2, 0x8e, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x8d, 0x3, 0x2, 
    0x2, 0x2, 0x8f, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 0x7, 0xf, 0x2, 
    0x2, 0x91, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x92, 0x93, 0x7, 0x10, 0x2, 0x2, 
    0x93, 0x21, 0x3, 0x2, 0x2, 0x2, 0x94, 0x95, 0x7, 0x11, 0x2, 0x2, 0x95, 
    0x23, 0x3, 0x2, 0x2, 0x2, 0x96, 0x97, 0x7, 0x14, 0x2, 0x2, 0x97, 0x25, 
    0x3, 0x2, 0x2, 0x2, 0x98, 0x9b, 0x5, 0x28, 0x15, 0x2, 0x99, 0x9b, 0x5, 
    0x2a, 0x16, 0x2, 0x9a, 0x98, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x99, 0x3, 0x2, 
    0x2, 0x2, 0x9b, 0x27, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x9d, 0x7, 0xb, 0x2, 
    0x2, 0x9d, 0x29, 0x3, 0x2, 0x2, 0x2, 0x9e, 0x9f, 0x7, 0xc, 0x2, 0x2, 
    0x9f, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x10, 0x2f, 0x3b, 0x46, 0x4c, 0x59, 
    0x5e, 0x64, 0x6f, 0x78, 0x7f, 0x84, 0x89, 0x8e, 0x9a, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

DWQMIParser::Initializer DWQMIParser::_init;
