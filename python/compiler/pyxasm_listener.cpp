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

#include "pyxasm_listener.hpp"
#include "Circuit.hpp"
#include "AnnealingProgram.hpp"
#include <memory>
#include <regex>

using namespace pyxasm;

namespace xacc {

PyXASMListener::PyXASMListener() {
  irProvider = xacc::getService<IRProvider>("quantum");
}

void PyXASMListener::enterXacckernel(pyxasmParser::XacckernelContext *ctx) {
  bufferName = ctx->qbit->getText();
  std::vector<std::string> variables;
  // First argument should be the buffer
  for (int i = 0; i < ctx->param().size(); i++) {
    variables.push_back(ctx->param(i)->id()->getText());
  }
  function = irProvider->createComposite(ctx->kernelname->getText(), variables);
}

void PyXASMListener::enterInstruction(pyxasmParser::InstructionContext *ctx) {

  auto instructionName = ctx->inst_name->getText();
  xacc::trim(instructionName);

  if (instructionName == "CX") {
    instructionName = "CNOT";
  } else if (instructionName == "MEASURE") {
    instructionName = "Measure";
  } else if (instructionName == "dwqmi") {
      auto isCircuit = std::dynamic_pointer_cast<quantum::Circuit>(function);
      if (isCircuit) {
          function = irProvider->createComposite(function->name(), function->getVariables(), "anneal");
      }
  }

  auto nRequiredBits = irProvider->getNRequiredBits(instructionName);
  std::vector<std::size_t> bits;

  for (int i = 0; i < nRequiredBits; i++) {
    if (ctx->bits_and_params->exp(i)->bufferIndex() != nullptr) {
      bits.push_back(std::stoi(
          ctx->bits_and_params->exp(i)->bufferIndex()->INT()->getText()));
    }
  }

  std::vector<InstructionParameter> params;
  for (int i = nRequiredBits; i < ctx->bits_and_params->exp().size(); i++) {
    auto paramStr = ctx->bits_and_params->exp(i)->getText();
    if (paramStr.find('[') != std::string::npos) {
      auto location = paramStr.find_first_of('[');
      auto varName = paramStr.substr(0,location);
      paramStr.erase(std::remove(paramStr.begin(), paramStr.end(), '['), paramStr.end());
      paramStr.erase(std::remove(paramStr.begin(), paramStr.end(), ']'), paramStr.end());
      auto existingVars = function->getVariables();

      // If varName (x in x[0]) is in variables, replace it with x0
      if (std::find(existingVars.begin(), existingVars.end(), varName) != std::end(existingVars)) {
        function->replaceVariable(varName, paramStr);
      } else {
        function->addVariable(paramStr);
      }
    }
    params.emplace_back(paramStr);
  }

  auto parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
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

//   std::stringstream ss;
//   options.print<int>(ss);
//   std::cout <<" Options:\n" << ss.str() <<" \n";
  // This will search services, contributed services, and compiled
  // compositeinstructions and will call xacc::error if it does not find it.
  auto tmpInst = irProvider->createInstruction(instructionName, bits, params);
  if (tmpInst->isComposite()) {
    for (int p = 1; p < params.size(); p++) {
        if (params[p].isVariable()) {
            std::dynamic_pointer_cast<CompositeInstruction>(tmpInst)->addVariable(params[p].toString());
        }
    }

    if (std::dynamic_pointer_cast<quantum::AnnealingProgram>(tmpInst) && std::dynamic_pointer_cast<quantum::Circuit>(function)) {
        function = irProvider->createComposite(function->name(), function->getVariables(), "anneal");
    }
  }
  function->addInstruction(tmpInst);
  function->expand(options);
}

} // namespace xacc
