/***********************************************************************************
 *
 * Contributors:
 *   Initial implementation - Amrut Nadgir
 *
 **********************************************************************************/
#ifndef XACC_LANG_ERRORLISTENER_HPP
#define XACC_LANG_ERRORLISTENER_HPP

using namespace antlr4;

class XACCLangErrorListener : public BaseErrorListener {
public:
  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid XACCLang source: ";
    output << "line " << line << ":" << charPositionInLine << " " << msg;
    xacc::error(output.str());
  }
};

#endif // XACC_LANG_ERRORLISTENER_HPP
