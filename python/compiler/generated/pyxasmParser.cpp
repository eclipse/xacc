
// Generated from pyxasm.g4 by ANTLR 4.7.2


#include "pyxasmListener.h"

#include "pyxasmParser.h"


using namespace antlrcpp;
using namespace pyxasm;
using namespace antlr4;

pyxasmParser::pyxasmParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

pyxasmParser::~pyxasmParser() {
  delete _interpreter;
}

std::string pyxasmParser::getGrammarFileName() const {
  return "pyxasm.g4";
}

const std::vector<std::string>& pyxasmParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& pyxasmParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- XaccsrcContext ------------------------------------------------------------------

pyxasmParser::XaccsrcContext::XaccsrcContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::XacckernelContext* pyxasmParser::XaccsrcContext::xacckernel() {
  return getRuleContext<pyxasmParser::XacckernelContext>(0);
}


size_t pyxasmParser::XaccsrcContext::getRuleIndex() const {
  return pyxasmParser::RuleXaccsrc;
}

void pyxasmParser::XaccsrcContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXaccsrc(this);
}

void pyxasmParser::XaccsrcContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXaccsrc(this);
}

pyxasmParser::XaccsrcContext* pyxasmParser::xaccsrc() {
  XaccsrcContext *_localctx = _tracker.createInstance<XaccsrcContext>(_ctx, getState());
  enterRule(_localctx, 0, pyxasmParser::RuleXaccsrc);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(38);
    xacckernel();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- XacckernelContext ------------------------------------------------------------------

pyxasmParser::XacckernelContext::XacckernelContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::MainprogContext* pyxasmParser::XacckernelContext::mainprog() {
  return getRuleContext<pyxasmParser::MainprogContext>(0);
}

std::vector<pyxasmParser::IdContext *> pyxasmParser::XacckernelContext::id() {
  return getRuleContexts<pyxasmParser::IdContext>();
}

pyxasmParser::IdContext* pyxasmParser::XacckernelContext::id(size_t i) {
  return getRuleContext<pyxasmParser::IdContext>(i);
}

std::vector<pyxasmParser::ParamContext *> pyxasmParser::XacckernelContext::param() {
  return getRuleContexts<pyxasmParser::ParamContext>();
}

pyxasmParser::ParamContext* pyxasmParser::XacckernelContext::param(size_t i) {
  return getRuleContext<pyxasmParser::ParamContext>(i);
}


size_t pyxasmParser::XacckernelContext::getRuleIndex() const {
  return pyxasmParser::RuleXacckernel;
}

void pyxasmParser::XacckernelContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXacckernel(this);
}

void pyxasmParser::XacckernelContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXacckernel(this);
}

pyxasmParser::XacckernelContext* pyxasmParser::xacckernel() {
  XacckernelContext *_localctx = _tracker.createInstance<XacckernelContext>(_ctx, getState());
  enterRule(_localctx, 2, pyxasmParser::RuleXacckernel);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(40);
    match(pyxasmParser::T__0);
    setState(41);
    dynamic_cast<XacckernelContext *>(_localctx)->kernelname = id();
    setState(42);
    match(pyxasmParser::T__1);
    setState(43);
    dynamic_cast<XacckernelContext *>(_localctx)->qbit = id();
    setState(48);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == pyxasmParser::T__2) {
      setState(44);
      match(pyxasmParser::T__2);
      setState(45);
      param();
      setState(50);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(51);
    match(pyxasmParser::T__3);
    setState(52);
    match(pyxasmParser::T__4);
    setState(53);
    mainprog();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext ------------------------------------------------------------------

pyxasmParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::IdContext* pyxasmParser::ParamContext::id() {
  return getRuleContext<pyxasmParser::IdContext>(0);
}


size_t pyxasmParser::ParamContext::getRuleIndex() const {
  return pyxasmParser::RuleParam;
}

void pyxasmParser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void pyxasmParser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}

pyxasmParser::ParamContext* pyxasmParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 4, pyxasmParser::RuleParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(55);
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

pyxasmParser::MainprogContext::MainprogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::ProgramContext* pyxasmParser::MainprogContext::program() {
  return getRuleContext<pyxasmParser::ProgramContext>(0);
}


size_t pyxasmParser::MainprogContext::getRuleIndex() const {
  return pyxasmParser::RuleMainprog;
}

void pyxasmParser::MainprogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMainprog(this);
}

void pyxasmParser::MainprogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMainprog(this);
}

pyxasmParser::MainprogContext* pyxasmParser::mainprog() {
  MainprogContext *_localctx = _tracker.createInstance<MainprogContext>(_ctx, getState());
  enterRule(_localctx, 6, pyxasmParser::RuleMainprog);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(57);
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

pyxasmParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<pyxasmParser::LineContext *> pyxasmParser::ProgramContext::line() {
  return getRuleContexts<pyxasmParser::LineContext>();
}

pyxasmParser::LineContext* pyxasmParser::ProgramContext::line(size_t i) {
  return getRuleContext<pyxasmParser::LineContext>(i);
}


size_t pyxasmParser::ProgramContext::getRuleIndex() const {
  return pyxasmParser::RuleProgram;
}

void pyxasmParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void pyxasmParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}

pyxasmParser::ProgramContext* pyxasmParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 8, pyxasmParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(60); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(59);
      line();
      setState(62); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << pyxasmParser::T__5)
      | (1ULL << pyxasmParser::COMMENT)
      | (1ULL << pyxasmParser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

pyxasmParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<pyxasmParser::StatementContext *> pyxasmParser::LineContext::statement() {
  return getRuleContexts<pyxasmParser::StatementContext>();
}

pyxasmParser::StatementContext* pyxasmParser::LineContext::statement(size_t i) {
  return getRuleContext<pyxasmParser::StatementContext>(i);
}

pyxasmParser::CommentContext* pyxasmParser::LineContext::comment() {
  return getRuleContext<pyxasmParser::CommentContext>(0);
}


size_t pyxasmParser::LineContext::getRuleIndex() const {
  return pyxasmParser::RuleLine;
}

void pyxasmParser::LineContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLine(this);
}

void pyxasmParser::LineContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLine(this);
}

pyxasmParser::LineContext* pyxasmParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 10, pyxasmParser::RuleLine);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(70);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case pyxasmParser::T__5:
      case pyxasmParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(65); 
        _errHandler->sync(this);
        alt = 1;
        do {
          switch (alt) {
            case 1: {
                  setState(64);
                  statement();
                  break;
                }

          default:
            throw NoViableAltException(this);
          }
          setState(67); 
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
        } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
        break;
      }

      case pyxasmParser::COMMENT: {
        enterOuterAlt(_localctx, 2);
        setState(69);
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

pyxasmParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::InstructionContext* pyxasmParser::StatementContext::instruction() {
  return getRuleContext<pyxasmParser::InstructionContext>(0);
}


size_t pyxasmParser::StatementContext::getRuleIndex() const {
  return pyxasmParser::RuleStatement;
}

void pyxasmParser::StatementContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStatement(this);
}

void pyxasmParser::StatementContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStatement(this);
}

pyxasmParser::StatementContext* pyxasmParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 12, pyxasmParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(74);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case pyxasmParser::ID: {
        enterOuterAlt(_localctx, 1);
        setState(72);
        instruction();
        break;
      }

      case pyxasmParser::T__5: {
        enterOuterAlt(_localctx, 2);
        setState(73);
        match(pyxasmParser::T__5);
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

pyxasmParser::CommentContext::CommentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* pyxasmParser::CommentContext::COMMENT() {
  return getToken(pyxasmParser::COMMENT, 0);
}


size_t pyxasmParser::CommentContext::getRuleIndex() const {
  return pyxasmParser::RuleComment;
}

void pyxasmParser::CommentContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterComment(this);
}

void pyxasmParser::CommentContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitComment(this);
}

pyxasmParser::CommentContext* pyxasmParser::comment() {
  CommentContext *_localctx = _tracker.createInstance<CommentContext>(_ctx, getState());
  enterRule(_localctx, 14, pyxasmParser::RuleComment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(76);
    match(pyxasmParser::COMMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InstructionContext ------------------------------------------------------------------

pyxasmParser::InstructionContext::InstructionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::IdContext* pyxasmParser::InstructionContext::id() {
  return getRuleContext<pyxasmParser::IdContext>(0);
}

pyxasmParser::ExplistContext* pyxasmParser::InstructionContext::explist() {
  return getRuleContext<pyxasmParser::ExplistContext>(0);
}

pyxasmParser::OptionsMapContext* pyxasmParser::InstructionContext::optionsMap() {
  return getRuleContext<pyxasmParser::OptionsMapContext>(0);
}


size_t pyxasmParser::InstructionContext::getRuleIndex() const {
  return pyxasmParser::RuleInstruction;
}

void pyxasmParser::InstructionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInstruction(this);
}

void pyxasmParser::InstructionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInstruction(this);
}

pyxasmParser::InstructionContext* pyxasmParser::instruction() {
  InstructionContext *_localctx = _tracker.createInstance<InstructionContext>(_ctx, getState());
  enterRule(_localctx, 16, pyxasmParser::RuleInstruction);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    dynamic_cast<InstructionContext *>(_localctx)->inst_name = id();
    setState(79);
    match(pyxasmParser::T__1);

    setState(80);
    dynamic_cast<InstructionContext *>(_localctx)->bits_and_params = explist();
    setState(83);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == pyxasmParser::T__2) {
      setState(81);
      match(pyxasmParser::T__2);
      setState(82);
      dynamic_cast<InstructionContext *>(_localctx)->options = optionsMap();
    }
    setState(85);
    match(pyxasmParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BufferIndexContext ------------------------------------------------------------------

pyxasmParser::BufferIndexContext::BufferIndexContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::IdContext* pyxasmParser::BufferIndexContext::id() {
  return getRuleContext<pyxasmParser::IdContext>(0);
}

tree::TerminalNode* pyxasmParser::BufferIndexContext::INT() {
  return getToken(pyxasmParser::INT, 0);
}


size_t pyxasmParser::BufferIndexContext::getRuleIndex() const {
  return pyxasmParser::RuleBufferIndex;
}

void pyxasmParser::BufferIndexContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBufferIndex(this);
}

void pyxasmParser::BufferIndexContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBufferIndex(this);
}

pyxasmParser::BufferIndexContext* pyxasmParser::bufferIndex() {
  BufferIndexContext *_localctx = _tracker.createInstance<BufferIndexContext>(_ctx, getState());
  enterRule(_localctx, 18, pyxasmParser::RuleBufferIndex);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(87);
    id();

    setState(88);
    match(pyxasmParser::T__6);
    setState(89);
    match(pyxasmParser::INT);
    setState(90);
    match(pyxasmParser::T__7);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BitsOrParamTypeContext ------------------------------------------------------------------

pyxasmParser::BitsOrParamTypeContext::BitsOrParamTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<pyxasmParser::BufferIndexContext *> pyxasmParser::BitsOrParamTypeContext::bufferIndex() {
  return getRuleContexts<pyxasmParser::BufferIndexContext>();
}

pyxasmParser::BufferIndexContext* pyxasmParser::BitsOrParamTypeContext::bufferIndex(size_t i) {
  return getRuleContext<pyxasmParser::BufferIndexContext>(i);
}

std::vector<pyxasmParser::ExpContext *> pyxasmParser::BitsOrParamTypeContext::exp() {
  return getRuleContexts<pyxasmParser::ExpContext>();
}

pyxasmParser::ExpContext* pyxasmParser::BitsOrParamTypeContext::exp(size_t i) {
  return getRuleContext<pyxasmParser::ExpContext>(i);
}


size_t pyxasmParser::BitsOrParamTypeContext::getRuleIndex() const {
  return pyxasmParser::RuleBitsOrParamType;
}

void pyxasmParser::BitsOrParamTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBitsOrParamType(this);
}

void pyxasmParser::BitsOrParamTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBitsOrParamType(this);
}

pyxasmParser::BitsOrParamTypeContext* pyxasmParser::bitsOrParamType() {
  BitsOrParamTypeContext *_localctx = _tracker.createInstance<BitsOrParamTypeContext>(_ctx, getState());
  enterRule(_localctx, 20, pyxasmParser::RuleBitsOrParamType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(106);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(92);
      bufferIndex();
      setState(97);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == pyxasmParser::T__2) {
        setState(93);
        match(pyxasmParser::T__2);
        setState(94);
        bufferIndex();
        setState(99);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(103);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << pyxasmParser::T__1)
        | (1ULL << pyxasmParser::T__11)
        | (1ULL << pyxasmParser::T__15)
        | (1ULL << pyxasmParser::T__16)
        | (1ULL << pyxasmParser::T__17)
        | (1ULL << pyxasmParser::T__18)
        | (1ULL << pyxasmParser::T__19)
        | (1ULL << pyxasmParser::T__20)
        | (1ULL << pyxasmParser::T__21)
        | (1ULL << pyxasmParser::ID)
        | (1ULL << pyxasmParser::REAL)
        | (1ULL << pyxasmParser::INT)
        | (1ULL << pyxasmParser::STRING))) != 0)) {
        setState(100);
        exp(0);
        setState(105);
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

pyxasmParser::OptionsMapContext::OptionsMapContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<pyxasmParser::OptionsTypeContext *> pyxasmParser::OptionsMapContext::optionsType() {
  return getRuleContexts<pyxasmParser::OptionsTypeContext>();
}

pyxasmParser::OptionsTypeContext* pyxasmParser::OptionsMapContext::optionsType(size_t i) {
  return getRuleContext<pyxasmParser::OptionsTypeContext>(i);
}


size_t pyxasmParser::OptionsMapContext::getRuleIndex() const {
  return pyxasmParser::RuleOptionsMap;
}

void pyxasmParser::OptionsMapContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOptionsMap(this);
}

void pyxasmParser::OptionsMapContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOptionsMap(this);
}

pyxasmParser::OptionsMapContext* pyxasmParser::optionsMap() {
  OptionsMapContext *_localctx = _tracker.createInstance<OptionsMapContext>(_ctx, getState());
  enterRule(_localctx, 22, pyxasmParser::RuleOptionsMap);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(108);
    match(pyxasmParser::T__8);
    setState(109);
    optionsType();
    setState(114);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == pyxasmParser::T__2) {
      setState(110);
      match(pyxasmParser::T__2);
      setState(111);
      optionsType();
      setState(116);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(117);
    match(pyxasmParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OptionsTypeContext ------------------------------------------------------------------

pyxasmParser::OptionsTypeContext::OptionsTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::StringContext* pyxasmParser::OptionsTypeContext::string() {
  return getRuleContext<pyxasmParser::StringContext>(0);
}

pyxasmParser::ExpContext* pyxasmParser::OptionsTypeContext::exp() {
  return getRuleContext<pyxasmParser::ExpContext>(0);
}


size_t pyxasmParser::OptionsTypeContext::getRuleIndex() const {
  return pyxasmParser::RuleOptionsType;
}

void pyxasmParser::OptionsTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOptionsType(this);
}

void pyxasmParser::OptionsTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOptionsType(this);
}

pyxasmParser::OptionsTypeContext* pyxasmParser::optionsType() {
  OptionsTypeContext *_localctx = _tracker.createInstance<OptionsTypeContext>(_ctx, getState());
  enterRule(_localctx, 24, pyxasmParser::RuleOptionsType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(119);
    dynamic_cast<OptionsTypeContext *>(_localctx)->key = string();
    setState(120);
    match(pyxasmParser::T__4);
    setState(121);
    dynamic_cast<OptionsTypeContext *>(_localctx)->value = exp(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExplistContext ------------------------------------------------------------------

pyxasmParser::ExplistContext::ExplistContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<pyxasmParser::ExpContext *> pyxasmParser::ExplistContext::exp() {
  return getRuleContexts<pyxasmParser::ExpContext>();
}

pyxasmParser::ExpContext* pyxasmParser::ExplistContext::exp(size_t i) {
  return getRuleContext<pyxasmParser::ExpContext>(i);
}


size_t pyxasmParser::ExplistContext::getRuleIndex() const {
  return pyxasmParser::RuleExplist;
}

void pyxasmParser::ExplistContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExplist(this);
}

void pyxasmParser::ExplistContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExplist(this);
}

pyxasmParser::ExplistContext* pyxasmParser::explist() {
  ExplistContext *_localctx = _tracker.createInstance<ExplistContext>(_ctx, getState());
  enterRule(_localctx, 26, pyxasmParser::RuleExplist);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(123);
    exp(0);
    setState(128);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(124);
        match(pyxasmParser::T__2);
        setState(125);
        exp(0); 
      }
      setState(130);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx);
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

pyxasmParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

pyxasmParser::IdContext* pyxasmParser::ExpContext::id() {
  return getRuleContext<pyxasmParser::IdContext>(0);
}

std::vector<pyxasmParser::ExpContext *> pyxasmParser::ExpContext::exp() {
  return getRuleContexts<pyxasmParser::ExpContext>();
}

pyxasmParser::ExpContext* pyxasmParser::ExpContext::exp(size_t i) {
  return getRuleContext<pyxasmParser::ExpContext>(i);
}

pyxasmParser::UnaryopContext* pyxasmParser::ExpContext::unaryop() {
  return getRuleContext<pyxasmParser::UnaryopContext>(0);
}

pyxasmParser::StringContext* pyxasmParser::ExpContext::string() {
  return getRuleContext<pyxasmParser::StringContext>(0);
}

pyxasmParser::RealContext* pyxasmParser::ExpContext::real() {
  return getRuleContext<pyxasmParser::RealContext>(0);
}

tree::TerminalNode* pyxasmParser::ExpContext::INT() {
  return getToken(pyxasmParser::INT, 0);
}

pyxasmParser::BufferIndexContext* pyxasmParser::ExpContext::bufferIndex() {
  return getRuleContext<pyxasmParser::BufferIndexContext>(0);
}


size_t pyxasmParser::ExpContext::getRuleIndex() const {
  return pyxasmParser::RuleExp;
}

void pyxasmParser::ExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExp(this);
}

void pyxasmParser::ExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExp(this);
}


pyxasmParser::ExpContext* pyxasmParser::exp() {
   return exp(0);
}

pyxasmParser::ExpContext* pyxasmParser::exp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  pyxasmParser::ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, parentState);
  pyxasmParser::ExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 28;
  enterRecursionRule(_localctx, 28, pyxasmParser::RuleExp, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(149);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(132);
      id();
      break;
    }

    case 2: {
      setState(133);
      match(pyxasmParser::T__11);
      setState(134);
      exp(9);
      break;
    }

    case 3: {
      setState(135);
      match(pyxasmParser::T__1);
      setState(136);
      exp(0);
      setState(137);
      match(pyxasmParser::T__3);
      break;
    }

    case 4: {
      setState(139);
      unaryop();
      setState(140);
      match(pyxasmParser::T__1);
      setState(141);
      exp(0);
      setState(142);
      match(pyxasmParser::T__3);
      break;
    }

    case 5: {
      setState(144);
      string();
      break;
    }

    case 6: {
      setState(145);
      real();
      break;
    }

    case 7: {
      setState(146);
      match(pyxasmParser::INT);
      break;
    }

    case 8: {
      setState(147);
      match(pyxasmParser::T__15);
      break;
    }

    case 9: {
      setState(148);
      bufferIndex();
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(168);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(166);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(151);

          if (!(precpred(_ctx, 13))) throw FailedPredicateException(this, "precpred(_ctx, 13)");
          setState(152);
          match(pyxasmParser::T__10);
          setState(153);
          exp(14);
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(154);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(155);
          match(pyxasmParser::T__11);
          setState(156);
          exp(13);
          break;
        }

        case 3: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(157);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(158);
          match(pyxasmParser::T__12);
          setState(159);
          exp(12);
          break;
        }

        case 4: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(160);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(161);
          match(pyxasmParser::T__13);
          setState(162);
          exp(11);
          break;
        }

        case 5: {
          _localctx = _tracker.createInstance<ExpContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleExp);
          setState(163);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(164);
          match(pyxasmParser::T__14);
          setState(165);
          exp(9);
          break;
        }

        } 
      }
      setState(170);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
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

pyxasmParser::UnaryopContext::UnaryopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t pyxasmParser::UnaryopContext::getRuleIndex() const {
  return pyxasmParser::RuleUnaryop;
}

void pyxasmParser::UnaryopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryop(this);
}

void pyxasmParser::UnaryopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryop(this);
}

pyxasmParser::UnaryopContext* pyxasmParser::unaryop() {
  UnaryopContext *_localctx = _tracker.createInstance<UnaryopContext>(_ctx, getState());
  enterRule(_localctx, 30, pyxasmParser::RuleUnaryop);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(171);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << pyxasmParser::T__16)
      | (1ULL << pyxasmParser::T__17)
      | (1ULL << pyxasmParser::T__18)
      | (1ULL << pyxasmParser::T__19)
      | (1ULL << pyxasmParser::T__20)
      | (1ULL << pyxasmParser::T__21))) != 0))) {
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

pyxasmParser::IdContext::IdContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* pyxasmParser::IdContext::ID() {
  return getToken(pyxasmParser::ID, 0);
}


size_t pyxasmParser::IdContext::getRuleIndex() const {
  return pyxasmParser::RuleId;
}

void pyxasmParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}

void pyxasmParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}

pyxasmParser::IdContext* pyxasmParser::id() {
  IdContext *_localctx = _tracker.createInstance<IdContext>(_ctx, getState());
  enterRule(_localctx, 32, pyxasmParser::RuleId);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(173);
    match(pyxasmParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealContext ------------------------------------------------------------------

pyxasmParser::RealContext::RealContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* pyxasmParser::RealContext::REAL() {
  return getToken(pyxasmParser::REAL, 0);
}


size_t pyxasmParser::RealContext::getRuleIndex() const {
  return pyxasmParser::RuleReal;
}

void pyxasmParser::RealContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterReal(this);
}

void pyxasmParser::RealContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitReal(this);
}

pyxasmParser::RealContext* pyxasmParser::real() {
  RealContext *_localctx = _tracker.createInstance<RealContext>(_ctx, getState());
  enterRule(_localctx, 34, pyxasmParser::RuleReal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(175);
    match(pyxasmParser::REAL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StringContext ------------------------------------------------------------------

pyxasmParser::StringContext::StringContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* pyxasmParser::StringContext::STRING() {
  return getToken(pyxasmParser::STRING, 0);
}


size_t pyxasmParser::StringContext::getRuleIndex() const {
  return pyxasmParser::RuleString;
}

void pyxasmParser::StringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterString(this);
}

void pyxasmParser::StringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<pyxasmListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitString(this);
}

pyxasmParser::StringContext* pyxasmParser::string() {
  StringContext *_localctx = _tracker.createInstance<StringContext>(_ctx, getState());
  enterRule(_localctx, 36, pyxasmParser::RuleString);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(177);
    match(pyxasmParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool pyxasmParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 14: return expSempred(dynamic_cast<ExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool pyxasmParser::expSempred(ExpContext *_localctx, size_t predicateIndex) {
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
std::vector<dfa::DFA> pyxasmParser::_decisionToDFA;
atn::PredictionContextCache pyxasmParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN pyxasmParser::_atn;
std::vector<uint16_t> pyxasmParser::_serializedATN;

std::vector<std::string> pyxasmParser::_ruleNames = {
  "xaccsrc", "xacckernel", "param", "mainprog", "program", "line", "statement", 
  "comment", "instruction", "bufferIndex", "bitsOrParamType", "optionsMap", 
  "optionsType", "explist", "exp", "unaryop", "id", "real", "string"
};

std::vector<std::string> pyxasmParser::_literalNames = {
  "", "'def'", "'('", "','", "')'", "':'", "'return'", "'['", "']'", "'{'", 
  "'}'", "'+'", "'-'", "'*'", "'/'", "'^'", "'pi'", "'sin'", "'cos'", "'tan'", 
  "'exp'", "'ln'", "'sqrt'"
};

std::vector<std::string> pyxasmParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "COMMENT", "ID", "REAL", "INT", "STRING", "WS", "EOL"
};

dfa::Vocabulary pyxasmParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> pyxasmParser::_tokenNames;

pyxasmParser::Initializer::Initializer() {
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
    0x3, 0x1f, 0xb6, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x3, 0x2, 0x3, 
    0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 
    0x3, 0x31, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x34, 0xb, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x6, 0x6, 0x6, 0x3f, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x40, 0x3, 0x7, 0x6, 
    0x7, 0x44, 0xa, 0x7, 0xd, 0x7, 0xe, 0x7, 0x45, 0x3, 0x7, 0x5, 0x7, 0x49, 
    0xa, 0x7, 0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x4d, 0xa, 0x8, 0x3, 0x9, 0x3, 
    0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 0x56, 
    0xa, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x7, 0xc, 0x62, 0xa, 0xc, 0xc, 
    0xc, 0xe, 0xc, 0x65, 0xb, 0xc, 0x3, 0xc, 0x7, 0xc, 0x68, 0xa, 0xc, 0xc, 
    0xc, 0xe, 0xc, 0x6b, 0xb, 0xc, 0x5, 0xc, 0x6d, 0xa, 0xc, 0x3, 0xd, 0x3, 
    0xd, 0x3, 0xd, 0x3, 0xd, 0x7, 0xd, 0x73, 0xa, 0xd, 0xc, 0xd, 0xe, 0xd, 
    0x76, 0xb, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 
    0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x7, 0xf, 0x81, 0xa, 0xf, 0xc, 0xf, 
    0xe, 0xf, 0x84, 0xb, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 
    0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 
    0x3, 0x10, 0x5, 0x10, 0x98, 0xa, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 
    0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 
    0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 
    0xa9, 0xa, 0x10, 0xc, 0x10, 0xe, 0x10, 0xac, 0xb, 0x10, 0x3, 0x11, 0x3, 
    0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 
    0x3, 0x14, 0x2, 0x3, 0x1e, 0x15, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 
    0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 
    0x2, 0x3, 0x3, 0x2, 0x13, 0x18, 0x2, 0xba, 0x2, 0x28, 0x3, 0x2, 0x2, 
    0x2, 0x4, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x6, 0x39, 0x3, 0x2, 0x2, 0x2, 0x8, 
    0x3b, 0x3, 0x2, 0x2, 0x2, 0xa, 0x3e, 0x3, 0x2, 0x2, 0x2, 0xc, 0x48, 
    0x3, 0x2, 0x2, 0x2, 0xe, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x10, 0x4e, 0x3, 
    0x2, 0x2, 0x2, 0x12, 0x50, 0x3, 0x2, 0x2, 0x2, 0x14, 0x59, 0x3, 0x2, 
    0x2, 0x2, 0x16, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x18, 0x6e, 0x3, 0x2, 0x2, 
    0x2, 0x1a, 0x79, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x7d, 0x3, 0x2, 0x2, 0x2, 
    0x1e, 0x97, 0x3, 0x2, 0x2, 0x2, 0x20, 0xad, 0x3, 0x2, 0x2, 0x2, 0x22, 
    0xaf, 0x3, 0x2, 0x2, 0x2, 0x24, 0xb1, 0x3, 0x2, 0x2, 0x2, 0x26, 0xb3, 
    0x3, 0x2, 0x2, 0x2, 0x28, 0x29, 0x5, 0x4, 0x3, 0x2, 0x29, 0x3, 0x3, 
    0x2, 0x2, 0x2, 0x2a, 0x2b, 0x7, 0x3, 0x2, 0x2, 0x2b, 0x2c, 0x5, 0x22, 
    0x12, 0x2, 0x2c, 0x2d, 0x7, 0x4, 0x2, 0x2, 0x2d, 0x32, 0x5, 0x22, 0x12, 
    0x2, 0x2e, 0x2f, 0x7, 0x5, 0x2, 0x2, 0x2f, 0x31, 0x5, 0x6, 0x4, 0x2, 
    0x30, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x31, 0x34, 0x3, 0x2, 0x2, 0x2, 0x32, 
    0x30, 0x3, 0x2, 0x2, 0x2, 0x32, 0x33, 0x3, 0x2, 0x2, 0x2, 0x33, 0x35, 
    0x3, 0x2, 0x2, 0x2, 0x34, 0x32, 0x3, 0x2, 0x2, 0x2, 0x35, 0x36, 0x7, 
    0x6, 0x2, 0x2, 0x36, 0x37, 0x7, 0x7, 0x2, 0x2, 0x37, 0x38, 0x5, 0x8, 
    0x5, 0x2, 0x38, 0x5, 0x3, 0x2, 0x2, 0x2, 0x39, 0x3a, 0x5, 0x22, 0x12, 
    0x2, 0x3a, 0x7, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x3c, 0x5, 0xa, 0x6, 0x2, 
    0x3c, 0x9, 0x3, 0x2, 0x2, 0x2, 0x3d, 0x3f, 0x5, 0xc, 0x7, 0x2, 0x3e, 
    0x3d, 0x3, 0x2, 0x2, 0x2, 0x3f, 0x40, 0x3, 0x2, 0x2, 0x2, 0x40, 0x3e, 
    0x3, 0x2, 0x2, 0x2, 0x40, 0x41, 0x3, 0x2, 0x2, 0x2, 0x41, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x42, 0x44, 0x5, 0xe, 0x8, 0x2, 0x43, 0x42, 0x3, 0x2, 
    0x2, 0x2, 0x44, 0x45, 0x3, 0x2, 0x2, 0x2, 0x45, 0x43, 0x3, 0x2, 0x2, 
    0x2, 0x45, 0x46, 0x3, 0x2, 0x2, 0x2, 0x46, 0x49, 0x3, 0x2, 0x2, 0x2, 
    0x47, 0x49, 0x5, 0x10, 0x9, 0x2, 0x48, 0x43, 0x3, 0x2, 0x2, 0x2, 0x48, 
    0x47, 0x3, 0x2, 0x2, 0x2, 0x49, 0xd, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x4d, 
    0x5, 0x12, 0xa, 0x2, 0x4b, 0x4d, 0x7, 0x8, 0x2, 0x2, 0x4c, 0x4a, 0x3, 
    0x2, 0x2, 0x2, 0x4c, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x4d, 0xf, 0x3, 0x2, 
    0x2, 0x2, 0x4e, 0x4f, 0x7, 0x19, 0x2, 0x2, 0x4f, 0x11, 0x3, 0x2, 0x2, 
    0x2, 0x50, 0x51, 0x5, 0x22, 0x12, 0x2, 0x51, 0x52, 0x7, 0x4, 0x2, 0x2, 
    0x52, 0x55, 0x5, 0x1c, 0xf, 0x2, 0x53, 0x54, 0x7, 0x5, 0x2, 0x2, 0x54, 
    0x56, 0x5, 0x18, 0xd, 0x2, 0x55, 0x53, 0x3, 0x2, 0x2, 0x2, 0x55, 0x56, 
    0x3, 0x2, 0x2, 0x2, 0x56, 0x57, 0x3, 0x2, 0x2, 0x2, 0x57, 0x58, 0x7, 
    0x6, 0x2, 0x2, 0x58, 0x13, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5a, 0x5, 0x22, 
    0x12, 0x2, 0x5a, 0x5b, 0x7, 0x9, 0x2, 0x2, 0x5b, 0x5c, 0x7, 0x1c, 0x2, 
    0x2, 0x5c, 0x5d, 0x7, 0xa, 0x2, 0x2, 0x5d, 0x15, 0x3, 0x2, 0x2, 0x2, 
    0x5e, 0x63, 0x5, 0x14, 0xb, 0x2, 0x5f, 0x60, 0x7, 0x5, 0x2, 0x2, 0x60, 
    0x62, 0x5, 0x14, 0xb, 0x2, 0x61, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x62, 0x65, 
    0x3, 0x2, 0x2, 0x2, 0x63, 0x61, 0x3, 0x2, 0x2, 0x2, 0x63, 0x64, 0x3, 
    0x2, 0x2, 0x2, 0x64, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x65, 0x63, 0x3, 0x2, 
    0x2, 0x2, 0x66, 0x68, 0x5, 0x1e, 0x10, 0x2, 0x67, 0x66, 0x3, 0x2, 0x2, 
    0x2, 0x68, 0x6b, 0x3, 0x2, 0x2, 0x2, 0x69, 0x67, 0x3, 0x2, 0x2, 0x2, 
    0x69, 0x6a, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x6b, 
    0x69, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x69, 
    0x3, 0x2, 0x2, 0x2, 0x6d, 0x17, 0x3, 0x2, 0x2, 0x2, 0x6e, 0x6f, 0x7, 
    0xb, 0x2, 0x2, 0x6f, 0x74, 0x5, 0x1a, 0xe, 0x2, 0x70, 0x71, 0x7, 0x5, 
    0x2, 0x2, 0x71, 0x73, 0x5, 0x1a, 0xe, 0x2, 0x72, 0x70, 0x3, 0x2, 0x2, 
    0x2, 0x73, 0x76, 0x3, 0x2, 0x2, 0x2, 0x74, 0x72, 0x3, 0x2, 0x2, 0x2, 
    0x74, 0x75, 0x3, 0x2, 0x2, 0x2, 0x75, 0x77, 0x3, 0x2, 0x2, 0x2, 0x76, 
    0x74, 0x3, 0x2, 0x2, 0x2, 0x77, 0x78, 0x7, 0xc, 0x2, 0x2, 0x78, 0x19, 
    0x3, 0x2, 0x2, 0x2, 0x79, 0x7a, 0x5, 0x26, 0x14, 0x2, 0x7a, 0x7b, 0x7, 
    0x7, 0x2, 0x2, 0x7b, 0x7c, 0x5, 0x1e, 0x10, 0x2, 0x7c, 0x1b, 0x3, 0x2, 
    0x2, 0x2, 0x7d, 0x82, 0x5, 0x1e, 0x10, 0x2, 0x7e, 0x7f, 0x7, 0x5, 0x2, 
    0x2, 0x7f, 0x81, 0x5, 0x1e, 0x10, 0x2, 0x80, 0x7e, 0x3, 0x2, 0x2, 0x2, 
    0x81, 0x84, 0x3, 0x2, 0x2, 0x2, 0x82, 0x80, 0x3, 0x2, 0x2, 0x2, 0x82, 
    0x83, 0x3, 0x2, 0x2, 0x2, 0x83, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x84, 0x82, 
    0x3, 0x2, 0x2, 0x2, 0x85, 0x86, 0x8, 0x10, 0x1, 0x2, 0x86, 0x98, 0x5, 
    0x22, 0x12, 0x2, 0x87, 0x88, 0x7, 0xe, 0x2, 0x2, 0x88, 0x98, 0x5, 0x1e, 
    0x10, 0xb, 0x89, 0x8a, 0x7, 0x4, 0x2, 0x2, 0x8a, 0x8b, 0x5, 0x1e, 0x10, 
    0x2, 0x8b, 0x8c, 0x7, 0x6, 0x2, 0x2, 0x8c, 0x98, 0x3, 0x2, 0x2, 0x2, 
    0x8d, 0x8e, 0x5, 0x20, 0x11, 0x2, 0x8e, 0x8f, 0x7, 0x4, 0x2, 0x2, 0x8f, 
    0x90, 0x5, 0x1e, 0x10, 0x2, 0x90, 0x91, 0x7, 0x6, 0x2, 0x2, 0x91, 0x98, 
    0x3, 0x2, 0x2, 0x2, 0x92, 0x98, 0x5, 0x26, 0x14, 0x2, 0x93, 0x98, 0x5, 
    0x24, 0x13, 0x2, 0x94, 0x98, 0x7, 0x1c, 0x2, 0x2, 0x95, 0x98, 0x7, 0x12, 
    0x2, 0x2, 0x96, 0x98, 0x5, 0x14, 0xb, 0x2, 0x97, 0x85, 0x3, 0x2, 0x2, 
    0x2, 0x97, 0x87, 0x3, 0x2, 0x2, 0x2, 0x97, 0x89, 0x3, 0x2, 0x2, 0x2, 
    0x97, 0x8d, 0x3, 0x2, 0x2, 0x2, 0x97, 0x92, 0x3, 0x2, 0x2, 0x2, 0x97, 
    0x93, 0x3, 0x2, 0x2, 0x2, 0x97, 0x94, 0x3, 0x2, 0x2, 0x2, 0x97, 0x95, 
    0x3, 0x2, 0x2, 0x2, 0x97, 0x96, 0x3, 0x2, 0x2, 0x2, 0x98, 0xaa, 0x3, 
    0x2, 0x2, 0x2, 0x99, 0x9a, 0xc, 0xf, 0x2, 0x2, 0x9a, 0x9b, 0x7, 0xd, 
    0x2, 0x2, 0x9b, 0xa9, 0x5, 0x1e, 0x10, 0x10, 0x9c, 0x9d, 0xc, 0xe, 0x2, 
    0x2, 0x9d, 0x9e, 0x7, 0xe, 0x2, 0x2, 0x9e, 0xa9, 0x5, 0x1e, 0x10, 0xf, 
    0x9f, 0xa0, 0xc, 0xd, 0x2, 0x2, 0xa0, 0xa1, 0x7, 0xf, 0x2, 0x2, 0xa1, 
    0xa9, 0x5, 0x1e, 0x10, 0xe, 0xa2, 0xa3, 0xc, 0xc, 0x2, 0x2, 0xa3, 0xa4, 
    0x7, 0x10, 0x2, 0x2, 0xa4, 0xa9, 0x5, 0x1e, 0x10, 0xd, 0xa5, 0xa6, 0xc, 
    0xa, 0x2, 0x2, 0xa6, 0xa7, 0x7, 0x11, 0x2, 0x2, 0xa7, 0xa9, 0x5, 0x1e, 
    0x10, 0xb, 0xa8, 0x99, 0x3, 0x2, 0x2, 0x2, 0xa8, 0x9c, 0x3, 0x2, 0x2, 
    0x2, 0xa8, 0x9f, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xa2, 0x3, 0x2, 0x2, 0x2, 
    0xa8, 0xa5, 0x3, 0x2, 0x2, 0x2, 0xa9, 0xac, 0x3, 0x2, 0x2, 0x2, 0xaa, 
    0xa8, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xab, 0x3, 0x2, 0x2, 0x2, 0xab, 0x1f, 
    0x3, 0x2, 0x2, 0x2, 0xac, 0xaa, 0x3, 0x2, 0x2, 0x2, 0xad, 0xae, 0x9, 
    0x2, 0x2, 0x2, 0xae, 0x21, 0x3, 0x2, 0x2, 0x2, 0xaf, 0xb0, 0x7, 0x1a, 
    0x2, 0x2, 0xb0, 0x23, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xb2, 0x7, 0x1b, 0x2, 
    0x2, 0xb2, 0x25, 0x3, 0x2, 0x2, 0x2, 0xb3, 0xb4, 0x7, 0x1d, 0x2, 0x2, 
    0xb4, 0x27, 0x3, 0x2, 0x2, 0x2, 0x10, 0x32, 0x40, 0x45, 0x48, 0x4c, 
    0x55, 0x63, 0x69, 0x6c, 0x74, 0x82, 0x97, 0xa8, 0xaa, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

pyxasmParser::Initializer pyxasmParser::_init;
