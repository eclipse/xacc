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
#include "exprtk_parsing_util.hpp"

#include "exprtk.hpp"
using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

namespace xacc {

bool ExprtkExpressionParsingUtil::validExpression(
    const std::string exprStr, const std::vector<std::string> variables) {

  // First double check if we can evaluate
  // this parameter to a constant, i.e. this
  // variable string contains only constants
  symbol_table_t symbol_table;
  symbol_table.add_constants();
  expression_t expr;
  expr.register_symbol_table(symbol_table);
  parser_t parser;

  // ok, now we should check that the expression
  // contains only our variables
  std::vector<double> tmp(variables.size());
  for (int i = 0; i < variables.size(); i++) {
    symbol_table.add_variable(variables[i], tmp[i]);
  }
  expression_t expr2;
  expr2.register_symbol_table(symbol_table);
  parser_t parser2;
  if (parser2.compile(exprStr, expr2)) {
    // This had only variables we know
    return true;
  }
  return false;
}
bool ExprtkExpressionParsingUtil::isConstant(const std::string exprStr,
                                             double &ref) {

  symbol_table_t symbol_table;
  symbol_table.add_constants();
  expression_t expr;
  expr.register_symbol_table(symbol_table);
  parser_t parser;
  if (parser.compile(exprStr, expr)) {
    ref = expr.value();
    return true;
  }
  return false;
}
bool ExprtkExpressionParsingUtil::evaluate(
    const std::string expression, const std::vector<std::string> variables,
    const std::vector<double> variableValues, double &ref) {

  std::vector<double> p = variableValues;
  symbol_table_t symbol_table;
  symbol_table.add_constants();
  std::vector<std::string> variableNames;
  std::vector<double> values;
  for (int i = 0; i < variables.size(); i++) {
    symbol_table.add_variable(variables[i], p[i]);
  }

  expression_t expr;
  expr.register_symbol_table(symbol_table);
  parser_t parser;
  if (parser.compile(expression, expr)) {
    ref = expr.value();
    return true;
  }
  return false;
}

} // namespace xacc