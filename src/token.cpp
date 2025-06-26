#include "token.h"
#include <unordered_map>

namespace dacite {

std::string_view token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::PACKAGE: return "PACKAGE";
        case TokenType::FN: return "FN";
        case TokenType::VOID: return "VOID";
        case TokenType::RETURN: return "RETURN";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TokenType::MULTIPLY_ASSIGN: return "MULTIPLY_ASSIGN";
        case TokenType::DIVIDE_ASSIGN: return "DIVIDE_ASSIGN";
        case TokenType::MODULO_ASSIGN: return "MODULO_ASSIGN";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LOGICAL_AND: return "LOGICAL_AND";
        case TokenType::LOGICAL_OR: return "LOGICAL_OR";
        case TokenType::LOGICAL_NOT: return "LOGICAL_NOT";
        case TokenType::ARROW: return "ARROW";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

bool is_keyword(TokenType type) {
    switch (type) {
        case TokenType::PACKAGE:
        case TokenType::FN:
        case TokenType::VOID:
        case TokenType::RETURN:
        case TokenType::IF:
        case TokenType::ELSE:
        case TokenType::WHILE:
        case TokenType::FOR:
        case TokenType::TRUE:
        case TokenType::FALSE:
            return true;
        default:
            return false;
    }
}

TokenType keyword_or_identifier(std::string_view text) {
    static const std::unordered_map<std::string_view, TokenType> keywords = {
        {"package", TokenType::PACKAGE},
        {"fn", TokenType::FN},
        {"void", TokenType::VOID},
        {"return", TokenType::RETURN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
    };

    auto it = keywords.find(text);
    return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
}

} // namespace dacite