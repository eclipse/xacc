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
  parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
}

void QuilToXACCListener::enterXacckernel(
    quil::QuilParser::XacckernelContext *ctx) {
  //   std::vector<std::string> params;
  //   for (int i = 0; i < ctx->typedparam().size(); i++) {
  //     params.push_back(
  //         ctx->typedparam(i)->IDENTIFIER()->getText());
  //   }
  //   function =
  //       gateRegistry->createComposite(ctx->kernelname->getText(), params);

  std::vector<std::string> variables,
      validTypes{"double", "float", "std::vector<double>", "int"};

  function = gateRegistry->createComposite(ctx->kernelname->getText());
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    auto type = ctx->typedparam(i)->type()->getText();
    auto vname = ctx->typedparam(i)->variable_param_name()->getText();
    function->addArgument(vname, type);
    // if (type == "qreg" || type == "qbit") {
    //   functionBufferNames.push_back(vname);
    // }
    if (xacc::container::contains(validTypes,
                                  ctx->typedparam(i)->type()->getText())) {
      variables.push_back(vname);
    }
  }
  function->addVariables(variables);
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
  if (gateName == "CPHASE")
      gateName = "CPhase";

  std::vector<std::size_t> qubits;
  for (int i = 0; i < ctx->qubit().size(); i++) {
    qubits.push_back(std::stoi(ctx->qubit(i)->getText()));
  }

  std::map<int, std::shared_ptr<CompositeArgument>> args;
  std::vector<InstructionParameter> params;
  for (int i = 0; i < ctx->param().size(); i++) {
    auto paramStr = ctx->param(i)->getText();
    // params.emplace_back(paramStr);

    double value;
    if (parsingUtil->isConstant(ctx->param(i)->getText(), value)) {
      xacc::debug("[XasmCompiler] Parameter added is " + std::to_string(value));
      params.emplace_back(value);
    } else {
      xacc::debug("[XasmCompiler] Parameter added is " +
                  ctx->param(i)->getText());
      InstructionParameter p(ctx->param(i)->getText());
      p.storeOriginalExpression();
      params.emplace_back(p);

      // This depends on a function argument

      auto arg = function->getArgument(ctx->param(i)->getText());

      if (!arg) {
        // we may have a case where the parameter is an expression string,
        // maybe something like 1.0 * theta[0]
        for (auto &_arg : function->getArguments()) {
          auto param_str = ctx->param(i)->getText();
          param_str.erase(std::remove_if(param_str.begin(), param_str.end(),
                                         [](char c) {
                                           return !std::isalpha(c) ||
                                                  c == '[' || c == ']';
                                         }),
                          param_str.end());
          //   std::cout << "PARAMSTR: " << param_str << "\n";
          double val;
          if (parsingUtil->validExpression(param_str, {_arg->name})) {
            arg = _arg;
            break;
          }
        }
      }
      args.insert({i, arg});
    }
  }

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction(gateName, qubits, params);

  for (auto &kv : args)
    instruction->addArgument(kv.second, kv.first);

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
