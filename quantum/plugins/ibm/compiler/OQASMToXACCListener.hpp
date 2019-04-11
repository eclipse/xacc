/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial implementation - H. Charles Zhao
 *
 **********************************************************************************/
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
  std::map<std::string, std::shared_ptr<Function>> functions;
  std::shared_ptr<Function> curFunc;

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
