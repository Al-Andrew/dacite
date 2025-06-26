#pragma once

#include <memory>
#include <vector>
#include <string>
#include "ast.h"
#include "token.h"
#include "lexer.h"

namespace dacite {

/// Error information for parser errors
struct ParserError {
    std::string message;
    SourceSpan span;

    ParserError(std::string message, const SourceSpan& span)
        : message(std::move(message)), span(span) {}
};

/// Configuration options for the parser
struct ParserConfig {
    bool debug_mode = false;        // Print parsing steps
    bool recover_from_errors = true; // Try to continue parsing after errors
};

/// The main parser class for parsing dacite tokens into an AST
class Parser {
public:
    /// Create a parser for the given token stream
    explicit Parser(std::vector<Token> tokens, const ParserConfig& config = {});

    /// Parse the tokens into a Program AST
    std::unique_ptr<Program> parse();

    /// Get any errors that occurred during parsing
    const std::vector<ParserError>& get_errors() const { return errors_; }

    /// Check if any errors occurred
    bool has_errors() const { return !errors_.empty(); }

private:
    std::vector<Token> tokens_;
    size_t current_token_;
    ParserConfig config_;
    std::vector<ParserError> errors_;

    // Token management
    const Token& current_token() const;
    const Token& peek_token(size_t offset = 1) const;
    bool at_end() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& error_message);
    void advance();

    // Error reporting
    void report_error(const std::string& message, const SourceSpan& span);
    void report_error(const std::string& message);

    // Debug output
    void debug_print(const std::string& message);

    // Parsing methods
    std::unique_ptr<Program> parse_program();
    std::unique_ptr<PackageDeclaration> parse_package_declaration();
    std::unique_ptr<FunctionDeclaration> parse_function_declaration();
    std::unique_ptr<Type> parse_type();
    std::unique_ptr<BlockStatement> parse_block_statement();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<ReturnStatement> parse_return_statement();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_comparison();
    std::unique_ptr<Expression> parse_term();
    std::unique_ptr<Expression> parse_factor();
    std::unique_ptr<Expression> parse_primary_expression();

    // Helper methods for expression parsing
    BinaryOperator token_to_binary_operator(TokenType token_type);

    // Synchronization for error recovery
    void synchronize();
};

} // namespace dacite