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