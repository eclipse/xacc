/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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
  bool inForLoop = false;
  std::vector<xasmParser::InstructionContext *> forInstructions;

public:
  HeterogeneousMap runtimeOptions;

  XASMListener();

  std::shared_ptr<CompositeInstruction> getFunction() { return function; }

  void enterXacckernel(xasmParser::XacckernelContext * /*ctx*/) override;
  void enterXacclambda(xasmParser::XacclambdaContext * /*ctx*/) override;
  void enterInstruction(xasmParser::InstructionContext * /*ctx*/) override;
  InstPtr enterForLoopInstruction(xasmParser::InstructionContext *ctx,
                                  std::string &varname, const std::size_t idx);
  void enterForstmt(xasmParser::ForstmtContext * /*ctx*/) override;
  void exitForstmt(xasmParser::ForstmtContext * /*ctx*/) override;
};

} // namespace xacc

#endif
