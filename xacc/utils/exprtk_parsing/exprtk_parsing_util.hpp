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