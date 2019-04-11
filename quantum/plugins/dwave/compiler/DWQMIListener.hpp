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
 *   Initial implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef XACC_IBM_DWQMILISTENER_H
#define XACC_IBM_DWQMILISTENER_H

#include "Accelerator.hpp"
#include "DWFunction.hpp"
#include "DWQMIBaseListener.h"
#include "IR.hpp"

using namespace dwqmi;

namespace xacc {
namespace quantum {

template <class C, class T>
auto contains(const C &v, const T &x) -> decltype(std::end(v), true) {
  return std::end(v) != std::find(std::begin(v), std::end(v), x);
}
/**
 *
 */
class DWQMIListener : public DWQMIBaseListener {
protected:
  bool foundAnneal = false;
  std::vector<std::string> functionVarNames;
  std::shared_ptr<IR> ir;
  std::shared_ptr<xacc::Graph> hardwareGraph;
  std::shared_ptr<AcceleratorBuffer> buffer;
  std::map<std::string, std::shared_ptr<DWFunction>> functions;
  std::shared_ptr<DWFunction> curFunc;

public:
  int maxBitIdx = 0;

  DWQMIListener(std::shared_ptr<xacc::IR> ir,
                std::shared_ptr<xacc::Graph> hardwareGraph,
                std::shared_ptr<AcceleratorBuffer> aqcBuffer);

  void enterInst(DWQMIParser::InstContext *ctx) override;
  void enterAnnealdecl(DWQMIParser::AnnealdeclContext *ctx) override;

  void enterXacckernel(DWQMIParser::XacckernelContext *ctx) override;
  void exitXacckernel(DWQMIParser::XacckernelContext *ctx) override;
  void exitKernelcall(DWQMIParser::KernelcallContext *ctx) override;
};

} // namespace quantum

} // namespace xacc

#endif
