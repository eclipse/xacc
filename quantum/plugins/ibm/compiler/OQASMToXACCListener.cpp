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
#include "IR.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "OQASM2BaseListener.h"
#include "OQASMToXACCListener.hpp"

// using namespace oqasm;
// using symbol_table_t = exprtk::symbol_table<double>;
// using expression_t = exprtk::expression<double>;
// using parser_t = exprtk::parser<double>;

namespace xacc {

namespace quantum {

constexpr static double pi = xacc::constants::pi;

OQASMToXACCListener::OQASMToXACCListener(std::shared_ptr<xacc::IR> ir)
    : ir(ir) {
  gateRegistry = xacc::getService<IRProvider>("quantum");
}

// double evalMathExpression(const std::string &expression) {
//   symbol_table_t symbol_table;
//   symbol_table.add_constant("pi", pi);
//   expression_t expr;
//   expr.register_symbol_table(symbol_table);
//   parser_t parser;
//   parser.compile(expression, expr);
//   return expr.value();
// }

InstructionParameter strToParam(const std::string &str) {
//   double num = evalMathExpression(str);
//   if (std::isnan(num)) {
    return InstructionParameter(str);
//   } else {
    // return InstructionParameter(num);
//   }
}

void OQASMToXACCListener::enterXacckernel(
    oqasm::OQASM2Parser::XacckernelContext *ctx) {
  std::vector<std::string> variables;
  // First argument should be the buffer

  for (int i = 0; i < ctx->typedparam().size(); i++) {

    variables.push_back(
        ctx->typedparam(i)->param()->getText());
  }
  curFunc =
      gateRegistry->createComposite(ctx->kernelname->getText(), variables);
  functions.insert({curFunc->name(), curFunc});
}

void OQASMToXACCListener::exitXacckernel(
    oqasm::OQASM2Parser::XacckernelContext *ctx) {
  ir->addComposite(curFunc);
}

void OQASMToXACCListener::exitKernelcall(
    oqasm::OQASM2Parser::KernelcallContext *ctx) {
  std::string gateName = ctx->kernelname->getText();
  if (functions.count(gateName)) {
    curFunc->addInstruction(functions[gateName]);
  } else {
    xacc::error("Tried calling undefined kernel.");
  }
}

void OQASMToXACCListener::exitU(oqasm::OQASM2Parser::UContext *ctx) {
  std::vector<std::size_t> qubits;
  std::vector<InstructionParameter> params;

  qubits.push_back(std::stoi(ctx->gatearg()->INT()->getText()));
  params.push_back(strToParam(ctx->explist()->exp(0)->getText())); // theta
  params.push_back(strToParam(ctx->explist()->exp(1)->getText())); // phi
  params.push_back(strToParam(ctx->explist()->exp(2)->getText())); // lambda

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("U", qubits);
  for (int i = 0; i < params.size(); i++) {
    instruction->setParameter(i, params[i]);
  }
  curFunc->addInstruction(instruction);
}

void OQASMToXACCListener::exitCX(oqasm::OQASM2Parser::CXContext *ctx) {
  std::vector<std::size_t> qubits;
  qubits.push_back(std::stoi(ctx->gatearg(0)->INT()->getText()));
  qubits.push_back(std::stoi(ctx->gatearg(1)->INT()->getText()));

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("CNOT", qubits);
  curFunc->addInstruction(instruction);
}

void OQASMToXACCListener::exitUserDefGate(
    oqasm::OQASM2Parser::UserDefGateContext *ctx) {
  std::string gateName = ctx->gatename()->id()->getText();

  gateName[0] = static_cast<char>(toupper(gateName[0]));

  std::vector<std::size_t> qubits;

  for (int i = 0; i < ctx->gatearglist()->gatearg().size(); i++) {
    qubits.push_back(std::stoi(
        ctx->gatearglist()->gatearg(static_cast<size_t>(i))->INT()->getText()));
  }

  if (gateName == "Cx") {
    std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("CNOT", qubits);
    curFunc->addInstruction(instruction);
    return;
  }

  InstructionParameter param;
  std::shared_ptr<xacc::Instruction> instruction;

  int count = 0;
  if (gateName == "U2") {
    instruction = gateRegistry->createInstruction("U", qubits);
    InstructionParameter p(pi / 2.0);
    instruction->setParameter(0, p);
    count = 1;
  } else if (gateName == "U1" || gateName == "u1") {
    instruction = gateRegistry->createInstruction("U", qubits);
    InstructionParameter p(0.0), p2(0.0);
    instruction->setParameter(0, p);
    instruction->setParameter(1, p2);
    count = 2;
  } else if (gateName == "U3" || gateName == "u3") {
    instruction = gateRegistry->createInstruction("U", qubits);
    count = 0;
  } else {
    instruction = gateRegistry->createInstruction(gateName, qubits);
  }

  if (ctx->explist() != nullptr) {
    for (int i = 0; i < ctx->explist()->exp().size(); i++) {
      param =
          strToParam(ctx->explist()->exp(static_cast<size_t>(i))->getText());
      instruction->setParameter(count, param);
      count++;
    }
  }

  curFunc->addInstruction(instruction);
}

void OQASMToXACCListener::exitMeasure(
    oqasm::OQASM2Parser::MeasureContext *ctx) {
  std::vector<std::size_t> qubits;
  qubits.push_back(std::stoi(ctx->qbit->INT()->getText()));
//   int classicalBit = std::stoi(ctx->cbit->INT()->getText());

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("Measure", qubits);
  curFunc->addInstruction(instruction);
}
} // namespace quantum
} // namespace xacc
