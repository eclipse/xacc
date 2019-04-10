#include "FermionListenerImpl.hpp"

namespace xacc {
namespace quantum {
void FermionListenerImpl::enterPlusorminus(FermionOperatorParser::PlusorminusContext * ctx) {
  isMinus = ctx->getText() == "-";
}

void FermionListenerImpl::enterTerm(FermionOperatorParser::TermContext * ctx) {

//   std::cout << "ENTER TERM: " << ctx->getText() << ", " << ctx->fermion().size() << "\n";

  std::complex<double> coeff(1.0,0.0);
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

      bool creation = false;
      if (str.find("^") != std::string::npos) {
          creation = true;
      }

      term.push_back({std::stoi(ctx->fermion(i)->op()->INT()->getText()), creation});

    //   std::cout << "HI: " << ctx->fermion(i)->getText() << ", " << "\n";
  }
//   std::cout << "SIZE OF TERMS: " << term.size() << "\n";
  _op += FermionOperator(term, coeff);

//   std::cout << "ENTER TERM: " << ctx->getText() << ", " << ctx->fermion().size() << "\n";

}

}
}