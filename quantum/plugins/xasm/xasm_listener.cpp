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
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "expression_parsing_util.hpp"
#include "InstructionIterator.hpp"

#include "xasmBaseListener.h"
#include "xasm_listener.hpp"

#include <regex>

using namespace xasm;

namespace xacc {

XASMListener::XASMListener() {
  irProvider = xacc::getService<IRProvider>("quantum");
  parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
}

void XASMListener::enterXacckernel(xasmParser::XacckernelContext *ctx) {
  std::vector<std::string> variables,
      validTypes{"double", "float", "std::vector<double>", "int"};
  // First argument should be the buffer
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    if (xacc::container::contains(validTypes,
                                  ctx->typedparam(i)->type()->getText())) {
      variables.push_back(ctx->typedparam(i)->id()->getText());
    } else {
      functionBufferNames.push_back(ctx->typedparam(i)->id()->getText());
    }
  }
  function = irProvider->createComposite(ctx->kernelname->getText(), variables);
}

void XASMListener::enterXacclambda(xasmParser::XacclambdaContext *ctx) {
  //   bufferName = ctx->acceleratorbuffer->getText();
  std::vector<std::string> variables;
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    variables.push_back(ctx->typedparam(i)->id()->getText());
  }
  function = irProvider->createComposite("tmp_lambda", variables);
}

void XASMListener::enterForstmt(xasmParser::ForstmtContext *ctx) {
  inForLoop = true;
  for_function =
      irProvider->createComposite("tmp_for", function->getVariables());
}
std::vector<std::size_t>
XASMListener::for_stmt_update_bits(Instruction *inst, const std::string varName,
                                   const int i) {
  std::vector<std::size_t> new_bits;
  int counter = 0;
  for (auto &b : inst->bits()) {
    if (b == -1) {
      auto bitExpr = inst->getBitExpression(counter);
      double value;
      if (parsingUtil->evaluate(bitExpr, {varName}, {(double)i}, value)) {
        new_bits.push_back((int)value);
      } else {
        xacc::error(
            inst->name() +
            ", Could not parse or evaluate bit idx expression: " + bitExpr);
      }
    } else {
      new_bits.push_back(b);
    }
    counter++;
  }
  return new_bits;
}

std::vector<InstructionParameter>
XASMListener::for_stmt_update_params(Instruction *inst,
                                     const std::string varName, const int i) {
  std::vector<InstructionParameter> new_params;
  for (auto &p : inst->getParameters()) {
    if (p.toString().find(varName) != std::string::npos) {

      auto newstr = std::regex_replace(p.toString(), std::regex(varName),
                                       std::to_string(i));

      if (xacc::container::contains(function->getVariables(), p.toString())) {
        function->replaceVariable(p.toString(), newstr);
      } else {
        function->addVariable(newstr);
      }
      new_params.push_back(newstr);
    } else {
      new_params.push_back(p);
    }
  }
  return new_params;
}
void XASMListener::exitForstmt(xasmParser::ForstmtContext *ctx) {
  auto start = std::stoi(ctx->start->getText());
  auto end = std::stoi(ctx->end->getText());
  auto varName = ctx->varname->getText();
  auto comp = ctx->comparator->getText();
  auto inc_or_dec = ctx->inc_or_dec->getText();

  std::vector<InstPtr> instructions;
  if (comp == "<") {

    for (std::size_t i = start; i < end;) {
      InstructionIterator iter(for_function);
      while (iter.hasNext()) {
        auto next = iter.next();
        if (next->isComposite()) {
          continue;
        }

        auto new_bits = for_stmt_update_bits(next.get(), varName, i);
        auto new_params = for_stmt_update_params(next.get(), varName, i);

        auto copy =
            irProvider->createInstruction(next->name(), new_bits, new_params);
        copy->setBufferNames(next->getBufferNames());
        instructions.push_back(copy);
      }

      if (inc_or_dec == "++") {
        i++;
      } else {
        i--;
      }
    }
  } else {
    for (std::size_t i = start; i > end;) {
      InstructionIterator iter(for_function);
      while (iter.hasNext()) {
        auto next = iter.next();
        if (next->isComposite()) {
          continue;
        }
        auto new_bits = for_stmt_update_bits(next.get(), varName, i);
        auto new_params = for_stmt_update_params(next.get(), varName, i);

        auto copy =
            irProvider->createInstruction(next->name(), new_bits, new_params);
        copy->setBufferNames(next->getBufferNames());
        instructions.push_back(copy);
      }

      if (inc_or_dec == "++") {
        i++;
      } else {
        i--;
      }
    }
  }
  inForLoop = false;
  //   function->clear();
  function->addInstructions(instructions);
  currentBitIdxExpressions.clear();
}

void XASMListener::enterIfstmt(xasmParser::IfstmtContext *ctx) {
  inIfStmt = true;
  auto buffer_name = ctx->id()->getText();
  std::size_t idx = std::stoi(ctx->idx->getText());
  std::cout << "IDX:  " << idx << "\n";
  if_stmt = xacc::ir::asComposite(irProvider->createInstruction(
      "ifstmt", std::vector<std::size_t>{idx}, {buffer_name}));
}

void XASMListener::exitIfstmt(xasmParser::IfstmtContext *ctx) {
  function->addInstruction(if_stmt);
  inIfStmt = false;
  //   if_stmt->clear();
}

void XASMListener::enterInstruction(xasmParser::InstructionContext *ctx) {
  currentInstructionName = ctx->inst_name->ID()->getText();
  xacc::debug("[XasmCompiler] Current Instruction Name: " +
              currentInstructionName);
}
void XASMListener::enterBufferList(xasmParser::BufferListContext *ctx) {
  auto nBits = ctx->bufferIndex().size();
  for (int i = 0; i < nBits; i++) {
    auto name = ctx->bufferIndex(i)->buffer_name->ID()->getText();

    // We may have something like Ry(q[0], x[0]) which will cause this
    // code to add x as a buffer name even though it is a std::vector<double>.
    // So here we make sure that name is in functionBufferNames and if not, then
    // we assume it is a parameter
    if (!functionBufferNames.empty() &&
        !xacc::container::contains(functionBufferNames, name)) {
      xacc::debug("[XasmCompiler] Found parameter in buffer list. " + name);

      // FIXME HANDLE things like x[i] or x[i+1]
      auto newVar = name + ctx->bufferIndex(i)->idx->getText();

      // If x is in existingVars (like std::vector<double> x) then
      // replace it with newVar
      if (xacc::container::contains(function->getVariables(), name)) {
        function->replaceVariable(name, newVar);
        if (inForLoop)
          for_function->replaceVariable(name, newVar);
        if (inIfStmt)
          if_stmt->replaceVariable(name, newVar);
      } else {
        function->addVariable(newVar);
        if (inForLoop)
          for_function->addVariable(newVar);
        if (inIfStmt)
          if_stmt->addVariable(newVar);
      }
      currentParameters.push_back(newVar);
      return;
    }

    currentBufferNames.push_back(name);
    if (ctx->bufferIndex(i)->idx->INT() != nullptr) {
      auto bit_idx_str = ctx->bufferIndex(i)->idx->getText();
      std::size_t bit = std::stoi(bit_idx_str);
      currentBits.push_back(bit);
    } else {
      // this was a variable bit for a forstmt, add a -1
      currentBits.push_back(-1);
      // Map bit idx to the expression
      currentBitIdxExpressions.insert({i, ctx->bufferIndex(i)->idx->getText()});
    }
  }
}

void XASMListener::enterParamList(xasmParser::ParamListContext *ctx) {
  for (int i = 0; i < ctx->parameter().size(); i++) {
    auto param = ctx->parameter(i)->exp();
    if (param->var_name != nullptr) {
      // this param is a x[0]-like parameter
      // our goal here is to replace x[0] with x0
      // and update the function variable list accordingly
      std::string newVar = param->var_name->getText() + param->idx->getText();
      auto existingVars = function->getVariables();

      // If x is in existingVars (like std::vector<double> x) then
      // replace it with newVar
      if (xacc::container::contains(existingVars, param->var_name->getText())) {
        function->replaceVariable(param->var_name->getText(), newVar);
      } else {
        function->addVariable(newVar);
      }

      currentParameters.push_back(newVar);
    } else {

      xacc::debug("[XasmCompiler] Parameter was " + param->getText());
      // This could be like just a string, or an expression like pi/2
      double value;
      if (parsingUtil->isConstant(param->getText(), value)) {
        xacc::debug("[XasmCompiler] Parameter added is " +
                    std::to_string(value));
        currentParameters.emplace_back(value);
      } else {
        xacc::debug("[XasmCompiler] Parameter added is " + param->getText());
        currentParameters.emplace_back(param->getText());
      }
    }
  }
}

void XASMListener::exitInstruction(xasmParser::InstructionContext *ctx) {
  auto inst = irProvider->createInstruction(currentInstructionName, currentBits,
                                            currentParameters);
  if (!currentBitIdxExpressions.empty()) {
    for (auto &kv : currentBitIdxExpressions) {
      inst->setBitExpression(kv.first, kv.second);
    }
  }

  inst->setBufferNames(currentBufferNames);

  if (inForLoop) {
    for_function->addInstruction(inst);
  } else if (inIfStmt) {
    if_stmt->addInstruction(inst);
  } else {
    function->addInstruction(inst);
  }

  // clear all current data
  currentInstructionName = "";
  currentBits.clear();
  currentParameters.clear();
  currentBufferNames.clear();
}

void XASMListener::enterComposite_generator(
    xasmParser::Composite_generatorContext *ctx) {
  currentCompositeName = ctx->composite_name->getText();
  xacc::debug("[XasmCompiler] Creating composite with name " +
              currentCompositeName);
}

void XASMListener::enterOptionsType(xasmParser::OptionsTypeContext *ctx) {

  if (ctx->key != nullptr) {
    auto key = ctx->key->getText();
    auto val = ctx->value;

    auto valStr = ctx->value->getText();

    key.erase(remove(key.begin(), key.end(), '\"'), key.end());
    xacc::debug("Entered Options Type with key/value = " + key + "," + valStr);

    double value;
    if (parsingUtil->isConstant(valStr, value)) {
      if (val->INT() != nullptr) {
        int tmp = (int)value;
        currentOptions.insert(key, tmp);
      } else {
        currentOptions.insert(key, value);
      }
    } else {
      valStr.erase(remove(valStr.begin(), valStr.end(), '\"'), valStr.end());
      if (currentOptions.stringExists(key)) {
        bool isConstChar = currentOptions.keyExists<const char *>(key);
        if (isConstChar) {
          currentOptions.get_mutable<const char *>(key) = valStr.c_str();
        } else {
          currentOptions.get_mutable<std::string>(key) = valStr;
        }

      } else {
        currentOptions.insert(key, valStr);
      }
    }
  }
}

void XASMListener::exitComposite_generator(
    xasmParser::Composite_generatorContext *ctx) {

  auto tmp = irProvider->createInstruction(currentCompositeName, {});
  auto composite = std::dynamic_pointer_cast<CompositeInstruction>(tmp);

  // Treat parameters as variables
  for (auto &a : currentParameters) {
    composite->addVariable(a.toString());
  }

  if (!composite->expand(currentOptions)) {
    xacc::debug("[XasmCompiler] Could not expand composite " +
                currentCompositeName);
  }

  function->addInstruction(composite);

  currentCompositeName = "";
  currentOptions.clear();
}

/*
void XASMListener::enterInstruction(xasmParser::InstructionContext *ctx) {
  if (!inForLoop) {
    auto instructionName = ctx->inst_name->getText();
    xacc::trim(instructionName);

    if (instructionName == "CX") {
      instructionName = "CNOT";
    } else if (instructionName == "MEASURE") {
      instructionName = "Measure";
    }

    auto nRequiredBits = irProvider->getNRequiredBits(instructionName);
    std::vector<std::size_t> bits;
    std::vector<std::string> bufferNames;

    for (int i = 0; i < nRequiredBits; i++) {
      if (ctx->bits_and_params->exp(i)->bufferIndex() != nullptr) {
        if (ctx->bits_and_params->exp(i)->bufferIndex()->exp()->INT() !=
            nullptr) {
          bits.push_back(std::stoi(ctx->bits_and_params->exp(i)
                                       ->bufferIndex()
                                       ->exp()
                                       ->INT()
                                       ->getText()));
          bufferNames.push_back(
              ctx->bits_and_params->exp(i)->bufferIndex()->id()->getText());
          //   std::cout << "BUFFER NAME IS " <<
          //   ctx->bits_and_params->exp(i)->bufferIndex()->id()->getText() <<
          //   "\n";
        }
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
            bool isConstChar = runtimeOptions.keyExists<const char *>(key);
            if (isConstChar) {
              runtimeOptions.get_mutable<const char *>(key) = val.c_str();
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
    tmpInst->setBufferNames(bufferNames);
    if (tmpInst->isComposite()) {
      if (params.size() > 1) {
        runtimeOptions.get_mutable<std::string>("param_id") =
            params[1].toString(); // first one is qbit
      }

      auto comp = std::dynamic_pointer_cast<CompositeInstruction>(tmpInst);
      // If this composite inst has parameters that match
      // any of the function vars, then we need to add them here
      auto vars = function->getVariables();
      for (auto &p : params) {
        if (std::find(vars.begin(), vars.end(), p.toString()) !=
            std::end(vars)) {
          comp->addVariable(p.toString());
        }
      }
      comp->expand(runtimeOptions);
    }

    function->addInstruction(tmpInst);

  } else {
    forInstructions.push_back(ctx);
  }
  //   std::cout << "XASM: " << function->getVariables() << "\n";
}*/

} // namespace xacc
