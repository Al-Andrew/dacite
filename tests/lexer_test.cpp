#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/lexer.h"

// Simple test framework
#define TEST(name) void test_##name()
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_TRUE(a) assert(a)
#define ASSERT_FALSE(a) assert(!(a))
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl; \
} while(0)

using namespace dacite;

TEST(keywords) {
    std::string source = "package fn void return if else while for true false";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<TokenType> expected = {
        TokenType::PACKAGE, TokenType::FN, TokenType::VOID, TokenType::RETURN,
        TokenType::IF, TokenType::ELSE, TokenType::WHILE, TokenType::FOR,
        TokenType::TRUE, TokenType::FALSE, TokenType::EOF_TOKEN
    };
    
    for (auto expected_type : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, expected_type);
    }
}

TEST(identifiers) {
    std::string source = "main print hello_world _private var123";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<std::string> expected = {"main", "print", "hello_world", "_private", "var123"};
    
    for (const auto& expected_value : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, TokenType::IDENTIFIER);
        ASSERT_EQ(token.value, expected_value);
    }
}

TEST(integers) {
    std::string source = "123 0x1F 0b1010 0777";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<std::string> expected = {"123", "0x1F", "0b1010", "0777"};
    
    for (const auto& expected_value : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, TokenType::INTEGER_LITERAL);
        ASSERT_EQ(token.value, expected_value);
    }
}

TEST(floats) {
    std::string source = "3.14 0.5 123.456";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<std::string> expected = {"3.14", "0.5", "123.456"};
    
    for (const auto& expected_value : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, TokenType::FLOAT_LITERAL);
        ASSERT_EQ(token.value, expected_value);
    }
}

TEST(strings) {
    std::string source = R"("hello" "world\n" "with \"quotes\"")";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<std::string> expected = {"hello", "world\n", "with \"quotes\""};
    
    for (const auto& expected_value : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, TokenType::STRING_LITERAL);
        ASSERT_EQ(token.value, expected_value);
    }
}

TEST(characters) {
    std::string source = "'a' '\\n' '\\\\' '\\''";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<std::string> expected = {"a", "\n", "\\", "'"};
    
    for (const auto& expected_value : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, TokenType::CHAR_LITERAL);
        ASSERT_EQ(token.value, expected_value);
    }
}

TEST(operators) {
    std::string source = "+ - * / % = += -= *= /= %= == != < <= > >= && || ! ->";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<TokenType> expected = {
        TokenType::PLUS, TokenType::MINUS, TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO,
        TokenType::ASSIGN, TokenType::PLUS_ASSIGN, TokenType::MINUS_ASSIGN, TokenType::MULTIPLY_ASSIGN,
        TokenType::DIVIDE_ASSIGN, TokenType::MODULO_ASSIGN, TokenType::EQUAL, TokenType::NOT_EQUAL,
        TokenType::LESS_THAN, TokenType::LESS_EQUAL, TokenType::GREATER_THAN, TokenType::GREATER_EQUAL,
        TokenType::LOGICAL_AND, TokenType::LOGICAL_OR, TokenType::LOGICAL_NOT, TokenType::ARROW,
        TokenType::EOF_TOKEN
    };
    
    for (auto expected_type : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, expected_type);
    }
}

TEST(punctuation) {
    std::string source = "; , . : ( ) { } [ ]";
    LexerConfig config;
    Lexer lexer(source, config);
    
    std::vector<TokenType> expected = {
        TokenType::SEMICOLON, TokenType::COMMA, TokenType::DOT, TokenType::COLON,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET, TokenType::EOF_TOKEN
    };
    
    for (auto expected_type : expected) {
        auto token = lexer.next_token();
        ASSERT_EQ(token.type, expected_type);
    }
}

TEST(comments) {
    std::string source = "// single line\n/* multi\nline */ test";
    LexerConfig config;
    config.emit_comments = true;
    Lexer lexer(source, config);
    
    auto token1 = lexer.next_token();
    ASSERT_EQ(token1.type, TokenType::SINGLE_LINE_COMMENT);
    ASSERT_EQ(token1.value, "// single line");
    
    auto token2 = lexer.next_token();
    ASSERT_EQ(token2.type, TokenType::MULTI_LINE_COMMENT);
    
    auto token3 = lexer.next_token();
    ASSERT_EQ(token3.type, TokenType::IDENTIFIER);
    ASSERT_EQ(token3.value, "test");
}

TEST(source_positions) {
    std::string source = "fn\nmain() {\n  return;\n}";
    LexerConfig config;
    Lexer lexer(source, config);
    
    auto token1 = lexer.next_token(); // fn
    ASSERT_EQ(token1.span.start.line, 1);
    ASSERT_EQ(token1.span.start.column, 1);
    
    auto token2 = lexer.next_token(); // main
    ASSERT_EQ(token2.span.start.line, 2);
    ASSERT_EQ(token2.span.start.column, 1);
    
    auto token3 = lexer.next_token(); // (
    ASSERT_EQ(token3.span.start.line, 2);
    ASSERT_EQ(token3.span.start.column, 5);
}

TEST(error_handling) {
    std::string source = "\"unterminated string";
    LexerConfig config;
    Lexer lexer(source, config);
    
    auto token = lexer.next_token();
    ASSERT_EQ(token.type, TokenType::ERROR);
    ASSERT_TRUE(lexer.has_errors());
}

TEST(whitespace_emission) {
    std::string source = "a   b\tc";
    LexerConfig config;
    config.emit_whitespace = true;
    Lexer lexer(source, config);
    
    auto token1 = lexer.next_token(); // a
    ASSERT_EQ(token1.type, TokenType::IDENTIFIER);
    
    auto token2 = lexer.next_token(); // whitespace
    ASSERT_EQ(token2.type, TokenType::WHITESPACE);
    ASSERT_EQ(token2.value, "   ");
    
    auto token3 = lexer.next_token(); // b
    ASSERT_EQ(token3.type, TokenType::IDENTIFIER);
    
    auto token4 = lexer.next_token(); // whitespace
    ASSERT_EQ(token4.type, TokenType::WHITESPACE);
    ASSERT_EQ(token4.value, "\t");
}

int main() {
    std::cout << "Running Lexer Tests..." << std::endl;
    
    RUN_TEST(keywords);
    RUN_TEST(identifiers);
    RUN_TEST(integers);
    RUN_TEST(floats);
    RUN_TEST(strings);
    RUN_TEST(characters);
    RUN_TEST(operators);
    RUN_TEST(punctuation);
    RUN_TEST(comments);
    RUN_TEST(source_positions);
    RUN_TEST(error_handling);
    RUN_TEST(whitespace_emission);
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}