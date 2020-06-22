
// Generated from PauliOperator.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "PauliOperatorListener.h"


namespace pauli {

/**
 * This class provides an empty implementation of PauliOperatorListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  PauliOperatorBaseListener : public PauliOperatorListener {
public:

  virtual void enterPauliSrc(PauliOperatorParser::PauliSrcContext * /*ctx*/) override { }
  virtual void exitPauliSrc(PauliOperatorParser::PauliSrcContext * /*ctx*/) override { }

  virtual void enterPlusorminus(PauliOperatorParser::PlusorminusContext * /*ctx*/) override { }
  virtual void exitPlusorminus(PauliOperatorParser::PlusorminusContext * /*ctx*/) override { }

  virtual void enterTerm(PauliOperatorParser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(PauliOperatorParser::TermContext * /*ctx*/) override { }

  virtual void enterPauli(PauliOperatorParser::PauliContext * /*ctx*/) override { }
  virtual void exitPauli(PauliOperatorParser::PauliContext * /*ctx*/) override { }

  virtual void enterCoeff(PauliOperatorParser::CoeffContext * /*ctx*/) override { }
  virtual void exitCoeff(PauliOperatorParser::CoeffContext * /*ctx*/) override { }

  virtual void enterComplex(PauliOperatorParser::ComplexContext * /*ctx*/) override { }
  virtual void exitComplex(PauliOperatorParser::ComplexContext * /*ctx*/) override { }

  virtual void enterReal(PauliOperatorParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(PauliOperatorParser::RealContext * /*ctx*/) override { }

  virtual void enterComment(PauliOperatorParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(PauliOperatorParser::CommentContext * /*ctx*/) override { }

  virtual void enterScientific(PauliOperatorParser::ScientificContext * /*ctx*/) override { }
  virtual void exitScientific(PauliOperatorParser::ScientificContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace pauli
