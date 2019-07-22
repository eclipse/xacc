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

XACCLangListener::XACCLangListener(std::shared_ptr<xacc::IR> ir) : ir(ir) {
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
  void XACCLangListener::exitXacckernel(XACCLangParser::XacckernelContext *ctx) {

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

void XACCLangListener::enterUop(XACCLangParser::UopContext *ctx) {
  // Note, if the user has specified something like
  // H(...) or H(0...2), then we have a different handle
  if (ctx->allbitsOp() != nullptr)
    return;

  auto is_double = [](const std::string &s) -> bool {
    try {
      std::stod(s);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  };
  auto is_int = [](const std::string &s) -> bool {
    try {
      std::stoi(s);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  };

  auto gateName = ctx->gatename->getText();
  xacc::trim(gateName);

  if (gateName == "CX") {
    gateName = "CNOT";
  } else if (gateName == "MEASURE") {
    gateName = "Measure";
  }

  //   if (xacc::hasService<GateInstruction>(gateName)) {

  // We have to accept GATE(params..., qbits..., opt=val,...)
  auto tmpInst = gateRegistry->createInstruction(gateName, std::vector<int>{});
  auto nBits = tmpInst->nRequiredBits();

  auto nArgs = ctx->explist()->exp().size();
  int nOptions = 0;
  // how many options are there?
  for (int i = 0; i < nArgs; i++) {
    if (ctx->explist()->exp(i)->key() != nullptr) {
      nOptions++;
    }
  }

  // Parameters may be optional sometimes
  auto nParams = nArgs - nOptions - nBits;

  // get all InstructionParameters first
  std::vector<InstructionParameter> params;
  for (int i = 0; i < nParams; i++) {
    auto tmp = ctx->explist()->exp(i);
    if (gateName == "Measure") {
      InstructionParameter param(std::stoi(tmp->getText()));
      params.push_back(param);
    } else {
      if (tmp->real() != nullptr || tmp->id() != nullptr) {
        auto str = tmp->getText();
        auto param = is_double(str) ? InstructionParameter(std::stod(str))
                                    : InstructionParameter(str);
        params.push_back(param);
      } else if (tmp->exp().size() == 1 && tmp->exp(0)->real() != nullptr) {
        // this is a neg double
        params.push_back(InstructionParameter(std::stod(tmp->getText())));
      } else {
        params.push_back(InstructionParameter(tmp->getText()));
      }
    }
  }

  // Get qubit indices
  std::vector<int> qubits;
  for (int i = nParams; i < nBits + nParams; i++) {
    qubits.push_back(std::stoi(ctx->explist()->exp(i)->INT()->getText()));
  }

  auto inst = gateRegistry->createInstruction(gateName, qubits);
  int counter = 0;
  for (auto &p : params) {
    inst->setParameter(counter, p);
    counter++;
  }

  // See if we have any options
  for (int i = nBits + nParams; i < nArgs; i++) {
    auto key = ctx->explist()->exp(i)->key()->getText();
    auto valStr = ctx->explist()->exp(i)->exp(0)->getText();

    if (is_int(valStr)) {
      InstructionParameter p(std::stoi(valStr));
      inst->setOption(key, p);
    } else if (is_double(valStr)) {
      InstructionParameter p(std::stod(valStr));
      inst->setOption(key, p);
    } else {
      InstructionParameter p(valStr);
      inst->setOption(key, p);
    }

    // FIXME HANDLE OTHER TYPES` ???
  }
  curFunc->addInstruction(inst);
}

} // namespace xacc
