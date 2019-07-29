/***********************************************************************************
 *
 * Contributors:
 *   Initial API and implementation - Amrut Nadgir, Alex McCaskey
 *
 **********************************************************************************/
#ifndef COMPILER_CLANG_ScaffoldPRAGMAHANDLER_HPP__
#define COMPILER_CLANG_ScaffoldPRAGMAHANDLER_HPP__
#include "XACC.hpp"

#include <iostream>
#include <sstream>

#include "QCORPragmaHandler.hpp"
#include "qcor.hpp"

using namespace clang;

namespace scaffold {
namespace compiler {
class ScaffoldPragmaHandler : public qcor::compiler::QCORPragmaHandler {
public:
  ScaffoldPragmaHandler() : QCORPragmaHandler() {}
  void HandlePragma(Preprocessor &PP, PragmaIntroducer Introducer,
                    Token &FirstTok) override {
    //Name of buffer to store results
    std::string bufName;
    //Number of qubits in buffer
    std::string bufSize;
    //Name of a vector of angle variables for the vqe application
    std::string anglesName;
    //Name of optimizer for vqe algorithm
    std::string optName;
    //Name of observable to minimize for vqe algorithm
    std::string obvName;

    // #pragma compile scaffold
    // Token to read through pragma statement
    Token Tok;
    bool foundScaffold = false;
    bool foundVQE = false;
    //Global xacc variable that communicates with ScaffCompiler.in.cpp
    xacc::setOption("foundVQE", "false");
    std::stringstream ss;
    std::string declaration;
    PP.Lex(Tok);
    //Parsing pragma statement #pragma qcor scaffold(bufName, bufSize)
    //and maybe vqe(anglesName, obvName, optName)
    if (PP.getSpelling(Tok) == "scaffold") {
      foundScaffold = true;
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != "(") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
      bufName = PP.getSpelling(Tok);
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != ",") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
      bufSize = PP.getSpelling(Tok);
    }
    if (!Tok.is(tok::eod)) {
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != ")") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
    }
    //Searching for vqe, optimizer and observable names in pragma
    if (PP.getSpelling(Tok) == "vqe") {
      foundVQE = true;
      xacc::setOption("foundVQE", "true");
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != "(") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
      anglesName = PP.getSpelling(Tok);
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != ",") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
      obvName = PP.getSpelling(Tok);
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) != ",") {
        xacc::error("Bad syntax for pragma statement.");
      }
      PP.Lex(Tok);
      optName = PP.getSpelling(Tok);
    }

    while (!Tok.is(tok::eod)) {
      PP.Lex(Tok);
      if (PP.getSpelling(Tok) == "{") {
        xacc::error("Put left brace on the next line after pragma statement");
      }
    }

    //Parsing code after pragma statement
    //Using left brace count to see when the code starts/ends
    int lBrace_count = 0;
    bool first_lBrace = false;
    if (foundScaffold && xacc::hasCompiler("scaffold")) {
      std::stringstream ss;
      auto compiler = xacc::getCompiler("scaffold");
      std::string src = "";
      std::vector<std::string> modules;
      // LBRACE Counter, decrement when you see rbrace
      while (lBrace_count != 0 || !first_lBrace) {
        PP.Lex(Tok);
        std::string curr = PP.getSpelling(Tok);
        //Creates a list of modules to add seperately outisde the main body
        if (curr == "module") {
          std::string module = curr + " ";
          int lBrace_count_module = 0;
          bool first_lBrace_module = false;
          while (lBrace_count_module != 0 || !first_lBrace_module) {
            PP.Lex(Tok);
            std::string curr = PP.getSpelling(Tok);
            module += curr + " ";
            if (curr == ";") {
              module += "\n";
            } else if (curr == "{") {
              first_lBrace_module = true;
              lBrace_count_module++;
            } else if (curr == "}") {
              lBrace_count_module--;
            }
          }
          modules.push_back(module);
        } else {
          ss << PP.getSpelling(Tok) << " ";
          if (curr == ";") {
            ss << "\n";
          } else if (curr == "{") {
            first_lBrace = true;
            lBrace_count++;
          } else if (curr == "}") {
            lBrace_count--;
          }
        }
      }
      src = ss.str();
      int lBrace_loc = src.find('{');
      src.erase(lBrace_loc, 1);
        // If using vqe, creates a map from the vector of angle variables to
        // numbers and replaces instances of these variables in the code with
        //these numbers to allow compilation by scaffold.sh
      if (foundVQE) {
        //location variable in string
        std::size_t loc = 0;
        //map from angle names to numbers
        std::map<std::string, std::string> angles_to_int;
        //map from numbers to angle names
        std::map<std::string, std::string> int_to_angles;
        //String for the name of each angle parameter
        std::string key;
        //Numerical string to replace the angle parameters by
        std::string assigned_num;

        //Finding all instances of angle parameters being used in the code
        //and replacing it with its assigned number while storing it in the maps
        while (loc < src.size()) {
          loc = src.find(anglesName, loc);
          std::size_t origLoc = loc;
          if (loc == std::string::npos) {
            break;
          }
          loc = src.find("[", loc);
          std::string index;
          loc++;
          while (src.at(loc) != ']') {
            index += src.at(loc);
            loc++;
          }
          index.erase(0, 1);
          //Sets the name for each angle variable based on the name of the
          //vector of variables and the index of the angle
          key = anglesName + index;
          key.pop_back();
          assigned_num = std::to_string(std::stoi(index) + 58374);
          angles_to_int.insert({key, assigned_num});
          int_to_angles.insert({assigned_num, key});
          src.replace(origLoc, loc - origLoc + 1, assigned_num);
        }
        //Converts map into string with scientific notation to pass into
        //ScaffCompiler
        std::stringstream ss;
        for (auto &t : angles_to_int) {
          ss << t.first << ":" << std::scientific << std::stod(t.second) << ",";
        }
        auto keyValue = ss.str();
        keyValue.pop_back();

        //Sets map as global variable so it can be read by ScaffCompiler.in.cpp
        xacc::setOption("__scaffold_handle_vars__", keyValue);
      }
    //Adds function modules and main section to source with a qubit definition statement
      src = "int main() { qbit " + bufName + " [" + bufSize + "]; \n" + src;

      std::string combinedModules = "";
      for (auto module : modules) {
        combinedModules += module + "\n";
      }
      src = combinedModules + src;



      //Compiles source code
      auto IR = compiler->compile(src);
      //Make an output cpp file to run that will work with the c++ compiler
      //and execute the quantum code through xacc
      SourceLocation start = Introducer.Loc;
      SourceLocation end = Tok.getLocation();
      SourceRange range(start, end);
      std::shared_ptr<xacc::Accelerator> acc;
      if (xacc::optionExists("accelerator")) {
        acc = xacc::getAccelerator();
      }
      auto f = qcor::persistCompiledCircuit(function, acc);
      std::string replacement;
      //Replaces the scaffold from the main program to compile with XACC
      //functions that run on a qpu.
      if (foundVQE) {
        replacement =
            "auto future = qcor::submit([&](qcor::qpu_handler &qh) {\n";
        replacement += "qh.vqe([&]() { return \"" + f + "\";}," + obvName +
                       ", " + optName + ");}, " + bufName + ");";
      } else {
        replacement =
            "auto future = qcor::submit([&](qcor::qpu_handler &qh) {\n";
        replacement +=
            "qh.execute([]() { return \"" + f + "\";});}, " + bufName + ");\n";
      }

      replacement += "future.get();\n";

      rewriter->ReplaceText(range, StringRef(replacement));
    }

    PP.Lex(Tok);
    PP.EnterToken(Tok, false);
  }
};
} // namespace compiler
} // namespace scaffold
#endif