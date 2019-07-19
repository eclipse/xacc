#include "exprtk.hpp"

#include "IR.hpp"
#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "XACCLangBaseListener.h"
#include "XACCLangListener.hpp"

using namespace xacclang;
using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

namespace xacc {


XACCLangListener::XACCLangListener(std::shared_ptr<xacc::IR> ir)
    : ir(ir) {
  gateRegistry = xacc::getService<IRProvider>("gate");
}

void XACCLangListener::enterXacckernel(
    xacclang::XACCLangParser::XacckernelContext *ctx) {
  std::vector<InstructionParameter> params;
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    params.push_back(InstructionParameter(
        ctx->typedparam(static_cast<size_t>(i))->param()->getText()));
  }
  curFunc =
      gateRegistry->createFunction(ctx->kernelname->getText(), {}, params);
  functions.insert({curFunc->name(), curFunc});
}

void XACCLangListener::exitXacckernel(
    xacclang::XACCLangParser::XacckernelContext *ctx) {
  ir->addKernel(curFunc);
}

void XACCLangListener::exitKernelcall(
    xacclang::XACCLangParser::KernelcallContext *ctx) {
  std::string gateName = ctx->kernelname->getText();
  if (functions.count(gateName)) {
    curFunc->addInstruction(functions[gateName]);
  } else {
    xacc::error("Tried calling undefined kernel.");
  }
}

void XACCLangListener::exitU(xacclang::XACCLangParser::UContext *ctx) {
  std::vector<int> qubits;
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

void XACCLangListener::exitCX(xacclang::XACCLangParser::CXContext *ctx) {
  std::vector<int> qubits;
  qubits.push_back(std::stoi(ctx->gatearg(0)->INT()->getText()));
  qubits.push_back(std::stoi(ctx->gatearg(1)->INT()->getText()));

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("CNOT", qubits);
  curFunc->addInstruction(instruction);
}

void XACCLangListener::exitUserDefGate(
    xacclang::XACCLangParser::UserDefGateContext *ctx) {
  std::string gateName = ctx->gatename()->id()->getText();

  gateName[0] = static_cast<char>(toupper(gateName[0]));

  std::vector<int> qubits;

  for (int i = 0; i < ctx->gatearglist()->gatearg().size(); i++) {
    qubits.push_back(std::stoi(
        ctx->gatearglist()->gatearg(static_cast<size_t>(i))->INT()->getText()));
  }
  std::shared_ptr<xacc::Instruction> instruction =
    gateRegistry -> createInstruction (gateName, qubits);
  currFunc -> addInstruction(instruction);
  return;

  if (gateName == "CX") {
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
  } else if (gateName == "U1") {
    instruction = gateRegistry->createInstruction("U", qubits);
    InstructionParameter p(0.0), p2(0.0);
    instruction->setParameter(0, p);
    instruction->setParameter(1, p2);
    count = 2;
  } else if (gateName == "U3") {
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

void XACCLangListener::exitMeasure(
    xacclang::XACCLangParser::MeasureContext *ctx) {
  std::vector<int> qubits;
  qubits.push_back(std::stoi(ctx->qbit->INT()->getText()));
  int classicalBit = std::stoi(ctx->cbit->INT()->getText());

  std::shared_ptr<xacc::Instruction> instruction =
      gateRegistry->createInstruction("Measure", qubits,
                                      {InstructionParameter(classicalBit)});
  curFunc->addInstruction(instruction);
}
} // namespace xacc
