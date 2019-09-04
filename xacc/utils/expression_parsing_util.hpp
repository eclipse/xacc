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
#ifndef XACC_EXPR_PARSING_HPP_
#define XACC_EXPR_PARSING_HPP_

#include <string>
#include <vector>

#include "Identifiable.hpp"

namespace xacc {
class ExpressionParsingUtil : public Identifiable {
public:
  virtual bool validExpression(const std::string expr,
                               const std::vector<std::string> variables) = 0;
  virtual bool isConstant(const std::string expr, double &ref) = 0;
  virtual bool evaluate(const std::string expr,
                        const std::vector<std::string> variables,
                        const std::vector<double> variableValues,
                        double &ref) = 0;
};
} // namespace xacc
#endif