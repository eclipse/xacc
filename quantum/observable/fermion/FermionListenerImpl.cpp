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
#include "FermionListenerImpl.hpp"

namespace xacc {
namespace quantum {
void FermionListenerImpl::enterPlusorminus(
    FermionOperatorParser::PlusorminusContext *ctx) {
  isMinus = ctx->getText() == "-";
}

void FermionListenerImpl::enterTerm(FermionOperatorParser::TermContext *ctx) {

//   std::cout << "ENTER TERM: " << ctx->getText() << ", " << ctx->fermion().size()
//             << "\n";

  std::complex<double> coeff(1.0, 0.0);
  if (ctx->coeff() != nullptr) {
    if (ctx->coeff()->complex() != nullptr) {
      auto complexAsStr = ctx->coeff()->complex()->getText();
      complexAsStr = complexAsStr.substr(1, complexAsStr.length() - 2);
      auto split = xacc::split(complexAsStr, ',');
      coeff = std::complex<double>(std::stod(split[0]), std::stod(split[1]));

    } else if (ctx->coeff()->real() != nullptr) {
      auto realAsStr = ctx->coeff()->real()->getText();
      coeff = std::complex<double>(std::stod(realAsStr), 0.0);
    }
  }

    if (isMinus) {
        coeff *= -1.0;
        isMinus=false;
    }

  Operators term;
  for (int i = 0; i < ctx->fermion().size(); i++) {
    auto str = ctx->fermion(i)->getText();

    bool creation = ctx->fermion(i)->op()->carat() != nullptr;
    //   if (str.find("^") != std::string::npos) {
    //       creation = true;
    //   }

    term.push_back(
        {std::stoi(ctx->fermion(i)->op()->INT()->getText()), creation});

    // std::cout << "HI: " << ctx->fermion(i)->getText() << ", "
    //           << "\n";
  }
//   std::cout << "SIZE OF TERMS: " << term.size() << ", " << coeff << "\n";
  FermionOperator tmp(term, coeff);
//   std::cout << "TMP: " << tmp.toString() << "\n";

  _op += tmp;

//   std::cout << _op.toString() << "\n";

  //   std::cout << "ENTER TERM: " << ctx->getText() << ", " <<
  //   ctx->fermion().size() << "\n";
}

} // namespace quantum
} // namespace xacc