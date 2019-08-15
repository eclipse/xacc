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