
// Generated from FermionOperator.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "FermionOperatorListener.h"


namespace fermion {

/**
 * This class provides an empty implementation of FermionOperatorListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  FermionOperatorBaseListener : public FermionOperatorListener {
public:

  virtual void enterFermionSrc(FermionOperatorParser::FermionSrcContext * /*ctx*/) override { }
  virtual void exitFermionSrc(FermionOperatorParser::FermionSrcContext * /*ctx*/) override { }

  virtual void enterPlusorminus(FermionOperatorParser::PlusorminusContext * /*ctx*/) override { }
  virtual void exitPlusorminus(FermionOperatorParser::PlusorminusContext * /*ctx*/) override { }

  virtual void enterTerm(FermionOperatorParser::TermContext * /*ctx*/) override { }
  virtual void exitTerm(FermionOperatorParser::TermContext * /*ctx*/) override { }

  virtual void enterFermion(FermionOperatorParser::FermionContext * /*ctx*/) override { }
  virtual void exitFermion(FermionOperatorParser::FermionContext * /*ctx*/) override { }

  virtual void enterOp(FermionOperatorParser::OpContext * /*ctx*/) override { }
  virtual void exitOp(FermionOperatorParser::OpContext * /*ctx*/) override { }

  virtual void enterCarat(FermionOperatorParser::CaratContext * /*ctx*/) override { }
  virtual void exitCarat(FermionOperatorParser::CaratContext * /*ctx*/) override { }

  virtual void enterCoeff(FermionOperatorParser::CoeffContext * /*ctx*/) override { }
  virtual void exitCoeff(FermionOperatorParser::CoeffContext * /*ctx*/) override { }

  virtual void enterComplex(FermionOperatorParser::ComplexContext * /*ctx*/) override { }
  virtual void exitComplex(FermionOperatorParser::ComplexContext * /*ctx*/) override { }

  virtual void enterReal(FermionOperatorParser::RealContext * /*ctx*/) override { }
  virtual void exitReal(FermionOperatorParser::RealContext * /*ctx*/) override { }

  virtual void enterComment(FermionOperatorParser::CommentContext * /*ctx*/) override { }
  virtual void exitComment(FermionOperatorParser::CommentContext * /*ctx*/) override { }

  virtual void enterScientific(FermionOperatorParser::ScientificContext * /*ctx*/) override { }
  virtual void exitScientific(FermionOperatorParser::ScientificContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace fermion
