#ifndef XACC_XASMCOMPILER_HPP
#define XACC_XASMCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {


class XASMCompiler : public xacc::Compiler {
public:
  XASMCompiler();

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                            std::shared_ptr<Accelerator> acc) override;

  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override;

  const std::string name() const override { return "xasm"; }

  const std::string description() const override {
    return "The XASM Compiler compiles kernels written in the XACC assembly language.";
  }

  /**
   * The destructor
   */
  virtual ~XASMCompiler() {}
};


} // namespace xacc
#endif
