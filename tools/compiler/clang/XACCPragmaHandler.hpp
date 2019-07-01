#ifndef COMPILER_CLANG_XACCPRAGMAHANDLER_HPP__
#define COMPILER_CLANG_XACCPRAGMAHANDLER_HPP__
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/TokenKinds.def"
#include "clang/Lex/LexDiagnostic.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <iostream>
#include <sstream>

using namespace clang;

namespace xacc {
namespace compiler {
class XACCPragmaHandler : public PragmaHandler {
protected:
//   Rewriter &rewriter;

  struct XACCPragmaAttributeInfo {
    //   enum ActionType { Push, Pop, Attribute };
    //   ParsedAttributes &Attributes;
    //   ActionType Action;
    const IdentifierInfo *Namespace = nullptr;
    ArrayRef<Token> Tokens;

    //   PragmaAttributeInfo(ParsedAttributes &Attributes) :
    //   Attributes(Attributes) {}
  };

public:

  std::string observable = "";
  std::string functionName = "";

  XACCPragmaHandler() :PragmaHandler("xacc") {}
//   XACCPragmaHandler(Rewriter &r) : PragmaHandler("xacc"), rewriter(r) {}

  void HandlePragma(Preprocessor &PP, PragmaIntroducer Introducer,
                    Token &FirstTok) override {
    // #pragma xacc observe hamiltonian

    // std::cout << "XACC HANDLING PRAGMA:\n";
    Token Tok;
//   // first slurp the directive content in a string.
//   std::ostringstream MyAnnotateDirective;
//   while(Tok.isNot(tok::eod)) {
//     PP.Lex(Tok);
//     if(Tok.isNot(tok::eod))
//       MyAnnotateDirective << PP.getSpelling(Tok) << "_";
//   }

//   std::cout << "HELLO: " << MyAnnotateDirective.str() << "\n";
//   Tok.startToken();
//   Tok.setKind(tok::annot_pragma_attribute);
//   Tok.setLocation(FirstTok.getLocation());
//   Tok.setAnnotationEndLoc(FirstTok.getLocation());
//   // there should be something better that this strdup :-/
//   Tok.setAnnotationValue(strdup(MyAnnotateDirective.str().c_str()));

//   PP.EnterTokenStream(Tok);
    // Token Tok;
    // PP.Lex(Tok);
    // std::cout << "T2: " << Tok.getIdentifierInfo()->getName().str() << "\n";
    // auto *Info = new (PP.getPreprocessorAllocator()) XACCPragmaAttributeInfo();
    // SmallVector<Token, 16> AttributeTokens;

    // if (Tok.getIdentifierInfo()->isStr("observe")) {
    //   std::cout << "We are requesting an observation\n";
    //   AttributeTokens.push_back(Tok);
    //   PP.Lex(Tok);
    //   std::cout << "Ham? " << Tok.getIdentifierInfo()->getName().str() << "\n";
    //   observable = Tok.getIdentifierInfo()->getName().str();

    //   AttributeTokens.push_back(Tok);

    //   SourceLocation EndLoc = Tok.getLocation();
    //   PP.Lex(Tok);
    // //   std::cout << "NEXT TOKEN: " << Tok.getIdentifierInfo()->getName().str() << "\n";
    //   // Terminate the attribute for parsing.
    //   Token EOFTok;
    //   EOFTok.startToken();
    //   EOFTok.setKind(tok::eof);
    //   EOFTok.setLocation(EndLoc);
    //   AttributeTokens.push_back(EOFTok);

    //   Info->Tokens = llvm::makeArrayRef(AttributeTokens)
    //                      .copy(PP.getPreprocessorAllocator());

    //   // Generate the annotated pragma token.
    //   auto TokenArray = llvm::make_unique<Token[]>(1);
    //   TokenArray[0].startToken();
    //   TokenArray[0].setKind(tok::annot_pragma_attribute);
    //   TokenArray[0].setLocation(FirstTok.getLocation());
    //   TokenArray[0].setAnnotationEndLoc(FirstTok.getLocation());
    //   TokenArray[0].setAnnotationValue(static_cast<void *>(Info));
    //   PP.EnterTokenStream(std::move(TokenArray), 1,
    //                       /*DisableMacroExpansion=*/false,
    //                       /*IsReinject=*/false);
    //   std::cout << "WE MADE IT HERE\n";
    // }
    // Token Tok;
    // // first slurp the directive content in a string.
    // std::stringstream ss;
    // SmallVector<Token, 16> Pragmas;
    // int found = -1;
    // // auto sl = FirstTok.getLocation();
    // // sl.dump(PP.getSourceManager());

    std::stringstream ss;
    int found = 0;
    std::string declaration;
    // //   FirstTok.getAnnotationRange().dump(PP.getSourceManager());
    while (!Tok.is(tok::eod)) {
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) == "observe") {
          PP.Lex(Tok);
          observable = PP.getSpelling(Tok);
      }
    }

    // This is the Function name we want!!!
    PP.Lex(Tok);
    functionName = PP.getSpelling(Tok);

//   std::cout << "SS: "<< functionName<< "\n";
  PP.EnterToken(Tok, false);

  }
};
} // namespace compiler
} // namespace xacc

// static PragmaHandlerRegistry::Add<xacc::compiler::XACCPragmaHandler> YYYY("xacc","xacc pragma description");

#endif