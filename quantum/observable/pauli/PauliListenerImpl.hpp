/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_PAULIOPERATOR_PARSER_H
#define XACC_PAULIOPERATOR_PARSER_H

#include "PauliOperator.hpp"
#include "PauliOperatorParser.h"

#include "PauliOperatorBaseListener.h"
#include "Utils.hpp"

using namespace pauli;

namespace xacc {

namespace quantum {
class PauliOperatorErrorListener : public antlr4::BaseErrorListener {
public:
  void syntaxError(antlr4::Recognizer *recognizer,
                   antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid Pauli Operator source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::XACCLogger::instance()->error(output.str());
  }
};

class PauliListenerImpl : public PauliOperatorBaseListener {

protected:
  std::string entryScopeName = ""; //name of the first TAProL scope to interpret
  PauliOperator _op;
  bool isMinus = false;
public:
  PauliListenerImpl() {}
  void enterPlusorminus(PauliOperatorParser::PlusorminusContext * ctx) override ;
  void enterTerm(PauliOperatorParser::TermContext * ctx) override;
  PauliOperator getOperator() {return _op;}
};
}
}
#endif