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
#include "xacc_service.hpp"
#include "expression_parsing_util.hpp"

#include "xasmBaseListener.h"
#include "xasm_listener.hpp"

#include <regex>

using namespace xasm;

namespace xacc {
void XASMListener::for_stmt_update_inst_args(Instruction *inst) {
  auto parameters = inst->getParameters();
  for (int i = 0; i < parameters.size(); i++) {
    std::cout << "HELLO: " << inst->name() << ", " << parameters[i].toString()
              << "\n";
    if (parameters[i].isVariable()) {
      auto arg = function->getArgument(parameters[i].toString());

      if (!arg) {
        auto param_str = parameters[i].toString();
        param_str.erase(
            std::remove_if(param_str.begin(), param_str.end(),
                           [](char c) { return c == ']' || c == '['; }),
            param_str.end());
        param_str.erase(std::remove_if(param_str.begin(), param_str.end(),
                                       [](char c) { return !std::isalpha(c); }),
                        param_str.end());

        std::cout << "IS THSI HRE: " << param_str << "\n";
        arg = function->getArgument(param_str);

        if (arg && arg->type.find("std::vector<double>") != std::string::npos) {
          // this was a container-like type
          // give the instruction a mapping from i to vector idx
          std::cout << "HELLO " << arg->name << "\n";

          auto tmp_str = parameters[i].toString();
          tmp_str.erase(
              std::remove_if(tmp_str.begin(), tmp_str.end(),
                             [](char c) { return c == '[' || c == ']'; }),
              tmp_str.end());
          tmp_str.erase(tmp_str.find(arg->name), arg->name.length());
          int vector_mapping = 0;
          try {
            auto vector_idx = std::stoi(tmp_str);
            vector_mapping = vector_idx;
          } catch (std::exception &e) {
            std::cout << e.what() << "\n";
            xacc::error("[xasm] could not compute vector index.");
          }
          inst->addIndexMapping(i, vector_mapping);
        }
      }

      if (!arg) {
        // we may have a case where the parameter is an expression string
        for (auto &_arg : function->getArguments()) {
          double val;
          if (parsingUtil->validExpression(parameters[i].toString(),
                                           {_arg->name})) {
            arg = _arg;
            break;
          }
        }
      }

      if (!arg) {
        xacc::error("Cannot associate function argument " +
                    parameters[i].toString() + " with this instruction " +
                    currentInstructionName);
      }
      inst->addArgument(arg, i);
    }
  }
}

template <>
void XASMListener::createForInstructions<XasmLessThan>(
    xasmParser::ForstmtContext *ctx, std::vector<InstPtr> &instructions,
    std::shared_ptr<CompositeInstruction> function) {

  auto start = std::stoi(ctx->start->getText());
  auto end = std::stoi(ctx->end->getText());
  auto varName = ctx->varname->getText();
  auto comp = ctx->comparator->getText();
  auto inc_or_dec = ctx->inc_or_dec->getText();

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

      for_stmt_update_inst_args(copy.get());

      instructions.push_back(copy);
    }

    if (inc_or_dec == "++") {
      i++;
    } else {
      i--;
    }
  }
}

template <>
void XASMListener::createForInstructions<XasmGreaterThan>(
    xasmParser::ForstmtContext *ctx, std::vector<InstPtr> &instructions,
    std::shared_ptr<CompositeInstruction> function) {

  auto start = std::stoi(ctx->start->getText());
  auto end = std::stoi(ctx->end->getText());
  auto varName = ctx->varname->getText();
  auto comp = ctx->comparator->getText();
  auto inc_or_dec = ctx->inc_or_dec->getText();
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
      for_stmt_update_inst_args(copy.get());
      instructions.push_back(copy);
    }

    if (inc_or_dec == "++") {
      i++;
    } else {
      i--;
    }
  }
}

template <>
void XASMListener::createForInstructions<XasmGreaterThanOrEqual>(
    xasmParser::ForstmtContext *ctx, std::vector<InstPtr> &instructions,
    std::shared_ptr<CompositeInstruction> function) {

  auto start = std::stoi(ctx->start->getText());
  auto end = std::stoi(ctx->end->getText());
  auto varName = ctx->varname->getText();
  auto comp = ctx->comparator->getText();
  auto inc_or_dec = ctx->inc_or_dec->getText();
  for (std::size_t i = start; i >= end;) {
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
      for_stmt_update_inst_args(copy.get());
      instructions.push_back(copy);
    }

    if (inc_or_dec == "++") {
      i++;
    } else {
      i--;
    }
  }
}

template <>
void XASMListener::createForInstructions<XasmLessThanOrEqual>(
    xasmParser::ForstmtContext *ctx, std::vector<InstPtr> &instructions,
    std::shared_ptr<CompositeInstruction> function) {

  auto start = std::stoi(ctx->start->getText());
  auto end = std::stoi(ctx->end->getText());
  auto varName = ctx->varname->getText();
  auto comp = ctx->comparator->getText();
  auto inc_or_dec = ctx->inc_or_dec->getText();
  for (std::size_t i = start; i <= end;) {
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
      for_stmt_update_inst_args(copy.get());
      instructions.push_back(copy);
    }

    if (inc_or_dec == "++") {
      i++;
    } else {
      i--;
    }
  }
}

XASMListener::XASMListener() {
  irProvider = xacc::getService<IRProvider>("quantum");
  parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
}

void XASMListener::enterXacckernel(xasmParser::XacckernelContext *ctx) {
  std::vector<std::string> variables,
      validTypes{"double", "float", "std::vector<double>", "int"};

  function = irProvider->createComposite(ctx->kernelname->getText());
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    auto type = ctx->typedparam(i)->type()->getText();
    auto vname = ctx->typedparam(i)->variable_param_name()->getText();
    function->addArgument(vname, type);
    if (type == "qreg" || type == "qbit") {
      functionBufferNames.push_back(vname);
    }
    if (xacc::container::contains(validTypes,
                                  ctx->typedparam(i)->type()->getText())) {
      variables.push_back(vname);
    }
  }
  function->addVariables(variables);
}

void XASMListener::enterXacclambda(xasmParser::XacclambdaContext *ctx) {
  std::vector<std::string> variables,
      validTypes{"double", "float", "std::vector<double>", "int"};
  function = irProvider->createComposite("tmp_lambda", variables);
  for (int i = 0; i < ctx->typedparam().size(); i++) {
    auto type = ctx->typedparam(i)->type()->getText();
    auto vname = ctx->typedparam(i)->variable_param_name()->getText();
    function->addArgument(vname, type);
    if (type == "qreg" || type == "qbit") {
      functionBufferNames.push_back(vname);
    }
    if (xacc::container::contains(validTypes,
                                  ctx->typedparam(i)->type()->getText())) {
      variables.push_back(vname);
    }
  }
  function->addVariables(variables);
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

      // Here we have something like x[i+1+2], need to eval that expr
      if (newstr.find("[") != std::string::npos) {
        auto f = newstr.find_first_of("[");
        auto e = newstr.find_first_of("]");
        auto sub = newstr.substr(f + 1, e - 2);
        double d;
        if (!parsingUtil->isConstant(sub, d)) {
          xacc::error("[XasmCompiler] Error in parsing parameter index " +
                      newstr);
        }
        newstr = newstr.substr(0, f) + "[" + std::to_string((int)d) + "]";
      }
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
  // Fixme use templates to improve this if else
  if (comp == "<") {
    createForInstructions<XasmLessThan>(ctx, instructions, for_function);
  } else if (comp == ">") {
    createForInstructions<XasmGreaterThan>(ctx, instructions, for_function);
  } else if (comp == "<=") {

  } else if (comp == ">=") {
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

      if (!inForLoop) {
        new_var_to_vector_idx.insert(
            {newVar, std::stoi(ctx->bufferIndex(i)->idx->getText())});
      }

      // Check if we have a for-loop parameterized parameter
      double ref;
      if (inForLoop &&
          !parsingUtil->isConstant(ctx->bufferIndex(i)->idx->getText(), ref)) {
        newVar = name + "[" + ctx->bufferIndex(i)->idx->getText() + "]";
      }

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

    xacc::debug("[XasmCompiler] Adding buffer name " + name);

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

      new_var_to_vector_idx.insert({newVar, std::stoi(param->idx->getText())});

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

  if (!inForLoop) {
    for (int i = 0; i < currentParameters.size(); i++) {
      if (currentParameters[i].isVariable()) {
        auto arg = function->getArgument(currentParameters[i].toString());

        if (!arg) {
          auto param_str = currentParameters[i].toString();
          param_str.erase(
              std::remove_if(param_str.begin(), param_str.end(),
                             [](char c) { return !std::isalpha(c); }),
              param_str.end());

          arg = function->getArgument(param_str);

          if (arg &&
              arg->type.find("std::vector<double>") != std::string::npos) {
            // this was a container-like type
            // give the instruction a mapping from i to vector idx

            inst->addIndexMapping(
                i, new_var_to_vector_idx[currentParameters[i].toString()]);
          }
        }

        if (!arg) {
          // we may have a case where the parameter is an expression string
          for (auto &_arg : function->getArguments()) {
            double val;
            if (parsingUtil->validExpression(currentParameters[i].toString(),
                                             {_arg->name})) {
              arg = _arg;
              break;
            }
          }
        }

        if (!arg) {
          xacc::error("Cannot associate function argument " +
                      currentParameters[i].toString() +
                      " with this instruction " + currentInstructionName);
        }
        inst->addArgument(arg, i);
      }
    }
  }

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
  new_var_to_vector_idx.clear();
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

  // check if this composite is actually a digital gate on all qubits
  // something like H(q) where q is the name of the buffer
  auto available_insts = irProvider->getInstructions();
  for (auto &i : available_insts) {
    if (irProvider->getNRequiredBits(i) == 1 && currentCompositeName == i) {
      // This is a digital gate that is to be applied to all qubits
      std::string buffer_name = ctx->buffer_name->getText();
      if (!xacc::hasBuffer(buffer_name)) {
        xacc::error("Cannot compile " + currentCompositeName + " on buffer " +
                    buffer_name + ". Could not find buffer to query size.");
      }
      auto buffer = xacc::getBuffer(buffer_name);

      auto size = buffer->size();
      for (std::size_t i = 0; i < size; i++) {
        auto inst = irProvider->createInstruction(currentCompositeName,
                                                  std::vector<std::size_t>{i});
        function->addInstruction(inst);
      }

      return;
    }
  }

  auto tmp = irProvider->createInstruction(currentCompositeName, {});
  auto composite = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
  for (int i = 0; i < currentParameters.size(); i++) {
    auto p = currentParameters[i];
    auto arg = function->getArgument(p.toString());
    int vector_mapping = 0;

    if (!arg) {
      auto param_str = currentParameters[i].toString();
      param_str.erase(std::remove_if(param_str.begin(), param_str.end(),
                                     [](char c) { return !std::isalpha(c); }),
                      param_str.end());
      arg = function->getArgument(param_str);

      if (arg && arg->type.find("std::vector<double>") != std::string::npos) {
        // this was a container-like type
        // give the instruction a mapping from i to vector idx
        vector_mapping = new_var_to_vector_idx[currentParameters[i].toString()];

      } else {
        // throw an error
        xacc::error(
            "[xasm] Error in setting arguments for composite instruction " +
            currentCompositeName);
      }
    }
    composite->addArgument(arg, vector_mapping);
  }

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

} // namespace xacc
