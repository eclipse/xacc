/***********************************************************************************
 *
 * Contributors:
 *   Initial implementation - Amrut Nadgir
 *
 **********************************************************************************/
#ifndef XACC_XASMLISTENER_H
#define XACC_XASMLISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "xasmBaseListener.h"

using namespace xasm;

namespace xacc {

class XASMListener : public xasmBaseListener {
protected:
  std::shared_ptr<IRProvider> irProvider;
  std::shared_ptr<CompositeInstruction> function;
  std::string bufferName = "";
  bool hasVecDouble = false;
  std::string param_id = "t";

public:
  HeterogeneousMap runtimeOptions;

  XASMListener();

  std::shared_ptr<CompositeInstruction> getFunction() { return function; }

  virtual void
  enterXacckernel(xasmParser::XacckernelContext * /*ctx*/) override;
  virtual void
  enterXacclambda(xasmParser::XacclambdaContext * /*ctx*/) override;
  virtual void
  enterInstruction(xasmParser::InstructionContext * /*ctx*/) override;
};

} // namespace xacc

#endif
