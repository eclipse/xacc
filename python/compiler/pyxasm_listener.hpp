#ifndef XACC_PYXASMLISTENER_H
#define XACC_PYXASMLISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "pyxasmBaseListener.h"

using namespace pyxasm;

namespace xacc {

class PyXASMListener : public pyxasmBaseListener {
protected:
  std::shared_ptr<IRProvider> irProvider;
  std::shared_ptr<CompositeInstruction> function;
  std::string bufferName = "";
public:
  PyXASMListener();

  std::shared_ptr<CompositeInstruction> getFunction() {return function;}

  virtual void enterXacckernel(pyxasmParser::XacckernelContext * /*ctx*/) override;
  virtual void enterInstruction(pyxasmParser::InstructionContext * /*ctx*/) override;

};


} // namespace xacc

#endif
