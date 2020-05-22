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
#include "AnnealingProgram.hpp"
#include "Instruction.hpp"
#include "xacc_service.hpp"

#include "IRProvider.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

namespace xacc {
namespace quantum {

void AnnealingProgram::throwIfInvalidInstructionParameter(InstPtr instruction) {
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
                     "' not valid for this AnnealingProgram.\nValid variables are:\n[";
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

std::shared_ptr<Graph> AnnealingProgram::toGraph() {
  int maxBit = 0;
  for (int i = 0; i < nInstructions(); ++i) {
    auto inst = getInstruction(i);
    auto bits = inst->bits();
    if (bits[0] > maxBit) {
      maxBit = bits[0];
    }
    if (bits[1] > maxBit) {
      maxBit = bits[1];
    }
  }

  auto graph = xacc::getService<Graph>("boost-ugraph");
  for (int i = 0; i < maxBit + 1; i++) {
    HeterogeneousMap props{std::make_pair("bias", 0.0)};
    graph->addVertex(props); //
  }

  for (int i = 0; i < nInstructions(); ++i) {
    auto inst = getInstruction(i);
    auto bits = inst->bits();
    if (bits[0] == bits[1]) {
      auto p = inst->getParameter(0);
      graph->getVertexProperties(bits[0]).insert("bias", xacc::InstructionParameterToDouble(p));//p.as<double>();
    } else {
      graph->addEdge(bits[0], bits[1], xacc::InstructionParameterToDouble(inst->getParameter(0)));
    }
  }

  return graph;
}

void AnnealingProgram::persist(std::ostream &outStream) {
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  writer.StartObject(); // start root object
  writer.String("function");
  writer.String(name());

  writer.String("instructions");
  writer.StartArray();

  for (auto &i : getInstructions()) {
    writer.StartObject();
    writer.String("instruction");
    writer.String(i->name().c_str());
    writer.String("enabled");
    writer.Bool(i->isEnabled());
    writer.String("qubits");
    writer.StartArray();
    for (auto qi : i->bits()) {
      writer.Int(qi);
    }
    writer.EndArray();

    writer.String("parameters");
    writer.StartArray();
    for (auto p : i->getParameters()) {
      switch (p.which()) {
      case 0:
        writer.Int(p.as<int>());
        break;
      case 1:
        writer.Double(p.as<double>());
        break;
      case 2:
        writer.String(p.as<std::string>());
        break;
      default:
        writer.String(p.toString());
      }
    }
    writer.EndArray();

    writer.EndObject();
  }

  // End Instructions
  writer.EndArray();

  // End Function
  writer.EndObject();

  outStream << buffer.GetString();

}

void AnnealingProgram::load(std::istream &inStream) {

  auto provider = xacc::getService<IRProvider>("quantum");
  std::string json(std::istreambuf_iterator<char>(inStream), {});
//   std::cout << "JSON: " << json << "\n";

  Document doc;
  doc.Parse(json);

//   auto &kernel = doc["kernels"].GetArray()[0];
  this->_name = doc["function"].GetString();
  auto instructionsArray = doc["instructions"].GetArray();

  for (int i = 0; i < instructionsArray.Size(); i++) {
    auto &inst = instructionsArray[i];
    auto gname = inst["instruction"].GetString();
    if (std::string(gname) == "dw-qmi") {
        gname = "qmi";
    }

    std::vector<std::size_t> qbits;
    auto bitsArray = inst["qubits"].GetArray();
    for (int k = 0; k < bitsArray.Size(); k++) {
      qbits.push_back(bitsArray[k].GetInt());
    }

    std::vector<InstructionParameter> local_parameters;
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

    std::shared_ptr<Instruction> instToAdd;

     instToAdd =
        provider->createInstruction(gname, qbits, local_parameters);

    if (!inst["enabled"].GetBool()) {
      instToAdd->disable();
    }

    addInstruction(instToAdd);
  }
}


std::shared_ptr<CompositeInstruction>
AnnealingProgram::operator()(const std::vector<double> &params) {
  if (params.size() != variables.size()) {
    xacc::XACCLogger::instance()->error(
        "Invalid AnnealingProgram evaluation: number "
        "of parameters don't match. " +
        std::to_string(params.size()) + ", " +
        std::to_string(variables.size()));
    exit(0);
  }

  auto evaluated = std::make_shared<AnnealingProgram>("evaled_" + name());

  // Walk the IR Tree, handle functions and instructions differently
  for (auto inst : getInstructions()) {
    if (inst->isComposite()) {
      // If a Function, call this method recursively
      auto evaled =
          std::dynamic_pointer_cast<AnnealingProgram>(inst)->operator()(params);
      evaluated->addInstruction(evaled);
    } else {
      // If a concrete Gate, then check that it
      // is parameterized and that it has a string parameter
      if (inst->isParameterized() && inst->getParameter(0).isVariable()) {
        InstructionParameter p = inst->getParameter(0);
        std::stringstream s;
        s << p.toString();
        double val;
        parsingUtil->evaluate(p.toString(), variables, params, val);
        // auto val = compileExpression(p);
        auto updatedInst = std::dynamic_pointer_cast<DWQMI>(inst)->clone();
        updatedInst->setParameter(0, val);
        updatedInst->setBits(inst->bits());
        updatedInst->setParameter(0, val);
        evaluated->addInstruction(updatedInst);
      } else {
        evaluated->addInstruction(inst);
      }
    }
  }
  return evaluated;
}
} // namespace quantum
} // namespace xacc
