#include "lexer.h"
#include <iostream>
#include <sstream>
#include <cctype>

namespace dacite {

Lexer::Lexer(std::string_view source, const LexerConfig& config)
    : source_(source), current_pos_(0), current_position_(1, 1, 0), config_(config) {}

Token Lexer::next_token() {
    // If we have a peeked token, return it
    if (peeked_token_) {
        auto token = std::move(*peeked_token_);
        peeked_token_.reset();
        return token;
    }

    // Skip whitespace (unless we're emitting it)
    while (current_pos_ < source_.length() && is_whitespace(current_char())) {
        if (config_.emit_whitespace) {
            auto start_pos = get_current_position();
            std::string whitespace;
            while (current_pos_ < source_.length() && is_whitespace(current_char())) {
                whitespace += current_char();
                advance();
            }
            auto token = make_token(TokenType::WHITESPACE, std::move(whitespace), start_pos);
            if (config_.debug_mode) debug_print_token(token);
            return token;
        }
        advance();
    }

    // Check for end of input
    if (current_pos_ >= source_.length()) {
        auto token = make_token(TokenType::EOF_TOKEN, get_current_position());
        if (config_.debug_mode) debug_print_token(token);
        return token;
    }

    auto start_pos = get_current_position();
    char c = current_char();

    // Handle identifiers and keywords
    if (is_alpha(c) || c == '_') {
        auto token = lex_identifier_or_keyword();
        if (config_.debug_mode) debug_print_token(token);
        return token;
    }

    // Handle numbers
    if (is_digit(c)) {
        auto token = lex_number();
        if (config_.debug_mode) debug_print_token(token);
        return token;
    }

    // Handle string literals
    if (c == '"') {
        auto token = lex_string_literal();
        if (config_.debug_mode) debug_print_token(token);
        return token;
    }

    // Handle character literals
    if (c == '\'') {
        auto token = lex_char_literal();
        if (config_.debug_mode) debug_print_token(token);
        return token;
    }

    // Handle comments
    if (c == '/' && peek_char() == '/') {
        auto token = lex_single_line_comment();
        if (config_.debug_mode) debug_print_token(token);
        if (config_.emit_comments) {
            return token;
        }
        return next_token(); // Skip comment and get next token
    }

    if (c == '/' && peek_char() == '*') {
        auto token = lex_multi_line_comment();
        if (config_.debug_mode) debug_print_token(token);
        if (config_.emit_comments) {
            return token;
        }
        return next_token(); // Skip comment and get next token
    }

    // Handle operators and punctuation
    auto token = lex_operator_or_punctuation();
    if (config_.debug_mode) debug_print_token(token);
    return token;
}

Token Lexer::peek_token() {
    if (!peeked_token_) {
        peeked_token_ = std::make_unique<Token>(next_token());
    }
    return *peeked_token_;
}

bool Lexer::at_end() const {
    return current_pos_ >= source_.length();
}

std::vector<Token> Lexer::tokenize_all() {
    std::vector<Token> tokens;
    while (!at_end()) {
        auto token = next_token();
        tokens.push_back(token);
        if (token.type == TokenType::EOF_TOKEN) {
            break;
        }
    }
    return tokens;
}

std::string Lexer::dump_tokens() {
    std::ostringstream oss;
    auto tokens = tokenize_all();
    for (const auto& token : tokens) {
        oss << token_type_to_string(token.type);
        if (!token.value.empty()) {
            oss << "(\"" << token.value << "\")";
        }
        oss << " [" << token.span.start.line << ":" << token.span.start.column << "-"
            << token.span.end.line << ":" << token.span.end.column << "]\\n";
    }
    return oss.str();
}

char Lexer::current_char() const {
    if (current_pos_ >= source_.length()) {
        return '\0';
    }
    return source_[current_pos_];
}

char Lexer::peek_char(size_t offset) const {
    size_t pos = current_pos_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

void Lexer::advance() {
    if (current_pos_ < source_.length()) {
        if (source_[current_pos_] == '\n') {
            current_position_.line++;
            current_position_.column = 1;
        } else {
            current_position_.column++;
        }
        current_pos_++;
        current_position_.offset++;
    }
}

void Lexer::advance_n(size_t n) {
    for (size_t i = 0; i < n && current_pos_ < source_.length(); ++i) {
        advance();
    }
}

bool Lexer::match(char expected) {
    if (current_char() != expected) {
        return false;
    }
    advance();
    return true;
}

bool Lexer::match_string(std::string_view expected) {
    if (current_pos_ + expected.length() > source_.length()) {
        return false;
    }
    if (source_.substr(current_pos_, expected.length()) != expected) {
        return false;
    }
    advance_n(expected.length());
    return true;
}

SourceSpan Lexer::make_span(const SourcePosition& start) const {
    return SourceSpan(start, get_current_position());
}

Token Lexer::make_token(TokenType type, const SourcePosition& start) {
    return Token(type, make_span(start));
}

Token Lexer::make_token(TokenType type, std::string value, const SourcePosition& start) {
    return Token(type, std::move(value), make_span(start));
}

Token Lexer::make_error_token(const std::string& message, const SourcePosition& start) {
    report_error(message, start);
    return Token(TokenType::ERROR, message, make_span(start));
}

Token Lexer::lex_identifier_or_keyword() {
    auto start_pos = get_current_position();
    std::string identifier;

    while (current_pos_ < source_.length() && (is_alnum(current_char()) || current_char() == '_')) {
        identifier += current_char();
        advance();
    }

    TokenType type = keyword_or_identifier(identifier);
    return make_token(type, std::move(identifier), start_pos);
}

Token Lexer::lex_number() {
    auto start_pos = get_current_position();
    std::string number;

    // Handle different number formats
    if (current_char() == '0' && peek_char() == 'x') {
        // Hexadecimal
        number += current_char(); advance();
        number += current_char(); advance();
        while (current_pos_ < source_.length() && is_hex_digit(current_char())) {
            number += current_char();
            advance();
        }
    } else if (current_char() == '0' && peek_char() == 'b') {
        // Binary
        number += current_char(); advance();
        number += current_char(); advance();
        while (current_pos_ < source_.length() && (current_char() == '0' || current_char() == '1')) {
            number += current_char();
            advance();
        }
    } else if (current_char() == '0' && is_digit(peek_char())) {
        // Octal
        while (current_pos_ < source_.length() && is_digit(current_char()) && current_char() < '8') {
            number += current_char();
            advance();
        }
    } else {
        // Decimal
        while (current_pos_ < source_.length() && is_digit(current_char())) {
            number += current_char();
            advance();
        }

        // Check for decimal point
        if (current_char() == '.' && is_digit(peek_char())) {
            number += current_char(); advance();
            while (current_pos_ < source_.length() && is_digit(current_char())) {
                number += current_char();
                advance();
            }
            return make_token(TokenType::FLOAT_LITERAL, std::move(number), start_pos);
        }
    }

    return make_token(TokenType::INTEGER_LITERAL, std::move(number), start_pos);
}

Token Lexer::lex_string_literal() {
    auto start_pos = get_current_position();
    std::string literal;
    
    advance(); // Skip opening quote

    while (current_pos_ < source_.length() && current_char() != '"') {
        if (current_char() == '\\') {
            advance(); // Skip backslash
            if (current_pos_ >= source_.length()) {
                return make_error_token("Unterminated string literal", start_pos);
            }
            
            // Handle escape sequences
            switch (current_char()) {
                case 'n': literal += '\n'; break;
                case 't': literal += '\t'; break;
                case 'r': literal += '\r'; break;
                case '\\': literal += '\\'; break;
                case '"': literal += '"'; break;
                case '\'': literal += '\''; break;
                case '0': literal += '\0'; break;
                default:
                    return make_error_token("Invalid escape sequence", start_pos);
            }
        } else {
            literal += current_char();
        }
        advance();
    }

    if (current_pos_ >= source_.length()) {
        return make_error_token("Unterminated string literal", start_pos);
    }

    advance(); // Skip closing quote
    return make_token(TokenType::STRING_LITERAL, std::move(literal), start_pos);
}

Token Lexer::lex_char_literal() {
    auto start_pos = get_current_position();
    std::string literal;
    
    advance(); // Skip opening quote

    if (current_pos_ >= source_.length() || current_char() == '\'') {
        return make_error_token("Empty character literal", start_pos);
    }

    if (current_char() == '\\') {
        advance(); // Skip backslash
        if (current_pos_ >= source_.length()) {
            return make_error_token("Unterminated character literal", start_pos);
        }
        
        // Handle escape sequences
        switch (current_char()) {
            case 'n': literal += '\n'; break;
            case 't': literal += '\t'; break;
            case 'r': literal += '\r'; break;
            case '\\': literal += '\\'; break;
            case '"': literal += '"'; break;
            case '\'': literal += '\''; break;
            case '0': literal += '\0'; break;
            default:
                return make_error_token("Invalid escape sequence in character literal", start_pos);
        }
    } else {
        literal += current_char();
    }
    advance();

    if (current_pos_ >= source_.length() || current_char() != '\'') {
        return make_error_token("Unterminated character literal", start_pos);
    }

    advance(); // Skip closing quote
    return make_token(TokenType::CHAR_LITERAL, std::move(literal), start_pos);
}

Token Lexer::lex_single_line_comment() {
    auto start_pos = get_current_position();
    std::string comment;

    while (current_pos_ < source_.length() && current_char() != '\n') {
        comment += current_char();
        advance();
    }

    return make_token(TokenType::SINGLE_LINE_COMMENT, std::move(comment), start_pos);
}

Token Lexer::lex_multi_line_comment() {
    auto start_pos = get_current_position();
    std::string comment;

    advance(); // Skip '/'
    advance(); // Skip '*'

    while (current_pos_ < source_.length()) {
        if (current_char() == '*' && peek_char() == '/') {
            comment += current_char(); advance();
            comment += current_char(); advance();
            break;
        }
        comment += current_char();
        advance();
    }

    if (current_pos_ >= source_.length()) {
        return make_error_token("Unterminated multi-line comment", start_pos);
    }

    return make_token(TokenType::MULTI_LINE_COMMENT, std::move(comment), start_pos);
}

Token Lexer::lex_operator_or_punctuation() {
    auto start_pos = get_current_position();
    char c = current_char();

    switch (c) {
        case '+':
            advance();
            if (match('=')) return make_token(TokenType::PLUS_ASSIGN, start_pos);
            return make_token(TokenType::PLUS, start_pos);
        
        case '-':
            advance();
            if (match('=')) return make_token(TokenType::MINUS_ASSIGN, start_pos);
            if (match('>')) return make_token(TokenType::ARROW, start_pos);
            return make_token(TokenType::MINUS, start_pos);
        
        case '*':
            advance();
            if (match('=')) return make_token(TokenType::MULTIPLY_ASSIGN, start_pos);
            return make_token(TokenType::MULTIPLY, start_pos);
        
        case '/':
            advance();
            if (match('=')) return make_token(TokenType::DIVIDE_ASSIGN, start_pos);
            return make_token(TokenType::DIVIDE, start_pos);
        
        case '%':
            advance();
            if (match('=')) return make_token(TokenType::MODULO_ASSIGN, start_pos);
            return make_token(TokenType::MODULO, start_pos);
        
        case '=':
            advance();
            if (match('=')) return make_token(TokenType::EQUAL, start_pos);
            return make_token(TokenType::ASSIGN, start_pos);
        
        case '!':
            advance();
            if (match('=')) return make_token(TokenType::NOT_EQUAL, start_pos);
            return make_token(TokenType::LOGICAL_NOT, start_pos);
        
        case '<':
            advance();
            if (match('=')) return make_token(TokenType::LESS_EQUAL, start_pos);
            return make_token(TokenType::LESS_THAN, start_pos);
        
        case '>':
            advance();
            if (match('=')) return make_token(TokenType::GREATER_EQUAL, start_pos);
            return make_token(TokenType::GREATER_THAN, start_pos);
        
        case '&':
            advance();
            if (match('&')) return make_token(TokenType::LOGICAL_AND, start_pos);
            return make_error_token("Invalid character '&'", start_pos);
        
        case '|':
            advance();
            if (match('|')) return make_token(TokenType::LOGICAL_OR, start_pos);
            return make_error_token("Invalid character '|'", start_pos);
        
        case ';': advance(); return make_token(TokenType::SEMICOLON, start_pos);
        case ',': advance(); return make_token(TokenType::COMMA, start_pos);
        case '.': advance(); return make_token(TokenType::DOT, start_pos);
        case ':': advance(); return make_token(TokenType::COLON, start_pos);
        case '(': advance(); return make_token(TokenType::LEFT_PAREN, start_pos);
        case ')': advance(); return make_token(TokenType::RIGHT_PAREN, start_pos);
        case '{': advance(); return make_token(TokenType::LEFT_BRACE, start_pos);
        case '}': advance(); return make_token(TokenType::RIGHT_BRACE, start_pos);
        case '[': advance(); return make_token(TokenType::LEFT_BRACKET, start_pos);
        case ']': advance(); return make_token(TokenType::RIGHT_BRACKET, start_pos);

        default:
            advance();
            return make_error_token("Unexpected character '" + std::string(1, c) + "'", start_pos);
    }
}

bool Lexer::is_alpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::is_digit(char c) const {
    return std::isdigit(c);
}

bool Lexer::is_hex_digit(char c) const {
    return std::isxdigit(c);
}

bool Lexer::is_alnum(char c) const {
    return std::isalnum(c);
}

bool Lexer::is_whitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void Lexer::report_error(const std::string& message, const SourcePosition& start) {
    errors_.emplace_back(message, make_span(start));
}

void Lexer::debug_print_token(const Token& token) {
    if (config_.debug_mode) {
        std::cout << "[" << token.span.start.line << ":" << token.span.start.column 
                  << "] " << token_type_to_string(token.type);
        if (!token.value.empty()) {
            std::cout << "(\"" << token.value << "\")";
        }
        std::cout << std::endl;
    }
}

} // namespace dacite