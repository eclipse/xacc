/***********************************************************************************
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey, Amrut Nadgir
 *
 **********************************************************************************/
#ifndef COMPILER_SCAFFCOMPILER_HPP_
#define COMPILER_SCAFFCOMPILER_HPP_
#include "XACC.hpp"
using namespace xacc;

namespace scaffold {

/**
 * The PyXACCCompiler is an XACC Compiler that compiles
 * python-like gate instruction source code to produce a
 * XACC IR.
 */
class ScaffCompiler : public xacc::Compiler {

public:
  /**
   * The Compiler.
   */
  ScaffCompiler() {}

  /*
   * Erase First Occurrence of given  substring from main string.
   */
  std::string eraseSubStr(std::string mainStr, const std::string toErase) {
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);

    if (pos != std::string::npos) {
      // If found then erase it from string
      mainStr.erase(pos, toErase.length());
    }
    return mainStr;
  }

  /**
   * Helper function for the compile method.
   * Takes in source string and writes to a scaffold file.
   * @param src The source code
   * @return
   */

  virtual std::string const writeScaff(const std::string &src);

  /**
   * Helper function for the compile method.
   * Reads in a qasmf file to string
   * @param
   * @return
   */

  virtual std::string readQasm(std::string filename);
  /**
   * Helper function for the compile method.
   * Reads in a qasmf string created by the scaffold compiler
   * and converts it to an IR representation
   * @param qasmf_string The qasmf assembly code
   * @return shared pointer to an IR
   */

  virtual std::shared_ptr<xacc::IR> qasm_to_IR(std::string qasm_string);
  /**
   * Compile the given kernel code for the
   * given Accelerator.
   *
   * @param src The source code
   * @param acc Reference to an Accelerator
   * @return
   */
  virtual std::shared_ptr<xacc::IR>
  compile(const std::string &src, std::shared_ptr<Accelerator> acc) override;

  /**
   * Compile the given kernel code.
   *
   * @param src The source code
   * @return
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  /**
   * No translations yet
   * @param bufferVariable
   * @param function
   * @return
   */
  virtual const std::string
  translate(const std::string &bufferVariable,
            std::shared_ptr<Function> function) override;

  virtual const std::string name() const { return "scaffold"; }

  virtual const std::string description() const {
    return "Compiler class for scaffold code";
  }

  /**
   * The destructor
   */
  virtual ~ScaffCompiler() {}
};

} // namespace scaffold

#endif
