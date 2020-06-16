
// Generated from FermionOperator.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "FermionOperatorParser.h"


namespace fermion {

/**
 * This interface defines an abstract listener for a parse tree produced by FermionOperatorParser.
 */
class  FermionOperatorListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterFermionSrc(FermionOperatorParser::FermionSrcContext *ctx) = 0;
  virtual void exitFermionSrc(FermionOperatorParser::FermionSrcContext *ctx) = 0;

  virtual void enterPlusorminus(FermionOperatorParser::PlusorminusContext *ctx) = 0;
  virtual void exitPlusorminus(FermionOperatorParser::PlusorminusContext *ctx) = 0;

  virtual void enterTerm(FermionOperatorParser::TermContext *ctx) = 0;
  virtual void exitTerm(FermionOperatorParser::TermContext *ctx) = 0;

  virtual void enterFermion(FermionOperatorParser::FermionContext *ctx) = 0;
  virtual void exitFermion(FermionOperatorParser::FermionContext *ctx) = 0;

  virtual void enterOp(FermionOperatorParser::OpContext *ctx) = 0;
  virtual void exitOp(FermionOperatorParser::OpContext *ctx) = 0;

  virtual void enterCarat(FermionOperatorParser::CaratContext *ctx) = 0;
  virtual void exitCarat(FermionOperatorParser::CaratContext *ctx) = 0;

  virtual void enterCoeff(FermionOperatorParser::CoeffContext *ctx) = 0;
  virtual void exitCoeff(FermionOperatorParser::CoeffContext *ctx) = 0;

  virtual void enterComplex(FermionOperatorParser::ComplexContext *ctx) = 0;
  virtual void exitComplex(FermionOperatorParser::ComplexContext *ctx) = 0;

  virtual void enterReal(FermionOperatorParser::RealContext *ctx) = 0;
  virtual void exitReal(FermionOperatorParser::RealContext *ctx) = 0;

  virtual void enterComment(FermionOperatorParser::CommentContext *ctx) = 0;
  virtual void exitComment(FermionOperatorParser::CommentContext *ctx) = 0;

  virtual void enterScientific(FermionOperatorParser::ScientificContext *ctx) = 0;
  virtual void exitScientific(FermionOperatorParser::ScientificContext *ctx) = 0;


};

}  // namespace fermion
