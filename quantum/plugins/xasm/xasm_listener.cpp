#include "IR.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "expression_parsing_util.hpp"

#include "xasmBaseListener.h"
#include "xasm_listener.hpp"

#include <regex>

using namespace xasm;

namespace xacc {

XASMListener::XASMListener() {
  irProvider = xacc::getService<IRProvider>("quantum");
}

void XASMListener::enterXacckernel(xasmParser::XacckernelContext *ctx) {
  bufferName = ctx->acceleratorbuffer->getText();
  std::vector<std::string> variables;
  // First argument should be the buffer
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    variables.push_back(ctx->typedparam(i)->id()->getText());
  }
  function = irProvider->createComposite(ctx->kernelname->getText(), variables);
}

void XASMListener::enterXacclambda(xasmParser::XacclambdaContext *ctx) {
  bufferName = ctx->acceleratorbuffer->getText();
  std::vector<std::string> variables;
  function = irProvider->createComposite("tmp_lambda", variables);
}

void XASMListener::enterInstruction(xasmParser::InstructionContext *ctx) {

  auto instructionName = ctx->inst_name->getText();
  xacc::trim(instructionName);

  if (instructionName == "CX") {
    instructionName = "CNOT";
  } else if (instructionName == "MEASURE") {
    instructionName = "Measure";
  }

  auto nRequiredBits = irProvider->getNRequiredBits(instructionName);
  std::vector<std::size_t> bits;

  for (int i = 0; i < nRequiredBits; i++) {
    if (ctx->bits_and_params->exp(i)->bufferIndex() != nullptr) {
      bits.push_back(std::stoi(
          ctx->bits_and_params->exp(i)->bufferIndex()->INT()->getText()));
    }
  }

  auto parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  std::vector<InstructionParameter> params;
  for (int i = nRequiredBits; i < ctx->bits_and_params->exp().size(); i++) {
    auto paramStr = ctx->bits_and_params->exp(i)->getText();
    if (paramStr.find('[') != std::string::npos) {
      auto location = paramStr.find_first_of('[');
      auto varName = paramStr.substr(0, location);
      paramStr.erase(std::remove(paramStr.begin(), paramStr.end(), '['),
                     paramStr.end());
      paramStr.erase(std::remove(paramStr.begin(), paramStr.end(), ']'),
                     paramStr.end());
      auto existingVars = function->getVariables();
      if (std::find(existingVars.begin(), existingVars.end(), varName) !=
          std::end(existingVars)) {
        function->replaceVariable(varName, paramStr);
      } else {
        function->addVariable(paramStr);
      }
    }
    double value;
    if (parsingUtil->isConstant(paramStr, value)) {
      params.emplace_back(value);
    } else {
      params.emplace_back(paramStr);
    }
  }

  HeterogeneousMap options;
  if (ctx->options != nullptr) {
    int nOptions = ctx->options->optionsType().size();
    for (int i = 0; i < nOptions; i++) {
      auto key = ctx->options->optionsType(i)->key->getText();
      key.erase(remove(key.begin(), key.end(), '\"'), key.end());
      std::string val = ctx->options->optionsType(i)->value->getText();

      double value;
      if (parsingUtil->isConstant(val, value)) {
        if (ctx->options->optionsType(i)->value->INT() != nullptr) {
          int tmp = (int)value;
          options.insert(key, tmp);
        } else {
          options.insert(key, value);
        }
      } else {
        val.erase(remove(val.begin(), val.end(), '\"'), val.end());
        options.insert(key, val);
      }
    }
  }

  options.insert("param_id", param_id);

  // This will search services, contributed services, and compiled
  // compositeinstructions and will call xacc::error if it does not find it.
  auto tmpInst = irProvider->createInstruction(instructionName, bits, params);
  if (tmpInst->isComposite()) {
    if (params.size() > 1) {
      options.get_mutable<std::string>("param_id") =
          params[1].toString(); // first one is qbit
    }
    auto comp = std::dynamic_pointer_cast<CompositeInstruction>(tmpInst);
    comp->expand(options);
    for (auto v : comp->getVariables()) {
      function->addVariable(v);
    }
  } else if (tmpInst->isParameterized()) {
    for (auto &p : tmpInst->getParameters()) {
      if (p.isVariable()) {
        if (!parsingUtil->validExpression(p.toString(),
                                          function->getVariables())) {
          function->addVariable(p.toString());
        }
      }
    }
  }

  function->addInstruction(tmpInst);
}

} // namespace xacc
