#ifndef XACC_XASMERRORLISTENER_HPP
#define XACC_XASMERRORLISTENER_HPP

using namespace antlr4;

class XASMErrorListener : public BaseErrorListener {
public:
  void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                   size_t charPositionInLine, const std::string &msg,
                   std::exception_ptr e) override {
    std::ostringstream output;
    output << "Invalid xasm source: [";
    output << line << ":" << charPositionInLine << "] " << msg;
    xacc::error(output.str());
  }
};

#endif // XACC_LANG_ERRORLISTENER_HPP
