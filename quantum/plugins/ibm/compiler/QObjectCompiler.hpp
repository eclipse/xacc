#ifndef IMPLS_IBM_QOBJECTCOMPILER_HPP
#define IMPLS_IBM_QOBJECTCOMPILER_HPP

#include "Compiler.hpp"

namespace xacc {

namespace quantum {

class QObjectCompiler : public xacc::Compiler {
public:
  QObjectCompiler() = default;

  std::shared_ptr<xacc::IR> compile(const std::string &src,
                                    std::shared_ptr<Accelerator> acc) override;
  std::shared_ptr<xacc::IR> compile(const std::string &src) override;

  const std::string translate(std::shared_ptr<CompositeInstruction> function) override;

  const std::string name() const override { return "qobj"; }
  const std::string description() const override {
    return "The QObject Compiler compiles an IBM QObject (represented as a "
           "JSON string) and maps it to the XACC IR.";
  }

  /**
   * The destructor
   */
  ~QObjectCompiler() override {}
};

} // namespace quantum

} // namespace xacc
#endif
