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
#include "PauliListenerImpl.hpp"

namespace xacc {
namespace quantum {
void PauliListenerImpl::enterPlusorminus(PauliOperatorParser::PlusorminusContext * ctx) {
    isMinus = ctx->getText() == "-";
}

void PauliListenerImpl::enterTerm(PauliOperatorParser::TermContext *ctx) {
  std::complex<double> coeff(1., 0.);

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

  if (ctx->pauli().empty()) {
    _op += PauliOperator(coeff);
  } else {
    PauliOperator tmp(1.);

    for (int i = 0; i < ctx->pauli().size(); i++) {
      auto pauliStr = ctx->pauli(i)->getText();
      auto pauli = pauliStr.substr(0, 1);
      int idx = 0;
      if (pauli != "I") {
        idx = std::stoi(pauliStr.substr(1, pauliStr.length()));
      }
      tmp *= PauliOperator({{idx, pauli}});
    }
    _op += coeff * tmp;
  }
}

} // namespace quantum
} // namespace xacc