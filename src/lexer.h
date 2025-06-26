#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <functional>
#include "token.h"

namespace dacite {

/// Configuration options for the lexer
struct LexerConfig {
    bool emit_comments = false;      // Include comment tokens in output
    bool emit_whitespace = false;    // Include whitespace tokens in output
    bool debug_mode = false;         // Print tokens as they are lexed
    bool verbose_mode = false;       // Include extra debug information
};

/// Error information for lexer errors
struct LexerError {
    std::string message;
    SourceSpan span;

    LexerError(std::string message, const SourceSpan& span)
        : message(std::move(message)), span(span) {}
};

/// The main lexer class for tokenizing dacite source code
class Lexer {
public:
    /// Create a lexer for the given source code
    explicit Lexer(std::string_view source, const LexerConfig& config = {});

    /// Get the next token from the input
    Token next_token();

    /// Peek at the next token without consuming it
    Token peek_token();

    /// Check if we've reached the end of input
    bool at_end() const;

    /// Get all tokens from the input (useful for debugging)
    std::vector<Token> tokenize_all();

    /// Get any errors that occurred during lexing
    const std::vector<LexerError>& get_errors() const { return errors_; }

    /// Check if any errors occurred
    bool has_errors() const { return !errors_.empty(); }

    /// Dump all tokens to a string (for debugging)
    std::string dump_tokens();

private:
    std::string_view source_;
    size_t current_pos_;
    SourcePosition current_position_;
    LexerConfig config_;
    std::vector<LexerError> errors_;
    std::unique_ptr<Token> peeked_token_;

    // Character manipulation
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void advance_n(size_t n);
    bool match(char expected);
    bool match_string(std::string_view expected);

    // Position tracking
    SourcePosition get_current_position() const { return current_position_; }
    SourceSpan make_span(const SourcePosition& start) const;

    // Token production
    Token make_token(TokenType type, const SourcePosition& start);
    Token make_token(TokenType type, std::string value, const SourcePosition& start);
    Token make_error_token(const std::string& message, const SourcePosition& start);

    // Lexing methods
    Token lex_identifier_or_keyword();
    Token lex_number();
    Token lex_string_literal();
    Token lex_char_literal();
    Token lex_single_line_comment();
    Token lex_multi_line_comment();
    Token lex_operator_or_punctuation();

    // Character classification
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_hex_digit(char c) const;
    bool is_alnum(char c) const;
    bool is_whitespace(char c) const;

    // Error reporting
    void report_error(const std::string& message, const SourcePosition& start);

    // Debug output
    void debug_print_token(const Token& token);
};

} // namespace dacite