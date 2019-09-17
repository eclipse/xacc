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
#include "FermionOperatorBaseListener.h"
#include "Utils.hpp"
#include "FermionOperator.hpp"

using namespace fermion;

namespace xacc {
namespace quantum {
class FermionOperatorErrorListener : public antlr4::BaseErrorListener {
public:
  void syntaxError(antlr4::Recognizer *recognizer,
                   antlr4::Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid Fermion Operator source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::XACCLogger::instance()->error(output.str());
  }
};

class FermionListenerImpl : public FermionOperatorBaseListener {
protected:
  FermionOperator _op;
  bool isMinus = false;
public:
  void enterPlusorminus(FermionOperatorParser::PlusorminusContext * ctx) override;

  void enterTerm(FermionOperatorParser::TermContext * ctx) override;

  FermionOperator getOperator() {return _op;}

};
}
}