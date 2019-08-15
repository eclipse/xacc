/***********************************************************************************
 *
 * Contributors:
 *   Initial API and implementation - Amrut Nadgir, Alex McCaskey
 *
 **********************************************************************************/
#include "ScaffCompiler.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <fstream>
#include <iostream>
#include <vector>

namespace scaffold {
//Writes scaffold code string into a .scaffold fild to be compiled by
//scaffold.sh
std::string const ScaffCompiler::writeScaff(const std::string &src) {
  std::ofstream scaff_src;
  scaff_src.open("src.scaffold", std::ios::trunc);
  scaff_src << src;
  scaff_src.close();
  return "src";
}

//Reads the qasm file created by scaffold.sh and returns it as a string
std::string ScaffCompiler::readQasm(std::string filename) {
  std::string line;
  std::string qasm_string;
  std::ifstream scaff_qasm(filename);
  if (scaff_qasm.is_open()) {
    while (getline(scaff_qasm, line)) {
      qasm_string += line + "\n";
    }
  } else {
    xacc::error("Unable to open qasm file.");
  }
  scaff_qasm.close();
  return qasm_string;
}

//Converts the qasm string to IR by using the build in xacc openqasm compile method.
//If vqe is found, the qasm string has to be edited before it can be compiled
std::shared_ptr<xacc::IR> ScaffCompiler::qasm_to_IR(std::string qasm_string) {



  auto compiler = xacc::getCompiler("openqasm");
  qasm_string = "__qpu__ f(AcceleratorBuffer b) {\n" + qasm_string + "\n}";

  //If the vqe algorithm is being run, use the global map variable from
  //ScaffoldPragmaHandler.hpp to convert all the stand in numbers
  //back into their correct variable names.
  if (xacc::optionExists("foundVQE")) {
      auto keyValue = xacc::getOption("__scaffold_handle_vars__");
      std::vector<std::string> split;
      split = xacc::split(keyValue, ',');
      for (auto pairString : split) {
          auto pair = xacc::split(pairString, ':');
          std::string string_to_replace = pair[1];
          std::string replacement = pair[0];
          std::size_t loc = 0;
          while(loc < qasm_string.size()) {
           loc = qasm_string.find(string_to_replace, loc);
           std::size_t origLoc = loc;
           if (loc == std::string::npos) {
               break;
           }
           qasm_string.replace(loc, string_to_replace.size(), replacement);
      }

    }
  }

  auto IR = compiler->compile(qasm_string);
  std::cout << "IR:\n" << IR->getKernels()[0]->toString() << "\n";
  return IR;



}

//Uses the helper functions above to take the source string,
//write it to a .scaffold file, convert it to openqasm
//using scaffold.sh, and then converting the openqasm to IR.
std::shared_ptr<IR> ScaffCompiler::compile(const std::string &src,
                                           std::shared_ptr<Accelerator> acc) {
  std::string filename;
  filename = writeScaff(src);
  std::string command;
  command = "@SCAFFOLD_PATH@/scaffold.sh -b " + filename + ".scaffold";
  std::system(command.c_str());
  std::string qasm;
  qasm = readQasm(filename + ".qasm");
  auto ir = qasm_to_IR(qasm);

  return ir;
}
//Compile methof without a buffer
std::shared_ptr<IR> ScaffCompiler::compile(const std::string &src) {
  return compile(src, nullptr);
}

const std::string ScaffCompiler::translate(const std::string &bufferVariable,
                                           std::shared_ptr<Function> function) {
  xacc::error("ScaffCompiler::translate() not implemented.");
  return "";
}

} // namespace scaffold
