#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/parser.h"
#include "../src/lexer.h"

// Simple test framework (reuse from lexer_test.cpp)
#define TEST(name) void test_##name()
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_TRUE(a) assert(a)
#define ASSERT_FALSE(a) assert(!(a))
#define ASSERT_NOT_NULL(a) assert((a) != nullptr)
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl; \
} while(0)

using namespace dacite;

// Helper function to tokenize source code
std::vector<Token> tokenize(const std::string& source) {
    LexerConfig config;
    Lexer lexer(source, config);
    return lexer.tokenize_all();
}

TEST(basic_package_declaration) {
    std::string source = "package main;";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
    ASSERT_NOT_NULL(program->package_declaration);
    ASSERT_EQ(program->package_declaration->package_name, "main");
}

TEST(basic_function_declaration) {
    std::string source = "fn main() i32 { return 5; }";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
    ASSERT_EQ(program->declarations.size(), 1);
    
    auto* func_decl = dynamic_cast<FunctionDeclaration*>(program->declarations[0].get());
    ASSERT_NOT_NULL(func_decl);
    ASSERT_EQ(func_decl->function_name, "main");
    ASSERT_NOT_NULL(func_decl->return_type);
    ASSERT_EQ(func_decl->return_type->name, "i32");
    ASSERT_NOT_NULL(func_decl->body);
    ASSERT_EQ(func_decl->body->statements.size(), 1);
}

TEST(return_statement_with_integer) {
    std::string source = "fn test() i32 { return 42; }";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
    ASSERT_EQ(program->declarations.size(), 1);
    
    auto* func_decl = dynamic_cast<FunctionDeclaration*>(program->declarations[0].get());
    ASSERT_NOT_NULL(func_decl);
    ASSERT_NOT_NULL(func_decl->body);
    ASSERT_EQ(func_decl->body->statements.size(), 1);
    
    auto* return_stmt = dynamic_cast<ReturnStatement*>(func_decl->body->statements[0].get());
    ASSERT_NOT_NULL(return_stmt);
    ASSERT_NOT_NULL(return_stmt->expression);
    
    auto* int_literal = dynamic_cast<IntegerLiteral*>(return_stmt->expression.get());
    ASSERT_NOT_NULL(int_literal);
    ASSERT_EQ(int_literal->value, "42");
}

TEST(complete_program) {
    std::string source = "package main;\n\nfn main() i32 { return 5; }";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
    
    // Check package declaration
    ASSERT_NOT_NULL(program->package_declaration);
    ASSERT_EQ(program->package_declaration->package_name, "main");
    
    // Check function declaration
    ASSERT_EQ(program->declarations.size(), 1);
    auto* func_decl = dynamic_cast<FunctionDeclaration*>(program->declarations[0].get());
    ASSERT_NOT_NULL(func_decl);
    ASSERT_EQ(func_decl->function_name, "main");
    ASSERT_NOT_NULL(func_decl->return_type);
    ASSERT_EQ(func_decl->return_type->name, "i32");
    
    // Check function body and return statement
    ASSERT_NOT_NULL(func_decl->body);
    ASSERT_EQ(func_decl->body->statements.size(), 1);
    auto* return_stmt = dynamic_cast<ReturnStatement*>(func_decl->body->statements[0].get());
    ASSERT_NOT_NULL(return_stmt);
    ASSERT_NOT_NULL(return_stmt->expression);
    auto* int_literal = dynamic_cast<IntegerLiteral*>(return_stmt->expression.get());
    ASSERT_NOT_NULL(int_literal);
    ASSERT_EQ(int_literal->value, "5");
}

TEST(ast_to_string) {
    std::string source = "package main;\n\nfn main() i32 { return 5; }";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
    
    // Test that to_string methods work
    std::string ast_string = program->to_string();
    ASSERT_TRUE(ast_string.find("Program") != std::string::npos);
    ASSERT_TRUE(ast_string.find("PackageDeclaration(main)") != std::string::npos);
    ASSERT_TRUE(ast_string.find("FunctionDeclaration(main") != std::string::npos);
    ASSERT_TRUE(ast_string.find("Type(i32)") != std::string::npos);
    ASSERT_TRUE(ast_string.find("ReturnStatement") != std::string::npos);
    ASSERT_TRUE(ast_string.find("IntegerLiteral(5)") != std::string::npos);
}

TEST(parser_error_handling) {
    std::string source = "package; fn () { return; }"; // Missing package name and function name
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_TRUE(parser.has_errors());
    ASSERT_TRUE(parser.get_errors().size() > 0);
}

TEST(missing_semicolon) {
    std::string source = "package main fn main() i32 { return 5; }"; // Missing semicolon after package
    auto tokens = tokenize(source);
    
    ParserConfig config;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    ASSERT_TRUE(parser.has_errors());
}

TEST(debug_mode) {
    std::string source = "package main;\n\nfn main() i32 { return 5; }";
    auto tokens = tokenize(source);
    
    ParserConfig config;
    config.debug_mode = true;
    Parser parser(std::move(tokens), config);
    auto program = parser.parse();
    
    // Debug mode should not affect parsing success
    ASSERT_FALSE(parser.has_errors());
    ASSERT_NOT_NULL(program);
}

int main() {
    std::cout << "Running Parser Tests..." << std::endl;
    
    RUN_TEST(basic_package_declaration);
    RUN_TEST(basic_function_declaration);
    RUN_TEST(return_statement_with_integer);
    RUN_TEST(complete_program);
    RUN_TEST(ast_to_string);
    RUN_TEST(parser_error_handling);
    RUN_TEST(missing_semicolon);
    RUN_TEST(debug_mode);
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}