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
#ifndef XACC_IBM_OQASMTOXACCLISTENER_H
#define XACC_IBM_OQASMTOXACCLISTENER_H

#include "IR.hpp"
#include "IRProvider.hpp"
#include "OQASM2BaseListener.h"

using namespace oqasm;

namespace xacc {

namespace quantum {

class OQASMToXACCListener : public OQASM2BaseListener {
  std::shared_ptr<IR> ir;
  std::shared_ptr<IRProvider> gateRegistry;
  std::map<std::string, std::shared_ptr<CompositeInstruction>> functions;
  std::shared_ptr<CompositeInstruction> curFunc;

public:
  explicit OQASMToXACCListener(std::shared_ptr<IR>);

  void enterXacckernel(OQASM2Parser::XacckernelContext *ctx) override;

  void exitXacckernel(OQASM2Parser::XacckernelContext *ctx) override;

  void exitU(OQASM2Parser::UContext *ctx) override;

  void exitCX(OQASM2Parser::CXContext *ctx) override;

  void exitUserDefGate(OQASM2Parser::UserDefGateContext *ctx) override;

  void exitMeasure(OQASM2Parser::MeasureContext *ctx) override;

  void exitKernelcall(OQASM2Parser::KernelcallContext *ctx) override;
};

} // namespace quantum

} // namespace xacc

#endif
