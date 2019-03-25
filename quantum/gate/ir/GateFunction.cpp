#include "GateFunction.hpp"
#include <algorithm>
#include <ctype.h>
#include <string>
#include "InstructionIterator.hpp"
#include "IRToGraphVisitor.hpp"

namespace xacc {
namespace quantum {

void GateFunction::mapBits(std::vector<int> bitMap) {
  for (auto i : instructions) {
    i->mapBits(bitMap);
  }
}

const int GateFunction::nInstructions() { return instructions.size(); }

std::list<InstPtr> GateFunction::getInstructions() { return instructions; }

const std::string GateFunction::name() const { return functionName; }

const std::vector<int> GateFunction::bits() { return std::vector<int>{}; }

// lambda functions for determining if an InstructionParameter is a
// number/double or a variable
auto isInt = [](std::string s) {
  try {
    std::stoi(s);
    return true;
  } catch (...) {
    return false;
  }
};
auto isDouble = [](std::string s) {
  try {
    std::stod(s);
    return true;
  } catch (...) {
    return false;
  }
};
auto isNumber = [](std::string s) {
  if (isInt(s) || isDouble(s)) {
    return true;
  } else {
    return false;
  }
};
// Lambda function for extracting the stripped (no mathematical operators or
// numbers/doubles) parameter
auto splitParameter = [](InstructionParameter instParam) {
  std::vector<std::string> split;
  InstructionParameter rawParam;
  auto paramStr = mpark::get<std::string>(instParam);
  std::replace(mpark::get<std::string>(instParam).begin(), mpark::get<std::string>(instParam).end(), '-', ' ');
  std::replace(mpark::get<std::string>(instParam).begin(), mpark::get<std::string>(instParam).end(), '+', ' ');
  std::replace(mpark::get<std::string>(instParam).begin(), mpark::get<std::string>(instParam).end(), '*', ' ');
  std::replace(mpark::get<std::string>(instParam).begin(), mpark::get<std::string>(instParam).end(), '/', ' ');

  auto instParamStr = instParam.as<std::string>();
  split = xacc::split(instParamStr, ' ');
  for (auto s : split) {
    if (!isNumber(s) && !s.empty()) {
      rawParam = s;
    }
  }
  return rawParam;
};

void GateFunction::removeInstruction(const int idx) {
  auto instruction = getInstruction(idx);
  // Check to see if instruction being removed is parameterized
  if (instruction->isParameterized() &&
      instruction->getParameter(0).isVariable()) {
    // Get InstructionParameter of instruction being removed
    bool dupParam = false;
    // strip the parameter of mathematical operators and numbers/doubles
    InstructionParameter strippedParam =
        splitParameter(instruction->getParameter(0));
    // check if the InstructionParameter is a duplicate
    for (auto i : instructions) {
      if (i->isParameterized() &&
          strippedParam == splitParameter(i->getParameter(0)) &&
          instruction != i) {
        dupParam = true;
      }
    }
    // If there are no parameters shared, then remove the parameter
    if (!dupParam) {
      parameters.erase(
          std::remove(parameters.begin(), parameters.end(), strippedParam),
          parameters.end());
    }
  }
  // Remove instruction
  instructions.remove(getInstruction(idx));
}

void GateFunction::addInstruction(InstPtr instruction) {
  // Check to see if new GateInstruction is parameterized and there is only 1
  // parameter
  if (instruction->isParameterized() && instruction->nParameters() <= 1) {
    xacc::InstructionParameter param = instruction->getParameter(0);
    // Check to see if parameter is a string
    if (param.isVariable()) {
      // check to see if the new parameter is a duplicate parameter
      bool dupParam = false;
      // strip the parameter of mathematical operators and numbers/doubles
      InstructionParameter strippedParam = splitParameter(param);
      // check if the instruction parameter is a duplicate
      for (auto p : parameters) {
        if (p.as<std::string>() == strippedParam.as<std::string>()) {
          dupParam = true;
        }
      }
      // if new parameter is not a duplicate, add the stripped version
      if (!dupParam) {
        parameters.push_back(strippedParam);
      }
    }
  }
  // Add the GateInstruction
  instructions.push_back(instruction);
}

void GateFunction::replaceInstruction(const int idx, InstPtr replacingInst) {
  auto currentInst = getInstruction(idx);
  // Check if the GateInstruction being replaced is parameterized and if
  // parameter is a string
  if (currentInst->isParameterized() &&
      currentInst->getParameter(0).isVariable()) {
    // strip the current InstructionParameter of mathematical operators and
    // numbers
    InstructionParameter strippedCurrent =
        splitParameter(currentInst->getParameter(0));
    bool dupParam = false;
    for (auto i : instructions) {
      // see if current instruction has a duplicate parameter
      if (strippedCurrent == splitParameter(i->getParameter(0)) &&
          currentInst != i) {
        dupParam = true;
      }
    }
    // Check if new instruction is parameterized and if parameter is a string
    if (replacingInst->isParameterized() &&
        replacingInst->getParameter(0).isVariable()) {
      InstructionParameter strippedNew =
          splitParameter(replacingInst->getParameter(0));
      // Check if current parameter is a duplicate parameter
      bool newDupParam = false;
      for (auto i : instructions) {
        // see if new param has a duplicate parameter
        if (strippedNew == splitParameter(i->getParameter(0))) {
          newDupParam = true;
        }
      }
      // Check if old GateInstruction parameter is different than new
      // GateInstruction parameter
      if (strippedCurrent != strippedNew) {
        if (!dupParam) {
          if (!newDupParam) {
            // if the current GateInstruction and the new GateInstruction both
            // do not have a parameter already in this GateFunction -> replace
            // parameter
            std::replace(parameters.begin(), parameters.end(), strippedCurrent,
                         strippedNew);
          } else {
            // If the current GateInstruction is not a duplicate but the new one
            // is  -> remove old parameter
            parameters.erase(std::remove(parameters.begin(), parameters.end(),
                                         strippedCurrent));
          }
        } else {
          // if the current GateInstruction is a duplicate but the new one is
          // not -> add new parameter
          if (!newDupParam) {
            parameters.push_back(strippedNew);
          }
        }
      }
      // if the current GateInstruction parameter is parameterized but the new
      // one is not, check if the parameter is a duplicate and erase (or not)
    } else if (!dupParam) {
      parameters.erase(
          std::remove(parameters.begin(), parameters.end(), strippedCurrent));
    }
    // if the current GateInstruction is not parameterized and the new
    // GateInstruction is, try to add parameter
  } else if (replacingInst->isParameterized() &&
             replacingInst->getParameter(0).isVariable()) {
    InstructionParameter strippedNew =
        splitParameter(replacingInst->getParameter(0));
    // Check if new parameter is a duplicate parameter
    bool newDupParam = false;
    for (auto i : instructions) {
      if (i->isParameterized() &&
          strippedNew == splitParameter(i->getParameter(0))) {
        newDupParam = true;
      }
    }
    // if new parameter is not a duplicate, add it to the parameters list
    if (!newDupParam) {
      parameters.push_back(strippedNew);
    }
  }
  // Replace old GateInstruction with new GateInstruction
  std::replace(instructions.begin(), instructions.end(), getInstruction(idx),
               replacingInst);
}

void GateFunction::insertInstruction(const int idx, InstPtr newInst) {
  // Check if new GateInstruction is parameterized with 1 parameter
  if (newInst->isParameterized() && newInst->nParameters() <= 1) {
    xacc::InstructionParameter param = newInst->getParameter(0);
    // Check if new parameter is a string
    if (param.isVariable()) {
      // If new parameter is not already in parameter vector -> add parameter to
      // GateFunction
      bool dupParam = false;
      // strip the parameter of mathematical operators and numbers/doubles
      InstructionParameter strippedParam = splitParameter(param);
      for (auto p : parameters) {
        if (mpark::get<std::string>(p) ==
            mpark::get<std::string>(strippedParam)) {
          dupParam = true;
        }
      }
      // If new parameter is not already in parameter vector -> add parameter to
      // GateFunction
      if (!dupParam) {
        parameters.push_back(param);
      }
    }
  }
  // Insert new GateInstruction to instructions vector
  auto iter = std::next(instructions.begin(), idx);
  instructions.insert(iter, newInst);
}

InstPtr GateFunction::getInstruction(const int idx) {
  InstPtr i;
  if (instructions.size() > idx) {
    i = *std::next(instructions.begin(), idx);
  } else {
    xacc::error("GateFunction getInstruction invalid instruction index - " +
                std::to_string(idx) + ".");
  }
  return i;
}

void GateFunction::setParameter(const int idx, InstructionParameter &p) {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error("Invalid Parameter requested.");
  }

  parameters[idx] = p;
}

InstructionParameter GateFunction::getParameter(const int idx) const {
  if (idx + 1 > parameters.size()) {
    XACCLogger::instance()->error("Invalid Parameter requested.");
  }

  return parameters[idx];
}

void GateFunction::addParameter(InstructionParameter instParam) {
  parameters.push_back(instParam);
}

std::vector<InstructionParameter> GateFunction::getParameters() {
  return parameters;
}

bool GateFunction::isParameterized() { return nParameters() > 0; }

const int GateFunction::nParameters() { return parameters.size(); }

const std::string GateFunction::toString(const std::string &bufferVarName) {
  std::string retStr = "";
  for (auto i : instructions) {
    if (i->isEnabled())
      retStr += i->toString(bufferVarName) + "\n";
  }
  return retStr;
}

std::shared_ptr<Function> GateFunction::
operator()(const std::vector<double> &params) {
  if (params.size() != nParameters()) {
    xacc::error("Invalid GateFunction evaluation: number "
                "of parameters don't match. " +
                std::to_string(params.size()) + ", " +
                std::to_string(nParameters()));
  }

  std::vector<double> p = params;
  symbol_table_t symbol_table;
  symbol_table.add_constants();
  std::vector<std::string> variableNames;
  std::vector<double> values;
  for (int i = 0; i < params.size(); i++) {
    auto var = getParameter(i).as<std::string>();
    variableNames.push_back(var);
    symbol_table.add_variable(var, p[i]);
  }

  auto compileExpression = [&](InstructionParameter &p) -> double {
    auto expression = mpark::get<std::string>(p);
    expression_t expr;
    expr.register_symbol_table(symbol_table);
    parser_t parser;
    parser.compile(expression, expr);
    return expr.value();
  };

  auto gateRegistry = xacc::getService<IRProvider>("gate");
  auto evaluatedFunction = std::make_shared<GateFunction>("evaled_" + name());

  // Walk the IR Tree, handle functions and instructions differently
  for (auto inst : getInstructions()) {
    if (inst->isComposite()) {
      // If a Function, call this method recursively
      auto evaled =
          std::dynamic_pointer_cast<Function>(inst)->operator()(p);
      evaluatedFunction->addInstruction(evaled);
    } else {
      // If a concrete GateInstruction, then check that it
      // is parameterized and that it has a string parameter
      if (inst->isParameterized() && inst->getParameter(0).isVariable()) {
        InstructionParameter p = inst->getParameter(0);
        std::stringstream s;
        s << p.toString();
        auto val = compileExpression(p);
        InstructionParameter pnew(val);
        auto updatedInst =
            gateRegistry->createInstruction(inst->name(), inst->bits());
        updatedInst->setParameter(0, pnew);
        evaluatedFunction->addInstruction(updatedInst);
      } else {
        evaluatedFunction->addInstruction(inst);
      }
    }
  }
  return evaluatedFunction;
}

Graph<CircuitNode, Directed> GateFunction::toGraph() {

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
