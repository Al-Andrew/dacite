#include "parser.h"
#include <iostream>

namespace dacite {

Parser::Parser(std::vector<Token> tokens, const ParserConfig& config)
    : tokens_(std::move(tokens)), current_token_(0), config_(config) {
}

std::unique_ptr<Program> Parser::parse() {
    debug_print("Starting parse");
    return parse_program();
}

const Token& Parser::current_token() const {
    if (current_token_ >= tokens_.size()) {
        static Token eof_token(TokenType::EOF_TOKEN, SourceSpan{});
        return eof_token;
    }
    return tokens_[current_token_];
}

const Token& Parser::peek_token(size_t offset) const {
    size_t index = current_token_ + offset;
    if (index >= tokens_.size()) {
        static Token eof_token(TokenType::EOF_TOKEN, SourceSpan{});
        return eof_token;
    }
    return tokens_[index];
}

bool Parser::at_end() const {
    return current_token_ >= tokens_.size() || current_token().type == TokenType::EOF_TOKEN;
}

bool Parser::check(TokenType type) const {
    return current_token().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& error_message) {
    if (check(type)) {
        Token token = current_token();
        advance();
        return token;
    }
    
    report_error(error_message);
    return current_token(); // Return current token even on error
}

void Parser::advance() {
    if (!at_end()) {
        current_token_++;
    }
}

void Parser::report_error(const std::string& message, const SourceSpan& span) {
    errors_.emplace_back(message, span);
    if (config_.debug_mode) {
        std::cerr << "Parser error at line " << span.start.line 
                  << ", column " << span.start.column 
                  << ": " << message << std::endl;
    }
}

void Parser::report_error(const std::string& message) {
    report_error(message, current_token().span);
}

void Parser::debug_print(const std::string& message) {
    if (config_.debug_mode) {
        std::cout << "[Parser] " << message << std::endl;
    }
}

std::unique_ptr<Program> Parser::parse_program() {
    debug_print("Parsing program");
    
    auto program = std::make_unique<Program>(SourceSpan{});
    
    // Parse package declaration
    if (check(TokenType::PACKAGE)) {
        auto package_decl = parse_package_declaration();
        if (package_decl) {
            program->set_package_declaration(std::move(package_decl));
        }
    }
    
    // Parse declarations
    while (!at_end()) {
        if (check(TokenType::FN)) {
            auto func_decl = parse_function_declaration();
            if (func_decl) {
                program->add_declaration(std::move(func_decl));
            }
        } else {
            report_error("Expected function declaration");
            break;
        }
    }
    
    return program;
}

std::unique_ptr<PackageDeclaration> Parser::parse_package_declaration() {
    debug_print("Parsing package declaration");
    
    auto package_token = consume(TokenType::PACKAGE, "Expected 'package'");
    auto name_token = consume(TokenType::IDENTIFIER, "Expected package name");
    consume(TokenType::SEMICOLON, "Expected ';' after package declaration");
    
    SourceSpan span(package_token.span.start, name_token.span.end);
    return std::make_unique<PackageDeclaration>(name_token.value, span);
}

std::unique_ptr<FunctionDeclaration> Parser::parse_function_declaration() {
    debug_print("Parsing function declaration");
    
    auto fn_token = consume(TokenType::FN, "Expected 'fn'");
    auto name_token = consume(TokenType::IDENTIFIER, "Expected function name");
    
    // Parse parameter list (simplified - just consume parentheses for now)
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    // Parse return type
    auto return_type = parse_type();
    
    // Parse function body
    auto body = parse_block_statement();
    
    SourceSpan span(fn_token.span.start, body ? body->span.end : current_token().span.end);
    return std::make_unique<FunctionDeclaration>(name_token.value, std::move(return_type), std::move(body), span);
}

std::unique_ptr<Type> Parser::parse_type() {
    debug_print("Parsing type");
    
    auto type_token = consume(TokenType::IDENTIFIER, "Expected type name");
    return std::make_unique<Type>(type_token.value, type_token.span);
}

std::unique_ptr<BlockStatement> Parser::parse_block_statement() {
    debug_print("Parsing block statement");
    
    auto left_brace = consume(TokenType::LEFT_BRACE, "Expected '{'");
    auto block = std::make_unique<BlockStatement>(left_brace.span);
    
    while (!check(TokenType::RIGHT_BRACE) && !at_end()) {
        auto statement = parse_statement();
        if (statement) {
            block->add_statement(std::move(statement));
        } else {
            // Error recovery: skip to next statement or end of block
            synchronize();
        }
    }
    
    auto right_brace = consume(TokenType::RIGHT_BRACE, "Expected '}' after block");
    block->span.end = right_brace.span.end;
    
    return block;
}

std::unique_ptr<Statement> Parser::parse_statement() {
    debug_print("Parsing statement");
    
    if (check(TokenType::RETURN)) {
        return parse_return_statement();
    }
    
    report_error("Expected statement");
    return nullptr;
}

std::unique_ptr<ReturnStatement> Parser::parse_return_statement() {
    debug_print("Parsing return statement");
    
    auto return_token = consume(TokenType::RETURN, "Expected 'return'");
    
    ExpressionPtr expression = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        expression = parse_expression();
    }
    
    auto semicolon = consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    
    SourceSpan span(return_token.span.start, semicolon.span.end);
    return std::make_unique<ReturnStatement>(std::move(expression), span);
}

std::unique_ptr<Expression> Parser::parse_expression() {
    debug_print("Parsing expression");
    
    return parse_primary_expression();
}

std::unique_ptr<Expression> Parser::parse_primary_expression() {
    debug_print("Parsing primary expression");
    
    if (check(TokenType::INTEGER_LITERAL)) {
        auto token = current_token();
        advance();
        return std::make_unique<IntegerLiteral>(token.value, token.span);
    }
    
    report_error("Expected expression");
    return nullptr;
}

void Parser::synchronize() {
    advance();
    
    while (!at_end()) {
        if (current_token().type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        
        switch (current_token().type) {
            case TokenType::FN:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

} // namespace dacite