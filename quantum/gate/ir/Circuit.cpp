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
#include "Circuit.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "JsonVisitor.hpp"
#include "IRProvider.hpp"
#include "IRToGraphVisitor.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {
void Circuit::throwIfInvalidInstructionParameter(InstPtr instruction) {
  if (!parsingUtil) {
    parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  }

  if (!instruction->isComposite() && instruction->isParameterized()) {
    for (int i = 0; i < instruction->nParameters(); i++) {
      auto parameter = instruction->getParameter(i);
      if (parameter.isVariable() &&
          (std::find(variables.begin(), variables.end(),
                     parameter.toString()) == variables.end())) {

        // First double check if we can evaluate
        // this parameter to a constant, i.e. this
        // variable string contains only constants
        double constant;
        if (parsingUtil->isConstant(parameter.toString(), constant)) {
          instruction->setParameter(i, constant);
          continue;
        }

        // ok, now we should check that the expression
        // contains only our variables
        std::vector<double> tmp(variables.size());
        if (parsingUtil->validExpression(parameter.toString(), variables)) {
          continue;
        }

        std::stringstream ermsg;
        ermsg << "\nInvalid parameterized instruction:\n" +
                     instruction->toString() + "\n'" + parameter.toString() +
                     "' not valid for this Circuit.\nValid variables are:\n[";
        if (!variables.empty()) {
          ermsg << variables[0];
          for (int i = 1; i < variables.size(); i++) {
            ermsg << "," << variables[i];
          }
        }
        ermsg << "]\n"
              << "Please call CompositeInstruction::addVariable(variable).\n";
        xacc::XACCLogger::instance()->error(ermsg.str());
        throw new std::runtime_error("Invalid instruction parameter.");
      }
    }
  }
}

std::shared_ptr<CompositeInstruction>
Circuit::operator()(const std::vector<double> &params) {
  if (!parsingUtil) {
    parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  }

  if (params.size() != variables.size()) {
    xacc::XACCLogger::instance()->error("Invalid Circuit evaluation: number "
                                        "of parameters don't match. " +
                                        std::to_string(params.size()) + ", " +
                                        std::to_string(variables.size()));
    exit(0);
  }

  std::vector<InstPtr> flatten;
  InstructionIterator iter(shared_from_this());
  while (iter.hasNext()) {
    auto inst = iter.next();
    if (!inst->isComposite()) {
      flatten.emplace_back(inst);
    }
  }

  auto evaluatedCircuit = std::make_shared<Circuit>("evaled_" + name());

  // Walk the IR Tree, handle functions and instructions differently
  for (auto inst : flatten) {
    if (inst->isParameterized()) {
      auto updatedInst = inst->clone();
      for (int i = 0; i < inst->nParameters(); i++) {
        if (inst->getParameter(i).isVariable()) {
          InstructionParameter p = inst->getParameter(i);
          std::stringstream s;
          double val;
          parsingUtil->evaluate(p.toString(), variables, params, val);
          updatedInst->setParameter(i, val);
        } else {
          auto a = inst->getParameter(i);
          updatedInst->setParameter(i, a);
        }
        updatedInst->setBits(inst->bits());
        if (!inst->getBufferNames().empty()) {
          updatedInst->setBufferNames(inst->getBufferNames());
        }
      }
      evaluatedCircuit->addInstruction(updatedInst);
    } else {
      evaluatedCircuit->addInstruction(inst);
    }
  }
  return evaluatedCircuit;
}

void Circuit::persist(std::ostream &outStream) {
  JsonVisitor<PrettyWriter<StringBuffer>, StringBuffer> visitor(
      shared_from_this());
  outStream << visitor.write();
}

void Circuit::load(std::istream &inStream) {

  //   std::vector<std::string> irGeneratorNames;
  //   auto irgens = xacc::getRegisteredIds<xacc::IRGenerator>();
  //   for (auto &irg : irgens) {
  //     irGeneratorNames.push_back(irg);
  //   }

  variables.clear();
  instructions.clear();

  auto provider = xacc::getService<IRProvider>("quantum");
  std::string json(std::istreambuf_iterator<char>(inStream), {});
  //   std::cout << "JSON: " << json << "\n";

  Document doc;
  doc.Parse(json);

  auto &kernel = doc["circuits"].GetArray()[0];
  circuitName = kernel["circuit"].GetString();
  auto vars = kernel["variables"].GetArray();
  for (int i = 0; i < vars.Size(); i++) {
    addVariable(vars[i].GetString());
  }

  auto coeff = kernel["coefficient"].GetDouble();
  setCoefficient(coeff);

  auto signature = kernel["accelerator_signature"].GetString();
  set_accelerator_signature(signature);

  auto instructionsArray = kernel["instructions"].GetArray();
  for (int i = 0; i < instructionsArray.Size(); i++) {
    auto &inst = instructionsArray[i];
    auto gname = inst["gate"].GetString();
    std::shared_ptr<Instruction> instToAdd;

    std::vector<std::size_t> qbits{};
    auto bitsArray = inst["qubits"].GetArray();
    for (int k = 0; k < bitsArray.Size(); k++) {
      qbits.push_back(bitsArray[k].GetUint64());
    }

    std::vector<InstructionParameter> local_parameters{};
    auto &paramsArray = inst["parameters"];
    for (int k = 0; k < paramsArray.Size(); k++) {
      auto &value = paramsArray[k];
      if (value.IsInt()) {
        local_parameters.push_back(InstructionParameter(value.GetInt()));
      } else if (value.IsDouble()) {
        local_parameters.push_back(InstructionParameter(value.GetDouble()));
      } else {
        local_parameters.push_back(InstructionParameter(value.GetString()));
      }
    }
    if (!inst["composite"].GetBool()) {
      instToAdd = provider->createInstruction(gname, qbits, local_parameters);
    } else {
      instToAdd = provider->createInstruction(gname, {}, {});
    }

    if (!inst["enabled"].GetBool()) {
      instToAdd->disable();
    }

    addInstruction(instToAdd);
  }
}

const int Circuit::depth() { return toGraph()->depth() - 2; }

const std::string Circuit::persistGraph() {
  std::stringstream s;
  toGraph()->write(s);
  return s.str();
}

std::shared_ptr<Graph> Circuit::toGraph() {

  // Compute number of qubits
  int maxBit = 0;
  InstructionIterator it(shared_from_this());
  while (it.hasNext()) {
    auto inst = it.next();
    for (auto &b : inst->bits()) {
      if (b > maxBit) {
        maxBit = b;
      }
    }
  }

  auto visitor = std::make_shared<IRToGraphVisitor>(maxBit + 1);
  InstructionIterator it2(shared_from_this());
  while (it2.hasNext()) {
    auto inst = it2.next();
    if (inst->isEnabled()) {
      inst->accept(visitor);
    }
  }

  return visitor->getGraph();
}
} // namespace quantum
} // namespace xacc