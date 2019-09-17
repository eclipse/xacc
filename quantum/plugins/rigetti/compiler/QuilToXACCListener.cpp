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
#include "exprtk.hpp"
#include "QuilToXACCListener.hpp"
#include "QuilBaseListener.h"
#include "xacc.hpp"
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

void QuilToXACCListener::exitKernelcall(
    quil::QuilParser::KernelcallContext *ctx) {
  std::string gateName = ctx->kernelname->getText();
  auto f = xacc::getCompiled(gateName);
  function->addInstruction(f);
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
