
// Generated from PauliOperator.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "PauliOperatorParser.h"


namespace pauli {

/**
 * This interface defines an abstract listener for a parse tree produced by PauliOperatorParser.
 */
class  PauliOperatorListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterPauliSrc(PauliOperatorParser::PauliSrcContext *ctx) = 0;
  virtual void exitPauliSrc(PauliOperatorParser::PauliSrcContext *ctx) = 0;

  virtual void enterPlusorminus(PauliOperatorParser::PlusorminusContext *ctx) = 0;
  virtual void exitPlusorminus(PauliOperatorParser::PlusorminusContext *ctx) = 0;

  virtual void enterTerm(PauliOperatorParser::TermContext *ctx) = 0;
  virtual void exitTerm(PauliOperatorParser::TermContext *ctx) = 0;

  virtual void enterPauli(PauliOperatorParser::PauliContext *ctx) = 0;
  virtual void exitPauli(PauliOperatorParser::PauliContext *ctx) = 0;

  virtual void enterCoeff(PauliOperatorParser::CoeffContext *ctx) = 0;
  virtual void exitCoeff(PauliOperatorParser::CoeffContext *ctx) = 0;

  virtual void enterComplex(PauliOperatorParser::ComplexContext *ctx) = 0;
  virtual void exitComplex(PauliOperatorParser::ComplexContext *ctx) = 0;

  virtual void enterReal(PauliOperatorParser::RealContext *ctx) = 0;
  virtual void exitReal(PauliOperatorParser::RealContext *ctx) = 0;

  virtual void enterComment(PauliOperatorParser::CommentContext *ctx) = 0;
  virtual void exitComment(PauliOperatorParser::CommentContext *ctx) = 0;

  virtual void enterScientific(PauliOperatorParser::ScientificContext *ctx) = 0;
  virtual void exitScientific(PauliOperatorParser::ScientificContext *ctx) = 0;


};

}  // namespace pauli
