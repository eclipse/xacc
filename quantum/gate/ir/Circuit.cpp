#include "Circuit.hpp"

// #include "exprtk.hpp"
// using symbol_table_t = exprtk::symbol_table<double>;
// using expression_t = exprtk::expression<double>;
// using parser_t = exprtk::parser<double>;

namespace xacc {
namespace quantum {
void Circuit::throwIfInvalidInstructionParameter(InstPtr instruction) {
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
        // symbol_table_t symbol_table;
        // symbol_table.add_constants();
        // expression_t expr;
        // expr.register_symbol_table(symbol_table);
        // parser_t parser;
        // if (parser.compile(parameter.toString(), expr)) {
        //   auto value = expr.value();
        //   instruction->setParameter(i, value);
        //   continue;
        // }

        // ok, now we should check that the expression
        // contains only our variables
        std::vector<double> tmp(variables.size());
        if (parsingUtil->validExpression(parameter.toString(), variables)) {
          continue;
        }
        // for (int i = 0; i < variables.size(); i++) {
        //   symbol_table.add_variable(variables[i], tmp[i]);
        // }
        // expression_t expr2;
        // expr2.register_symbol_table(symbol_table);
        // parser_t parser2;
        // if (parser2.compile(parameter.toString(), expr2)) {
        //   // This had only variables we know
        //   continue;
        // }

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
  if (params.size() != variables.size()) {
    xacc::XACCLogger::instance()->error(
        "Invalid GateFunction evaluation: number "
        "of parameters don't match. " +
        std::to_string(params.size()) + ", " +
        std::to_string(variables.size()));
    exit(0);
  }

  //   std::vector<double> p = params;
  //   symbol_table_t symbol_table;
  //   symbol_table.add_constants();
  //   std::vector<std::string> variableNames;
  //   std::vector<double> values;
  //   for (int i = 0; i < variables.size(); i++) {
  //     auto var = variables[i];
  //     variableNames.push_back(var);
  //     symbol_table.add_variable(var, p[i]);
  //   }

  //   auto compileExpression = [&](InstructionParameter &p) -> double {
  //     auto expression = mpark::get<std::string>(p);
  //     expression_t expr;
  //     expr.register_symbol_table(symbol_table);
  //     parser_t parser;
  //     parser.compile(expression, expr);
  //     return expr.value();
  //   };

  auto evaluatedCircuit = std::make_shared<Circuit>("evaled_" + name());

  // Walk the IR Tree, handle functions and instructions differently
  for (auto inst : getInstructions()) {
    if (inst->isComposite()) {
      // If a Function, call this method recursively
      auto evaled =
          std::dynamic_pointer_cast<Circuit>(inst)->operator()(params);
      evaluatedCircuit->addInstruction(evaled);
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
        auto updatedInst = std::dynamic_pointer_cast<Gate>(inst)->clone();
        updatedInst->setParameter(0, val);
        updatedInst->setBits(inst->bits());
        updatedInst->setParameter(0, val);
        evaluatedCircuit->addInstruction(updatedInst);
      } else {
        evaluatedCircuit->addInstruction(inst);
      }
    }
  }
  return evaluatedCircuit;
}
} // namespace quantum
} // namespace xacc