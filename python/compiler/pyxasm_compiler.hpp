#ifndef XACC_PYASMCOMPILER_HPP
#define XACC_PYXASMCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

class PyXASMCompiler : public xacc::Compiler {
public:
  PyXASMCompiler();

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) override;

  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override;

  const std::string name() const override { return "pyxasm"; }

  const std::string description() const override {
    return "The PyXASM Compiler compiles kernels written in the Pythonic XACC assembly language.";
  }

  /**
   * The destructor
   */
  virtual ~PyXASMCompiler() {}
};


} // namespace xacc
#endif
