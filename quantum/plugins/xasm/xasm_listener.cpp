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
for (int i = 0; i < ctx->typedparam().size(); i++) {
    variables.push_back(ctx->typedparam(i)->id()->getText());
  }
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

//   HeterogeneousMap options;
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
          runtimeOptions.insert(key, tmp);
        } else {
          runtimeOptions.insert(key, value);
        }
      } else {
        val.erase(remove(val.begin(), val.end(), '\"'), val.end());
        if (runtimeOptions.stringExists(key)) {
            bool isConstChar = runtimeOptions.keyExists<const char*>(key);
            if (isConstChar) {
                runtimeOptions.get_mutable<const char*>(key) = val.c_str();
            } else {
                runtimeOptions.get_mutable<std::string>(key) = val;
            }

        } else {
          runtimeOptions.insert(key, val);
        }
      }
    }
  }

  runtimeOptions.insert("param_id", param_id);

  // This will search services, contributed services, and compiled
  // compositeinstructions and will call xacc::error if it does not find it.
  auto tmpInst = irProvider->createInstruction(instructionName, bits, params);
  if (tmpInst->isComposite()) {
    if (params.size() > 1) {
      runtimeOptions.get_mutable<std::string>("param_id") =
          params[1].toString(); // first one is qbit
    }
    auto comp = std::dynamic_pointer_cast<CompositeInstruction>(tmpInst);
    comp->expand(runtimeOptions);

  }
  //else if (tmpInst->isParameterized()) {
    // for (auto &p : tmpInst->getParameters()) {
    //   if (p.isVariable()) {
    //     if (!parsingUtil->validExpression(p.toString(),
    //                                       function->getVariables())) {
    //     //   function->addVariable(p.toString());
    //     }
    //   }
    // }
//   }

  function->addInstruction(tmpInst);
//   std::cout << "XASM: " << function->getVariables() << "\n";
}

} // namespace xacc
