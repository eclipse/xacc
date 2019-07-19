#ifndef XACC_LANG_COMPILER_HPP
#define XACC_LANG_COMPILER_HPP

#include "Compiler.hpp"

namespace xacc {


class XACCLangCompiler : public xacc::Compiler {
public:
  XACCLangCompiler();

  /**
   * Translate OpenQASM to the XACC intermediate representation.
   *
   * @return ir XACC intermediate representation
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc);

  /**
   *
   * @param src
   * @return ir
   */
  virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

  /**
   * This produces an XACCLang source code representation of the given IR
   * Function.
   *
   * @param function The XACC IR Function to translate
   * @return src The source code as a string
   */
  virtual const std::string translate(const std::string &bufferVariable,
                                      std::shared_ptr<Function> function);

  virtual const std::string name() const { return "openqasm"; }

  virtual const std::string description() const {
    return "The XACCLang Compiler compiles kernels written in the XACC quantum language.";
  }

  /**
   * The destructor
   */
  virtual ~XACCLangCompiler() {}
};


} // namespace xacc
#endif
