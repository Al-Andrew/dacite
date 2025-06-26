#pragma once

#include <string>
#include <string_view>
#include "source_span.h"

namespace dacite {

/// Token types supported by the dacite language
enum class TokenType {
    // End of input
    EOF_TOKEN,
    
    // Identifiers and keywords
    IDENTIFIER,
    
    // Keywords
    PACKAGE,
    FN,
    VOID,
    RETURN,
    IF,
    ELSE,
    WHILE,
    FOR,
    TRUE,
    FALSE,
    
    // Literals
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    
    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    ASSIGN,         // =
    PLUS_ASSIGN,    // +=
    MINUS_ASSIGN,   // -=
    MULTIPLY_ASSIGN,// *=
    DIVIDE_ASSIGN,  // /=
    MODULO_ASSIGN,  // %=
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS_THAN,      // <
    LESS_EQUAL,     // <=
    GREATER_THAN,   // >
    GREATER_EQUAL,  // >=
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    LOGICAL_NOT,    // !
    ARROW,          // ->
    
    // Punctuation
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    
    // Comments and whitespace (for debug mode)
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    WHITESPACE,
    
    // Error token
    ERROR
};

/// Represents a single token with its type, value, and source location
struct Token {
    TokenType type;
    std::string value;
    SourceSpan span;

    Token(TokenType type, std::string value, const SourceSpan& span)
        : type(type), value(std::move(value)), span(span) {}

    Token(TokenType type, const SourceSpan& span)
        : type(type), value(), span(span) {}

    bool operator==(const Token& other) const {
        return type == other.type && value == other.value && span == other.span;
    }
};

/// Convert token type to string for debugging
std::string_view token_type_to_string(TokenType type);

/// Check if a token type represents a keyword
bool is_keyword(TokenType type);

/// Get the keyword token type for a given identifier, or IDENTIFIER if not a keyword
TokenType keyword_or_identifier(std::string_view text);

} // namespace dacite