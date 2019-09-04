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
#ifndef XACC_EXPRTK_EXPR_PARSING_HPP_
#define XACC_EXPRTK_EXPR_PARSING_HPP_

#include "expression_parsing_util.hpp"

namespace xacc {
class ExprtkExpressionParsingUtil : public ExpressionParsingUtil {
public:
  ExprtkExpressionParsingUtil() = default;
  bool validExpression(const std::string expr,
                       const std::vector<std::string> variables) override;
  bool isConstant(const std::string expr, double &ref) override;
  bool evaluate(const std::string expr,
                const std::vector<std::string> variables,
                const std::vector<double> variableValues, double &ref) override;
  const std::string name() const override { return "exprtk"; }
  const std::string description() const override { return ""; }
};
} // namespace xacc
#endif