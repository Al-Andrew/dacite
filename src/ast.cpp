#include "ast.h"

namespace dacite {

std::string binary_operator_to_string(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::ADD:           return "+";
        case BinaryOperator::SUBTRACT:      return "-";
        case BinaryOperator::MULTIPLY:      return "*";
        case BinaryOperator::DIVIDE:        return "/";
        case BinaryOperator::EQUAL:         return "==";
        case BinaryOperator::NOT_EQUAL:     return "!=";
        case BinaryOperator::LESS_THAN:     return "<";
        case BinaryOperator::LESS_EQUAL:    return "<=";
        case BinaryOperator::GREATER_THAN:  return ">";
        case BinaryOperator::GREATER_EQUAL: return ">=";
        default:                            return "?";
    }
}

} // namespace dacite