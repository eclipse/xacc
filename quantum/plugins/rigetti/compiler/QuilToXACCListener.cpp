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

#include "exprtk.hpp"
#include "QuilToXACCListener.hpp"
#include "QuilBaseListener.h"
#include "XACC.hpp"
#include "IRProvider.hpp"
#include "IR.hpp"
#include "xacc_service.hpp"

using namespace quil;

namespace xacc {
namespace quantum {

QuilToXACCListener::QuilToXACCListener() {
  gateRegistry = xacc::getService<IRProvider>("quantum");
}

void QuilToXACCListener::enterXacckernel(
    quil::QuilParser::XacckernelContext *ctx) {
  std::vector<std::string> params;
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    params.push_back(
        ctx->typedparam(i)->IDENTIFIER()->getText());
  }
  function =
      gateRegistry->createComposite(ctx->kernelname->getText(), params);
}

void QuilToXACCListener::exitGate(quil::QuilParser::GateContext *ctx) {
  std::string gateName = ctx->name()->getText();
  if (gateName == "RX")
    gateName = "Rx";
  if (gateName == "RY")
    gateName = "Ry";
  if (gateName == "RZ")
    gateName = "Rz";
  if (gateName == "CX")
    gateName = "CNOT";

  std::vector<std::size_t> qubits;
  for (int i = 0; i < ctx->qubit().size(); i++) {
    qubits.push_back(std::stoi(ctx->qubit(i)->getText()));
  }

  std::vector<InstructionParameter> params;
 for (int i = 0; i < ctx->param().size(); i++) {
    auto paramStr = ctx->param(i)->getText();
    params.emplace_back(paramStr);
  }

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction(gateName, qubits, params);

  function->addInstruction(instruction);
}

void QuilToXACCListener::exitMeasure(quil::QuilParser::MeasureContext *ctx) {
  std::vector<std::size_t> qubits;
  qubits.push_back(std::stoi(ctx->qubit()->getText()));
  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("Measure", qubits);
  function->addInstruction(instruction);
}
} // namespace quantum
} // namespace xacc
