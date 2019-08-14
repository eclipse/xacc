#include "exprtk.hpp"

#include "IR.hpp"
#include "IRProvider.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

#include "xasmBaseListener.h"
#include "xasm_listener.hpp"

#include "GateInstruction.hpp"

using namespace xasm;

using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

namespace xacc {

XASMListener::XASMListener() {
  irProvider = xacc::getService<IRProvider>("quantum");
}

void XASMListener::enterXacckernel(xasmParser::XacckernelContext *ctx) {
  bufferName = ctx->acceleratorbuffer->getText();
  function = irProvider->createFunction(ctx->kernelname->getText(), {}, {});
}

void XASMListener::enterXacclambda(xasmParser::XacclambdaContext *ctx) {
  bufferName = ctx->acceleratorbuffer->getText();
  function = irProvider->createFunction("tmp_lambda",
                                        {}, {});
}

void XASMListener::enterInstruction(xasmParser::InstructionContext *ctx) {

  auto instructionName = ctx->inst_name->getText();
  xacc::trim(instructionName);

  if (instructionName == "CX") {
    instructionName = "CNOT";
  } else if (instructionName == "MEASURE") {
    instructionName = "Measure";
  }

  std::vector<int> bits;

  int nBits = ctx->bits->bufferIndex().size();
  for (int i = 0; i < nBits; i++) {
    if (ctx->bits->bufferIndex(i)->INT() != nullptr) {
      bits.push_back(std::stoi(ctx->bits->bufferIndex(i)->INT()->getText()));
    }
  }

  std::vector<InstructionParameter> params;
  if (ctx->params != nullptr) {

    int nParams = ctx->params->exp().size();
    for (int i = 0; i < nParams; i++) {
      auto paramStr = ctx->params->exp(i)->getText();
      symbol_table_t symbol_table;
      symbol_table.add_constants();
      expression_t expr;
      expr.register_symbol_table(symbol_table);
      parser_t parser;
      if (parser.compile(paramStr, expr)) {
        auto value = expr.value();
        params.emplace_back(value);
      } else {
        params.emplace_back(paramStr);
      }
    }
  }

  std::map<std::string, InstructionParameter> options;
  if (ctx->options != nullptr) {
    int nOptions = ctx->options->optionsType().size();
    for (int i = 0; i < nOptions; i++) {
      auto key = ctx->options->optionsType(i)->key->getText();
      key.erase(remove(key.begin(), key.end(), '\"'), key.end());
      auto val = ctx->options->optionsType(i)->value->getText();

      symbol_table_t symbol_table;
      symbol_table.add_constants();
      expression_t expr;
      expr.register_symbol_table(symbol_table);
      parser_t parser;
      if (parser.compile(val, expr)) {
        auto value = expr.value();
        if (ctx->options->optionsType(i)->value->INT() != nullptr) {
          int tmp = (int) value;
          options.insert({key, tmp});
        } else {
          options.insert({key, value});
        }
      } else {
        val.erase(remove(val.begin(), val.end(), '\"'), val.end());
        options.insert({key, val});
      }
    }
  }

  if (xacc::hasService<quantum::GateInstruction>(instructionName)) {
    auto tmpInst = irProvider->createInstruction(instructionName, bits, params);
    function->addInstruction(tmpInst);
  } else if (xacc::hasService<IRGenerator>(instructionName) ||
             xacc::hasContributedService<IRGenerator>(instructionName)) {
    auto generator =
        xacc::getService<xacc::IRGenerator>(instructionName, false);
    if (!generator) {
      generator = xacc::getContributedService<IRGenerator>(instructionName);
      if (!generator) {
        xacc::error("Cannot create IRGenerator with name " + instructionName);
      }
    }
    for (auto& kv : options) generator->setOption(kv.first, kv.second);
    if (generator->validateOptions()) {
    auto genF = generator->generate(options);
    function->addInstruction(genF);
    } else {
    function->addInstruction(generator);
    }
  } else if (xacc::hasCompiled(instructionName)) {
    auto f = xacc::getCompiled(instructionName);
    function->addInstruction(f);
  }
}

} // namespace xacc
